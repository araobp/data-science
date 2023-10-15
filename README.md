# Edge AI and Digital Signal Processing

Hand gesture "Paper" captured with [AMG8833 infrared array sensor (8x8 resolution)](https://industrial.panasonic.com/products/pt/grid-eye/models/AMG8833), and its DCT Type-II coefficients.

<img src="python/ThermographyGUI/screenshots/screen_shot_paper.png" width=350>

Spectrogram of [Paco De Lucia "Cepa Andaluza" (Bulerias)](https://youtu.be/NIKWBdthzg4?si=O71p1-v_ynUPurbq) captured with my "Acoustic Feature Camera" device.

<img src="doc/screen_shot_cepa_andaluza_cante.png" width=450>

Edge AI with low-power-consumption MCU such as **[STM32L4 from STMicroelectronics](https://www.st.com/en/microcontrollers-microprocessors/stm32l4-series.html)** is one of key technologies to stop Global Warming.

In this project, I pick up two achievements on edge AI from my past projects, and update them with the latest [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) and [STM32Cube.AI](https://stm32ai.st.com/stm32-cube-ai/).

I would rather be interested in Generative Art based on Physics than Generative AI. I will also try to create Generative Art with edge AI in this project.

My past projects on edge AI:
- [acoustic-features](https://github.com/araobp/acoustic-features)
- [stm32-mcu](https://github.com/araobp/stm32-mcu)

## Project 1: Rock Paper Scissors

<img src="doc/blender_dnn.png" width=500>

- "Rock Paper Scissors" game machine with NUCLEO-L476RG board.
- Digital Twin of the machine made with Blender.

=> **[Rock Paper Scissors](RockPaperScissors.md)**

## Project 2: Acoustic Scene Classification (Work in progress)

<img src="doc/MFSCs_in_Blender.png" width=500>

- Acoustic scene classifcation with NUCLEO L476RG board.
- Generative Art of acoustic features made with Blender.

=> **[Acoustic Scene Classification](AcousticSceneClassification.md)**

---

## Skills required for this project

The purpose of this project for me is to retrain myself with the following skills:

### Science

- Basics of signal processing
- Basics of 3DCG

### Programming Languages

- C
- Python
- Kotlin

### IDEs/tools

- CubeIDE with STM32Cube.AI
- Jupyter Notebook with numpy, pandas and matplotlib
- Android Studio
- Blender

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

### [Arduino-version of the thermography](misc/Arduino)

Make Jupyter Notebook collect data from this Arduino version of the thermograpy device with Panasonic AMG8833.

<img src='doc/me.jpg' width=200>

This is me.

### Learning by Doing

### DSP-related

- [Digital Audio Processing](DigitalAudioProcessing)
- [Speech Processing for AI](SpeechProcessing)

### Others

- [CIFAR 10](misc/CIFAR10): The best accuracy I have ever achieved is 75%...
- [MINIST](misc/MNIST)
- [IMDb](misc/IMDb)
- [Titanic](misc/Titanic)
- [Audio](misc/Audio)
