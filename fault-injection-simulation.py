#!/usr/bin/env python

import argparse
from subprocess import call

parser = argparse.ArgumentParser(description='Gem5')
parser.add_argument('-b', '--benchmarks', type=str, dest='benchmarks',
                    nargs='+', help='Benchmark set for the simulation')

parser.add_argument('-i', '--fault-input', type=str, dest='faultInput',
                    nargs='+', help='Fault source files')

args = parser.parse_args()

for benchmark in args.benchmarks:
    for inputFile in args.faultInput:
        call(["./build/ALPHA/gem5.opt",
            "configs/fault_injector/injector_system.py",
            "-b", benchmark,
            "-i", inputFile])
