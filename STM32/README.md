# NUCLEO-L476RG board (STM32L476RGT6 Microcontroller)

## Basic apps

- [HelloWorld](HelloWorld)
- [Clock](Clock)

## Rock Paper Scissors by Cube.AI

This project is just a thermography app with Panasonic AMG8833 (no AI):
- [Thermography](Thermography)

### CMSIS-DSP library

- [CMSIS-DSP API reference](https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html)
- [How to install CMSIS-DSP in CubeIDE](https://community.st.com/t5/stm32-mcus/configuring-dsp-libraries-on-stm32cubeide/ta-p/49637)

### Cube.AI

[Bug?] The latest version of CubeMX does not seem to generate "syscalls.c" in Src folder.

#### APIs ("app_x-cube-ai.h")

Find AI network
```
const char* ai_mnetwork_find(const char *name, ai_int idx);
```

Create AI network
```
ai_error ai_mnetwork_create(const char *name, ai_handle* network, const ai_buffer* network_config);
```

Initialize AI network
```
ai_bool ai_mnetwork_init(ai_handle network);
```

Run inference on AI network
```
ai_i32 ai_mnetwork_run(ai_handle network, const ai_buffer* input, ai_buffer* output);
```

## References

- https://stackoverflow.com/questions/59395181/cmsis-dsp-in-stm32cubeide
