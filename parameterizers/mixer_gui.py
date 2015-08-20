import time
import sys

import windows
import mix


if __name__ == '__main__':

    #mix.combine_tkapps([lambda master: windows.StateGUI(master, 4, 2),
    #                    lambda master: windows.Sequencer(master, 3)])

    # sequencer = mix.GUIThread(lambda master: windows.Sequencer(master, 4))
    # sequencer.start()
    # time.sleep(0.1)

    # for i in range(1000000):

    #     time.sleep(0.05)
    #     sys.stdout.write(' '.join(map(str, sequencer.frame.sample(i % 64))))
    #     sys.stdout.write('\n')
    #     sys.stdout.flush()

    apps = mix.start_tkapps(
            [lambda master: windows.SmoothSequencer(
                master, 500, 300, 200) for i in range(5)])

    x = 0
    while True:
        time.sleep(0.05)
        x = (x + 0.01) % 1

        sys.stdout.write(' '.join([str(app.sample(x)) for app in apps]))
        sys.stdout.write('\n')
        sys.stdout.flush()

