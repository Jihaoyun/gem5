import copy

class SingleRegFaultEntry:
	def __init__(self, label, stuckBit, regCategory, faultReg, bitPosition, tick, operation):
		self.label = label
		self.stuckBit = stuckBit
		self.regCategory = regCategory
		self.faultReg = faultReg
		self.bitPosition = bitPosition
		self.tick = tick
		self.operation = operation
		opNameDict = { 
						0: "Bit flip",
						1: "Set stuck-at",
						2: "Reset stuck-at"}
		self.opName = opNameDict[self.operation]

	def __str__(self):
		return ("Label: " + self.label + "\n" +
            "StuckBit: " + self.stuckBit + "\n" +
            "RegCategory: " + self.regCategory + "\n" +
            "FaultReg: " + self.faultReg + "\n" +
            "BitPosition: " + str(self.bitPosition) + "\n" +
            "Tick: " + self.tick + "\n" +
            "Operation: " + self.opName + "\n")

	def __getattr__(self, name):
		return "SingleRegFaultEntry does not have this `{}` attribute! \n".format(str(name))


class MultiRegFaultEntry:
	def __init__(self, label):
		self.label = label
		self.stuckBitList = []
		self.regCategoryList = []
		self.faultRegList = []
		self.bitPositionList = []
		self.tickList = []
		self.operationList = []
		self.opNameList = []

	def __str__(self):
		return ("Label: " + self.label + "\n" + 
			"stuckBitList: " + ", ".join([str(elem) for elem in self.stuckBitList]) + "\n" +
			"regCategoryList: " + ", ".join([str(elem) for elem in self.regCategoryList]) + "\n" +
			"faultRegList: " + ", ".join([str(elem) for elem in self.faultRegList]) + "\n" +
			"bitPositionList: " + ", ".join([str(elem) for elem in self.bitPositionList]) + "\n" +
			"tickList: " + ", ".join([str(elem) for elem in self.tickList]) + "\n" +
			"operationNameList: " + ", ".join([str(elem) for elem in self.opNameList]) + "\n")

	def __getattr__(self, name):
		return "MultiRegFaultEntry does not have this `{}` attribute! \n".format(str(name))

	def append(self, faultEntry):
		if not isinstance(faultEntry, SingleRegFaultEntry):
			raise TypeError, "Fault entry appended to MultiRegFaultEntry class " \
					"must be SingleRegFaultEntry type"
		if (self.label == faultEntry.label):
			self.stuckBitList.append(faultEntry.stuckBit)
			self.regCategoryList.append(faultEntry.regCategory)
			self.faultRegList.append(faultEntry.faultReg)
			self.bitPositionList.append(faultEntry.bitPosition)
			self.tickList.append(faultEntry.tick)
			self.operationList.append(faultEntry.operation)
			self.opNameList.append(faultEntry.opName)

	def

class MultiRegFaultParser:
	def __init__(self, fileName):
		self.multiFaultEntry = MultiRegFaultEntry("GOLDEN")
		try:
			self.faultsFile = open(fileName, "r")
		except IOError:
			raise

	def hasNext(self):
		self.currentLine = self.faultsFile.readline()

		if self.currentLine == '':
			return False

		while self.currentLine[0] == '#' or\
                len(self.currentLine.split(":")) < 2:
            self.currentLine = self.faultsFile.readline()
            if self.currentLine == '':
                return False

        return True

    def next(self):
    	if self.currentLine[0] == '#' or len(self.currentLine.split(":")) < 2:
            return None

        # Delete all whitespaces
        line = "".join(self.currentLine.strip().replace(" ", "").split())

        label = line.split(":")[0]
        entries = line.split(":")[1].split(",")

        faultEntry = SingleRegFaultEntry(
                label,
                entries[0],
                entries[1],
                entries[2],
                entries[3],
                entries[4],
                entries[5])

        if (self.multiFaultEntry.label == label):
        	self.multiFaultEntry.append(faultEntry)
        	return None

        if (self.multiFaultEntry.label == "GOLDEN"):
        	multiFaultEntryCopy = None
        else:	
        	multiFaultEntryCopy = MultiRegFaultEntry(self.multiFaultEntry.label)
        	multiFaultEntryCopy = copy.deepcopy(self.multiFaultEntry)

        self.multiFaultEntry = MultiRegFaultEntry(label)
        self.multiFaultEntry.append(faultEntry)

        return multiFaultEntryCopy

    def getLastFaultEntry(self):
    	if self.currentLine == '':
    		return self.multiFaultEntry
