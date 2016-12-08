#!/usr/bin/env python

import argparse
from subprocess import call

import os, sys
sys.path.append(os.getcwd() + "/configs/fault_injector")

from FaultParser import *

parser = argparse.ArgumentParser(description='Gem5')
parser.add_argument('-b', '--benchmarks', type=str, dest='benchmarks',
                    nargs='+', help='Benchmark set for the simulation')

parser.add_argument('-i', '--fault-input', type=str, dest='faultInput',
                    nargs='+', help='Fault source files')

args = parser.parse_args()

for benchmark in args.benchmarks:
    print "\n\nRunning " + benchmark + " GOLDEN\n"
    call(["./build/ALPHA/gem5.opt",
        "--stats-file", benchmark.split("/")[-1] + "_" +
        "GOLDEN.txt",
        "configs/fault_injector/injector_system.py",
        "-b", benchmark ])

    for inputFile in args.faultInput:
        fp = FaultParser(inputFile)
        while fp.hasNext():
            fe = fp.next()
            print "\n\nRunning " + benchmark + " with fault:\n" + str(fe)
            call(["./build/ALPHA/gem5.opt",
                "--stats-file", benchmark.split("/")[-1] + "_" +
                fe.label + ".txt",
                "configs/fault_injector/injector_system.py",
                "-fe",
                "-b", benchmark,
                "-l", fe.label,
                "-sb", fe.stuckBit,
                "-f", fe.field,
                "-e", fe.entry,
                "-bp", fe.bitPosition,
                "-tb", fe.tickBegin,
                "-te", fe.tickEnd])
