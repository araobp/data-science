# NUCLEO-L476RG board

- [HelloWorld](HelloWorld)
- [Clock](Clock)
- [Thermography](Thermography)

## Rock Paper Scissors by Cube.AI

- [Validation](RockPaperScissorsValidation)

[Bug?] The latest version of CubeMX does not seem to generate "syscalls.c" in Src folder.

## CMSIS-DSP library

https://github.com/ARM-software/CMSIS_5

## Cube.AI APIs ("app_x-cube-ai.h")

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
