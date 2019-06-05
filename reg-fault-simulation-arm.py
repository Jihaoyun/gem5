#! /usr/bin/env python

import argparse
from subprocess import call

import os, sys

sys.path.append(os.getcwd() + "/configs/lapo")
sys.path.append(os.getcwd() + "/configs/fault_injector")

from RegFaultParser import *

from threading import Thread, Semaphore

parser = argparse.ArgumentParser(description = 'Gem5')

parser.add_argument('-b', '--benchmarks', type = str, dest = 'benchmarks',
					required = True, nargs = '+',
					help = 'Benchmark set for the simulation')

parser.add_argument('-i', '--fault-input', type = str, dest = 'faultInput',
					required = True,
					help = 'The file of input faults.')

parser.add_argument('-df', '--debug-flags', type=str,
					dest='debugFlags',
					help='Gem5 debug flags for debugging purpose')

parser.add_argument('-o', '--options', type = str, dest = 'options',
					help = 'Options for binary benchmark.')

parser.add_argument('-out', '--output', type = str, dest = 'outputFile',
					help = 'The output file name.')

parser.add_argument('-mt', '--multithread', type = int, dest = 'multiThread',
					help = 'The number of threads when multithreading is enabled.')

parser.set_defaults(multiThread = None)

args = parser.parse_args()

# Validate arguments
if args.multiThread is not None:
	assert args.multiThread > 1, "Multithread should be greater than 1"

	# Define data structures needed for TLP
if args.multiThread:
	sem = Semaphore(args.multiThread)


def StartRegFaultSim(statFolder, benchmark, fault):
	cmd = ["./build/ARM/gem5.opt", 
		"--stats-file", statFolder + "/" + fault.label + ".txt",
		"configs/lapo/reg_fault_injector.py",
		"-fe",
		"-b", benchmark,
		"-l", fault.label,
		"-sb", fault.stuckBit,
		"-rfc", fault.regCategory,
		"-fr", fault.faultReg,
		"-rfbp", fault.bitPosition,
		"-tb", fault.tickBegin,
		"-te", fault.tickEnd ]

	if args.debugFlags is not None:
		cmd.insert(1, "--debug-flags=" + args.debugFlags)

	try:
		call(cmd)
	finally:
		if args.multiThread:
			sem.release()

if __name__ == '__main__':
	for benchmark in args.benchmarks:
		print "\n\nRunning " + benchmark + " GOLDEN\n"

		statFolder = benchmark.split("/")[-1] + "-regFault-ARM"
		outputFolder = "m5out/" + statFolder + "/"

		if not os.path.exists(outputFolder):
			os.makedirs(outputFolder)

		if args.options != None:
			benchmark = " ".join([benchmark, args.options])

		# Run Golden simulation
		benchmarkRun = benchmark
		if args.outputFile != None:
			benchmarkRun = benchmark.replace(args.outputFile,
				outputFolder + args.outputFile + "_GOLDEN.txt")

		cmd = ["./build/ARM/gem5.opt", 
			"--stats-file", statFolder + "/" + "GOLDEN" + ".txt",
			"configs/lapo/reg_fault_injector.py",
			"-b", benchmarkRun ]

		if args.debugFlags is not None:
			cmd.insert(1, "--debug-flags=" + args.debugFlags)

		try:
			call(cmd)
		finally:
			if args.faultInput is not None:
				for inputFile in args.faultInput.split(" "):
					fp = RegFaultParser(inputFile)

					if args.multiThread:
						tpool = []

					while fp.hasNext():

						fe = fp.next()

						benchmarkRun = benchmark
						if args.outputFile != None:
							benchmarkRun = benchmark.replace(args.outputFile,
								outputFolder + args.outputFile+"_"+fe.label+".txt")
						print "\n\nRunning " + benchmarkRun +\
							" with fault:\n" + str(fe)

						if args.multiThread:

							sem.acquire()

							t = Thread(target = StartRegFaultSim,
								args = (statFolder, benchmarkRun, fe))

							tpool.append(t)

							t.start()

						else:
							StartRegFaultSim(statFolder, benchmarkRun, fe)

					if args.multiThread:
						for t in tpool:
							t.join()




