#include "multi_register_fault.hh"

MultiRegisterFault::MultiRegisterFault(MultiRegisterFaultParams *p):
	SimObject(p),
	faultEvent(this, false)
	{
		sys = p->system;
		bitPosition = p->bitPosition;
		faultLabel = p->faultLabel;
		faultRegister = p->faultRegister;
		registerCategory = p->registerCategory;
		faultStuckBit = p->faultStuckBit;
        tick = p->tick;
        operation = p->operation;

        schedule(faultEvent, p->tick);
	}

void MultiRegisterFault::faultSimOperation()
{
	ThreadContext* tc;
	uint64_t registerBits;
    tc = (*sys).getThreadContext(0);

    if ( registerCategory == 1 ) {

        //registerBits = (*tc).readFloatRegBits(faultRegister);
        //DPRINTF(RegisterIntermittentFault,
        //        "before FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
        if (operation == 0)
            (*tc).setFloatRegBitsTransFault(faultRegister, bitPosition);
        else if (operation == 1)
            (*tc).setFloatRegBitsFault(faultRegister, bitPosition, faultStuckBit);
        else if (operation == 2)
            (*tc).resetFloatRegBitsFault(faultRegister, bitPosition);
        //registerBits = (*tc).readFloatRegBits(faultRegister);
        //DPRINTF(RegisterIntermittentFault,
        //        "after FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
    }
    else if (registerCategory == 2 ){

        //registerBits = (*tc).readCCReg(faultRegister);
        //DPRINTF(RegisterIntermittentFault,
        //        "before CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        if (operation == 0)
            (*tc).setCCRegTransFault(faultRegister, bitPosition);
        else if (operation == 1)
            (*tc).setCCRegFault(faultRegister, bitPosition, faultStuckBit);
        else if (operation == 2)
            (*tc).resetCCRegFault(faultRegister, bitPosition);
        //registerBits = (*tc).readCCReg(faultRegister);
        //DPRINTF(RegisterIntermittentFault,
        //        "after CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        }
    else {
        //registerBits =
        //        (*tc).readIntRegFlat((faultRegister));

        //DPRINTF(RegisterIntermittentFault,
        //        "before INTREG_R%d: 0x%x\n",
        //        (faultRegister), registerBits);

        if (operation == 0)
            (*tc).setIntRegTransFault(faultRegister, bitPosition);
        else if (operation == 1)
            (*tc).setIntRegFault(faultRegister, bitPosition, faultStuckBit);
        else if (operation == 2)
            (*tc).resetIntRegFault(faultRegister, bitPosition);

        //registerBits =
        //        (*tc).readIntRegWithFaultFlat((faultRegister));

        //        DPRINTF(RegisterIntermittentFault,
        //        "after INTREG_R%d: 0x%x\n",
        //        (faultRegister), registerBits);
    }
}

MultiRegisterFault*
MultiRegisterFaultParams::create()
{
	return new MultiRegisterFault(this);
}