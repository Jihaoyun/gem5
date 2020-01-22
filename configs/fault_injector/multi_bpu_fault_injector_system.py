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
					help = "It is true if the BPU is faulted")
parser.set_defaults(faultEnabled = False)

parser.add_argument('-b', '--benchmark', type = str, dest = 'benchmark',
					required = True,
					help = 'Benchmark set on which to run the simulation')

parser.add_argument('-l', '--label', type = str, dest = 'label',
					help = 'Bpu fault name')

parser.add_argument('-sb', '--stuck-bit', type = str, dest = 'stuckBitStr',
					help = 'Stuck bit to 1 or 0.')

parser.add_argument('-f', '--field', type = str, dest = 'fieldStr',
					help = 'Field where to inject the fault.')

parser.add_argument('-e', '--entry', type = str, dest = 'entryStr',
					help = 'Entry where to inject the fault.')

parser.add_argument('-bp', '---bit-position', type = str, dest = 'bitPositionStr',
					help = 'Bit position of the field where to inject the fault.')

parser.add_argument('-t', '--tick', type = str, dest = 'tickStr',
					help = 'Tick for BPU fault.')

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

system.cpu.branchPred = TournamentBP()

field = args.fieldStr.strip('\'').split()
field = [int(elem) for elem in field]
entry = args.entryStr.strip('\'').split()
entry = [int(elem) for elem in entry]
bitPosition = args.bitPositionStr.strip('\'').split()
bitPosition = [int(elem) for elem in bitPosition]
stuckBit = args.stuckBitStr.strip('\'').split()
stuckBit = [int(elem) for elem in stuckBit]
tick = args.tickStr.strip('\'').split()
tick = [int(elem) for elem in tick]
operation = args.operationStr.strip('\'').split()
operation = [int(elem) for elem in operation]

if args.faultEnabled:
	root.faultEntry = MultiBpuFaultList()
	root.faultEntry.bpu = system.cpu.branchPred
	root.faultEntry.fieldList = field
	root.faultEntry.entryList = entry
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
