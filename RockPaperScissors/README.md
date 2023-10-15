# Rock Paper Scissors

An infrared array sensor such as AMG8833 is not suitable for recognizing details of hand gestures, that is the reason why I use DCT Type-II as image features of heat map taken with AMG8833.

The lower-frequency part of the DCT coefficients (blurred heat map) is used as inputs to DNN. The neural network model ignores details of hand gestures such as gaps between fingers, and it shows the best convergence among various neural network models.

## The best model

- [RockPaperScissorsDCT.ipynb](RockPaperScissorsDCT.ipynb)
- [How does the neural network model see hand gestures?](DCT%20and%20IDCT.ipynb)

## The other models with less accuracies

- [RockPaperScissorsDNN.ipynb](RockPaperScissorsDNN.ipynb)
- [RockPaperScissorsCNN.ipynb](RockPaperScissorsCNN.ipynb)


