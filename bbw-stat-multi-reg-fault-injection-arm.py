#! /usr/bin/env python

import argparse
from subprocess import call

import os, sys

import re

sys.path.append(os.getcwd() + "/configs/lapo")
sys.path.append(os.getcwd() + "/configs/fault_injector")

from MultiRegFaultParser import *

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
		"--stats-file", statFolder + "/" + fault.label + "/" + fault.label + ".txt",
		"configs/lapo/multi_reg_fault_injector_o3.py",
		"-fe",
		"-b", benchmark,
		"-l", fault.label,
		"-sb", " ".join([str(elem) for elem in fault.stuckBitList]),
		"-rfc", " ".join([str(elem) for elem in fault.regCategoryList]),
		"-fr", " ".join([str(elem) for elem in fault.faultRegList]),
		"-rfbp", " ".join([str(elem) for elem in fault.bitPositionList]),
		"-t", " ".join([str(elem) for elem in fault.tickList]),
		"-op", " ".join([str(elem) for elem in fault.operationList])]

	cmd.insert(1, "--debug-file=" + statFolder + "/" + fault.label + "/" +\
		fault.label + ".log")

	#time = int(round(eval(re.match("FAULT(.*)", fault.label).group(1)) / 100000))

	#cmd.insert(1, "--debug-start=" + str(500000000))
	cmd.insert(1, "--debug-end=" + str(20000000))

	if args.debugFlags is not None:
		cmd.insert(1, "--debug-flags=" + args.debugFlags)
	else:
		#cmd.insert(1, "--debug-flags=" + "DataCommMonitor")
		cmd.insert(1, "--debug-flags=" + "Registers,O3Registers,PseudoInst")

	#if eval(re.match("FAULT(.*)", fault.label).group(1)) % 20000 == 12:
	try:
		f = open("m5out/" + statFolder + "/" + fault.label + "/debug.log", "w")
		call(cmd, stdout = f, stderr = f)
		f.close()
	finally:
		if args.multiThread:
			sem.release()

if __name__ == '__main__':
	for benchmark in args.benchmarks:
		print "\n\nRunning " + benchmark + " GOLDEN\n"

		statFolder = benchmark.split("/")[-1] + "-multiRegFault-ARM-stat"
		outputFolder = "m5out/" + statFolder + "/" + "GOLDEN/"

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
			"--stats-file", statFolder + "/GOLDEN/" + "GOLDEN" + ".txt",
			"configs/lapo/multi_reg_fault_injector_o3.py",
			"-b", benchmarkRun ]

		cmd.insert(1, "--debug-file=" + statFolder + "/" + "GOLDEN" + "/" +\
			"GOLDEN" + ".log")

		#cmd.insert(1, "--debug-start=500000000")
		#cmd.insert(1, "--debug-end=1000000000")

		if args.debugFlags is not None:
			cmd.insert(1, "--debug-flags=" + args.debugFlags)
		else:
			cmd.insert(1, "--debug-flags=" + "Registers,O3Registers,PseudoInst")
			#cmd.insert(1, "--debug-flags=" + "DataCommMonitor")

		try:
			call(cmd)
		finally:
			if args.faultInput is not None:
				for inputFile in args.faultInput.split(" "):
					fp = MultiRegFaultParser(inputFile)
					if args.multiThread:
						tpool = []
					while fp.hasNext():
						fe = fp.next()
						if fe is not None:
							outputFolder = "m5out/" + statFolder + "/"  + fe.label + "/"
							#if eval(re.match("FAULT(.*)", fe.label).group(1)) % 20000 == 12:
							if not os.path.exists(outputFolder):
								os.makedirs(outputFolder)
							#benchmarkRun = benchmark + " " + outputFolder + "simData.dat" + " " + outputFolder + "checkData.dat"
							benchmarkRun = benchmark
							if args.outputFile != None:
								benchmarkRun = benchmark.replace(args.outputFile,
									outputFolder + args.outputFile+"_"+fe.label+".txt")
							#if eval(re.match("FAULT(.*)", fe.label).group(1)) % 20000 == 12:	
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

					fe = fp.getLastFaultEntry()
					outputFolder = "m5out/" + statFolder + "/"  + fe.label + "/"
					#if eval(re.match("FAULT(.*)", fe.label).group(1)) % 20000 == 12:
					if not os.path.exists(outputFolder):
						os.makedirs(outputFolder)
					#benchmarkRun = benchmark + " " + outputFolder + "simData.dat" + " " + outputFolder + "checkData.dat"
					benchmarkRun = benchmark
					if args.outputFile != None:
						benchmarkRun = benchmark.replace(args.outputFile,
							outputFolder + args.outputFile+"_"+fe.label+".txt")
					#if eval(re.match("FAULT(.*)", fe.label).group(1)) % 20000 == 12:	
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




