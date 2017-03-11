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
    def __init__(self, source):
        self.source = source
        self.nodeCount = 0
        self.edgeCount = 0
        self.nodeString = ""
        self.edgeString = ""

    def clean(self, string):
        string = string.replace(" ", "")
        strList = list(string)
        if(strList[0] == '('):
                strList[0] = ' '
        if(strList[len(string)-1] == ')'):
                strList[len(string)-1] = ' '
        return ''.join(strList).replace(" ", "")

    def parse(self, string):
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
                        self.parse(self.clean(string[(i+1):]))
                else:
                    if(string[i+1] == '='):
                        i+=1
                        op += '='
                        leftString = string[:(i-1)]
                    else:
                        leftString = string[:(i)]
                        rightString = string[(i+1):]

                    tmpNode = Node("o", op)
                    tmpNode.left = self.parse(self.clean(leftString))
                    tmpNode.right = self.parse(self.clean(rightString))
                return tmpNode
            else:
                if(string[i] == '('):
                    counter += 1
                if(string[i] == ')'):
                    counter -= 1


    def visit(self, n):
        self.nodeString += str(n.id) + " " + n.nodeType + " " + \
                n.nodeValue + "\n"

        if n.left is not None:
            self.edgeCount += 1
            self.nodeCount += 1
            n.left.id = self.nodeCount
            self.edgeString += str(n.id) + " " + str(n.left.id) + "\n"
            self.visit(n.left)
        if n.right is not None:
            self.edgeCount += 1
            self.nodeCount += 1
            n.right.id = self.nodeCount
            self.edgeString += str(n.id) + " " + str(n.right.id) + "\n"
            self.visit(n.right)


    def parseString(self):
        rootNode = self.parse(self.source)
        rootNode.id = 0
        self.visit(rootNode)
        self.nodeCount += 1

        return str(p.nodeCount) + " " + str(p.edgeCount) + "\n\n" \
                + p.nodeString + "\n" + p.edgeString

    def parseFile(self):
        with open(self.source) as inputFile:
            self.source = inputFile.read().replace('\n', '')
        return self.parseString()


#Examples

#p = Parser("inp.txt")
#print p.parseFile()

#p = Parser("!((index & 0x01) | (index & 0x80))")
#print p.parseString()
