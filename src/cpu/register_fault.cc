#include "register_fault.hh"

RegisterFault::RegisterFault(RegisterFaultParams *p):
    SimObject(p),
    faultEvent(this, false)
{
    sys = p->system;
        bitPosition = p->bitPosition;
    faultRegister = p->faultRegister;
    registerCategory = p->registerCategory;
    schedule(faultEvent, p->startTick);
}

void
RegisterFault::fault()
{
        ThreadContext* tc;
    uint64_t registerBits;
    uint64_t bitMask = 1 << bitPosition;
    tc = (*sys).getThreadContext(0);

    if ( registerCategory == 1 ) {
                registerBits = (*tc).readFloatRegBits(faultRegister);
        DPRINTF(RegisterBitFlip,
                "before FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
        (*tc).setFloatRegBits(faultRegister, registerBits ^ bitMask );
                registerBits = (*tc).readFloatRegBits(faultRegister);
                DPRINTF(RegisterBitFlip,
                "after FLOATREG_F%d: 0x%x\n", faultRegister, registerBits);
    }
    else if (registerCategory == 2 ){
        registerBits = (*tc).readCCReg(faultRegister);
        DPRINTF(RegisterBitFlip,
                "before CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        (*tc).setCCReg(faultRegister, registerBits ^ bitMask );
                registerBits = (*tc).readCCReg(faultRegister);
                DPRINTF(RegisterBitFlip,
                "after CCREG_C%d: 0x%x\n", faultRegister, registerBits);
        }
    else {
        registerBits =
                (*tc).readIntRegFlat((*tc).flattenIntIndex(faultRegister));

                DPRINTF(RegisterBitFlip,
                "before INTREG_R%d: 0x%x\n",
                (*tc).flattenIntIndex(faultRegister), registerBits);

                (*tc).setIntRegFlat(faultRegister, registerBits ^ bitMask );
        registerBits =
                (*tc).readIntRegFlat((*tc).flattenIntIndex(faultRegister));

                DPRINTF(RegisterBitFlip,
                "after INTREG_R%d: 0x%x\n",
                (*tc).flattenIntIndex(faultRegister), registerBits);
    }

}

RegisterFault*
RegisterFaultParams::create()
{
    return new RegisterFault(this);
}
