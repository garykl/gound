import time
import sys
import numpy as np

import goundface.windows as windows
import goundface.launch as launch


def periodic_interpolator():

    num = int(sys.argv[1])
    apps = launch.start_tkapps(
            [lambda master: windows.SmoothSequencer(
                master, 500, 100, 100) for i in range(num)])

    speed = np.array([0.02, 0.03, 0.01, 0.02, 0.01])
    position = np.array([0, 0, 0, 0, 0.0])

    while True:
        time.sleep(0.05)

        sys.stdout.write(' '.join([str(app.sample(x))
            for (x, app) in zip(position, apps)]))
        sys.stdout.write('\n')
        sys.stdout.flush()

        position = (position + speed) % 1



if __name__ == '__main__':

    periodic_interpolator()
    # launch.combine_tkapps([
    #     lambda master: windows.StateGUI(master, 4, 2),
    #     lambda master: windows.Sequencer(master, 3)])

    # sequencer = launch.GUIThread(lambda master: windows.Sequencer(master, 4))
    # sequencer.start()
    # time.sleep(0.1)

    # for i in range(1000000):

    #     time.sleep(0.05)
    #     sys.stdout.write(' '.join(map(str, sequencer.frame.sample(i % 64))))
    #     sys.stdout.write('\n')
    #     sys.stdout.flush()

    # sys.exit()

