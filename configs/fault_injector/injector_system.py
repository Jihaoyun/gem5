# import the m5 (gem5) library created when gem5 is built
import m5
# import all of the SimObjects
from m5.objects import *
from FaultParser import *
import argparse

#parse and save the arguments
parser = argparse.ArgumentParser(description='Gem5')

parser.add_argument('-fe', '--fault-enabled', dest='faultEnabled',
                    action='store_true',
                    help='It is true if the BPU is faulted')
parser.set_defaults(faultEnabled=False)

parser.add_argument('-b', '--benchmark', type=str, dest='benchmark',
                    help='Benchmark set on which to run the simulation')

parser.add_argument('-l', '--label', type=str, dest='label',
                    help='Fault name')

parser.add_argument('-sb', '--stuck-bit', type=int, dest='stuckBit',
                    help='Stuck bit to 1 or 0')

parser.add_argument('-f', '--field', type=int, dest='field',
                    help='Field where to inject the fault')

parser.add_argument('-e', '--entry', type=int, dest='entry',
                    help='Entry where to inject the fault')

parser.add_argument('-bp', '--bit-position', type=int, dest='bitPosition',
                    help='Bit position of the field where to inject the fault')

parser.add_argument('-tb', '--tick-begin', type=int, dest='tickBegin',
                    help='Inject fault at this tick')

parser.add_argument('-te', '--tick-end', type=int, dest='tickEnd',
                    help='Remove fault at this tick')

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
system.cpu = MinorCPU()

# Create a memory bus, a coherent crossbar, in this case
system.membus = SystemXBar()

# Hook the CPU ports up to the membus
system.cpu.icache_port = system.membus.slave
system.cpu.dcache_port = system.membus.slave

# create the interrupt controller for the CPU and connect to the membus
system.cpu.createInterruptController()

# Create a DDR3 memory controller and connect it to the membus
system.mem_ctrl = DDR3_1600_x64()
system.mem_ctrl.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.master

# Connect the system up to the membus
system.system_port = system.membus.slave

# Create a process for a simple "Hello World" application
process = LiveProcess()

# Set the command
process.cmd = args.benchmark

# Set the cpu to use the process as its workload and create thread contexts
system.cpu.workload = process
system.cpu.createThreads()

# set up the root SimObject and start the simulation
root = Root(full_system = False, system = system)



#run all the simulation
if args.faultEnabled:
    system.cpu.branchPred.faultEnabled = True
    system.cpu.branchPred.faultLabel = args.label
    system.cpu.branchPred.faultStuckBit = args.stuckBit
    system.cpu.branchPred.faultField = args.field
    system.cpu.branchPred.faultEntry = args.entry
    system.cpu.branchPred.faultBitPosition = args.bitPosition
    system.cpu.branchPred.faultPermanent = \
            (args.tickBegin == 0 and args.tickEnd == -1)
    system.cpu.branchPred.faultTickBegin = args.tickBegin
    system.cpu.branchPred.faultTickEnd = args.tickEnd
else:
    system.cpu.branchPred.faultEnabled = False

m5.instantiate()
print "Beginning simulation!"
exit_event = m5.simulate()
print 'Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause())
