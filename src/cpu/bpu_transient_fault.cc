#include "bpu_transient_fault.hh"
#include "cpu/pred/faultInjected.hh"

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
    struct FaultBPU::injFault parameters(
        true,
        faultLabel,
        faultStuckBit,
        faultField,
        faultEntry,
        faultBitPosition,
        tick,
        tick);
    std::cout << "I'm scheduling" << faultEnd << " " << tick << std::endl;
    bpu->insertFault(parameters,faultEnd);

}

BpuTransientFault*
BpuTransientFaultParams::create()
{
  return new BpuTransientFault(this);
}
