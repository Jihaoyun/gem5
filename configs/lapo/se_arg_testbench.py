# import the m5 (gem5) library created when gem5 is built
import m5
# import all of the SimObjects
from m5.objects import *
import sys

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

system.cpu.imonitor = CommMonitor()
#system.cpu.monitor.name = "instruction"
system.cpu.dmonitor = CommMonitor()
#system.cpu.monitor2.name = "data"
#system.cpu.monitor.trace = MemTraceProbe(trace_file="my_trace.trc.gz")

# Create a memory bus, a coherent crossbar, in this case
system.membus = SystemXBar()

# Hook the CPU ports up to the membus
#system.cpu.icache_port = system.membus.slave
#system.cpu.dcache_port = system.membus.slave
system.cpu.dcache_port = system.cpu.dmonitor.slave
system.cpu.dmonitor.master = system.membus.slave

system.cpu.icache_port = system.cpu.imonitor.slave
system.cpu.imonitor.master = system.membus.slave







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
sys.argv.pop(0)
process.cmd = sys.argv

# Set the cpu to use the process as its workload and create thread contexts
system.cpu.workload = process
system.cpu.createThreads()

# set up the root SimObject and start the simulation
root = Root(full_system = False, system = system)
# instantiate all of the objects we've created above

if True:
   root.registerFault = RegisterFault()
   root.registerFault.startTick =  18000
   root.registerFault.system = system
   root.registerFault.registerCategory = 0
   root.registerFault.faultRegister = 0
   root.registerFault.bitPosition = 25






m5.instantiate()

print "Beginning simulation!"
exit_event = m5.simulate()
print 'Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause())
