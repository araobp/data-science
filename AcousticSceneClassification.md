# Acoustic Scene Classification

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

