# Acoustic Scene Classification

This is a project to develop edge AI for Acoustic Scene Classification based on STM32Cube.AI, referring to my past project: https://github.com/araobp/acoustic-features

### System Architecture

```
   [Android app] ---- BLE ---- [NUCLEO L476RG]
```

### Step 1: Evaluation of MEMS microphone and DFSDM

=> [MEMSMIC](STM32/MEMSMIC.md)

### Step 2: Acoustic Feature Camera (without AI)

=> [ACOUSTIC_FEATURE_CAMERA](STM32/ACOUSTIC_FEATURE_CAMERA.md) (Work in progress)

### Step 3: Microphone Array for Beamforming

=> [MicrophoneArray simulation](MicrophoneArray)

=> [Arduino shield revision to support two MEMS microphones]

=> [ACOUSTIC_FEATURE_CAMERA with two MEMS microphones]

### Step 4: Training CNN on Jupyter Notebook

### Step 5: Acoustic Feature Camera (with AI)

### Step 6: Android app
