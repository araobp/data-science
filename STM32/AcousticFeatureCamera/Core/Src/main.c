/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include "dsp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DFSDM_Filter_HandleTypeDef hdfsdm1_filter0;
DFSDM_Channel_HandleTypeDef hdfsdm1_channel3;
DMA_HandleTypeDef hdma_dfsdm1_flt0;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

// N / 2
const int NN_HALF = NN / 2;

// N * 2
const int NN_DOUBLE = NN * 2;

// flag: "new PCM data has just been copied to buf"
volatile bool new_pcm_data_a = false;
volatile bool new_pcm_data_b = false;

// Output trigger
volatile bool uart_output = false;

// Eight-bit shift to fit 24bit PCM into 16bit PCM
volatile int pcm_bit_shift = 0;

// UART output mode
volatile mode output_mode = FEATURES;

// UART one-byte input buffer
uint8_t rxbuf[1];

// Pre-emphasis toggle
volatile bool pre_emphasis_enabled = true;

// Buffers
// Note: these variables are declared as "extern(al)".
int8_t mfsc_buffer[NUM_FILTERS * 200] = { 0.0f };
int8_t mfcc_buffer[NUM_FILTERS * 200] = { 0.0f };

int pos = 0;

// Debug
volatile debug debug_output = DISABLED;
uint32_t elapsed_time = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DFSDM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
 * Output raw wave or feature to UART by memory-to-peripheral DMA
 */
void uart_tx(float32_t *in, int8_t *f, mode mode, bool dma_start, bool reset) {

  int length = 0;
  static int idx = 0;

#if NUM_FILTERS * 2 > NN*2
  static char uart_buf[NUM_FILTERS * 2] = { 0 };
#else
  static char uart_buf[NN * 2] = { 0 };
#endif

  if (reset) {
    idx = 0;
  }

  switch (mode) {
  case RAW_WAVE:
    length = NN;
    break;

  case SFFT:
    length = NN / 2;
    break;

  case FEATURES:
    length = NUM_FILTERS * 2;
    break;

  default:
    break;
  }

  // Quantization: convert float into int
  if (mode == RAW_WAVE) {
    for (int n = 0; n < length; n++) {
      // fit 24bit PCM into 18bit PCM by right bit shift
      int32_t raw32 = (int32_t) in[n] >> pcm_bit_shift;
      int16_t raw16 = (int16_t) raw32;
      uart_buf[idx++] = (uint8_t) (raw16 >> 8);      // MSB
      uart_buf[idx++] = (uint8_t) (raw16 & 0x00ff);  // LSB
    }
  } else if (mode == FEATURES) {
    if (f != NULL) {
      memcpy(uart_buf + idx, f, NUM_FILTERS * 2);
      idx += NUM_FILTERS * 2;
    }
  } else {
    for (int n = 0; n < length; n++) {
      if (in[n] < -128.0f)
        in[n] = -128.0f;
      uart_buf[idx++] = (int8_t) in[n];
    }
  }

  // memory-to-peripheral DMA to UART
  if (dma_start) {
    HAL_UART_Transmit_DMA(&huart2, (uint8_t*) uart_buf, idx);
  }

}

/*
 * DSP pipeline
 */
void dsp(float32_t *s1, int8_t *f, mode mode) {

  uint32_t start = 0;
  uint32_t end = 0;

  start = HAL_GetTick();

#ifdef AC_COUPLING
  apply_ac_coupling(s1);  // remove DC
#endif

  if (mode >= SFFT) {
    // Pre-emphasis
    if (pre_emphasis_enabled) {
      apply_pre_emphasis(s1);
    }
    apply_hann(s1);
    apply_fft(s1);
    apply_psd(s1);
    if (mode < FEATURES) {
      apply_psd_logscale(s1);
    } else {
      apply_filterbank(s1);
      apply_filterbank_logscale(s1);
      for (int i = 0; i < NUM_FILTERS; i++) {
        mfsc_buffer[pos * NUM_FILTERS + i] = (int8_t) s1[i];
      }
      memcpy(f, mfsc_buffer + pos * NUM_FILTERS, NUM_FILTERS);
      apply_dct2(s1);
      for (int i = 0; i < NUM_FILTERS; i++) {
        mfcc_buffer[pos * NUM_FILTERS + i] = (int8_t) s1[i];
      }
      memcpy(f + NUM_FILTERS, mfcc_buffer + pos * NUM_FILTERS, NUM_FILTERS);
    }
  }
  if (++pos >= 200)
    pos = 0;

  end = HAL_GetTick();
  elapsed_time = end - start;
}

