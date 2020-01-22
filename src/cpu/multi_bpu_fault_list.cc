#include "multi_bpu_fault_list.hh"
#include "cpu/pred/faultInjected.hh"

MultiBpuFaultList::MultiBpuFaultList(MultiBpuFaultListParams *p):
	SimObject(p),
    bpu(p->bpu),
    faultEvent(this, false)
	{
		bitPositionVector = p->bitPositionList;
		faultLabel = p->faultLabel;
		entryVector = p->entryList;
		fieldVector = p->fieldList;
		faultStuckBitVector = p->faultStuckBitList;
        tickVector = p->tickList;
        operationVector = p->operationList;

        faultEnd = false;

        timesLeft = operationVector.size();

        schedule(faultEvent, tickVector[0]);
	}

void MultiBpuFaultList::faultSimOperation()
{
    int index = operationVector.size() - timesLeft;

    struct FaultBPU::injFault parameters(
        true,
        faultLabel,
        faultStuckBitVector[index],
        fieldVector[index],
        entryVector[index],
        bitPositionVector[index],
        tickVector[index],
        tickVector[index]);

    if (operationVector[index] == 0) {
        bpu->insertFault(parameters,faultEnd);
    }
    else if (operationVector[index] == 1) {
        bpu->insertInterFault(parameters,faultEnd);
    }
    else if (operationVector[index] == 2) {
        bpu->recoverInterFault(parameters,faultEnd);
    }

    if (--timesLeft > 0)
        schedule(faultEvent, tickVector[index + 1]);
}

MultiBpuFaultList*
MultiBpuFaultListParams::create()
{
	return new MultiBpuFaultList(this);
}