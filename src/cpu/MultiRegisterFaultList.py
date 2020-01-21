from m5.params import *
from m5.SimObject import SimObject

class MultiRegisterFaultList(SimObject):
	type = 'MultiRegisterFaultList'
	cxx_header = "cpu/multi_register_fault_list.hh"
	system = Param.System(NULL, "System")
	registerCategoryList = VectorParam.Unsigned([0], 
	"The group of registers to which the register belong")
	faultRegisterList = VectorParam.Unsigned([0], "Register where inject the bit flip")
	bitPositionList = VectorParam.Unsigned([0], "Bit to flip")
	faultLabel = Param.String(None, "Injected fault label")
	faultStuckBitList = VectorParam.Unsigned([0], "Register file stuck at fault value")
	tickList = VectorParam.UInt64([0], "Injected fault at this tick")
	operationList = VectorParam.UInt64([0], "The operation of the injected fault")
