from m5.params import *
from m5.SimObject import SimObject

class MultiRegisterFault(SimObject):
	type = 'MultiRegisterFault'
	cxx_header = "cpu/multi_register_fault.hh"
	system = Param.System(NULL, "System")
	registerCategory = Param.Unsigned(0, 
	"The group of registers to which the register belong")
	faultRegister = Param.Unsigned(0, "Register where inject the bit flip")
	bitPosition = Param.Unsigned(0, "Bit to flip")
	faultLabel = Param.String(None, "Injected fault label")
	faultStuckBit = Param.Unsigned(0, "Register file stuck at fault value")
	tick = Param.UInt64(0, "Injected fault at this tick")
	operation = Param.UInt64(0, "The operation of the injected fault")
