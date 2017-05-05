#!/usr/bin/env python

import re
import sys

class Node:
    def __init__(self, nodeType, nodeValue):
        self.left = None
        self.right = None
        self.id = -1
        self.nodeType = nodeType
        self.nodeValue = nodeValue

class ControlFaultEntry:

    trigger = None
    action = None

    def __init__(self, trigger, action):
        self.trigger = trigger
        self.action = action

class ControlFaultParser:
    def __init__(self, fileName):
        try:
            self.faultsFile = open(fileName, "r")
        except IOError:
            raise

    def hasNext(self):
        # Read trigger string
        self.currentLine = self.faultsFile.readline()
        if self.currentLine == '':
            return False

        while self.currentLine[0] == '#':
            self.currentLine = self.faultsFile.readline()
            if self.currentLine == '':
                return False

        self.currentTriggerLine = self.currentLine

        # Read action line
        self.currentLine = self.faultsFile.readline()
        if self.currentLine == '':
            return False

        while self.currentLine[0] == '#':
            self.currentLine = self.faultsFile.readline()
            if self.currentLine == '':
                return False

        self.currentActionLine = self.currentLine

        return True

    def next(self):
        if self.currentLine[0] == '#':
            return None

        return ControlFaultEntry(
            self.parseTriggerString(self.currentTriggerLine),
            self.parseActionString(self.currentActionLine)
        )

    def clean(self, string):
        string = string.replace(" ", "")
        string = string.replace("\n", "")
        strList = list(string)
        if(strList[0] == '(' and strList[len(string)-1] == ')'):
                strList[0] = ' '
                strList[len(string)-1] = ' '
        return ''.join(strList).replace(" ", "")

    def parseTrigger(self, string):
        string = self.clean(string)

        opsre = r".*(&|\||\<|\>|=|\!).*"

        #Check if final case
        if re.match(opsre, string) is None:
            if string == "index":
                return Node("i", string)
            else:
                return Node("c", str(int(string, 16)))
            #Find most extern operator
        counter = 0;
        for i in range(len(string)):
            if re.match(opsre, string[i]) is not None \
                and counter == 0:
                op = string[i]
                if(op == '!'):
                    if string[i+1] != '(':
                        sys.exit("Missing '(' After '!'")
                    tmpNode = Node("o", op)
                    tmpNode.right = \
                        self.parseTrigger(self.clean(string[(i+1):]))
                else:
                    if(string[i+1] == '=' or \
                            string[i+1] == '&' or \
                            string[i+1] == '|'):
                        i+=1
                        op += string[i]
                        leftString = string[:(i-1)]
                    else:
                        leftString = string[:(i)]
                    rightString = string[(i+1):]

                    tmpNode = Node("o", op)
                    tmpNode.left = self.parseTrigger(self.clean(leftString))
                    tmpNode.right = self.parseTrigger(self.clean(rightString))
                return tmpNode
            else:
                if(string[i] == '('):
                    counter += 1
                if(string[i] == ')'):
                    counter -= 1

    def parseAction(self, string):
        #Check if final case
        if re.match(r".*[&|^~<>].*", string) is None:
            if string == "index":
                return Node("i", string)
            else:
                return Node("c", str(int(string, 16)))

        #Find most extern operator
        counter = 0;
        for i in range(len(string)):
            if re.match(r".*[&|^~<>].*", string[i]) is not None \
                and counter == 0:
                op = string[i]

                if(op == '~'):
                    if string[i+1] != '(':
                        sys.exit("Missing '(' After '~'")
                    tmpNode = Node("o", op)
                    tmpNode.right = \
                        self.parseAction(self.clean(string[(i+1):]))
                else:
                    if(string[i+1] == '<' or string[i+1] == '>'):
                        i+=1
                        op += string[i]
                        leftString = string[:(i-1)]
                    else:
                        leftString = string[:(i)]
                    rightString = string[(i+1):]

                    tmpNode = Node("o", op)
                    tmpNode.left = self.parseAction(self.clean(leftString))
                    tmpNode.right = self.parseAction(self.clean(rightString))
                return tmpNode
            else:
                if(string[i] == '('):
                    counter += 1
                if(string[i] == ')'):
                    counter -= 1

    def visit(self, n):
        self.nodeString += str(n.id) + " " + n.nodeType + " " + \
            n.nodeValue + " "

        if n.left is not None:
            self.edgeCount += 1
            self.nodeCount += 1
            n.left.id = self.nodeCount
            self.edgeString += str(n.id) + " " + str(n.left.id) + " "
            self.visit(n.left)
        if n.right is not None:
            self.edgeCount += 1
            self.nodeCount += 1
            n.right.id = self.nodeCount
            self.edgeString += str(n.id) + " " + str(n.right.id) + " "
            self.visit(n.right)

    def parseTriggerString(self, string):
        self.nodeCount = 0
        self.edgeCount = 0
        self.nodeString = ""
        self.edgeString = ""

        rootNode = self.parseTrigger(string)
        rootNode.id = 0
        self.visit(rootNode)
        self.nodeCount += 1

        return str(self.nodeCount) + " " + str(self.edgeCount) + " " \
            + self.nodeString + " " + self.edgeString


    def parseActionString(self, string):
        self.nodeCount = 0
        self.edgeCount = 0
        self.nodeString = ""
        self.edgeString = ""

        rootNode = self.parseAction(string)
        rootNode.id = 0
        self.visit(rootNode)
        self.nodeCount += 1

        return str(self.nodeCount) + " " + str(self.edgeCount) + " " \
            + self.nodeString + " " + self.edgeString

#Examples
if __name__ == "__main__":
    p = ControlFaultParser("control_fault.txt")
    n = 1
    while p.hasNext():
        cfe = p.next()
        print cfe.trigger
        print cfe.action
