import re
import os
import argparse

parser = argparse.ArgumentParser(description='Script to check if reg intermittent fault gives correct faulty value')

parser.add_argument('-fault', '--fault-file', type = str, dest='faultFile',
					help = 'The input file of injected fault entries.')
parser.add_argument('-log', '--log-dir', type = str, dest = 'logDir',
					help = 'The directory of debug files of simulation.')

args = parser.parse_args()

setPattern = re.compile("Setting fault in int reg (\d+) \(.*\) at bit (\d+) to (0(x1)?)")
resetPattern = re.compile("Resetting fault in int reg (\d+) \(.*\) at bit (\d+)")

readPattern = re.compile("Reading int reg (\d+) \(.*\) as (0(x[0-9a-fA-F]+)?)")
writePattern = re.compile("Setting int reg (\d+) \(.*\) to (0(x[0-9a-fA-F]+)?)")

abortPattern = re.compile("Program aborted at tick (\d+)")
exitPattern = re.compile("Exiting @ tick (\d+)")

def findEndTick(fileName):
	for lines in open(fileName):
		if abortPattern.match(lines):
			return abortPattern.match(lines).group(1)
		elif exitPattern.match(lines):
			return exitPattern.match(lines).group(1)

def isFaultMatch(faultEntry, entryIndex, label, currentLine):
	if entryIndex % 2 == 0:
		# Should match set
		if setPattern.match(currentLine) and \
			eval(setPattern.match(currentLine).group(1)) == eval(faultEntry[2]) and \
			eval(setPattern.match(currentLine).group(2)) == eval(faultEntry[3]) and \
			eval(setPattern.match(currentLine).group(3), {'0':0, '0x1':1}) == eval(faultEntry[0]):
			print("[INFO]: Fault: "+ label+ " inject at "+ faultEntry[entryIndex]+ " (index:"+ str(entryIndex)+ ") match successfully.")
			return ["S"]
		else:
			return None
	else:
		# Should match reset
		if resetPattern.match(currentLine) and \
			eval(resetPattern.match(currentLine).group(1)) == eval(faultEntry[2]) and \
			eval(resetPattern.match(currentLine).group(2)) == eval(faultEntry[3]):
			print("[INFO]: Fault: "+ label+ " inject at "+ faultEntry[entryIndex]+ " (index:"+ str(entryIndex)+ ") match successfully.")
			return ["Re"]
		else:
			return None		

def isAccessMatch(faultEntry, label, currentTick, currentLine, trueValue, faultEnable):
	if writePattern.match(currentLine):
		if eval(writePattern.match(currentLine).group(1)) == eval(faultEntry[2]):
			return ["W", writePattern.match(currentLine).group(2)]
		else:
			return None
	elif readPattern.match(currentLine):
		if eval(readPattern.match(currentLine).group(1)) == eval(faultEntry[2]):
			stuck_at = eval(faultEntry[0]) << eval(faultEntry[3])
			mask = 1 << eval(faultEntry[3])
			expectedValue = ((trueValue & ~mask) | (stuck_at & mask)) if faultEnable else trueValue

			simValueStr = readPattern.match(currentLine).group(2)
			if eval(faultEntry[3]) == 31:
				#print("[DEBUG]"+simValueStr)
				simValueStr = re.sub(r'0xf{8}([0-9a-f]{8})', r'0x\1', simValueStr)
				#print("[DEBUG]"+simValueStr)
			if not expectedValue == eval(simValueStr):
				print("[ERROR]: Fault " + label + " at " + currentTick + 
								" read reg "+faultEntry[2]+ " as " + readPattern.match(currentLine).group(2) +
								", but expect "+str(expectedValue) + ".")
			return ["R", str(expectedValue)]
		else:
			return None
	else:
		return None

