# Digital Signal Processing

I have started re-learning digital signal processing by re-doing the works in my past projects. Especially, I am very interested in Digital **Audio** Processing and **Acoustinc Scene Classification** with a low-end and low-power-consumption MCU such as STM32L476RG from STMicroelectronics.

## Digital Audio Processing (Work in progress)

=> [Digital Audio Processing](DigitalAudioProcessing)

## Speech Processing for AI

=> [Speech Processing for AI](SpeechProcessing)

## AI Rock Paper Scissors

=> [Rock Paper Scissors](RockPaperScissors.md)

## Acoustic Scene Classification

This is a project to develop edge AI for Acoustic Scene Classification based on STM32Cube.AI, referring to my past project: https://github.com/araobp/acoustic-features which was the best thing I have ever developed in my carrier as an ICT engineer, although it was my hobby project (not a commercial product).

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
### Step 1: Evaluation of MEMS microphone and DFSDM

=> [MEMSMIC](STM32/MEMSMIC.md)

### Step 2: Acoustic Feature Camera (without AI)

=> [ACOUSTIC_FEATURE_CAMERA](STM32/ACOUSTIC_FEATURE_CAMERA.md) (Work in progress)

### Step 3: Training CNN on Jupyter Notebook

### Step 4: Acoustic Feature Camera (with AI)


## Mic Array

...

## References

### Delta-Sigma ADCs

- [How delta-sigma ADCs work, Part 1](https://www.ti.com/lit/an/slyt423a/slyt423a.pdf?ts=1694396702991)
- [How delta-sigma ADCs work, Part 2](https://www.ti.com/lit/an/slyt438/slyt438.pdf?ts=1694411423855)
- [Interfacing PDM digital microphones using
 STM32 MCUs and MPUs](https://www.st.com/resource/en/application_note/an5027-interfacing-pdm-digital-microphones-using-stm32-mcus-and-mpus-stmicroelectronics.pdf)
- [Getting started with sigma-delta digital interface
on applicable STM32 microcontrollers](https://www.st.com/resource/en/application_note/an4990-getting-started-with-sigmadelta-digital-interface-on-applicable-stm32-microcontrollers-stmicroelectronics.pdf)
- [Description of STM32L4/L4+ HAL and low-layer drivers](https://www.st.com/resource/en/user_manual/um1884-description-of-stm32l4l4-hal-and-lowlayer-drivers-stmicroelectronics.pdf)
- [Getting started with analog](https://wiki.st.com/stm32mcu/wiki/STM32StepByStep:Getting_started_with_analog)
- [STM32 Microphone Audio Acquisition: Part 8, STM32 - DFSDM interface for MEMS microphones](https://youtu.be/uMCTkd0PGRs)

### CMSIS-DSP

- CMSIP-DSP: https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html
- Python-version of CMSIS-DSP: https://pypi.org/project/cmsisdsp/

---
## Misc

The below works are for training myself in the field of data science and Deep Learning.

### [Arduino-version of the thermography](misc/Arduino)

Make Jupyter Notebook collect data from this Arduino version of the thermograpy device with Panasonic AMG8833.

<img src='doc/me.jpg' width=200>

This is me.

### [CIFAR 10](misc/CIFAR10)

The best accuracy I have ever achieved is 75%. Can I get a higher score than 75%?

### [MINIST](misc/MNIST)

### [IMDb](misc/IMDb)

### [Titanic](misc/Titanic)

### [Audio](misc/Audio)
