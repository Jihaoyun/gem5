#!/usr/bin/env python

import argparse
from subprocess import call

import os, sys
sys.path.append(os.getcwd() + "/configs/fault_injector")

from FaultParser import *
from ControlFaultParser import *

from threading import Thread, Semaphore

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

parser.add_argument('-out', '--output', type=str, dest='outputFile',
                    help='Benchmark produced output file name')

parser.add_argument('-mt', '--multithread', dest='multithread', type=int,
                    help='When it is set multithreading is enabled on the\
                        specified number of threads')
parser.set_defaults(multithread=None)

args = parser.parse_args()

# Validate arguments
if args.multithread is not None:
    assert args.multithread > 1, "Multithread should be greater than 1"

# Define data structures needed for TLP
if args.multithread:
    sem = Semaphore(args.multithread)

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

    try:
        call(cmd)
    finally:
        # Notify thread completition
        if args.multithread:
            sem.release()

def startBPUControlFaultedSim(statFolder, fname, benchmark, trigger, action):
    # Acquire rights to execute in multithreading context
    if args.multithread:
        sem.acquire()

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

    try:
        call(cmd)
    finally:
        # Notify thread completition
        if args.multithread:
            sem.release()

if __name__ == '__main__':
    # Run a simulation for each specified benchmark program
    for benchmark in args.benchmarks:
        print "\n\nRunning " + benchmark + " GOLDEN\n"

        # Create a folder to store stats releated to the current benchmark
        statFolder = benchmark.split("/")[-1]
        outputFolder = "m5out/" + statFolder + "/"
        if not os.path.exists(outputFolder):
            os.makedirs(outputFolder)

        if args.options != None:
            benchmark = " ".join([benchmark, args.options])

        # Run Golden simulation
        benchmarkRun = benchmark.replace(args.outputFile,
            outputFolder + args.outputFile + "_GOLDEN.txt")

        cmd = ["./build/ALPHA/gem5.opt",
            "--stats-file", statFolder + "/" +
            "GOLDEN.txt",
            "configs/fault_injector/injector_system.py",
            "-b", benchmarkRun ]

        if args.debugFlags is not None:
            cmd.insert(1, "--debug-flags=" + args.debugFlags)

        try:
            call(cmd)
        finally:
            # If GOLDEN execution succeded...
            # Read all fault input files
            if args.faultInput is not None:
                for inputFile in args.faultInput:
                    fp = FaultParser(inputFile)

                    if args.multithread:
                        tpool = []

                    while fp.hasNext():
                        # Load the next fault entry
                        fe = fp.next()

                        benchmarkRun = benchmark.replace(args.outputFile,
                            outputFolder + args.outputFile+"_"+fe.label+".txt")
                        print "\n\nRunning " + benchmarkRun +\
                            " with fault:\n" + str(fe)

                        if args.multithread:
                            # Acquire rights to execute in multithreading context
                            if args.multithread:
                                sem.acquire()
                            # Run faulted simulation on an indipendent thread
                            t = Thread(target=startBPUFaultedSim,
                                args=(benchmarkRun, fe))
                            tpool.append(t)
                            t.start()
                        else:
                            startBPUFaultedSim(benchmarkRun, fe);

                    # Join on the generated thread pool
                    if args.multithread:
                        for t in tpool:
                            t.join()

            if args.controlFaultInput is not None:
                for inputFile in args.controlFaultInput:
                    fname = inputFile.split("/")[-1].replace(".txt","")
                    parser = ControlFaultParser(inputFile)
                    c = 0

                    if args.multithread:
                        tpool = []

                    while parser.hasNext():
                        cfault = parser.next()

                        benchmarkRun = benchmark.replace(args.outputFile,
                            outputFolder + args.outputFile+"_"+str(c)+".txt")
                        fnameRun = fname + str(c) + ".txt"
                        print "\n\nRunning " + benchmarkRun +\
                            " with fault:\n" + "control-fault@" +\
                            str(inputFile)

                        if args.multithread:
                            # Run faulted simulation on an indipendent thread
                            t = Thread(target=startBPUControlFaultedSim,
                                args=(statFolder, fnameRun, benchmarkRun,
                                    cfault.trigger,
                                    cfault.action))
                            tpool.append(t)
                            t.start()
                        else:
                            startBPUControlFaultedSim(statFolder,
                                fnameRun,
                                benchmarkRun,
                                cfault.trigger,
                                cfault.action)
                        c = c + 1

                # Join on the generated thread pool
                if args.multithread:
                    for t in tpool:
                        t.join()
