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

parser.add_argument('-sb', '--stuck-bit', type = str, dest = 'stuckBitStr',
					help = 'Start tick for register fault.')

parser.add_argument('-rfc', '--reg-fault-category', type = str, dest = 'regCategoryStr',
					help = 'Register category for register fault.')

parser.add_argument('-fr', '--fault-reg', type = str, dest = 'faultRegStr',
					help = 'Fault register.')

parser.add_argument('-rfbp', '--reg-fault-bit-posi', type = str, dest = 'bitPositionStr',
					help = 'Register fault bit position.')

parser.add_argument('-t', '--tick', type = str, dest = 'tickStr',
					help = 'Tick for register fault.')

parser.add_argument('-op', '--operation', type = str, dest = 'operationStr',
					help = 'Operation of the fault injection.')

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
system.cpu = DerivO3CPU()
system.cache_line_size = 64
system.cpu.numPhysIntRegs = 256
system.cpu.numPhysFloatRegs = 256
system.cpu.numIQEntries = 32
system.cpu.LQEntries = 16
system.cpu.SQEntries = 16
system.cpu.numROBEntries = 40

icache = L1_ICache(size = '32kB')
dcache = L1_DCache(size = '32kB')
l2cache = L2Cache(size = '1MB')
#system.cpu.addPrivateSplitL1Caches(icache, dcache, None, None)

system.cpu.addTwoLevelCacheHierarchy(icache, dcache, l2cache, None, None)
system.cpu.icache.assoc = 4
system.cpu.dcache.assoc = 4
system.cpu.l2cache.assoc = 16

#system.cpu.monitor = CommMonitor()
#system.cpu.monitor.trace = MemTraceProbe(trace_file="my_trace.trc.gz")

# Create a memory bus, a coherent crossbar, in this case
system.membus = SystemXBar()

# Hook the CPU ports up to the membus
#system.cpu.icache_port = system.membus.slave
#system.cpu.dcache_port = system.membus.slave
#system.cpu.icache_port = system.cpu.monitor.slave
#system.cpu.monitor.master = system.membus.slave



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

system.cpu.connectAllPorts(system.membus)

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

if args.regCategoryStr is not None:
	regCategory = args.regCategoryStr.strip('\'').split()
	regCategory = [int(elem) for elem in regCategory]
else:
	regCategory = []

if args.faultRegStr is not None:
	faultReg = args.faultRegStr.strip('\'').split()
	faultReg = [int(elem) for elem in faultReg]
else:
	faultReg = []

if args.bitPositionStr is not None:
	bitPosition = args.bitPositionStr.strip('\'').split()
	bitPosition = [int(elem) for elem in bitPosition]
else:
	bitPosition = []

if args.stuckBitStr is not None:
	stuckBit = args.stuckBitStr.strip('\'').split()
	stuckBit = [int(elem) for elem in stuckBit]
else:
	stuckBit = []

if args.tickStr is not None:
	tick = args.tickStr.strip('\'').split()
	tick = [int(elem) for elem in tick]
else:
	tick = []

if args.operationStr is not None:
	operation = args.operationStr.strip('\'').split()
	operation = [int(elem) for elem in operation]
else:
	operation = []

if args.faultEnabled:
	root.faultEntry = MultiRegisterFaultList()
	root.faultEntry.system = system
	root.faultEntry.registerCategoryList = regCategory
	root.faultEntry.faultRegisterList = faultReg
	root.faultEntry.bitPositionList = bitPosition
	root.faultEntry.faultLabel = args.label
	root.faultEntry.faultStuckBitList = stuckBit
	root.faultEntry.tickList = tick
	root.faultEntry.operationList = operation

# instantiate all of the objects we've created above
m5.instantiate()

print "Beginning simulation!"
exit_event = m5.simulate()
print 'Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause())
