"""
In  this module, windows are created. They are instances of type tk.Frame
and should contain the method show. Show produces the output that is consumed
by gound synths.
"""
import tkinter as tk

import colors
import widgets


class Sequencer(tk.Frame):

    def __init__(self, master, rows, callback=lambda: None):

        tk.Frame.__init__(self, master)
        self.pack()

        self.rows = rows
        self.callback = callback
        self.regulators = {i: [] for i in range(rows)}
        self.regulator_number = [[] for r in range(rows)]

        for r in range(rows):

            slider = tk.Scale(self, from_=1, to=16,
                              sliderrelief=tk.RIDGE,
                              relief=tk.GROOVE,
                              showvalue=False,
                              command=self.set_regulators(r))
            slider.grid(row=r, column=17)

    def set_regulators(self, row):
        def func(num):
            num = int(num)
            self.regulator_number[row] = num
            current_num = len(self.regulators[row])
            if num > current_num:
                for i in range(num - current_num):
                    regulator = widgets.CanvasRegulator(self)
                    self.regulators[row].append(regulator)
                    regulator.grid(row=row, column=current_num + i)
            elif num < current_num:
                for i in range(current_num - num):
                    self.regulators[row][-1].destroy()
                    del self.regulators[row][-1]
        return func

    def react(self):
        self.callback()

    def show(self):
        return ' '.join([str(r.value) for r in self.regulators])

    def sample(self, num):
        """ return values at position num \in [0, 64) """
        index = [int(num / 64 * len(self.regulators[r]))
                for r in range(self.rows)]
        values = [self.regulators[r][i].value for (r, i) in enumerate(index)]
        return values


class StateGUI(tk.Frame):

    def __init__(self, master, ranges, rows, callback=lambda: None):

        tk.Frame.__init__(self, master)
        self.pack()

        self.ranges = ranges
        self.rows = rows
        self.callback = callback
        self.connections = [[False for i in range(ranges)]
                                   for j in range(rows)]
        self.regulators = []

        for i in range(self.ranges):

            button = widgets.CanvasRegulator(self, callback=self.react)
            self.regulators.append(button)
            button.grid(row=0, column=i + 1)

            for j in range(self.rows):
                widgets.CanvasToggler(self,
                              self.turn_on_connection(j, i),
                              self.turn_off_connection(j, i)).grid(
                        row=j + 1, column=i + 1)

        for j in range(self.rows):
            widgets.CanvasSender(self,
                         self.inc_connected(j),
                         self.dec_connected(j)).grid(
                    row=j + 1, column=0)


    def turn_on_connection(self, i, j):
        def func():
            self.connections[i][j] = True
        return func

    def turn_off_connection(self, i, j):
        def func():
            self.connections[i][j] = False
        return func

    def connection_toggler(self, i, j):
        def func():
            self.connections[i][j] = not self.connections[i][j]
        return func

    def inc_connected(self, j):
        def func(value):
            for (i, c) in enumerate(self.connections[j]):
                if c: self.regulators[i].increase(value)()
        return func

    def dec_connected(self, j):
        def func(value):
            for (i, c) in enumerate(self.connections[j]):
                if c: self.regulators[i].decrease(value)()
        return func

    def show(self):
        return ' '.join([str(r.value) for r in self.regulators])

    def print(self):
        print(self.show())

    def react(self):
        self.callback()


import cubic
import numpy as np
class SmoothSequencer(tk.Frame):

    def __init__(self, master, width, height, num):

        tk.Frame.__init__(self, master)
        self.pack()
        self.width = width
        self.height = height

        self.canvas = widgets.canvas_with_elements(self, width, height,
                self.onleftclick, self.onrightclick, self.draw)
        self.canvas.pack()

        self.num = num
        self.cuber = cubic.Cuber()
        self.draw(None)

    def normalize(self, x, y):
        return (x / self.width, 1 - y / self.height)

    def denormalize(self, x, y):
        return (x * self.width, (1 - y) * self.height)

    def onleftclick(self, event):
        (x, y) = self.normalize(event.x, event.y)
        self.cuber.add(x, y)
        self.draw(event)

    def onrightclick(self, event):
        (x, y) = self.normalize(event.x, event.y)
        num = self.cuber.remove(x, y)
        self.draw(event)

    def draw(self, event):
        self.canvas.create_rectangle(0, 0, self.width, self.height,
                                     fill=colors.toggle_color((0.1, 0.1, 0.1)))
        self.update()
        x = np.linspace(0, 1, self.num)
        for (u, v) in zip(x, self.cuber(x)):
            (u, v) = self.denormalize(u, v)
            self.canvas.create_oval(u - 1, v - 1, u + 1, v + 1,
                    fill=colors.toggle_color((1, 1, 1)))
        for (u, v) in zip(self.cuber.x, self.cuber.y):
            (u, v) = self.denormalize(u, v)
            self.canvas.create_oval(u - 3, v - 3, u + 3, v + 3,
                    fill=colors.toggle_color((1, 1, 1)))

    def update(self):
        self.cuber.spline()

    def sample(self, x):
        return self.cuber(x)
