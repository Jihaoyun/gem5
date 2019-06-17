import os
import argparse

parser = argparse.ArgumentParser(description = 'gem5 with fault injection')

parser.add_argument('-log', '--log-file', type = str, dest = 'logFile',
					help = 'The input file of debug info of Gem5 simulator')
parser.add_argument('-in', '--input-fault', type = str, dest = 'faultFile',
					help = 'The input file of faults')
parser.add_argument('-out', '--output-fault', type = str, dest = 'newFaultFile',
					help = 'The output file of faults')
args = parser.parse_args()

class FaultEntry:
	def __init__(self, stuckBit, category, reg, bitPosition, tick):
		self.stuckBit = stuckBit
		self.category = category
		self.reg = reg
		self.bitPosition = bitPosition
		self.tick = tick

class RegFaultGenerator:

	def __init__(self, filename):
		try:
			self.file = open(filename, "r")
		except IOError:
			raise

	def setFault(self, stuckBit, category, reg, bitPosition, tick):
		self.fault = FaultEntry(stuckBit, category, reg, bitPosition, tick)

	def haveNext(self):
		self.nextLine = self.file.readline().strip()
		if self.nextLine == "":
			return False
		return True

	def next(self):
		currentLine = self.nextLine.replace(" ","").split(":")

		if currentLine[2] == "PseudoInst" and currentLine[4] == "rpns()":
			if eval(currentLine[0]) < eval(self.fault.tick):
				faultLine = ",".join([self.fault.stuckBit, self.fault.category, self.fault.reg,\
					self.fault.bitPosition, currentLine[0], currentLine[0]])
				return faultLine

		return ""

class FaultParser:

	def __init__(self, filename):
		try:
			self.file = open(filename, "r")
		except IOError:
			raise

	def haveNext(self):
		self.nextLine = self.file.readline().strip()
		if self.nextLine == "":
			return False

		if self.nextLine[0] == '#':
			return False

		return True

	def next(self):
		currentLine = self.nextLine.replace(" ","")

		entries = currentLine.split(",")

		return FaultEntry(entries[0], entries[1], entries[2], entries[3], entries[4])


if __name__ == '__main__':

	newFaultFP = open(args.newFaultFile, "w")

	faultFP = FaultParser(args.faultFile)

	lineLabel = 0
	while faultFP.haveNext():
		fault = faultFP.next()
		logFP = RegFaultGenerator(args.logFile)
		logFP.setFault(fault.stuckBit, fault.category, fault.reg, fault.bitPosition, fault.tick)

		while logFP.haveNext():
			newLine = logFP.next()
			if not newLine == "":
				newFaultFP.write("FAULT" + str(lineLabel) + ":" + newLine + "\n")
				lineLabel = lineLabel + 1

	newFaultFP.close()