/*
 * Overlap dsp for spectrogram calculation
 *
 *
 * 26.3msec  stride 13.2msec  overlap 50%
 * --- overlap dsp -----------------------
 * [ 1 | 2 ]                       a(1/2)
 *     [ 2 | 3 ]                   a(2/2)
 * --- overlap dsp -----------------------
 *         [ 3 | 4 ]               b(1/2)
 *             [ 4 | 5 ]           b(2/2)
 * --- overlap dsp -----------------------
 *                 [ 5 | 6 ]       a(1/2)
 *                     [ 6 | 7 ]   a(2/2)
 * --- overlap dsp -----------------------
 *                         :
 */
void overlap_dsp(float32_t *buf, mode mode) {

  float32_t signal[NN] = { 0.0f };
  int8_t features[NUM_FILTERS * 2] = { 0U };

  /*---- (1/2) -----------------------------------*/
  arm_copy_f32(buf, signal, NN);
  dsp(signal, features, mode);  // (1/2)

  if (uart_output) {
    switch (mode) {
    case RAW_WAVE:
      uart_tx(signal, NULL, mode, true, true);
      break;
    case SFFT:
      uart_tx(signal, NULL, mode, false, true);
      break;
    default:
      uart_tx(signal, features, mode, false, true);
      break;
    }
  }

  /*---- (2/2) -----------------------------------*/
  arm_copy_f32(buf + NN_HALF, signal, NN);
  dsp(signal, features, mode);  // (2/2)

  if (uart_output) {
    switch (mode) {
    case RAW_WAVE:
      __NOP();
      break;
    case SFFT:
      uart_tx(signal, NULL, mode, true, false);
      break;
    default:
      uart_tx(signal, features, mode, true, false);
      break;
    }
  }

}

/*
 * Dump debug info
 */
void dump(void) {
  if (debug_output != DISABLED) {
    switch (debug_output) {
    case FILTERBANK:
      for (int m = 0; m < NUM_FILTERS + 2; m++) {
        printf("%d:%d,", k_range[m][0], k_range[m][1]);
        for (int n = 0; n < FILTER_LENGTH; n++) {
          printf("%.3f,", filterbank[m][n]);
        }
        printf("\n");
      }
      printf("e\n");
      break;
    case ELAPSED_TIME:
      printf("mode: %d, elapsed_time: %lu(msec)\n", output_mode, elapsed_time);
      break;
    default:
      break;
    }
    debug_output = DISABLED;
  }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */
// Audio sample rate and period
  float32_t f_s;

// DMA peripheral-to-memory double buffer
  int32_t input_buf[NN * 2] = { 0 };

// PCM data store for further processing (FFT etc)
  float32_t signal_buf[NN + NN / 2] = { 0.0f };  // NN/2 overlap

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_DFSDM1_Init();
  /* USER CODE BEGIN 2 */

  f_s = SystemCoreClock / hdfsdm1_channel3.Init.OutputClock.Divider
      / hdfsdm1_filter0.Init.FilterParam.Oversampling
      / hdfsdm1_filter0.Init.FilterParam.IntOversampling;

// DSP initialization
  init_dsp(f_s);

  if (HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1_filter0, input_buf, NN * 2)
      != HAL_OK) {
    Error_Handler();
  }

