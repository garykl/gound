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

