from m5.params import *
from m5.SimObject import SimObject

class RegisterFault(SimObject):
    type = 'RegisterFault'
    cxx_header = "cpu/register_fault.hh"
    startTick = Param.Unsigned(0, "Inject fault at this tick")
    system = Param.System(NULL, "System")
    registerCategory = Param.Unsigned(0,
    "The group of registers to which the register belong")
    faultRegister = Param.Unsigned(0, "Register where inject the bit flip")
    bitPosition = Param.Unsigned(0, "Bit to flip")
