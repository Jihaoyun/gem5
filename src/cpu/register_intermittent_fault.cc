#include "register_intermittent_fault.hh"

RegisterIntermittentFault::RegisterIntermittentFault(RegisterIntermittentFaultParams *p):
	SimObject(p),
	faultBeginEvent(this, false),
    faultEndEvent(this, false)
	{
		sys = p->system;
		bitPosition = p->bitPosition;
		faultLabel = p->faultLabel;
		faultRegister = p->faultRegister;
		registerCategory = p->registerCategory;
		faultStuckBit = p->faultStuckBit;
        tickBegin = p->tickBegin;
        tickEnd = p->tickEnd;

        schedule(faultBeginEvent, p->tickBegin);
		schedule(faultEndEvent, p->tickEnd);
	}

void RegisterIntermittentFault::faultSimBegin()
{
	ThreadContext* tc;
	uint64_t registerBits;
    tc = (*sys).getThreadContext(0);

    if ( registerCategory == 1 ) {

        registerBits = (*tc).readFloatRegBits(faultRegister);
        DPRINTF(RegisterIntermittentFault,
                "before FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
        (*tc).setFloatRegBitsFault(faultRegister, bitPosition, faultStuckBit);
        registerBits = (*tc).readFloatRegBits(faultRegister);
        DPRINTF(RegisterIntermittentFault,
                "after FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
    }
    else if (registerCategory == 2 ){

        registerBits = (*tc).readCCReg(faultRegister);
        DPRINTF(RegisterIntermittentFault,
                "before CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        (*tc).setCCRegFault(faultRegister, bitPosition, faultStuckBit);
        registerBits = (*tc).readCCReg(faultRegister);
        DPRINTF(RegisterIntermittentFault,
                "after CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        }
    else {
        registerBits =
                (*tc).readIntRegFlat((*tc).flattenIntIndex(faultRegister));

        DPRINTF(RegisterIntermittentFault,
                "before INTREG_R%d: 0x%x\n",
                (*tc).flattenIntIndex(faultRegister), registerBits);

        (*tc).setIntRegFault(faultRegister, bitPosition, faultStuckBit);

        registerBits =
                (*tc).readIntRegWithFaultFlat((*tc).flattenIntIndex(faultRegister));

                DPRINTF(RegisterIntermittentFault,
                "after INTREG_R%d: 0x%x\n",
                (*tc).flattenIntIndex(faultRegister), registerBits);
    }
}

void RegisterIntermittentFault::faultSimEnd()
{
    ThreadContext* tc;
    uint64_t registerBits;
    tc = (*sys).getThreadContext(0);

    if ( registerCategory == 1 ) {

        registerBits = (*tc).readFloatRegBits(faultRegister);
        DPRINTF(RegisterIntermittentFault,
                "before FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
        (*tc).resetFloatRegBitsFault(faultRegister, bitPosition);
        registerBits = (*tc).readFloatRegBits(faultRegister);
        DPRINTF(RegisterIntermittentFault,
                "after FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
    }
    else if (registerCategory == 2 ){

        registerBits = (*tc).readCCReg(faultRegister);
        DPRINTF(RegisterIntermittentFault,
                "before CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        (*tc).resetCCRegFault(faultRegister, bitPosition);
        registerBits = (*tc).readCCReg(faultRegister);
        DPRINTF(RegisterIntermittentFault,
                "after CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        }
    else {
        registerBits =
                (*tc).readIntRegWithFaultFlat((*tc).flattenIntIndex(faultRegister));

        DPRINTF(RegisterIntermittentFault,
                "before INTREG_R%d: 0x%x\n",
                (*tc).flattenIntIndex(faultRegister), registerBits);

        (*tc).resetIntRegFault(faultRegister, bitPosition);

        registerBits =
                (*tc).readIntRegWithFaultFlat((*tc).flattenIntIndex(faultRegister));

                DPRINTF(RegisterIntermittentFault,
                "after INTREG_R%d: 0x%x\n",
                (*tc).flattenIntIndex(faultRegister), registerBits);
    }
}

RegisterIntermittentFault*
RegisterIntermittentFaultParams::create()
{
	return new RegisterIntermittentFault(this);
}