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

parser.add_argument('-nf', '--nfields', dest='nfields', type=int,
                    help='The number of available fields')
parser.set_defaults(nfields=3)

parser.add_argument('-fl', '--fields-len', type=int,
                    dest='fieldsLen', nargs='+',
                    help='Length in bit of each field')
parser.set_defaults(fieldsLen=[32,20,1])

parser.add_argument('-hr', '--history-register', dest='hreg',
                    action='store_true',
                    help='Wether we should generate history\
                        register fault or not')
parser.set_defaults(hreg=False)

faultInstance = []

# Format is:
# LABEL: STACK@, FIELD, ENTRY_NUMBER, BIT_POSITION, START, END
faultstr = "FAULT_{}:{},{},{},{},{},{}"

# Generate permanent faults for BTB entries
def genereteBTBPermanents(numFields,fieldsLen):
    with open(args.bacFile, "r") as f:
        lines = f.readlines()
        nentry = 0
        for l in lines:
            if int(l) > 0:
                for field in range(0,numFields):
                    for bit in range(0,fieldsLen[field]):
                        for stuckBit in range(0,2):
                            lbl = "e" + str(nentry) + "_f" +\
                                str(field) + "_b" +\
                                str(bit) + "_s" +\
                                str(stuckBit)
                            faultInstance.append(
                                faultstr.format(lbl,
                                    stuckBit,
                                    field,
                                    nentry,
                                    bit,
                                    0,
                                    -1)
                            )
            nentry += 1

# Generate history registers permanent faults
def generateHistoryRegs(fieldLen=2):
    field = 3
    with open(args.bacFile, "r") as f:
        lines = f.readlines()
        nentry = 0
        for l in lines:
            if int(l) > 0:
                for n in range(0,16):
                    for bit in range(0,fieldLen):
                        for stuckBit in range(0,2):
                            lbl = "e" + str(nentry+(n*1024)) + "_f" +\
                                str(field) + "_b" +\
                                str(bit) + "_s" +\
                                str(stuckBit)
                            faultInstance.append(
                                faultstr.format(lbl,
                                    stuckBit,
                                    field,
                                    nentry+(n*1024),
                                    bit,
                                    0,
                                    -1)
                            )
            nentry += 1

args = parser.parse_args()

# Generate transient faults for BTB entries
def genereteBTBTransients():
    # TODO
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
        genereteBTBPermanents(args.nfields, args.fieldsLen)
        genereteBTBTransients()
        if args.hreg:
            generateHistoryRegs()

        writeout()
        print "Fault file generated successfully"
