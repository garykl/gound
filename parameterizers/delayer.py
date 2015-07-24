import sys
import time

for line in sys.stdin:

    #sys.stdout.write(line)
    for h in line.split()[1:]:
        sys.stdout.write(h + ' ')
        sys.stdout.write('\n')
    sys.stdout.flush()
    time.sleep(1 / 12)
