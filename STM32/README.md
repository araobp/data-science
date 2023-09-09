# NUCLEO-L476RG board (STM32L476RGT6 Microcontroller)

## Basic apps

- [HelloWorld](HelloWorld)
- [Clock](Clock)

### I2C pin assignment for Arduino shield

Arduino shield uses PB8 and PB9 for I2C SCL and SDA.

<img src="doc/I2C1_SCL.png" width=300>

<img src="doc/I2C1_SDA.png" width=300>

## Rock Paper Scissors with STM32Cube.AI

### Thermography (without AI)

<img src="doc/thermography_interporated.png" width=300>

This project is just a thermography app with Panasonic AMG8833 (no AI):
- [Thermography](Thermography)

Its GUI is in this folder: [GUI(Python3)](Thermography/GUI)

'''
[NUCLEO L476RG]--- UART ---[Thermography GUI]
'''

### Rock Paper Scissors (with AI)

This project is AI Rock Paper Scissors with Panasonic AMG8333 (still work in progress):
- [RockPaperScissors](RockPaperScissors)

UART output:
```
--- AI ROCK PAPER SCISSORS ---
[NETWORK FIND] NN_NAME: network
[NETWORK CREATION] err.type: 0x00, err.code: 0x00
[NETWORK INITIALIZATION] 1
```

### printf issues

I faced malloc-related build errors after I added "printf" (_write() function) to "main.c". The following setting solved the problem.

<img src="doc/printf_issue.png" width=450>

### CubeMX source file overwriting issues

Every time CubeMX generates code, all the source files excluding user-defined parts and user-defined files are overwritten.

Use Github Desktop to discard changes on files on which you made some modifications, especially on those files under X-CUBE-AI/APP folder.

### Arudino shield for Panasonic AMG8833

I designed my original Arduino shield for AMG8833 in 2019. Refer to [this page](https://github.com/araobp/stm32-mcu/tree/master/NUCLEO-F401RE/Thermography).

=> [Schematic](https://github.com/araobp/stm32-mcu/blob/master/NUCLEO-F401RE/Thermography/kicad/arduino_board.pdf)

## References

### CMSIS-DSP library

Refer to the following site: Google search "Configuring DSP libraries on STM32CubeIDE"

[API reference](https://arm-software.github.io/CMSIS_5/General/html/index.html)

### STM32Cube.AI project generation

I use [STM32Cube.AI Developer Cloud](https://stm32ai-cs.st.com/home) to generate a CubeIDE project including AI, since the software quality of X-CUBE-AI on CubeMX does not seem to be so good.

#### APIs ("app_x-cube-ai.h")

The following is the basic APIs of STM32Cube.AI:

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
