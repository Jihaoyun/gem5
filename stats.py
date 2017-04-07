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

# GOLDEN instance reference name
GOLDEN = 'GOLDEN.txt'

# Command line arguments
parser = argparse.ArgumentParser(description='Gem5 Stats')
parser.add_argument('-d', '--dir', type=str, dest='directory', required=True,
    help='The root directory of the stats files to be parsed')

parser.add_argument('-g', '--graphical', dest='graphicalStats',
    action='store_true',
    help='It is true if we want to display graphical stats')
parser.set_defaults(graphicalStats=False)

parser.add_argument('-s', '--stats', type=str, dest='stats', required=True,
    nargs='+', help='The statistics we want to display')

parser.add_argument('-c', '--csv', dest='csvStats',
    action='store_true',
    help='If true a CSV file will be generated for the given statistics')
parser.set_defaults(csvStats=False)

args = parser.parse_args()

# This data structure contains key-value fields where
# key is equal to a file name and value is equa to
# its respective parsed Stat object
statInstances = {}

# Print some simple statistics
def printstat(stats, props):
    for p in props:
        print p, "values"
        for inst in stats:
            print inst, stats[inst].get(p, 'not found')

# Display a barchart displaying the given statistics
def showgraph(stats, props):
    # Iterate over all stats
    for p in props:
        # Store labels for the horizontal axis
        labels = []
        # Store values fro the vertical axis
        values = []
        for inst in stats:
            labels.append(inst)
            values.append(stats[inst].get(p, 0))

        # Put GOLDEN value at first position
        idx = labels.index(GOLDEN)
        labels[idx], labels[0] = labels[0], labels[idx]
        values[idx], values[0] = values[0], values[idx]

        # Plot labels and values
        fig, ax = plt.subplots()
        rects = ax.bar(np.arange(len(labels)), tuple(values),
            width=0.3, color='r')
        ax.set_ylabel(p)
        ax.set_title('Variation of ' + p)
        ax.set_xticklabels(tuple(labels))
        ax.set_xticks(np.arange(len(labels)) + 0.3 / 2)
        autolabel(rects, ax)
        plt.show()

# Create a CSV file containing one row per instance and the requested stats
def createcsv(stats, props):
    with open('stats.csv', 'w') as csvfile:
        swriter = csv.writer(csvfile,
            delimiter=",",quotechar="|",quoting=csv.QUOTE_MINIMAL)
        # Iterate over all instances
        for inst in stats:
            row = []
            row.append(inst) # Instance name should be the first entry

            # Iterate all the requested stats
            for p in props:
                row.append(stats[inst].get(p, 0))

            # Write the CSV entry
            swriter.writerow(row)

# Attach a text label above each bar displaying its height
def autolabel(rects, ax):
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., 1.05*height,
                '%d' % int(height),
                ha='center', va='bottom')

# List all files in the directory
if __name__ == "__main__":
    # Load statistics
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

    # If we are in command line mode just display the requested statistics
    if not args.graphicalStats:
        printstat(statInstances, args.stats)
    # Else show some graphics
    else:
        # Import the requested libraries
        import numpy as np
        import matplotlib.pyplot as plt

        # Plot stats
        showgraph(statInstances, args.stats)

    # Create the CSV stat file if required
    if args.csvStats:
        # Import the requested library
        import csv

        # Generate CSV file
        createcsv(statInstances, args.stats);
