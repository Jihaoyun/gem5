#include "bpu_intermittent_fault.hh"
#include "cpu/pred/faultInjected.hh"

BpuIntermittentFault::BpuIntermittentFault(BpuIntermittentFaultParams *params) :
	SimObject(params),
	bpu(params->bpu),
	tickBegin(params->tickBegin),
	tickEnd(params->tickEnd),
	faultLabel(params->faultLabel),
	faultField(params->faultField),
	faultEntry(params->faultEntry),
	faultBitPosition(params->faultBitPosition),
	faultStuckBit(params->faultStuckBit),
	faultEnd(params->faultEnd),
	faultBeginEvent(this, false),
	faultEndEvent(this, false)
	{
		schedule(faultBeginEvent, params->tickBegin);
		schedule(faultEndEvent, params->tickEnd);
	}


void 
BpuIntermittentFault::faultSimBegin()
{
    struct FaultBPU::injFault parameters(
        true,
        faultLabel,
        faultStuckBit,
        faultField,
        faultEntry,
        faultBitPosition,
        tickBegin,
        tickEnd);

    bpu->insertInterFault(parameters,faultEnd);
}

void 
BpuIntermittentFault::faultSimEnd()
{
    struct FaultBPU::injFault parameters(
        true,
        faultLabel,
        faultStuckBit,
        faultField,
        faultEntry,
        faultBitPosition,
        tickBegin,
        tickEnd);

    bpu->recoverInterFault(parameters,faultEnd);
}

BpuIntermittentFault*
BpuIntermittentFaultParams::create()
{
	return new BpuIntermittentFault(this);
}