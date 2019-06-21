#include "register_permanent_fault.hh"

RegisterPermanentFault::RegisterPermanentFault(RegisterPermanentFaultParams *p):
	SimObject(p),
	faultEvent(this, false)
	{
		sys = p->system;
		bitPosition = p->bitPosition;
		faultLabel = p->faultLabel;
		faultRegister = p->faultRegister;
		registerCategory = p->registerCategory;
		faultStuckBit = p->faultStuckBit;

		schedule(faultEvent, 0);
	}

void RegisterPermanentFault::fault()
{
	ThreadContext* tc;
	uint64_t registerBits;
    tc = (*sys).getThreadContext(0);

    if ( registerCategory == 1 ) {

        //registerBits = (*tc).readFloatRegBits(faultRegister);
        //DPRINTF(RegisterPermanentFault,
        //        "before FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
        (*tc).setFloatRegBitsFault(faultRegister, bitPosition, faultStuckBit);
        //registerBits = (*tc).readFloatRegBits(faultRegister);
        //DPRINTF(RegisterPermanentFault,
        //        "after FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
    }
    else if (registerCategory == 2 ){

        //registerBits = (*tc).readCCReg(faultRegister);
        //DPRINTF(RegisterPermanentFault,
        //        "before CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        (*tc).setCCRegFault(faultRegister, bitPosition, faultStuckBit);
        //registerBits = (*tc).readCCReg(faultRegister);
        //DPRINTF(RegisterPermanentFault,
        //        "after CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        }
    else {
        //registerBits =
        //        (*tc).readIntRegFlat((faultRegister));

        //DPRINTF(RegisterPermanentFault,
        //        "before INTREG_R%d: 0x%x\n",
        //        (faultRegister), registerBits);

        (*tc).setIntRegFault(faultRegister, bitPosition, faultStuckBit);

        //registerBits =
        //        (*tc).readIntRegWithFaultFlat((faultRegister));

        //DPRINTF(RegisterPermanentFault,
        //        "after INTREG_R%d: 0x%x\n",
        //        (faultRegister), registerBits);
    }
}

RegisterPermanentFault*
RegisterPermanentFaultParams::create()
{
	return new RegisterPermanentFault(this);
}