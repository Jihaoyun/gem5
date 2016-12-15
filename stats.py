#!/usr/bin/env python

# This script parses the content of a list of file.
# To be correctly parsed that content should have
# the following format:
# NAME VALUE
# which is the exact format of the Gem5's stats files.

import sys

from os import listdir
from os.path import isdir

import argparse

parser = argparse.ArgumentParser(description='Gem5 Stats')
parser.add_argument('-d', '--dir', type=str, dest='directory',
                    help='The root directory of the stats files to be parsed')

args = parser.parse_args()

# This data structure contains key-value fields where
# key is equal to a file name and value is equa to
# its respective parsed Stat object
statInstances = {}

# Print some simple statistics
def simplestat(stats, props):
    for p in props:
        print p, "values"
        for inst in stats:
            print inst, stats[inst][p]

# List all files in the directory
if __name__ == "__main__":
    # Exit if wrong usage is detected
    if args.directory == None:
        print "Usage: ./stats.py -d STAT_ROOT_DIR"
        sys.exit(1)

    # Star by listing all the files in the directory
    for f in listdir(args.directory):
        # If the file is a directory skip it
        if isdir(f):
            continue

        # Otherwise start reading the file
        with open("/".join([args.directory, f])) as statFile:
            # Create the new statistic object
            stat = {}

            # Read all statistics
            for line in statFile:
                fields = line.split()
                # if the line has less then 2 fields, skip it
                if len(fields) < 2:
                    continue

                # If the first character of the first field is '-'
                # it means that the line is something written as
                # an info message by Gem5
                if len(fields[0]) > 0 and fields[0][0] == "-":
                    continue

                # Otherwise get its key-value pair
                # And store it into the Stat object
                stat[fields[0]] = fields[1]

            # Store the stat object
            statInstances[f] = stat

            print f, stat["system.cpu.branchPred.lookups"]
            print statInstances["GOLDEN.txt"]["system.cpu.branchPred.lookups"]

    simplestat(statInstances, ["system.cpu.branchPred.condIncorrect"])
