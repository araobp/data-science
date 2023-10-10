# Acoustic Feature Camera

Acoustic Feature Camera is a device to capture acoustic features for edge AI.

This device is a sort of human ear: log-scale auditory perception and Fourier transform with Mel scaling as feature for training a brain. Connecting this device to Keras/TensorFlow mimics the human auditory system.

## STM32L4 DFSDM configuration

```
System clock: 80MHz
Clock divider: 64
FOSR(decimation): 64
sinc filter: sinc4
resolution: 2 * 64^4 = 2^25 (25 bits)
right bit shift in DFSDM: 1 (results in 24 bit PCM)
Sampling frequency: 80_000_000/64/64 = 19.5kHz
```

### Architecture

```
                                                         ARM Cortex-M4(STM32L476RG)
                                         ***** pre-processing *****           ***** inference *****
                                      ................................................................
                                      :   Filters for feature extraction        Inference on CNN     :
                                      :                                         ..................   :
Sound/voice ))) [MEMS mic]--PDM-->[DFSDM]--+->[]->[]->[]->[]---+----Features--->: code generated :   :
                                      :                        |                : by X-CUBE-AI   :   :
                                      :                        |                ..................   :
                                      :     +------------------+                                     :
                                      :     |                                                        :
                                      :     V                                                        :
                                      :..[USART].....................................................:
                                            |
                                            |
                                            |
                                       (features)
                                            |
                                            | *** learning ***
                                            +--(dataset)--> [oscilloscope.py] Keras/TensorFlow
                                            |
                                            | *** inference ***
                                            +--(dataset)--> [oscilloscope.py] Keras/TensorFlow
```

### Making use of DMA

STMicro's HAL library supports "HAL_DFSDM_FilterRegConvHalfCpltCallback" that is very useful to implemente ring-buffer-like buffering for real-time processing.

I split buffers for DMA into two segments: segment A and segment B.

```
                                                  Interrupt
                          Clock                 ..............
                      +--------------+          : .......... :
                      |              |          : :        V V
                      V              |          : :   +-------------+
Sound/voice ))) [MEMS mic]-+-PDM->[DFSDM]-DMA->[A|B]->|             |->[A|B]->DMA->[DAC] --> Analog filter->head phone ))) Sound/Voice
                                                      |ARM Cortex-M4|->[Feature]->DMA->[UART] --> Oscilloscope on PC or RasPi3
                                                      |             |
                                                      +-------------+

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

## PCM audio output test

=> [PCM](data/PCM.ipynb)
