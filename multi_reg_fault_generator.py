import re, math
import argparse
import random

parser = argparse.ArgumentParser(description = 'gem5 with fault injection')

parser.add_argument('-log', '--log-file', type = str, dest = 'logFile', nargs = '+',
					help = 'The input file of debug info of Gem5 simulator')
parser.add_argument('-out', '--output-fault', type = str, dest = 'faultFile',
					help = 'The output file of faults')
parser.add_argument('-t', '--end-time', type = str, dest = 'endTime',
					help = 'The simulation end time.')
parser.add_argument('-n', type = int, dest = 'num',
					help = 'Number of experiments for each register.')
parser.add_argument('-len', '--length-limit', type=int, dest = 'lengthLimit',
					help = 'Limit of intermittent faults in one simulation.')

args = parser.parse_args()

intRegReadPatt = re.compile("[ \t]*Reading int reg[ \t]*(.*)[ \t]*\(.*\) as (.*)")
intRegWritePatt = re.compile("[ \t]*Setting int reg[ \t]*(.*)[ \t]*\(.*\) to (.*)")

floatRegReadPatt = re.compile("[ \t]*Reading float reg[ \t]*(.*)[ \t]*\(.*\) as (.*)")
floatRegWritePatt = re.compile("[ \t]*Setting float reg[ \t]*(.*)[ \t]*\(.*\) to (.*)")

ccRegReadPatt = re.compile("[ \t]*Reading CC reg[ \t]*(.*)[ \t]*\(.*\) as (.*)")
ccRegWritePatt = re.compile("[ \t]*Setting CC reg[ \t]*(.*)[ \t]*\(.*\) to (.*)")

debugFlagPatt = re.compile("system\.cpu")

digitPatt = re.compile("\d+")

intRegRead = {}
intRegWrite = {}
floatRegRead = {}
floatRegWrite = {}
ccRegRead = {}
ccRegWrite = {}

intRegTickCand = {}
floatRegTickCand = {}
ccRegTickCand = {}
#if args.minInterval != '':
#	minimal_interval = eval(args.minInterval)
#else:	
#	minimal_interval = 1000
if args.endTime is None:
	endTime = float("inf")
else:
	endTime = args.endTime

faultFile = open(args.faultFile, "w")
#ccFaultFile = open(".".join(args.faultFile.split(".")[0:-1]) + "_ccReg." + args.faultFile.split(".")[-1], "w")

lineLabel = 0
ccRegLineLabel = 0

