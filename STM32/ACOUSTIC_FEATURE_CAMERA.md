# Acoustic Feature Camera

Acoustic Feature Camera is a device to capture acoustic features for edge AI.

This device is a sort of human ear: log-scale auditory perception and Fourier transform with Mel scaling as feature for training a brain. Connecting this device to Keras/TensorFlow mimics the human auditory system.

## STM32L4 DFSDM configuration

- The highest frequency on a piano is 4186Hz, but it generate overtones: ~10kHz.
- Human voice also generates overtones: ~ 10kHz.

So the sampling frequency of MEMS mic should be around 20kHz: 20kHz/2 = 10kHz (Nyquist frequency)

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

## Short-time FFT

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

## Pre-processing for Acoustic Scene Classification

The device outputs raw PCM, Short-time FFT, Spectrogram, MFSCs(Mel-Frequency Spectral Coefficients) or MFCCs(Mel-Frequency Cepstral Coefficients) to Oscilloscope GUI via UART at 406800 baudrate.

It also outputs MFSCs or MFCCs to CNN(Convolutional Neural Network).

```
   << MEMS mic >>
         |
         V
   DFSDM w/ DMA
         |
  [16bit PCM data] --> DAC w/ DMA for montoring the sound with a headset
         |
  float32_t data
         |
         |                .... CMSIS-DSP APIs() .........................................
  [ AC coupling  ]-----+  arm_mean_f32(), arm_offset_f32
         |             |
  [ Pre-emphasis ]-----+  arm_fir_f32()
         |             |
[Overlapping frames]   |  arm_copy_f32()
         |             |
  [Windowing(hann)]    |  arm_mult_f32()
         |             |
  [   Real FFT   ]     |  arm_rfft_fast_f32()
         |             |
  [     PSD      ]-----+  arm_cmplx_mag_f32(), arm_scale_f32()
         |             |
  [Filterbank(MFSCs)]--+  arm_dot_prod_f32()
         |             |
     [Log scale]-------+  arm_scale_f32() with log10 approximation
         |             |
 [DCT Type-II(MFCCs)]  |  my original "dct_f32()" function based on CMSIS-DSP
         |             |
         +<------------+
         |
 data the size of int8_t or int16_t (i.e., quantization)
         |
         +-------------> CNN (STM32Cube.AI)
         |
         V
    UART w/ DMA
         |
         V
<< Oscilloscope GUI >>
```


## PCM audio output test

=> [PCM](data/PCM.ipynb)