if __name__ == "__main__":
	for lines in open(args.faultFile):
		if lines[0] == '#' or len(lines.split(":")) < 2:
			continue
		else:
			currentLine = lines.strip().split(":")
			label = currentLine[0]
			print(label)
			faultEntry = currentLine[1].strip().replace(" ","").split(",")

			simEndTick = 0
			path = args.logDir
			path = os.path.join(path, label, "debug.log")
			if not os.path.exists(path):
				print("[WARNING]: Cannot find debug information for fault "+ label)
				continue
			else:
				simEndTick = eval(findEndTick(path))

			# TODO
			i = 0
			entryIndex = 4
			entryLen = len(faultEntry)
			currentTick = 0
			faultEnable = False
			resetFlag = False
			trueValue = 0
			lastOp = "R"
			readDone = False

			while(1):
				path = args.logDir
				if i is 0:
					path = os.path.join(path, label, str(label)+".log")	
				else:
					path = os.path.join(path, label, str(label)+"_"+str(i)+".log")

				if not os.path.exists(path):
					print("[WARNING]: Cannot find more debug files for fault " + label + ". Faults injection after" +
						currentTick + " will not be detected.")
					break
				else:
					for currentLine in open(path):
						currentTick = currentLine.strip().split(":")[0]
						currentLine = currentLine.strip().split(":")[3].strip()

						if entryIndex < entryLen:
							if eval(currentTick) == eval(faultEntry[entryIndex]):							
								faultMatch = isFaultMatch(faultEntry, entryIndex, label, currentLine)
								if faultMatch is not None:
									entryIndex = entryIndex + 1
									if entryIndex >= entryLen:
										print("[INFO]: Fault "+ label+ " has gone through!")
									if faultMatch[0] == "S":
										faultEnable = True 
										resetFlag = False
										lastOp = "S"
										readDone = False
									elif faultMatch[0] == "Re":
										resetFlag = True
										lastOp = "Re"
										readDone = False
										
							elif eval(currentTick) > eval(faultEntry[entryIndex]):
								print("[ERROR]: Fault " + label + " at " + str(faultEntry[entryIndex]) +
									" (index:" + str(entryIndex) + ") not injected!")
								entryIndex = entryIndex + 1
								if entryIndex >= entryLen:
									print("[INFO]: Fault "+ label+ " has gone through!")

						accessMatch = isAccessMatch(faultEntry, label, currentTick, currentLine, trueValue, faultEnable)
						if accessMatch is not None:
							if accessMatch[0] == "W":
								trueValue = eval(accessMatch[1])
								lastOp = "W"
								readDone = False
								if resetFlag is True:
									faultEnable = False
							elif accessMatch[0] == "R" and readDone is False:
								if lastOp == "S":
									if eval(accessMatch[1]) == trueValue:
										print("[INFO]: W-S-R-M: Pass W-S-R fault get masked situation at tick " + currentTick + ".")
									else:
										print("[INFO]: W-S-R-NM: Pass W-S-R fault not masked situation at tick " + currentTick + ".")
								elif lastOp == "Re":
									if eval(accessMatch[1]) == trueValue:
										print("[INFO]: W-Re-R-M: Pass W-Re-R fault get masked situation at tick " + currentTick + ".")
									else:
										print("[INFO]: W-Re-R-NM: Pass W-Re-R fault not masked situation at tick " + currentTick + ".")
								elif lastOp == "W":
									# After fault set, before reset
									if faultEnable is True and resetFlag is False:
										if eval(accessMatch[1]) == trueValue:
											print("[INFO]: S-W-R-M: Pass S-W-R fault get masked situation at tick " + currentTick + ".")
										else:
											print("[INFO]: S-W-R-NM: Pass S-W-R fault not masked situation at tick " + currentTick + ".")
									elif resetFlag is True:
										print("[INFO]: Re-W-R: Pass Re-W-R fault get masked situation at tick " + currentTick + ".")
								readDone = True


					i = i + 1
							
				if (simEndTick <= eval(currentTick)):
					print("[INFO]: Simulation stop at "+ str(simEndTick)+" for fault "+label+". Faults injection after "+
						str(currentTick)+ " will not be injected.")
					break




