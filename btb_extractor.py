import re
import argparse

parser = argparse.ArgumentParser(description = 'gem5 with fault injection')

parser.add_argument('-log', '--log-file', type = str, dest = 'logFile', nargs = '+',
					help = 'The input file of debug info of Gem5 simulator')
parser.add_argument('-out', '--output-fault', type = str, dest = 'faultFile',
					help = 'The output file of faults')
args = parser.parse_args()

btbPatt = re.compile("[ \t]*Looking up BTB entry[ \t]*(.*),[ \t]*BTB tag(.*)")
digitPatt = re.compile("0x\d+")

btb = {}

faultFile = open(args.faultFile, "w")

for logFile in args.logFile:

	for line in open(logFile):
		currentLine = line.strip().split(":")
		if currentLine[1].strip() == "global" and \
			currentLine[2].strip() == "BTB":

			if btbPatt.match(currentLine[3]):
				btbIndex = btbPatt.match(currentLine[3]).group(1)
				if digitPatt.match(btbIndex):
					if not (btbIndex in btb.keys() and eval(currentLine[0].strip()) < eval(btb[btbIndex])):
						btb[btbIndex] = currentLine[0].strip()

for key in btb.keys():
	faultFile.write("1,0," + str(eval(key)) + ",0," + btb[key] + "\n")

for key in btb.keys():
	faultFile.write("1,1," + str(eval(key)) + ",0," + btb[key] + "\n")

for key in btb.keys():
	faultFile.write("1,2," + str(eval(key)) + ",0," + btb[key] + "\n")	

faultFile.close()