// Enable UART receive interrupt to receive a command
// from an application processor
  HAL_UART_Receive_IT(&huart2, rxbuf, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    // Wait for next PCM samples from M1
    if (new_pcm_data_a) {  // 1st half of the buffer

      // Overlap
      arm_copy_f32(signal_buf + NN, signal_buf, NN_HALF);

      // Bit shift to obtain 16-bit PCM
      for (uint32_t n = 0; n < NN; n++) {
        signal_buf[n + NN_HALF] = (float32_t) (input_buf[n]
            >> REGISTER_BIT_SHIFT);
      }

      // Overlap dsp
      overlap_dsp(signal_buf, output_mode);

      new_pcm_data_a = false;

    }

    if (new_pcm_data_b) {  // 2nd half of the buffer

      // Overlap
      arm_copy_f32(signal_buf + NN, signal_buf, NN_HALF);

      // Bit shift to obtain 16-bit PCM
      for (uint32_t n = 0; n < NN; n++) {
        signal_buf[n + NN_HALF] = (float32_t) (input_buf[NN + n]
            >> REGISTER_BIT_SHIFT);
      }

      // Overlap dsp
      overlap_dsp(signal_buf, output_mode);

      new_pcm_data_b = false;
    }

    // Dump debug info
    dump();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  /** Configure the main internal regulator output voltage
   */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
      | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief DFSDM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_DFSDM1_Init(void) {

  /* USER CODE BEGIN DFSDM1_Init 0 */

  /* USER CODE END DFSDM1_Init 0 */

  /* USER CODE BEGIN DFSDM1_Init 1 */

  /* USER CODE END DFSDM1_Init 1 */
  hdfsdm1_filter0.Instance = DFSDM1_Filter0;
  hdfsdm1_filter0.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
  hdfsdm1_filter0.Init.RegularParam.FastMode = ENABLE;
  hdfsdm1_filter0.Init.RegularParam.DmaMode = ENABLE;
  hdfsdm1_filter0.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC4_ORDER;
  hdfsdm1_filter0.Init.FilterParam.Oversampling = 64;
  hdfsdm1_filter0.Init.FilterParam.IntOversampling = 1;
  if (HAL_DFSDM_FilterInit(&hdfsdm1_filter0) != HAL_OK) {
    Error_Handler();
  }
  hdfsdm1_channel3.Instance = DFSDM1_Channel3;
  hdfsdm1_channel3.Init.OutputClock.Activation = ENABLE;
  hdfsdm1_channel3.Init.OutputClock.Selection =
  DFSDM_CHANNEL_OUTPUT_CLOCK_SYSTEM;
  hdfsdm1_channel3.Init.OutputClock.Divider = 64;
  hdfsdm1_channel3.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  hdfsdm1_channel3.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
  hdfsdm1_channel3.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  hdfsdm1_channel3.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_RISING;
  hdfsdm1_channel3.Init.SerialInterface.SpiClock =
  DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
  hdfsdm1_channel3.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
  hdfsdm1_channel3.Init.Awd.Oversampling = 1;
  hdfsdm1_channel3.Init.Offset = 0;
  hdfsdm1_channel3.Init.RightBitShift = 0x1;
  if (HAL_DFSDM_ChannelInit(&hdfsdm1_channel3) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1_filter0, DFSDM_CHANNEL_3,
  DFSDM_CONTINUOUS_CONV_ON) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN DFSDM1_Init 2 */

  /* USER CODE END DFSDM1_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 460800;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
 * @brief  Half regular conversion complete callback.
 * @param  hdfsdm_filter DFSDM filter handle.
 * @retval None
 */
void HAL_DFSDM_FilterRegConvHalfCpltCallback(
    DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
  if (!new_pcm_data_a && (hdfsdm_filter == &hdfsdm1_filter0)) {
    new_pcm_data_a = true;  // ready for 1st half of the buffer
  }
}

/**
 * @brief  Regular conversion complete callback.
 * @note   In interrupt mode, user has to read conversion value in this function
 using HAL_DFSDM_FilterGetRegularValue.
 * @param  hdfsdm_filter : DFSDM filter handle.
 * @retval None
 */
void HAL_DFSDM_FilterRegConvCpltCallback(
    DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
  if (!new_pcm_data_b && (hdfsdm_filter == &hdfsdm1_filter0)) {
    new_pcm_data_b = true;  // ready for 2nd half of the buffer
  }
}

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
int _write(int file, char *ptr, int len) {
  HAL_UART_Transmit(&huart2, (uint8_t*) ptr, (uint16_t) len, 0xFFFFFFFF);
  return len;
}

//  (This func is commented out: for a debug purpose only)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == GPIO_PIN_13) {  // User button (blue tactile switch)
    //
  }
}

/*
 * One-byte command reception from an application processor
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  char cmd;

  cmd = rxbuf[0];

  switch (cmd) {

// Pre-emphasis
  case 'r':
    output_mode = RAW_WAVE;
    break;
  case 's':
    output_mode = SFFT;
    break;
  case 'f':
    output_mode = FEATURES;
    break;
  case 'p':
    pre_emphasis_enabled = true;
    break;
  case 'P':
    pre_emphasis_enabled = false;
    break;
  case 'o':  // TX On
    uart_output = true;
    break;
  case 'O':  // Tx off
    uart_output = false;
  case 'F':
    debug_output = FILTERBANK;
    break;
  case 't':
    debug_output = ELAPSED_TIME;
    break;
  default:
    if (cmd >= 0x30 && cmd <= 0x38) {
      pcm_bit_shift = (int) (cmd - 0x30);
    }
    break;
  }

  HAL_UART_Receive_IT(&huart2, rxbuf, 1);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
