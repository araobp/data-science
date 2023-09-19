# MEMS mic

What does MEMS mean? [Wikipedia](https://en.wikipedia.org/wiki/MEMS)

I usually use a MEMS mic "SPM0405HD4H" from Knowles, and [its breakout board](https://akizukidenshi.com/catalog/g/gM-05577/) from Akizuki-Denshi, Akihabara.

In this project, I will test its frequency response in the following configuration.:

```
                    . . . . . . . . . . . . . . Arm Cortex-M4 . . . . . . . . . . . .
                    .   +-------+                                FFT                .
     +----- Clock ------|       |          +-----+   +-----+   +------+   +------+  .
     |              .   | DFSDM |-- PCM -->| DMA |-->| RAM |-->| Core |-->|USART2|-- UART/USB --> PC
     |              .   |       |          +-----+   +-----+   | DSP  |   +------+  . 
     v              .   |       |                              +------+             .
[MEMS mic]--- PDM ----->|       |                                                   .
                    .   +-------+                                                   .
                    . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
```

I use [my original Arduino shield](https://github.com/araobp/acoustic-features/tree/master/kicad) with Nucleo L476RG board in this project.

## Filter Regular Channel Conversion

Quote from [Description of STM32L4/L4+ HAL and low-layer drivers](https://www.st.com/resource/en/user_manual/um1884-description-of-stm32l4l4-hal-and-lowlayer-drivers-stmicroelectronics.pdf):

1. Select regular channel and enable/disable continuous mode using HAL_DFSDM_FilterConfigRegChannel().
2. Start regular conversion using HAL_DFSDM_FilterRegularStart(), HAL_DFSDM_FilterRegularStart_IT(),
HAL_DFSDM_FilterRegularStart_DMA() or HAL_DFSDM_FilterRegularMsbStart_DMA().
3. In polling mode, use HAL_DFSDM_FilterPollForRegConversion() to detect the end of regular conversion.
4. In interrupt mode, HAL_DFSDM_FilterRegConvCpltCallback() will be called at the end of regular conversion.
5. Get value of regular conversion and corresponding channel using HAL_DFSDM_FilterGetRegularValue().
6. In DMA mode, HAL_DFSDM_FilterRegConvHalfCpltCallback() and
HAL_DFSDM_FilterRegConvCpltCallback() will be called respectively at the half transfer and at the transfer
complete. Please note that HAL_DFSDM_FilterRegConvHalfCpltCallback() will be called only in DMA
circular mode.
7. Stop regular conversion using HAL_DFSDM_FilterRegularStop(), HAL_DFSDM_FilterRegularStop_IT() or
HAL_DFSDM_FilterRegularStop_DMA().

## DFSDM Output Data Resolution

Quote from [Getting started with sigma-delta digital interface
on applicable STM32 microcontrollers](https://www.st.com/resource/en/application_note/an4990-getting-started-with-sigmadelta-digital-interface-on-applicable-stm32-microcontrollers-stmicroelectronics.pdf):

#### Output data resolution

A consequence of the Sinc filter operation (moving average) is to increase the resolution of
the sampled signal (by a factor FOSR). Multiple averaging increases even more the
resolution. The total resolution (in LSBs) of the output signal is then:
Resolution_out = Resolution_in * FOSR ^ FORD.
- Resolution_in correspond to the input data resolution (2 in case of serial data input or
wider in case of parallel data input, for example 4096 for 12-bit parallel input).
- Caution must be taken to not increase Resolution_out over the 32

#### Output data unit

The output data unit performs a final correction that consists in shifting the bits to the right
and applying an offset correction on the data coming from the integrator.
DFSDM peripheral operation AN4990
24/56 AN4990 Rev 1
Shifting bits to the right is used to:
- fit the 32-bit internal output into the final 24-bit register
- limit even more the final resolution (to 16-bit for instance in case of audio data)

The offset correction allows to calibrate the external sigma-delta modulator offset error. The
user configures the offset register with a signed 24-bit correction, and this register is
automatically added to the output result. The offset correction value is usually the result of a
calibration routine embedded within the microcontroller software that performs the offset
calibration calculation and stores the correction into the offset register.
All operations in the DFSDM peripheral are in signed format (filtering, integration, offset
correction, right bit shift).

## Code

I reuse [this code](https://github.com/araobp/NUCLEO-L476RG_DFSDM_PDM-Mic) with some minor modifications.

=> [Modified code](./MEMSMIC)

## Test results

=> [Test results on Jupyter Notebook](./data/MEMSMIC.ipynb)

I have made the tests under the following conditions. 

DFSDM configuration 1:
- System clock: 80MHz
- Clock divider: 32
- FOSR(decimation): 128
- sinc filter: sinc3
- resolution: 2 * 128^3 = 2^22 (22 bits)
- right bit shift in DFSDM: 6 (results in 16 bit PCM)
- Sampling frequency: 80_000_000/32/128 = 19.5kHz

DFSDM configuration 2:
- System clock: 80MHz
- Clock divider: 32
- FOSR(decimation): 64
- sinc filter: sinc3
- resolution: 2 * 64^3 = 2^19 (19 bits)
- right bit shift in DFSDM: 3 (results in 16 bit PCM)
- Sampling frequency: 80_000_000/32/64 = 39.1kHz

DFSDM configuration 3:
- System clock: 80MHz
- Clock divider: 32
- FOSR(decimation): 32
- sinc filter: sinc3
- resolution: 2 * 32^3 = 2^16 (16 bits)
- right bit shift in DFSDM: 0 (results in 16 bit PCM)
- Sampling frequency: 80_000_000/32/64 = 78.1kHz

DFSDM configuration 4:
- System clock: 80MHz
- Clock divider: 64
- FOSR(decimation): 64
- sinc filter: sinc5
- resolution: 2 * 64^5 = 2^31 (31 bits)
- right bit shift in DFSDM: 15 (results in 16 bit PCM)
- Sampling frequency: 80_000_000/64/64 = 19.5kHz

DFSDM configuration 5:
- System clock: 80MHz
- Clock divider: 64
- FOSR(decimation): 64
- sinc filter: sinc4
- resolution: 2 * 64^4 = 2^25 (25 bits)
- right bit shift in DFSDM: 9 (results in 16 bit PCM)
- Sampling frequency: 80_000_000/64/64 = 19.5kHz

| Config | Zero LSBs confirmed | Clock divider  | FOSR   | fs(kHz) | sinc   | bit shift in DFSDM | bit shift by software  |
| ------ | ------------------- | -------------- | ------ | ------- | ------ | ------------------ | ---------------------- |
| 1      |              9 bits |             32 |    128 |    19.5 |  sinc3 |             6 bits |                 8 bits |
| 2      |                     |             32 |     64 |    39.1 |  sinc3 |             3 bits |                 8 bits |
| 3      |                     |             32 |     32 |    78.1 |  sinc3 |             0 bits |                 8 bits |
| 4      |              8 bits |             64 |     64 |    19.5 |  sinc5 |            15 bits |                 8 bits |
| 5      |              8 bits |             64 |     64 |    19.5 |  sinc4 |             9 bits |                 8 bits |

**Conclusion**

The configuration 5 seems to be the best for feature engineering for musical instrument recognition use case that I am planing to develop after this DFSDM evaluation.

## Bit shift operations on DFSDM

I have been confused on this issue for many years.

There seems to be two kinds of right bit shift operations surrounding DFSDM:
- right bit shift to fit the internal data into the 24bit-length data field (RDATA[23:8]) of the data output register.
- right bit shift (8bit) to ignore the eight LSBs of the data output register.

```
[ Overflowed internal data               ]
:                                        :
:                                +- - - -+
:                                :
V                                V
[ 24bit data containing PCM data | 8bit LSBs ]  (However, it seems to me that 9bit LSBs are always zero.)

                                |
                                V

                  [ 24bit data containing PCM data ]
```

What is more, it seems to me that nine LSBs of the data ouput register is always zero in certain conditions (Clock Divider and FOSR). In the other conditions, eight LSBs of the data output is always zero. Why?

I have never received any formal education on digital signal processing and DFSDM in my life, so I do not know if I am right or wrong.

Anyway, I have taken the following approach in my projects so far:
- right bit shift operation inside DFSDM to fit the internal data into 24bit length (or into 16bit length for feature engineering).
- "output_data >> 9" operation by CPU

**I am going take the DFSDM configutaion below for feature engineering for musical instrument recognition**
- right bit shift operation inside DFSDM to fit the internal data into 16bit length.
- "output_data >> 8" operation by CPU.

## Reference

- [Getting started with sigma-delta digital interface
on applicable STM32 microcontrollers](https://www.st.com/resource/en/application_note/an4990-getting-started-with-sigmadelta-digital-interface-on-applicable-stm32-microcontrollers-stmicroelectronics.pdf)
- [Description of STM32L4/L4+ HAL and low-layer drivers](https://www.st.com/resource/en/user_manual/um1884-description-of-stm32l4l4-hal-and-lowlayer-drivers-stmicroelectronics.pdf)
- [PCM data resolution on STM32L4](https://github.com/araobp/stm32-mcu/blob/master/tips/RESOLUTION.md)
