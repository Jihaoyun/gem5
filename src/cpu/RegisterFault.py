from m5.params import *
from m5.SimObject import SimObject

class RegisterFault(SimObject):
    type = 'RegisterFault'
    cxx_header = "cpu/register_fault.hh"
    startTick = Param.Unsigned(0, "Inject fault at this tick")
