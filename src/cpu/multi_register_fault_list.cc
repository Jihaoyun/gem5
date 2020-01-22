#include "multi_register_fault_list.hh"

MultiRegisterFaultList::MultiRegisterFaultList(MultiRegisterFaultListParams *p):
	SimObject(p),
    faultEvent(this, false)
	{
		sys = p->system;
		bitPositionVector = p->bitPositionList;
		faultLabel = p->faultLabel;
		faultRegisterVector = p->faultRegisterList;
		registerCategoryVector = p->registerCategoryList;
		faultStuckBitVector = p->faultStuckBitList;
        tickVector = p->tickList;
        operationVector = p->operationList;

        timesLeft = operationVector.size();

        schedule(faultEvent, tickVector[0]);
	}

void MultiRegisterFaultList::faultSimOperation()
{
    ThreadContext* tc;
    uint64_t registerBits;
    tc = (*sys).getThreadContext(0);

    int index = operationVector.size() - timesLeft;

    if ( registerCategoryVector[index] == 1 ) {

        //registerBits = (*tc).readFloatRegBits(faultRegister);
        //DPRINTF(MultiRegisterFaultList,
        //        "before FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
        if (operationVector[index] == 0)
            (*tc).setFloatRegBitsTransFault(faultRegisterVector[index], bitPositionVector[index]);
        else if (operationVector[index] == 1)
            (*tc).setFloatRegBitsFault(faultRegisterVector[index], bitPositionVector[index], faultStuckBitVector[index]);
        else if (operationVector[index] == 2)
            (*tc).resetFloatRegBitsFault(faultRegisterVector[index], bitPositionVector[index]);
        //registerBits = (*tc).readFloatRegBits(faultRegister);
        //DPRINTF(MultiRegisterFaultList,
        //        "after FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
    }
    else if (registerCategoryVector[index] == 2 ){

        //registerBits = (*tc).readCCReg(faultRegister);
        //DPRINTF(MultiRegisterFaultList,
        //        "before CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        if (operationVector[index] == 0)
            (*tc).setCCRegTransFault(faultRegisterVector[index], bitPositionVector[index]);
        else if (operationVector[index] == 1)
            (*tc).setCCRegFault(faultRegisterVector[index], bitPositionVector[index], faultStuckBitVector[index]);
        else if (operationVector[index] == 2)
            (*tc).resetCCRegFault(faultRegisterVector[index], bitPositionVector[index]);
        //registerBits = (*tc).readCCReg(faultRegister);
        //DPRINTF(MultiRegisterFaultList,
        //        "after CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        }
    else {
        //registerBits =
        //        (*tc).readIntRegFlat((faultRegister));

        //DPRINTF(MultiRegisterFaultList,
        //        "before INTREG_R%d: 0x%x\n",
        //        (faultRegister), registerBits);

        if (operationVector[index] == 0)
            (*tc).setIntRegTransFault(faultRegisterVector[index], bitPositionVector[index]);
        else if (operationVector[index] == 1)
            (*tc).setIntRegFault(faultRegisterVector[index], bitPositionVector[index], faultStuckBitVector[index]);
        else if (operationVector[index] == 2)
            (*tc).resetIntRegFault(faultRegisterVector[index], bitPositionVector[index]);

        //registerBits =
        //        (*tc).readIntRegWithFaultFlat((faultRegister));

        //        DPRINTF(MultiRegisterFaultList,
        //        "after INTREG_R%d: 0x%x\n",
        //        (faultRegister), registerBits);
    }

    if (--timesLeft > 0)
        schedule(faultEvent, tickVector[index + 1]);
}

MultiRegisterFaultList*
MultiRegisterFaultListParams::create()
{
	return new MultiRegisterFaultList(this);
}