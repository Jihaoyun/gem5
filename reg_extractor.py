import re
import argparse

parser = argparse.ArgumentParser(description = 'gem5 with fault injection')

parser.add_argument('-log', '--log-file', type = str, dest = 'logFile', nargs = '+',
					help = 'The input file of debug info of Gem5 simulator')
parser.add_argument('-out', '--output-fault', type = str, dest = 'faultFile',
					help = 'The output file of faults')
args = parser.parse_args()

intRegPatt = re.compile("[ \t]*Access to int register[ \t]*(.*), has data(.*)")
floatRegPatt = re.compile("[ \t]*Access to float register[ \t]*(.*) as int, has data(.*)")
ccRegPatt = re.compile("[ \t]*Access to cc register[ \t]*(.*), has data(.*)")
digitPatt = re.compile("\d+")

intReg = {}
floatReg = {}
ccReg = {}

faultFile = open(args.faultFile, "w")

for logFile in args.logFile:

	for line in open(logFile):
		currentLine = line.strip().split(":")
		if currentLine[1].strip() == "global" and \
			currentLine[2].strip() == "RegFile":

			if intRegPatt.match(currentLine[3]):
				intRegIndex = intRegPatt.match(currentLine[3]).group(1)
				if digitPatt.match(intRegIndex):
					if not (intRegIndex in intReg.keys() and eval(currentLine[0].strip()) < eval(intReg[intRegIndex])):
						intReg[intRegIndex] = currentLine[0].strip()

			elif floatRegPatt.match(currentLine[3]):
				floatRegIndex = floatRegPatt.match(currentLine[3]).group(1)
				if digitPatt.match(floatRegIndex):
					if not (floatRegIndex in floatReg.keys() and eval(currentLine[0].strip()) < eval(floatReg[floatRegIndex])):
						floatReg[floatRegIndex] = currentLine[0].strip()

			elif ccRegPatt.match(currentLine[3]):
				ccRegIndex = ccRegPatt.match(currentLine[3]).group(1)
				if digitPatt.match(ccRegIndex):
					if not (ccRegIndex in ccReg.keys() and eval(currentLine[0].strip()) < eval(ccReg[ccRegIndex])):
						ccReg[ccRegIndex] = currentLine[0].strip()



for key in intReg.keys():
	faultFile.write("1,0," + key + ",0," + intReg[key] + "\n")

for key in floatReg.keys():
	faultFile.write("1,1," + key + ",0," + floatReg[key] + "\n")

for key in ccReg.keys():
	faultFile.write("1,2," + key + ",0," + ccReg[key] + "\n")	

faultFile.close()
