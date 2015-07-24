import tkinter as tk

import colors


class CanvasToggler(tk.Frame):

    def __init__(self, master, callback_on, callback_off):
        tk.Frame.__init__(self, master)
        self.callback_on = callback_on
        self.callback_off = callback_off

        self.state = False
        self.canvas = clickable_canvas(self, 30, self.callback)
        self.canvas.pack()
        self.set_optics()

    def set_optics(self):
        if self.state:
            self.canvas.create_oval(1, 1, 29, 29,
                                    fill=colors.triple_2_rgb((0.5, 0.5, 0.5)),
                                    outline=colors.green_color,
                                    width=2)
        else:
            self.canvas.create_oval(1, 1, 29, 29,
                                    fill=colors.triple_2_rgb((0.2, 0.2, 0.2)),
                                    outline=colors.red_color,
                                    width=2)

    def callback(self, event):
        if self.state:
            self.callback_off()
            self.state = False
        else:
            self.callback_on()
            self.state = True
        self.set_optics()



class CanvasSender(tk.Frame):

    def __init__(self, master, callback1, callback2):

        self.callback1 = callback1
        self.callback2 = callback2

        tk.Frame.__init__(self, master)
        self.canvas_fine = scrollable_canvas(
                self, 20, self.decrease(0.01), self.increase(0.01))
        self.canvas_middle = scrollable_canvas(
                self, 30, self.decrease(0.05), self.increase(0.05))
        self.canvas_rough = scrollable_canvas(
                self, 40, self.decrease(0.2), self.increase(0.2))
        self.canvas_fine.pack(side=tk.LEFT)
        self.canvas_middle.pack(side=tk.LEFT)
        self.canvas_rough.pack(side=tk.LEFT)
        self.set_color(True)

    def set_color(self, which):
        if which:
            self.canvas_fine.create_rectangle(
                    0, 0, 50, 50, fill=colors.blue_color)
            self.canvas_middle.create_rectangle(
                    0, 0, 50, 50, fill=colors.blue_color)
            self.canvas_rough.create_rectangle(
                    0, 0, 50, 50, fill=colors.blue_color)
        else:
            self.canvas_fine.create_rectangle(
                    0, 0, 50, 50, fill=colors.red_color)
            self.canvas_middle.create_rectangle(
                    0, 0, 50, 50, fill=colors.red_color)
            self.canvas_rough.create_rectangle(
                    0, 0, 50, 50, fill=colors.red_color)

    def increase(self, stepsize):
        def func(event):
            self.set_color(True)
            self.callback1(stepsize)
        return func

    def decrease(self, stepsize):
        def func(event):
            self.set_color(False)
            self.callback2(stepsize)
        return func


class CanvasRegulator(tk.Frame):

    def __init__(self, master, callback=lambda: None):

        self.callback = callback
        self.value = 0.5

        tk.Frame.__init__(self, master)
        self.canvas_fine = scrollable_canvas(
                self, 20, self.decrease(0.01), self.increase(0.01))
        self.canvas_middle = scrollable_canvas(
                self, 30, self.decrease(0.05), self.increase(0.05))
        self.canvas_rough = scrollable_canvas(
                self, 40, self.decrease(0.2), self.increase(0.2))

        self.canvas_fine.pack()
        self.canvas_middle.pack()
        self.canvas_rough.pack()
        self.set_color()

    def set_color(self):
        self.canvas_fine.create_rectangle(
                0, 0, 20, 20, fill=colors.toggle_color((0.1, self.value, 0.1)))
        self.canvas_middle.create_rectangle(
                0, 0, 30, 30, fill=colors.toggle_color((0.1, self.value, 0.1)))
        self.canvas_rough.create_rectangle(
                0, 0, 40, 40, fill=colors.toggle_color((0.1, self.value, 0.1)))

    def increase(self, stepsize):
        def func(*event):
            self.value += stepsize
            if self.value > 1:
                self.value = 1
            self.set_color()
            self.callback()
        return func

    def decrease(self, stepsize):
        def func(*event):
            self.value -= stepsize
            if self.value < 0:
                self.value = 0
            self.set_color()
            self.callback()
        return func



def clickable_canvas(master, size, callback):
    canvas = tk.Canvas(master, width=size, height=size)
    canvas.bind("<Button-1>", callback)
    return canvas


def scrollable_canvas(master, size, callback_down, callback_up):
    canvas = tk.Canvas(master, width=size, height=size)
    canvas.bind("<Button-4>", callback_down)
    canvas.bind("<Button-5>", callback_up)
    return canvas

