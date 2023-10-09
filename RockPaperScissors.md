# AI Rock Paper Scissors

<img src="STM32/doc/AMG8833_breakout.jpg" width=400>

I developed Rock Paper Scissors on Nucleo L401RE board with Panasonic AMG8833 in January 2019, just after the first relase of STM32Cube.AI (December 2018) became available for developers: [the demo in on YouTube](https://www.youtube.com/shorts/d6OYSllaVEs). I remember my excitement on STM32Cube.AI -- AI can run on such a tiny device!

I relearn it in this project by doing this: https://github.com/araobp/stm32-mcu/tree/master/NUCLEO-F401RE/Thermography

STMicroelectronics also developed a toy similar to mine, but with VL53L5: https://stm32ai.st.com/use-case/shifumi-gesture-recognition/

```
Step 1: Data collection of 8x8 matrix temperature data.

[NUCLEO-L476RG] ---- data ----> [Thermography GUI] --> CSV files in "/data" folder

Step 2: Training DNN and generate Keras model.

Step 3: Generate code with the Keras model.

Keras model --> CubeIDE with STM32Cube.AI --> Generated code

Step 4: Add code to the generated one.

Step 5: Run the device to play "Rock Paper Scissors".

```

### Step 1: Data collection of 8x8 matrix temperature data

=> [Thermography](STM32/Thermography.md)

### Step 2: Training DNN and generate Keras model

=> [Training DNN with DCT Type-II coefficients as heatmap feature](RockPaperScissors)

The DNN model's recognition performance is much better than [that](https://github.com/araobp/stm32-mcu/blob/master/NUCLEO-F401RE/Thermography/tensorflow/rock_paper_scissors_dct.ipynb) of my old project in 2019: overfitting occured in the old model. The performance has been improved by dropping DCT coefficients of higher frequencies drastically and adding another dense layer.

### Step 3 ~ Step 5: Edge AI implementation and experiment 

=> [RockPaperScissors with CubeIDE and STM32Cube.AI](STM32/RockPaperScissors.md)

The hardware part of "RockPaperScissors" on STMicroelectronics NUCLEO-L476RG board with the latest version of CubeIDE/CubeMX and STM32Cube.AI.

### 3DCG animation

Use Blender and GIMP to animate the scene of my edge AI device. This is quite a challenge but worth doing.

Video on YouTube
- [Part 1](https://youtu.be/e6F0C5PsM-8)
- [Part 2](https://youtu.be/bS-bzMsygZQ)

=> [Blender files](blender)

I used Python to animate text on the LCD in the 3DCG scene. The script must be run just once after the blender file is opened.

<img src="doc/blender_script.jpg" width=700>

I also used Python to generate DNN programatically:

<img src="doc/blender_script2.jpg" width=700>



