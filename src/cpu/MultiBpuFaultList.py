from m5.params import *
from m5.SimObject import SimObject

class MultiBpuFaultList(SimObject):
	type = 'MultiBpuFaultList'
	cxx_header = "cpu/multi_bpu_fault_list.hh"
	fieldList = VectorParam.Unsigned([0], 
	"BTB field where to inject the fault")
	entryList = VectorParam.Unsigned([0], "BTB entry where to inject the fault")
	bitPositionList = VectorParam.Unsigned([0], "Bit position to inject the fault")
	faultLabel = Param.String(None, "Injected fault label")
	faultStuckBitList = VectorParam.Unsigned([0], "BPU fault stuck at fault value")
	tickList = VectorParam.UInt64([0], "Injected fault at this tick")
	operationList = VectorParam.UInt64([0], "The operation of the injected fault")
	bpu = Param.BranchPredictor(NULL, "BPredUnit")
