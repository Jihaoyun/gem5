#include "physical_register_transient_fault.hh"

PhysicalRegisterTransientFault::PhysicalRegisterTransientFault(PhysicalRegisterTransientFaultParams *p):
	SimObject(p),
	faultEvent(this, false)
	{
		sys = p->system;
		bitPosition = p->bitPosition;
		faultLabel = p->faultLabel;
		faultRegister = p->faultRegister;
		registerCategory = p->registerCategory;
        tickBegin = p->tickBegin;

		schedule(faultEvent, p->tickBegin);
	}

void PhysicalRegisterTransientFault::fault()
{
	ThreadContext* tc;
	uint64_t registerBits;
    tc = (*sys).getThreadContext(0);

    if ( registerCategory == 1 ) {

        registerBits = (*tc).readFloatRegBits(faultRegister);
        DPRINTF(PhysicalRegisterTransientFault,
                "before FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
        (*tc).setFloatRegBitsTransFault(faultRegister, bitPosition);
        registerBits = (*tc).readFloatRegBits(faultRegister);
        DPRINTF(PhysicalRegisterTransientFault,
                "after FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
    }
    else if (registerCategory == 2 ){

        registerBits = (*tc).readCCReg(faultRegister);
        DPRINTF(PhysicalRegisterTransientFault,
                "before CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        (*tc).setCCRegTransFault(faultRegister, bitPosition);
        registerBits = (*tc).readCCReg(faultRegister);
        DPRINTF(PhysicalRegisterTransientFault,
                "after CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        }
    else {
        registerBits =
                (*tc).readIntRegFlat((*tc).flattenIntIndex(faultRegister));

        DPRINTF(PhysicalRegisterTransientFault,
                "before INTREG_R%d: 0x%x\n",
                (*tc).flattenIntIndex(faultRegister), registerBits);

        (*tc).setIntRegTransFault(faultRegister, bitPosition);

        registerBits =
                (*tc).readIntRegWithFaultFlat((*tc).flattenIntIndex(faultRegister));

        DPRINTF(PhysicalRegisterTransientFault,
                "after INTREG_R%d: 0x%x\n",
                (*tc).flattenIntIndex(faultRegister), registerBits);
    }
}

PhysicalRegisterTransientFault*
PhysicalRegisterTransientFaultParams::create()
{
	return new PhysicalRegisterTransientFault(this);
}