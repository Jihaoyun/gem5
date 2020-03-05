import re
import os
import argparse

parser = argparse.ArgumentParser(description='Script to check multiple intermittent reg fault injection.')

parser.add_argument('-fault', '--fault-file', type=str, dest='faultFile',
					help = 'The input file of injected fault entries.')
parser.add_argument('-log', '--log-dir', type=str, dest = 'logDir',
					help = 'The directory of debug files of simulations.')

args = parser.parse_args()

setPattern = re.compile("Setting fault in int reg (\d+) \(.*\) at bit (\d+) to (0(x1)?)")
resetPattern = re.compile("Resetting fault in int reg (\d+) \(.*\) at bit (\d+)")

abortPattern = re.compile("Program aborted at tick (\d+)")
exitPattern = re.compile("Exiting @ tick (\d+)")

def findEndTick(fileName):
	for lines in open(fileName):
		if abortPattern.match(lines):
			return abortPattern.match(lines).group(1)
		elif exitPattern.match(lines):
			return exitPattern.match(lines).group(1)

def isMatch(faultEntry, entryIndex, label, currentLine):
	if entryIndex % 2 == 0:
		# Should match set
		if setPattern.match(currentLine) and \
			eval(setPattern.match(currentLine).group(1)) == eval(faultEntry[2]) and \
			eval(setPattern.match(currentLine).group(2)) == eval(faultEntry[3]) and \
			eval(setPattern.match(currentLine).group(3), {'0':0, '0x1':1}) == eval(faultEntry[0]):
			print("[INFO]: Fault: "+ label+ " inject at "+ faultEntry[entryIndex]+ " (index:"+ str(entryIndex)+ ") match successfully.")
			return True
		else:
			return False
	else:
		# Should match reset
		if resetPattern.match(currentLine) and \
			eval(resetPattern.match(currentLine).group(1)) == eval(faultEntry[2]) and \
			eval(resetPattern.match(currentLine).group(2)) == eval(faultEntry[3]):
			print("[INFO]: Fault: "+ label+ " inject at "+ faultEntry[entryIndex]+ " (index:"+ str(entryIndex)+ ") match successfully.")
			return True
		else:
			return False

if __name__ == '__main__':
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

			i = 0
			entryIndex = 4
			entryLen = len(faultEntry)
			currentTick = 0
			while(1):
				path = args.logDir
				if i is 0:
					path = os.path.join(path, label, str(label)+".log")	
				else:
					path = os.path.join(path, label, str(label)+"_"+str(i)+".log")

				if not os.path.exists(path):
					print("[WARNING]: Cannot find more debug files for fault " + label + ". Faults injection after" +
						faultEntry[entryIndex] + " (index:" + str(entryIndex) + ") will not be detected.")
					break
				else:
					for currentLine in open(path):
						currentTick = currentLine.strip().split(":")[0]
						if eval(currentTick) == eval(faultEntry[entryIndex]):
							currentLine = currentLine.strip().split(":")[3].strip()
							matchSuccess = isMatch(faultEntry, entryIndex, label, currentLine)
							if matchSuccess is True:
								entryIndex = entryIndex + 1
								if entryIndex >= entryLen:
									print("[INFO]: Fault "+ label+ " has gone through!")
									break;
						elif eval(currentTick) > eval(faultEntry[entryIndex]):
							print("[ERROR]: Fault " + label + " at " + str(faultEntry[entryIndex]) +
								" (index:" + str(entryIndex) + ") not injected!")
							entryIndex = entryIndex + 1
							if entryIndex >= entryLen:
								print("[INFO]: Fault "+ label+ " has gone through!")
								break;
					i = i + 1
							
				if entryIndex >= entryLen:
					break

				if (simEndTick < eval(faultEntry[entryIndex])):
					print("[INFO]: Simulation stop at "+ str(simEndTick)+" for fault "+label+". Faults injection after "+
						faultEntry[entryIndex]+ " (index:"+ str(entryIndex)+ ") will not be injected.")
					break






