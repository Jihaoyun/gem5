#!/usr/bin/env python
from sys import argv
import sys
import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

if len(argv) < 2:
    sys.exit("Usage: " + argv[0] + "<input_file>")

with open(argv[1]) as f:
    count = map(int, f.read().splitlines())

entry = range(0, len(count))

y_pos = np.arange(len(entry))

plt.bar(y_pos, count, align='center')
plt.xticks(y_pos, entry)

plt.grid(True)
plt.xlabel('Entry')
plt.ylabel('Access count')
plt.title('BTB access histogram')
plt.show()
