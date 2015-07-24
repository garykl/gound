import tkinter as tk
from threading import Thread

import colors


class GUIThread(Thread):

    def __init__(self, frame_thunk):
        Thread.__init__(self)
        self.frame_thunk = frame_thunk

    def run(self):
        self.master = tk.Tk()
        self.master.tk_setPalette(colors.triple_2_rgb((0, 0, 0)))
        self.frame = self.frame_thunk(self.master)
        self.master.mainloop()


class InputRemember(object):

    def __init__(self, inputcallback):
        self.inputcallback = inputcallback
        self.values = []

    def __call__(self):
        self.values = self.inputcallback()
        return self

    def show(self):
        return ' '.join([str(v) for v in self.values])



def tkapp_pipe(tkapp):

    app = GUIThread(tkapp)
    app.start()

    inputter = InputRemember(
            lambda: [float(l) for l in sys.stdin.readline().split()])

    def show_state():
        print('{0} {1}'.format(inputter.show(), app.frame.show()))

    time.sleep(1)
    app.frame.callback = show_state

    while True:
        inputter()
        show_state()


def combine_tkapps(tkapps):
    import time

    apps = []
    for tkapp in tkapps:
        app = GUIThread(tkapp)
        app.start()
        apps.append(app)
        time.sleep(0.5)

    def show_state():
        print(' '.join([app.frame.show() for app in apps]))

    for app in apps:
        app.frame.callback = show_state

