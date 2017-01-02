#!/usr/bin/env python
import numpy as np
import sys

m = int(sys.argv[1])
n = int(sys.argv[2])
r = int(sys.argv[3])

mat = np.random.randint(r, size=(m, n))
print str(m) + " " + str(n)
for line in mat:
    for num in line:
        print "%d " % (num),
    print ""

print
mat = np.random.randint(r, size=(m, n))
print str(m) + " " + str(n)
for line in mat:
    for num in line:
        print "%d " % (num),
    print ""
