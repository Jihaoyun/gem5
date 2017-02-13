#include "bpu_transient_fault.hh"

BpuTransientFault::BpuTransientFault(BpuTransientFaultParams *params):
    SimObject(params),
    sys(params->system),
    tick(params->tick),
    faultField(params->faultField),
    faultEntry(params->faultEntry),
    faultBitPosition(params->faultBitPosition),
    faultStuckBit(params->faultStuckBit),
    faultEvent(this, false)
{
  schedule(faultEvent, params->tick);
}

void
BpuTransientFault::fault()
{
  // TODO
}

BpuTransientFault*
BpuTransientFaultParams::create()
{
  return new BpuTransientFault(this);
}
