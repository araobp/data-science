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
#include <math.h>
#include "arm_math.h"
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

/* USER CODE BEGIN PV */
bool flag = true;
int32_t buf[NN * 2] = { 0 };

arm_rfft_fast_instance_f32 S;
float32_t fs;
int32_t fft_in_int32[NN] = { 0 };
float32_t fft_in[NN] = { 0.0f };
float32_t fft_out[NN] = { 0.0f };
float32_t fft_mag[NN / 2] = { 0.0f };
float32_t fft_db[NN / 2] = { 0.0f };
float32_t fft_freq[NN / 2] = { 0.0f };
float32_t fft_win[NN] = { 0.0f };

bool output_result = false;
bool ac_coupling = true;

// UART one-byte input buffer
uint8_t cmd;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DFSDM1_Init(void);
/* USER CODE BEGIN PFP */

void apply_ac_coupling(float32_t *signal);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

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
  HAL_Delay(100);
  if (HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1_filter0, buf, NN) != HAL_OK) {
    Error_Handler();
  }

  // FFT init
  fs = SystemCoreClock / hdfsdm1_channel3.Init.OutputClock.Divider
      / hdfsdm1_filter0.Init.FilterParam.Oversampling
      / hdfsdm1_filter0.Init.FilterParam.IntOversampling;

  // Hanning window
  const float tmp = 2.0f * M_PI / (float) NN;
  for (uint32_t i = 0; i < NN; i++) {
    *(fft_win + i) = 0.5f - 0.5f * arm_cos_f32((float) i * tmp);
  }

  for (uint32_t i = 0; i < NN / 2; i++) {
    *(fft_freq + i) = (float) i * (float) fs / (float) NN;
  }

  arm_rfft_fast_init_f32(&S, NN);

  printf("\r\nType 'p' to output single-shot FFT.\r\n");
  printf("Type 'a' to enable AC coupling.\r\n");
  printf("Type 'A' to disable AC coupling.\r\n");

  HAL_UART_Receive_IT(&huart2, (uint8_t*) &cmd, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    // Wait
    while (flag)
      ;
    // Raw data output
    /*for (uint32_t i = 0; i < FFT_SampleNum; i++)
     printf("%d\r\n", FFT_inp_int32[i]);*/

    // Note: I checked the CubeIDE debug output of fft_in_int32 and
    // confirmed that all the 9 bits of LSBs are always zero.
    // That is the reason why 9 bit right shift is applied to
    // the variable.
    for (uint32_t i = 0; i < NN; i++) {
      fft_in[i] = (float32_t) (fft_in_int32[i] >> 9);
    }

    // AC Coupling
    if (ac_coupling) {
      apply_ac_coupling(fft_in);
    }

    // Windowing
    arm_mult_f32(fft_in, fft_win, fft_in, NN);

    // Execute FFT
    arm_rfft_fast_f32(&S, fft_in, fft_out, 0);

    // calculate magnitude
    arm_cmplx_mag_f32(fft_out, fft_mag, NN / 2);

    // Normalization (Unitary transformation) of magnitude
    arm_scale_f32(fft_mag, 1.0f / sqrtf((float32_t) NN), fft_mag, NN / 2);

    // AC coupling
    /*
     for (uint32_t i = 0; i < NN / 2; i++) {
     if (*(fft_freq + i) < FFT_AC_COUPLING_HZ)
     fft_mag[i] = 1.0f;
     else
     break;
     }
     */

    float32_t inv_dB_base_mag = 1.0f / 1.0f;
    for (uint32_t i = 0; i < NN / 2; i++)
      fft_db[i] = 10.0f * log10f(fft_mag[i] * inv_dB_base_mag);

    // calc max mag
    float32_t mag_max, frq_max;
    uint32_t maxIndex;
    arm_max_f32(fft_mag, NN / 2, &mag_max, &maxIndex);
    frq_max = *(fft_freq + maxIndex);

    if (output_result) {
      printf(
          "\r\nSampleRate=%d, frq_max = %.1f, mag_max = %f\r\nFreq\tMag\tMag(dB)\r\n",
          (int) fs, frq_max, mag_max);
      for (uint32_t i = 0; i < NN / 2; i++) {
        printf("%.1f\t%f\t%f\r\n", fft_freq[i], fft_mag[i], fft_db[i]);
      }

      output_result = false;
    }

    // HAL_Delay(2000);

    flag = true;        // <- Continuous transformation
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
  hdfsdm1_filter0.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
  hdfsdm1_filter0.Init.FilterParam.Oversampling = 32;
  hdfsdm1_filter0.Init.FilterParam.IntOversampling = 1;
  if (HAL_DFSDM_FilterInit(&hdfsdm1_filter0) != HAL_OK) {
    Error_Handler();
  }
  hdfsdm1_channel3.Instance = DFSDM1_Channel3;
  hdfsdm1_channel3.Init.OutputClock.Activation = ENABLE;
  hdfsdm1_channel3.Init.OutputClock.Selection =
      DFSDM_CHANNEL_OUTPUT_CLOCK_SYSTEM;
  hdfsdm1_channel3.Init.OutputClock.Divider = 32;
  hdfsdm1_channel3.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  hdfsdm1_channel3.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
  hdfsdm1_channel3.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  hdfsdm1_channel3.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_RISING;
  hdfsdm1_channel3.Init.SerialInterface.SpiClock =
      DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
  hdfsdm1_channel3.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
  hdfsdm1_channel3.Init.Awd.Oversampling = 1;
  hdfsdm1_channel3.Init.Offset = 0x0;
  hdfsdm1_channel3.Init.RightBitShift = 0x0;
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
  huart2.Init.BaudRate = 115200;
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

// AC coupling (to remove DC)
void apply_ac_coupling(float32_t *signal) {
  float32_t mean;
  static float32_t mean_hist[NUM_MEANS] = { 0.0f };
  arm_copy_f32(mean_hist + 1, mean_hist, NUM_MEANS - 1);
  arm_mean_f32(signal, NN, mean_hist + NUM_MEANS - 1);
  arm_mean_f32(signal, NUM_MEANS, &mean);
  arm_offset_f32(signal, -mean, signal, NN);
}

void HAL_DFSDM_FilterRegConvCpltCallback(
    DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
  if (flag) {
    for (uint32_t i = 0; i < NN; i++) {
      fft_in_int32[i] = buf[i];
    }
    flag = false;
  }

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

  switch (cmd) {

  case 'p':  // pixels
    output_result = true;
    break;
  case 'a':
    ac_coupling = true;
    break;
  case 'A':
    ac_coupling = false;
    break;
  default:
    break;
  }

  HAL_UART_Receive_IT(&huart2, (uint8_t*) &cmd, 1);
}

int _write(int file, char *pbuf, int len) {
  HAL_UART_Transmit(&huart2, (uint8_t*) pbuf, len, 1000);
  return len;
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
