from m5.params import *
from m5.SimObject import SimObject

class RegisterPermanentFault(SimObject):
	type = 'RegisterPermanentFault'
	cxx_header = "cpu/register_permanent_fault.hh"
	system = Param.System(NULL, "System")
	registerCategory = Param.Unsigned(0,
	"The group of registers to which the register belong")
	faultRegister = Param.Unsigned(0, "Register where inject the bit flip")
	bitPosition = Param.Unsigned(0, "Bit to flip")
	faultLabel = Param.String(None, "Injected fault label")
	faultStuckBit = Param.Unsigned(0, "Register file stuck at fault value")
