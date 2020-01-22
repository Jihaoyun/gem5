import copy

class SingleBpuFaultEntry:
    def __init__(self, label, stuckBit, field, entry,
            bitPosition, tick, operation):
        self.label = label
        self.stuckBit = int(stuckBit)
        self.field = int(field)
        self.entry = int(entry)
        self.bitPosition = int(bitPosition)
        self.tick = int(tick)
        self.operation = int(operation)
        opNameDict = { 
                        0: "Bit flip",
                        1: "Set stuck-at",
                        2: "Reset stuck-at"}
        self.opName = opNameDict[self.operation]

    def __str__(self):
        return ("Label: " + self.label + "\n" +
                "StuckBit: " + str(self.stuckBit) + "\n" +
                "Field: " + str(self.field) + "\n" +
                "Entry: " + str(self.entry) + "\n" +
                "BitPosition: " + str(self.bitPosition) + "\n" +
                "Tick: " + str(self.tick) + "\n" +
                "Operation: " + self.opName + "\n")

class MultiBpuFaultEntry:
    def __init__(self, label):
        self.label = label
        self.stuckBitList = []
        self.fieldList = []
        self.entryList = []
        self.bitPositionList = []
        self.tickList = []
        self.operationList = []
        self.opNameList = []

    def __str__(self):
        return ("Label: " + self.label + "\n" + 
            "stuckBitList: " + ", ".join([str(elem) for elem in self.stuckBitList]) + "\n" +
            "fieldList: " + ", ".join([str(elem) for elem in self.fieldList]) + "\n" +
            "entryList: " + ", ".join([str(elem) for elem in self.entryList]) + "\n" +
            "bitPositionList: " + ", ".join([str(elem) for elem in self.bitPositionList]) + "\n" +
            "tickList: " + ", ".join([str(elem) for elem in self.tickList]) + "\n" +
            "operationNameList: " + ", ".join([str(elem) for elem in self.opNameList]) + "\n")

    def __getattr__(self, name):
        return "MultiBpuFaultEntry does not have this `{}` attribute! \n".format(str(name))

    def append(self, faultEntry):
        if not isinstance(faultEntry, SingleBpuFaultEntry):
            raise TypeError, "Fault entry appended to MultiBpuFaultEntry class " \
                    "must be SingleBpuFaultEntry type"
        if (self.label == faultEntry.label):
            self.stuckBitList.append(faultEntry.stuckBit)
            self.fieldList.append(faultEntry.field)
            self.entryList.append(faultEntry.entry)
            self.bitPositionList.append(faultEntry.bitPosition)
            self.tickList.append(faultEntry.tick)
            self.operationList.append(faultEntry.operation)
            self.opNameList.append(faultEntry.opName)

    def copy(self, multiFaultEntry):
        if not isinstance(multiFaultEntry, MultiBpuFaultEntry):
            raise TypeError, "Fault entry copied to MultiBpuFaultEntry class" \
                    "must be MultiBpuFaultEntry type"
        self.stuckBitList = copy.deepcopy(multiFaultEntry.stuckBitList)
        self.fieldList = copy.deepcopy(multiFaultEntry.fieldList)
        self.entryList = copy.deepcopy(multiFaultEntry.entryList)
        self.bitPositionList = copy.deepcopy(multiFaultEntry.bitPositionList)
        self.tickList = copy.deepcopy(multiFaultEntry.tickList)
        self.operationList = copy.deepcopy(multiFaultEntry.operationList)
        self.opNameList = copy.deepcopy(multiFaultEntry.opNameList)

class MultiBpuFaultParser:
    def __init__(self, fileName):
        self.multiFaultEntry = MultiBpuFaultEntry("GOLDEN")
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

        faultEntry =  SingleBpuFaultEntry(
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
            multiFaultEntryCopy = MultiBpuFaultEntry(self.multiFaultEntry.label)
            multiFaultEntryCopy.copy(self.multiFaultEntry)

        self.multiFaultEntry = MultiBpuFaultEntry(label)
        self.multiFaultEntry.append(faultEntry)

        return multiFaultEntryCopy

    def getLastFaultEntry(self):
        if self.currentLine == "":
            return self.multiFaultEntry
