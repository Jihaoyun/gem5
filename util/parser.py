#!/usr/bin/env python

import re

class Node:
    def __init__(self, nodeType, nodeValue):
        self.left = None
        self.right = None
        self.id = -1
        self.nodeType = nodeType
        self.nodeValue = nodeValue

class Parser:

    def clean(self, string):
        string = string.replace(" ", "")
        string = string.replace("\n", "")
        strList = list(string)
        if(strList[0] == '('):
                strList[0] = ' '
        if(strList[len(string)-1] == ')'):
                strList[len(string)-1] = ' '
        return ''.join(strList).replace(" ", "")

    def parseTrigger(self, string):
        #Check if final case
        if re.match(r".*[&|><!=].*", string) is None:
            if string == "index":
                return Node("i", string)
            else:
                return Node("c", str(int(string, 16)))

        #Find most extern operator
        counter = 0;
        for i in range(len(string)):
            if re.match(r".*[&|><!=].*", string[i]) is not None \
                and counter == 0:
                op = string[i]

                if(op == '!'):
                    tmpNode = Node("o", op)
                    tmpNode.right = \
                        self.parseTrigger(self.clean(string[(i+1):]))
                else:
                    if(string[i+1] == '='):
                        i+=1
                        op += '='
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


    def parseFile(self, path):
        with open(path) as fp:
            self.triggerString = self.parseTriggerString(fp.readline())
            self.actionString = self.parseActionString(fp.readline())


    def getTrigger(self):
        return self.triggerString

    def getAction(self):
        return self.actionString

#Examples
#p = Parser()
#p.parseFile("inp.txt")
#print p.getTrigger()
#print p.getAction()


#print p.parseTriggerString("!((index & 0x01) | (index & 0x80))")
#print p.parseActionString("(index ^ 0x01) << 2")
