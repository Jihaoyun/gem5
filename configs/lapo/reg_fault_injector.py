# import the m5 (gem5) library created when gem5 is built
import m5
# import all of the SimObjects
from m5.objects import *
from m5.util import addToPath
import sys, os
import argparse

addToPath(os.path.join('..', 'common'))
from Caches import *

parser = argparse.ArgumentParser(description='Gem5')
parser.add_argument('-fe', '--fault-enabled', dest = 'faultEnabled',
					action = 'store_true',
					help = "It is true if register flip fault is enabled.")
parser.set_defaults(faultEnabled = False)

parser.add_argument('-b', '--benchmark', type = str, dest = 'benchmark',
					required = True,
					help = 'Benchmark for reg fault simulation')

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
# create the system we are going to simulate
system = System()

# Set the clock fequency of the system (and all of its children)
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()

# Set up the system
system.mem_mode = 'timing'               # Use timing accesses
system.mem_ranges = [AddrRange('512MB')] # Create an address range

# Create a simple CPU
system.cpu = TimingSimpleCPU()

#icache = L1_ICache(size = '4MB')
#dcache = L1_DCache(size = '4MB')

#system.cpu.addPrivateSplitL1Caches(icache, dcache, None, None)

system.cpu.monitor = CommMonitor()
#system.cpu.monitor.trace = MemTraceProbe(trace_file="my_trace.trc.gz")

# Create a memory bus, a coherent crossbar, in this case
system.membus = SystemXBar()

# Hook the CPU ports up to the membus
#system.cpu.icache_port = system.membus.slave
system.cpu.dcache_port = system.membus.slave
system.cpu.icache_port = system.cpu.monitor.slave
system.cpu.monitor.master = system.membus.slave



# create the interrupt controller for the CPU and connect to the membus
system.cpu.createInterruptController()
#system.cpu.interrupts[0].pio = system.membus.master
#system.cpu.interrupts[0].int_master = system.membus.slave
#system.cpu.interrupts[0].int_slave = system.membus.master

# Create a DDR3 memory controller and connect it to the membus
system.mem_ctrl = DDR3_1600_x64()
system.mem_ctrl.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.master

# Connect the system up to the membus
system.system_port = system.membus.slave

# Create a process for a simple "Hello World" application
process = LiveProcess()
# Set the command
# cmd is a list which begins with the executable (like argv)
#process.cmd = sys.argv[1]
process.cmd = args.benchmark.split()

# Set the cpu to use the process as its workload and create thread contexts
system.cpu.workload = process
system.cpu.createThreads()

# set up the root SimObject and start the simulation
root = Root(full_system = False, system = system)

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
		root.registerFault = RegisterFault()
		root.registerFault.startTick = args.tickBegin
		root.registerFault.system = system	
		root.registerFault.registerCategory = args.regCategory
		root.registerFault.faultRegister = args.faultReg
		root.registerFault.bitPosition = args.bitPosition
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

# instantiate all of the objects we've created above
m5.instantiate()

print "Beginning simulation!"
exit_event = m5.simulate()
print 'Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause())
