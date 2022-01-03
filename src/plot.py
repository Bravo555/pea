#!/usr/bin/python3

import sys
import matplotlib
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    sys.exit(0)

filename = sys.argv[1]

data = list(map(int, open(filename, 'r').read().splitlines()))

plt.plot(data)
plt.show()
