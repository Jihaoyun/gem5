#!/usr/bin/env python

import argparse
from subprocess import call

import os, sys
sys.path.append(os.getcwd() + "/configs/fault_injector")

from FaultParser import *
from ControlFaultParser import *

from threading import Thread

parser = argparse.ArgumentParser(description='Gem5')
parser.add_argument('-b', '--benchmarks', type=str, dest='benchmarks',
                    required=True, nargs='+',
                    help='Benchmark set for the simulation')

parser.add_argument('-i', '--fault-input', type=str, dest='faultInput',
                    nargs='+', help='Fault source files')

parser.add_argument('-ci', '--control-fault-input', type=str,
                    dest='controlFaultInput', nargs='+',
                    help='Control fault source files')

parser.add_argument('-df', '--debug-flags', type=str,
                    dest='debugFlags',
                    help='Gem5 debug flags for debugging purpose')

parser.add_argument('-o', '--options', type=str, dest='options',
                    help='Options for the binary benchmark')

parser.add_argument('-mt', '--multithread', dest='multithread',
                    action='store_true',
                    help='Set it to enable multithreaded simulation')
parser.set_defaults(multithread=False)

args = parser.parse_args()

def startBPUFaultedSim(benchmark, fault):
    cmd = ["./build/ALPHA/gem5.opt",
        "--stats-file", statFolder + "/" +
        fault.label + ".txt",
        "configs/fault_injector/injector_system.py",
        "-fe",
        "-b", benchmark,
        "-l", fault.label,
        "-sb", fault.stuckBit,
        "-f", fault.field,
        "-e", fault.entry,
        "-fsb", fault.stuckBit,
        "-bp", fault.bitPosition,
        "-tb", fault.tickBegin,
        "-te", fault.tickEnd]

    if args.debugFlags is not None:
        cmd.insert(1, "--debug-flags=" + args.debugFlags)

    call(cmd)

def startBPUControlFaultedSim(statFolder, fname, benchmark, trigger, action):
    cmd = ["./build/ALPHA/gem5.opt",
        "--stats-file", statFolder + "/" +
        fname,
        "configs/fault_injector/injector_system.py",
        "-fe",
        "-b", benchmark,
        "-l", "control-fault_" + fname,
        "-cft", trigger,
        "-cfa", action ]

    if args.debugFlags is not None:
        cmd.insert(1, "--debug-flags=" + args.debugFlags)

    call(cmd)

if __name__ == '__main__':
    # Run a simulation for each specified benchmark program
    for benchmark in args.benchmarks:
        print "\n\nRunning " + benchmark + " GOLDEN\n"

        # Create a folder to store stats releated to the current benchmark
        statFolder = benchmark.split("/")[-1]
        if not os.path.exists("m5out/" + statFolder):
            os.makedirs("m5out/" + statFolder)

        if args.options != None:
            benchmark = " ".join([benchmark, args.options])

        # Run Golden simulation
        cmd = ["./build/ALPHA/gem5.opt",
            "--stats-file", statFolder + "/" +
            "GOLDEN.txt",
            "configs/fault_injector/injector_system.py",
            "-b", benchmark ]

        call(cmd)

        # Read all fault input files
        if args.faultInput is not None:
            for inputFile in args.faultInput:
                fp = FaultParser(inputFile)

                if args.multithread:
                    tpool = []

                while fp.hasNext():
                    # Load the next fault entry
                    fe = fp.next()

                    print "\n\nRunning " + benchmark + " with fault:\n" + \
                        str(fe)

                    if args.multithread:
                        # Run faulted simulation on an indipendent thread
                        t = Thread(target=startBPUFaultedSim,
                            args=(benchmark, fe))
                        tpool.append(t)
                        t.start()
                    else:
                        startBPUFaultedSim(benchmark, fe);

                # Join on the generated thread pool
                if args.multithread:
                    for t in tpool:
                        t.join()

        if args.controlFaultInput is not None:
            for inputFile in args.controlFaultInput:
                fname = inputFile.split("/")[-1]

                parser = ControlFaultParser()
                parser.parseFile(inputFile)

                print "\n\nRunning " + benchmark + " with fault:\n" + \
                    "control-fault@" + str(inputFile)

                if args.multithread:
                    # Run faulted simulation on an indipendent thread
                    t = Thread(target=startBPUControlFaultedSim,
                        args=(statFolder, fname, benchmark,
                            parser.getTrigger().strip(),
                            parser.getAction().strip()))
                    tpool.append(t)
                    t.start()
                else:
                    startBPUControlFaultedSim(statFolder, fname, benchmark,
                        parser.getTrigger().strip(),
                        parser.getAction().strip())

            # Join on the generated thread pool
            if args.multithread:
                for t in tpool:
                    t.join()
