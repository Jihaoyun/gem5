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

parser.add_argument('-o', '--options', type=str, dest='options',
                    nargs='+', help='Options for the binary benchmark')

args = parser.parse_args()

# Run a simulation for each specified benchmark program
for benchmark in args.benchmarks:
    print "\n\nRunning " + benchmark + " GOLDEN\n"

    # Create a folder to store stats releated to the current benchmark
    statFolder = benchmark.split("/")[-1]
    if not os.path.exists("m5out/" + statFolder):
        os.makedirs("m5out/" + statFolder)

    # Run Golden simulation
    cmd = ["./build/ALPHA/gem5.opt",
        "--stats-file", statFolder + "/" +
        "GOLDEN.txt",
        "configs/fault_injector/injector_system.py",
        "-b", benchmark ]
    if args.options != None:
        cmd.append("--options")
        cmd.append(args.options)

    call(cmd)

    # Read all fault input files
    if args.faultInput == None:
        break

    for inputFile in args.faultInput:
        fp = FaultParser(inputFile)
        while fp.hasNext():
            # Load the next fault entry
            fe = fp.next()

            print "\n\nRunning " + benchmark + " with fault:\n" + str(fe)

            # Run faulted simulation
            cmd = ["./build/ALPHA/gem5.opt",
                "--stats-file", statFolder + "/" +
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
                "-te", fe.tickEnd]
            if args.options != None:
                cmd.append("--options")
                cmd.append(args.options)
            call(cmd)
