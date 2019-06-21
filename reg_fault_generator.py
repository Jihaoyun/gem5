import re
import argparse
import random

parser = argparse.ArgumentParser(description = 'gem5 with fault injection')

parser.add_argument('-log', '--log-file', type = str, dest = 'logFile', nargs = '+',
					help = 'The input file of debug info of Gem5 simulator')
parser.add_argument('-out', '--output-fault', type = str, dest = 'faultFile',
					help = 'The output file of faults')
args = parser.parse_args()

intRegReadPatt = re.compile("[ \t]*Access to int register[ \t]*(.*), has data(.*)")
intRegWritePatt = re.compile("[ \t]*Setting int register[ \t]*(.*) to (.*)")

floatRegReadPatt = re.compile("[ \t]*Access to float register[ \t]*(.*) as int, has data(.*)")
floatRegWritePatt = re.compile("[ \t]*Setting float register[ \t]*(.*) to (.*)")

ccRegReadPatt = re.compile("[ \t]*Access to cc register[ \t]*(.*), has data(.*)")
ccRegWritePatt = re.compile("[ \t]*Setting cc register[ \t]*(.*) to (.*)")

digitPatt = re.compile("\d+")

intRegRead = {}
intRegWrite = {}
floatRegRead = {}
floatRegWrite = {}
ccRegRead = {}
ccRegWrite = {}

faultFile = open(args.faultFile, "w")

lineLabel = 0

for logFile in args.logFile:

	for line in open(logFile):
		currentLine = line.strip().split(":")
		if currentLine[1].strip() == "global" and \
			currentLine[2].strip() == "RegFile":

			if intRegReadPatt.match(currentLine[3]):
				intRegIndex = intRegReadPatt.match(currentLine[3]).group(1)
				if digitPatt.match(intRegIndex):
					if not (intRegIndex in intRegRead.keys() and eval(currentLine[0].strip()) < eval(intRegRead[intRegIndex])):
						intRegRead[intRegIndex] = currentLine[0].strip()

			elif intRegWritePatt.match(currentLine[3]):
				intRegIndex = intRegWritePatt.match(currentLine[3]).group(1)
				if digitPatt.match(intRegIndex):
					if not (intRegIndex in intRegWrite.keys() and eval(currentLine[0].strip()) < eval(intRegWrite[intRegIndex])):
						if intRegIndex in intRegRead.keys() and intRegIndex in intRegWrite.keys():
							if eval(intRegRead[intRegIndex]) > eval(intRegWrite[intRegIndex]):
								tickFault = int(round((eval(intRegRead[intRegIndex]) + eval(intRegWrite[intRegIndex])) / 2))
								faultFile.write("FAULT" + str(lineLabel) + ":1,0," + intRegIndex + "," + str(random.randint(0, 31)) + \
									"," + str(tickFault) + \
									"," + str(tickFault) + "\n")
								lineLabel = lineLabel + 1	
						intRegWrite[intRegIndex] = currentLine[0].strip()


			elif floatRegReadPatt.match(currentLine[3]):
				floatRegIndex = floatRegReadPatt.match(currentLine[3]).group(1)
				if digitPatt.match(floatRegIndex):
					if not (floatRegIndex in floatRegRead.keys() and eval(currentLine[0].strip()) < eval(floatRegRead[floatRegIndex])):
						floatRegRead[floatRegIndex] = currentLine[0].strip()

			elif floatRegWritePatt.match(currentLine[3]):
				floatRegIndex = floatRegWritePatt.match(currentLine[3]).group(1)
				if digitPatt.match(floatRegIndex):
					if not (floatRegIndex in floatRegWrite.keys() and eval(currentLine[0].strip()) < eval(floatRegWrite[floatRegIndex])):
						if floatRegIndex in floatRegRead.keys() and floatRegIndex in floatRegWrite.keys():
							if eval(floatRegRead[floatRegIndex]) > eval(floatRegWrite[floatRegIndex]):
								tickFault = int(round((eval(floatRegRead[floatRegIndex]) + eval(floatRegWrite[floatRegIndex])) / 2))
								faultFile.write("FAULT" + str(lineLabel) + ":1,1," + floatRegIndex + "," + str(random.randint(0, 31)) + \
									"," + str(tickFault) + \
									"," + str(tickFault) + "\n")
								lineLabel = lineLabel + 1	
						floatRegWrite[floatRegIndex] = currentLine[0].strip()


for key in intRegWrite.keys():
	if key in intRegRead.keys():
		tickFault = int(round((eval(intRegRead[key]) + eval(intRegWrite[key])) / 2))
		faultFile.write("FAULT" + str(lineLabel) + ":1,0," + key + "," + str(random.randint(0, 31)) + \
			"," + str(tickFault) + "," + str(tickFault) + "\n")
		lineLabel = lineLabel + 1

for key in floatRegWrite.keys():
	if key in floatRegRead.keys():
		tickFault = int(round((eval(floatRegRead[key]) + eval(floatRegWrite[key])) / 2))
		faultFile.write("FAULT" + str(lineLabel) + ":1,1," + key + "," + str(random.randint(0, 31)) + \
			"," + str(tickFault) + "," + str(tickFault) + "\n")
		lineLabel = lineLabel + 1

#for key in ccReg.keys():
	#faultFile.write("1,2," + key + ",0," + ccReg[key] + "\n")	

faultFile.close()
