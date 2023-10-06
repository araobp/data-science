# Acoustic Feature Camera

Acoustic Feature Camera is a device to capture acoustic features for edge AI.

### Architecture

```
                                                         ARM Cortex-M4(STM32L476RG)
                                         ***** pre-processing *****           ***** inference *****
                                      ................................................................
                                      :   Filters for feature extraction        Inference on CNN     :
                                      :                                         ..................   :
Sound/voice ))) [MEMS mic]--PDM-->[DFSDM]--+->[]->[]->[]->[]---+----Features--->: code generated :   :
                                      :    |                   |                : by X-CUBE-AI   :   :
                                      :    +------------+      |                ..................   :
                                      :     +-----------|------+                                     :
                                      :     |           |                                            :
                                      :     V           V                                            :
                                      :..[USART]......[DAC]..........................................:
                                            |           |
                                            |           | *** monitoring raw sound ***
                                            |           +---> [Analog filter] --> head phone
                                       (features)
                                            |
                                            | *** learning ***
                                            +--(dataset)--> [oscilloscope.py/Win10 or RasPi3] Keras/TensorFlow
                                            |
                                            | *** inference ***
                                            +--(dataset)--> [oscilloscope.py/Win10 or RasPi3] Keras/TensorFlow
```

## Short-time FFT on STM32

```
<DFSDM-to-Memory DMA interrupt A>
void HAL_DFSDM_FilterRegConvHalfCpltCallback()
[ 4 | 5 ] ----------------------+---+
                                |   |
                                V   V         FFT bin        FFT bin
                  Buffer  [ 3 | 4 | 5 ] - - > [ 3 | 4 ] and [ 4 | 5 ]
                            ^
Buffer                      |
[ 1 | 2 | 3 ] --------------+


<DFSDM-to-Memory DMA interrupt B>
void HAL_DFSDM_FilterRegConvCpltCallback()
[ 6 | 7 ] ----------------------+---+
                                |   |
                                V   V         FFT bin        FFT bin
                  Buffer  [ 5 | 6 | 7 ] - - > [ 5 | 6 ] and [ 6 | 7 ]
                            ^
Buffer                      |
[ 3 | 4 | 5 ] --------------+

```

## DFSDM config

```
System clock: 80MHz
Clock divider: 64
FOSR(decimation): 64
sinc filter: sinc4
resolution: 2 * 64^4 = 2^25 (25 bits)
right bit shift in DFSDM: 1 (results in 24 bit PCM)
Sampling frequency: 80_000_000/64/64 = 19.5kHz
```

## PCM audio output test

=> [PCM](PCM.ipynb)
