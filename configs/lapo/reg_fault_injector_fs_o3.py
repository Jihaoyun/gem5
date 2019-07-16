import sys,os

import m5
from m5.objects import *

sys.path.append('configs/common/')
sys.path.append('configs/example/')
import SimpleOpts
from Caches import *

import argparse

from system import MySystem

parser = argparse.ArgumentParser(description='Gem5')
#parser.add_argument("--script", default='', dest = 'script',
#					help="Script to execute in simulation.")

parser.add_argument('-fe', '--fault-enabled', dest = 'faultEnabled',
					action = 'store_true',
					help = "It is true if register flip fault is enabled.")
parser.set_defaults(faultEnabled = False)

parser.add_argument('-s', '--script', type = str, dest = 'script',
					required = True,
					help = 'Benchmark for reg fault simulation')

#parser.add_argument('-b', '--benchmark', type = str, dest = 'benchmark',
#					required = True,
#					help = 'Benchmark for reg fault simulation')

parser.add_argument('-l', '--label', type = str, dest = 'label',
					help = 'Reg fault name')

parser.add_argument('-sb', '--stuck-bit', type = int, dest = 'stuckBit',
					help = 'Start tick for register fault.')

parser.add_argument('-rfc', '--reg-fault-category', type = int, dest = 'regCategory',
					help = 'Register category for register fault.')

parser.add_argument('-fr', '--fault-reg', type = int, dest = 'faultReg',
					help = 'Fault register.')

parser.add_argument('-rfbp', '--reg-fault-bit-posi', type = int, dest = 'bitPosition',
					help = 'Register fault bit position.')

parser.add_argument('-tb', '--tick-begin', type = int, dest = 'tickBegin',
					help = 'Start tick for register fault.')

parser.add_argument('-te', '--tick-end', type = int, dest = 'tickEnd',
					help = 'End tick for register fault.')

args = parser.parse_args()

if __name__ == "__m5_main__":

	system = MySystem(args)

	system.readfile = args.script

	root = Root(full_system = True, system = system)

	if args.faultEnabled:
		if args.tickBegin == 0 and args.tickEnd == -1:
			# If the fault is permanent
			root.registerPermanentFault = RegisterPermanentFault()
			root.registerPermanentFault.system = system
			root.registerPermanentFault.registerCategory = args.regCategory
			root.registerPermanentFault.faultRegister = args.faultReg
			root.registerPermanentFault.bitPosition = args.bitPosition
			root.registerPermanentFault.faultLabel = args.label
			root.registerPermanentFault.faultStuckBit = args.stuckBit
		elif args.tickBegin == args.tickEnd:
			# If the fault is transient
			root.registerTransientFault = PhysicalRegisterTransientFault()
			root.registerTransientFault.tickBegin = args.tickBegin
			root.registerTransientFault.system = system	
			root.registerTransientFault.registerCategory = args.regCategory
			root.registerTransientFault.faultRegister = args.faultReg
			root.registerTransientFault.bitPosition = args.bitPosition
			root.registerTransientFault.faultLabel = args.label
		else:
			root.registerIntermittentFault = RegisterIntermittentFault()
			root.registerIntermittentFault.system = system
			root.registerIntermittentFault.registerCategory = args.regCategory
			root.registerIntermittentFault.faultRegister = args.faultReg
			root.registerIntermittentFault.bitPosition = args.bitPosition
			root.registerIntermittentFault.faultLabel = args.label
			root.registerIntermittentFault.faultStuckBit = args.stuckBit
			root.registerIntermittentFault.tickBegin = args.tickBegin
			root.registerIntermittentFault.tickEnd = args.tickEnd

	m5.instantiate()

	print "Running the simulation"
	exit_event = m5.simulate()
	print "Exiting @ tick %i because %s" % (m5.curTick(),
											exit_event.getCause())
