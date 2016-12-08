from FaultParser import *

fp = FaultParser("faults.txt")

while fp.hasNext():
    print fp.next()
