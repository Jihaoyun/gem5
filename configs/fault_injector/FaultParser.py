class FaultEntry:
    def __init__(self, label, stuckBit, field, entry,
            bitPosition, tickBegin, tickEnd):
        self.label = label
        self.stuckBit = stuckBit
        self.field = field
        self.entry = entry
        self.bitPosition = bitPosition
        self.permanent = (int(tickBegin) == 0 and int(tickEnd) == -1)
        self.tickBegin = tickBegin
        self.tickEnd = tickEnd

    def __str__(self):
        return ("Label: " + self.label + "\n" +
                "StuckBit: " + str(self.stuckBit) + "\n" +
                "Field: " + str(self.field) + "\n" +
                "Entry: " + str(self.entry) + "\n" +
                "BitPosition: " + str(self.bitPosition) + "\n" +
                "Permanent: " + str(self.permanent) + "\n" +
                "TickBegin: " + str(self.tickBegin) + "\n" +
                "TickEnd: " + str(self.tickEnd) + "\n")

class FaultParser:
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

        line = self.currentLine.strip().replace(" ", "")
        label = line.split(":")[0]
        entries = line.split(":")[1].split(",")

        return FaultEntry(
                label,
                entries[0],
                entries[1],
                entries[2],
                entries[3],
                entries[4],
                entries[5])
