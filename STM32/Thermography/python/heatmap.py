import numpy as np
import seaborn as sns
import math

import interface

POINTS = [(math.floor(n / 8), (n % 8)) for n in range(0, 64)]

class GUI:

    def __init__(self, interface):
        self.interface = interface

    def plot(self, ax, cmd, cmap=None):

        data = self.interface.read(cmd)

        if cmd == interface.PIXELS:
            data = data * 0.25
            data_flip = np.flip(np.flip(data.reshape(8,8), axis=0), axis=1)

            sns.heatmap(data_flip, cmap=cmap, ax=ax, annot=True, cbar=False)
