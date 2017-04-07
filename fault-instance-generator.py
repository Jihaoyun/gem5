#!/usr/bin/env python

import argparse

from random import randint

parser = argparse.ArgumentParser(description='Gem5 Fault Instance Generator')

parser.add_argument('-b', '--btb-access-count', type=str, dest='bacFile',
                    help='File containing BTB access references')
parser.set_defaults(bacFile="m5out/btb-access-count.txt")

parser.add_argument('-o', '--output', type=str, dest='outputFile',
                    help='Where to write the generated fault instance')
parser.set_defaults(outputFile="configs/fault_injector/faults.txt")

faultInstance = []

# Generate permanent faults for BTB entries
def genereteBTBPermanents(numFields=2):
    # Format is:
    # LABEL: STACK@, FIELD, ENTRY_NUMBER, BIT_POSITION, START, END
    faultstr = "FAULT{}:{},{},{},{},{},{}"
    with open(args.bacFile, "r") as f:
        idx = 0
        lines = f.readlines()
        nentries = len(lines)
        for l in lines:
            if int(l) > 0:
                for field in range(0,numFields):
                    faultInstance.append(
                        faultstr.format(idx,
                            randint(0,1),
                            field,
                            randint(0,nentries-1),
                            randint(0,31),
                            0,
                            -1)
                    )
                    idx += 1 # increment entry index

args = parser.parse_args()

# Generate transient faults for BTB entries
def genereteBTBTransients():
    return

# Write the generated fault instances to a file
def writeout():
    with open(args.outputFile, "w") as f:
        for fi in faultInstance:
            f.write(fi)
            f.write("\n")

if __name__ == "__main__":
    # Generate BTB faults
    if args.bacFile is not None:
        genereteBTBPermanents()
        genereteBTBTransients()

        writeout()
        print faultInstance
