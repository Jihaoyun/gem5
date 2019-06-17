from m5.params import *
from m5.SimObject import SimObject

class PhysicalRegisterTransientFault(SimObject):
    type = 'PhysicalRegisterTransientFault'
    cxx_header = "cpu/physical_register_transient_fault.hh"
    tickBegin = Param.UInt64(0, "Inject fault at this tick")
    system = Param.System(NULL, "System")
    registerCategory = Param.Unsigned(0,
    "The group of registers to which the register belong")
    faultRegister = Param.Unsigned(0, "Register where inject the bit flip")
    bitPosition = Param.Unsigned(0, "Bit to flip")
    faultLabel = Param.String(None, "Injected fault label")
