#include "bpu_transient_fault.hh"

BpuTransientFault::BpuTransientFault(BpuTransientFaultParams *params):
    SimObject(params),
    bpu(params->bpu),
    tick(params->tick),
    faultLabel(params->faultLabel),
    faultField(params->faultField),
    faultEntry(params->faultEntry),
    faultBitPosition(params->faultBitPosition),
    faultStuckBit(params->faultStuckBit),
    faultEnd(params->faultEnd),
    faultEvent(this, false)
{
  schedule(faultEvent, params->tick);
}

void
BpuTransientFault::fault()
{
  // TODO
    // costruire inject fault
    // Bpred.insertfault(struct, faultEnd);

}

BpuTransientFault*
BpuTransientFaultParams::create()
{
  return new BpuTransientFault(this);
}
