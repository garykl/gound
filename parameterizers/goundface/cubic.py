"""
The cuber makes a cubic spline interpolation of points. Points can be added
and a new interpolation is performed, matching all past points and the new one.
Points can also be removed.
"""

from scipy.interpolate import interp1d
import numpy as np


class Cuber(object):

    def __init__(self, epsilon=0.1):
        self.x = []
        self.y = []
        self.epsilon = epsilon
        self.func = np.vectorize(lambda x: 0)

    def add(self, x, y):
        self.x.append(x)
        self.y.append(y)

    def remove(self, x, y):
        indices = [i for (i, (u, v)) in enumerate(zip(self.x, self.y))
                if distance((u, v), (x, y)) < self.epsilon]
        for i in reversed(indices):
            del self.x[i]
            del self.y[i]
        return len(indices)

    def spline(self):
        if len(self.x) == 1:
            self.func = np.vectorize(lambda x: self.y[0])
        elif len(self.x) > 1:
            x = np.array(self.x)
            x = np.concatenate([x - 1, x, x + 1])
            y = np.concatenate([self.y, self.y, self.y])
            self.func = interp1d(x, y, kind='cubic')

    def __call__(self, x):
        return self.func(x)


def distance(p1, p2):
    return np.linalg.norm(np.array(p1) - np.array(p2))
