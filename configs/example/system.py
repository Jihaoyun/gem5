# import the m5 (gem5) library created when gem5 is built
import m5
# import all of the SimObjects
from m5.objects import *
from m5.util import addToPath
import sys, os
import argparse
import x86

addToPath(os.path.join('..', 'common'))
from Caches import *

class MySystem(LinuxX86System):

	def __init__(self, opts):
		super(MySystem, self).__init__()
		self._opts = opts

		self.clk_domain = SrcClockDomain()
		self.clk_domain.clock = '3GHz'
		self.clk_domain.voltage_domain = VoltageDomain()

		mem_size = '3GB'
		self.mem_ranges = [	AddrRange(mem_size),
							AddrRange(0xC0000000, size = 0x100000),
						]

		self.membus = SystemXBar()
		self.membus.badaddr_responder = BadAddr()
		self.membus.default = self.membus.badaddr_responder.pio	

		self.system_port = self.membus.slave	

		x86.init_fs(self, self.membus)	

		self.kernel = '/dist/m5/system/binaries/x86_64-vmlinux-2.6.22.9'	

		boot_options = ['earlyprintk = ttyS0', 'console=ttyS0', 'lpj=7999923',
						'root=/dev/hda1']
		self.boot_osflags = ' '.join(boot_options)	

		self.setDiskImage('/dist/m5/system/disks/linux-x86.img')
		self.createCPU()
		self.createCacheHierarchy()
		#self.setupInterrupts()
		self.createMemoryControllers()	

	def setDiskImage(self, img_path):
		disk0 = CowDisk(img_path)
		self.pc.south_bridge.ide.disks = [disk0]

	def createCPU(self):
		self.cpu = DerivO3CPU()
		self.cpu.numPhysIntRegs = 256
		self.cpu.numPhysFloatRegs = 256
		self.cpu.numIQEntries = 32
		self.cpu.LQEntries = 16
		self.cpu.SQEntries = 16
		self.cpu.numROBEntries = 40
		self.mem_mode = 'timing'
		self.cpu.createThreads()

	def createCacheHierarchy(self):
		self.cache_line_size = 64
		icache = L1_ICache(size = '32kB')
		dcache = L1_DCache(size = '32kB')
		l2cache = L2Cache(size = '1MB')
		#system.cpu.addPrivateSplitL1Caches(icache, dcache, None, None)		

		self.cpu.addTwoLevelCacheHierarchy(icache, dcache,\
			l2cache, None, None)
		self.cpu.icache.assoc = 4
		self.cpu.dcache.assoc = 4
		self.cpu.l2cache.assoc = 16

		self.cpu.createInterruptController()

		#self.cpu.itb.walker.port = self.membus.slave
		#self.cpu.dtb.walker.port = self.membus.slave
		self.cpu.connectAllPorts(self.membus)

	def createMemoryControllers(self):
		self.mem_ctrl = DDR3_1600_x64()
		self.mem_ctrl.range = self.mem_ranges[0]
		self.mem_ctrl.port = self.membus.master

	def setupInterrupts(self):
		self.cpu.createInterruptController()
		self.cpu.interrupts[0].pio = self.membus.master
		self.cpu.interrupts[0].int_master = self.membus.slave
		self.cpu.interrupts[0].int_slave = self.membus.master


class CowDisk(IdeDisk):
	def __init__(self, filename):
		super(CowDisk, self).__init__()
		self.driveID = 'master'
		self.image = CowDiskImage(child=RawDiskImage(read_only=True),
									read_only=False)
		self.image.child.image_file = filename