for logFile in args.logFile:

	for line in open(logFile):
		currentLine = line.strip().split(":")
		if debugFlagPatt.match(currentLine[1].strip()):

			if intRegReadPatt.match(currentLine[3]):
				intRegIndex = intRegReadPatt.match(currentLine[3]).group(1).strip()
				if digitPatt.match(intRegIndex):
					if not (intRegIndex in intRegRead.keys() and eval(currentLine[0].strip()) < eval(intRegRead[intRegIndex])):
						intRegRead[intRegIndex] = currentLine[0].strip()

			elif intRegWritePatt.match(currentLine[3]):
				intRegIndex = intRegWritePatt.match(currentLine[3]).group(1).strip()
				if digitPatt.match(intRegIndex):
					if not (intRegIndex in intRegWrite.keys() and eval(currentLine[0].strip()) < eval(intRegWrite[intRegIndex])):
						if intRegIndex in intRegRead.keys() and intRegIndex in intRegWrite.keys():
							if eval(intRegRead[intRegIndex]) > eval(intRegWrite[intRegIndex]) and eval(intRegRead[intRegIndex]) < (endTime):
								tickFault = int(round((eval(intRegRead[intRegIndex]) + eval(intRegWrite[intRegIndex])) / 2))

								if intRegIndex in intRegTickCand.keys():
									intRegTickCand[intRegIndex].append(tickFault)
								else:
									intRegTickCand[intRegIndex] = [tickFault]
								#tickEndFault = random.randint(eval(intRegRead[intRegIndex]), int(endTime))
								#faultFile.write("FAULT" + str(lineLabel) + ":" + str(random.randint(0,1)) + ",0," + intRegIndex + "," + str(random.randint(0, 31)) + \
								#	"," + str(tickFault) + \
								#	"," + str(tickEndFault) + "\n")
								#lineLabel = lineLabel + 1	
						intRegWrite[intRegIndex] = currentLine[0].strip()


			elif floatRegReadPatt.match(currentLine[3]):
				floatRegIndex = floatRegReadPatt.match(currentLine[3]).group(1).strip()
				if digitPatt.match(floatRegIndex):
					if not (floatRegIndex in floatRegRead.keys() and eval(currentLine[0].strip()) < eval(floatRegRead[floatRegIndex])):
						floatRegRead[floatRegIndex] = currentLine[0].strip()

			elif floatRegWritePatt.match(currentLine[3]):
				floatRegIndex = floatRegWritePatt.match(currentLine[3]).group(1).strip()
				if digitPatt.match(floatRegIndex):
					if not (floatRegIndex in floatRegWrite.keys() and eval(currentLine[0].strip()) < eval(floatRegWrite[floatRegIndex])):
						if floatRegIndex in floatRegRead.keys() and floatRegIndex in floatRegWrite.keys():
							if eval(floatRegRead[floatRegIndex]) > eval(floatRegWrite[floatRegIndex]) and eval(floatRegRead[floatRegIndex])< (endTime):
								tickFault = int(round((eval(floatRegRead[floatRegIndex]) + eval(floatRegWrite[floatRegIndex])) / 2))
								
								if floatRegIndex in floatRegTickCand.keys():
									floatRegTickCand[floatRegIndex].append(tickFault)
								else:
									floatRegTickCand[floatRegIndex] = [tickFault]
								#tickEndFault = random.randint(eval(floatRegRead[floatRegIndex]), int(endTime))
								#faultFile.write("FAULT" + str(lineLabel) + ":" + str(random.randint(0,1)) + ",1," + floatRegIndex + "," + str(random.randint(0, 31)) + \
								#	"," + str(tickFault) + \
								#	"," + str(tickEndFault) + "\n")
								#lineLabel = lineLabel + 1	
						floatRegWrite[floatRegIndex] = currentLine[0].strip()

			elif ccRegReadPatt.match(currentLine[3]):
				ccRegIndex = ccRegReadPatt.match(currentLine[3]).group(1).strip()
				if digitPatt.match(ccRegIndex):
					if not (ccRegIndex in ccRegRead.keys() and eval(currentLine[0].strip()) < eval(ccRegRead[ccRegIndex])):
						ccRegRead[ccRegIndex] = currentLine[0].strip()

			elif ccRegWritePatt.match(currentLine[3]):
				ccRegIndex = ccRegWritePatt.match(currentLine[3]).group(1).strip()
				if digitPatt.match(ccRegIndex):
					if not (ccRegIndex in ccRegWrite.keys() and eval(currentLine[0].strip()) < eval(ccRegWrite[ccRegIndex])):
						if ccRegIndex in ccRegRead.keys() and ccRegIndex in ccRegWrite.keys():
							if eval(ccRegRead[ccRegIndex]) > eval(ccRegWrite[ccRegIndex]) and eval(ccRegRead[ccRegIndex]) < (endTime):
								tickFault = int(round((eval(ccRegRead[ccRegIndex]) + eval(ccRegWrite[ccRegIndex])) / 2))
								
								if ccRegIndex in ccRegTickCand.keys():
									ccRegTickCand[ccRegIndex].append(tickFault)
								else:
									ccRegTickCand[ccRegIndex] = [tickFault]
								#tickEndFault = random.randint(eval(ccRegRead[ccRegIndex]), int(endTime))
#								faultFile.write("FAULT" + str(ccRegLineLabel) + ":1,2," + ccRegIndex + "," + str(0) + \
#									"," + str(tickFault) + \
#									"," + str(tickEndFault) + "\n")
								#ccRegLineLabel = ccRegLineLabel + 1	
						ccRegWrite[ccRegIndex] = currentLine[0].strip()


