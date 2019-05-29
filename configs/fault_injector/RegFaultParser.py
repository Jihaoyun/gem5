class RegFaultEntry:
    def __init__(self, label, faultTick, regCategory, faultReg, bitPosition):
        self.label = label
        self.faultTick = faultTick
        self.regCategory = regCategory
        self.faultReg = faultReg
        self.bitPosition = bitPosition

    def __str__(self):
        return ("Label: " + self.label + "\n" +
                "FaultTick: " + self.faultTick + "\n" +
                "RegCategory: " + self.regCategory + "\n" +
                "FaultReg: " + self.faultReg + "\n" +
                "BitPosition: " + str(self.bitPosition) + "\n")

class RegFaultParser:
    def __init__(self, fileName):
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

        return RegFaultEntry(
                label,
                entries[0],
                entries[1],
                entries[2],
                entries[3])
