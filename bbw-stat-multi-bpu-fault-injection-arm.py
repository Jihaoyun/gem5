#!/usr/bin/env python

import re

import argparse
from subprocess import call

import os, sys
sys.path.append(os.getcwd() + "/configs/fault_injector")

from MultiBpuFaultParser import *
from ControlFaultParser import *

from threading import Thread, Semaphore

parser = argparse.ArgumentParser(description='Gem5')
parser.add_argument('-b', '--benchmarks', type=str, dest='benchmarks',
                    required=True, nargs='+',
                    help='Benchmark set for the simulation')

parser.add_argument('-i', '--fault-input', type=str, dest='faultInput',
                    nargs='+', help='Fault source files')

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
    cmd = ["./build/ARM/gem5.opt",
        "--stats-file", statFolder + "/" + fault.label + "/" + 
        fault.label + ".txt",
        "configs/fault_injector/multi_bpu_fault_injector_system.py",
        "-fe",
        "-b", benchmark,
        "-l", fault.label,
        "-sb", " ".join([str(elem) for elem in fault.stuckBitList]),
        "-f", " ".join([str(elem) for elem in fault.fieldList]),
        "-e", " ".join([str(elem) for elem in fault.entryList]),
        "-bp", " ".join([str(elem) for elem in fault.bitPositionList]),
        "-t", " ".join([str(elem) for elem in fault.tickList]),
        "-op", " ".join([str(elem) for elem in fault.operationList])]

    cmd.insert(1, "--debug-file=" + statFolder + "/" + fault.label + "/" +\
        fault.label + ".log")

    #cmd.insert(1, "--debug-start=50000000")
    cmd.insert(1, "--debug-end=10000000")

    if args.debugFlags is not None:
        cmd.insert(1, "--debug-flags=" + args.debugFlags)
    else:
        cmd.insert(1, "--debug-flags=" + "Fetch,PseudoInst")

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
        statFolder = benchmark.split("/")[-1] + "-multiBpuFault-ARM-stat"
        outputFolder = "m5out/" + statFolder + "/"  + "GOLDEN/"
        if not os.path.exists(outputFolder):
            os.makedirs(outputFolder)

        if args.options != None:
            benchmark = " ".join([benchmark, args.options])

        # Run Golden simulation
        #benchmarkRun = benchmark + " " + outputFolder + "simData.dat" + " " + outputFolder + "checkData.dat"
        benchmarkRun = benchmark + " " + benchmark
        if args.outputFile != None:
            benchmarkRun = benchmark.replace(args.outputFile,
                outputFolder + args.outputFile + "_GOLDEN.txt")

        cmd = ["./build/ARM/gem5.opt",
            "--stats-file", statFolder + "/GOLDEN/" +
            "GOLDEN.txt",
            "configs/fault_injector/multi_bpu_fault_injector_system.py",
            "-b", benchmarkRun]

        cmd.insert(1, "--debug-file=" + statFolder + "/" + "GOLDEN" + "/" +\
            "GOLDEN" + ".log")

        cmd.insert(1, "--debug-start=3000000000")
        cmd.insert(1, "--debug-end=3500000000")

        if args.debugFlags is not None:
            cmd.insert(1, "--debug-flags=" + args.debugFlags)
        #else:
            #cmd.insert(1, "--debug-flags=" + "Fetch,PseudoInst")

        try:
            call(cmd)
        finally:
            # If GOLDEN execution succeded...
            # Read all fault input files
            if args.faultInput is not None:
                for inputFile in args.faultInput:
                    fp = MultiBpuFaultParser(inputFile)

                    if args.multithread:
                        tpool = []

                    while fp.hasNext():
                        # Load the next fault entry
                        fe = fp.next()

                        if fe is not None:
                            outputFolder = "m5out/" + statFolder + "/"  + fe.label + "/"
                            if not os.path.exists(outputFolder):
                                os.makedirs(outputFolder)   

                            #benchmarkRun = benchmark + " " + outputFolder + "simData.dat" + " " + outputFolder + "checkData.dat"
                            benchmarkRun = benchmark
                            if args.outputFile != None:
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

                    fe = fp.getLastFaultEntry()
                    outputFolder = "m5out/" + statFolder + "/"  + fe.label + "/"
                    if not os.path.exists(outputFolder):
                        os.makedirs(outputFolder)   

                    #benchmarkRun = benchmark + " " + outputFolder + "simData.dat" + " " + outputFolder + "checkData.dat"
                    benchmarkRun = benchmark
                    if args.outputFile != None:
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