for key in intRegWrite.keys():
	if key in intRegRead.keys():
		if eval(intRegRead[key]) > eval(intRegWrite[key]) and eval(intRegRead[key]) < (endTime):
			tickFault = int(round((eval(intRegRead[key]) + eval(intRegWrite[key])) / 2))
			if key in intRegTickCand.keys():
				intRegTickCand[key].append(tickFault)
			else:
				intRegTickCand[key] = [tickFault]
			#tickEndFault = random.randint(eval(intRegRead[key]), int(endTime))
			#faultFile.write("FAULT" + str(lineLabel) + ":" + str(random.randint(0,1)) + ",0," + key + "," + str(random.randint(0, 31)) + \
			#	"," + str(tickFault) + "," + str(tickEndFault) + "\n")
			#lineLabel = lineLabel + 1

for key in floatRegWrite.keys():
	if key in floatRegRead.keys():
		if eval(floatRegRead[key]) > eval(floatRegWrite[key]) and eval(floatRegRead[key]) < (endTime):
			tickFault = int(round((eval(floatRegRead[key]) + eval(floatRegWrite[key])) / 2))
			if key in floatRegTickCand.keys():
				floatRegTickCand[key].append(tickFault)
			else:
				floatRegTickCand[key] = [tickFault]
			#tickEndFault = random.randint(eval(floatRegRead[key]), int(endTime))
			#faultFile.write("FAULT" + str(lineLabel) + ":" + str(random.randint(0,1)) + ",1," + key + "," + str(random.randint(0, 31)) + \
			#	"," + str(tickFault) + "," + str(tickEndFault) + "\n")
			#lineLabel = lineLabel + 1

for key in ccRegWrite.keys():
	if key in ccRegRead.keys():
		if eval(ccRegRead[key]) > eval(ccRegWrite[key]) and eval(ccRegRead[key]) < (endTime):
			tickFault = int(round((eval(ccRegRead[key]) + eval(ccRegWrite[key])) / 2))
			if key in ccRegTickCand.keys():
				ccRegTickCand[key].append(tickFault)
			else:
				ccRegTickCand[key] = [tickFault]
			#tickEndFault = random.randint(eval(ccRegRead[key]), int(endTime))
#			faultFile.write("FAULT" + str(ccRegLineLabel) + ":1,2," + key + "," + str(0) + \
#				"," + str(tickFault) + "," + str(tickEndFault) + "\n")
			#ccRegLineLabel = ccRegLineLabel + 1

#for key in ccReg.keys():
	#faultFile.write("1,2," + key + ",0," + ccReg[key] + "\n")	
for key in intRegTickCand.keys():
	if (len(intRegTickCand[key]) == 1 and args.endTime is not None):
		intRegTickCand[key].append(int(endTime))
	numCand = len(intRegTickCand[key])
	numExp = numCand * (numCand - 1) / 2;
	if (numCand >= 4):
		numExp = numExp + int(numCand * (numCand - 1) * (numCand - 2) * (numCand - 3) / 24);
	numExp = int(args.num) if (numExp > int(args.num)) else numExp;
	for i in range(numExp):
		faultNum = random.randint(1, math.floor(numCand / 2))
		faultNum = int(args.lengthLimit) if (faultNum > int(args.lengthLimit)) else faultNum
		opNum = faultNum * 2
		tmpArray = intRegTickCand[key][:opNum]
		for j in range(opNum, numCand):
			randIdx = random.randint(1, j)
			if (randIdx < opNum):
				tmpArray.pop(randIdx)
				tmpArray.append(intRegTickCand[key][j])
		faultFile.write("FAULT" + str(lineLabel) + ":" + str(random.randint(0,1)) + ",0," + key + "," + str(random.randint(0, 31)) + "," +\
									",".join(str(elem) for elem in tmpArray) + "\n")
		lineLabel = lineLabel + 1	

faultFile.close()
#ccFaultFile.close()