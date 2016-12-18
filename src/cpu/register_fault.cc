#include "register_fault.hh"

RegisterFault::RegisterFault(RegisterFaultParams *p):
    SimObject(p),
    faultEvent(this, false)
{
    sys = p->system;
    schedule(faultEvent, p->startTick);
}

void
RegisterFault::fault()
{
    tc = (*sys).getThreadContext(0);

    printf("Before injection\n");
    printf("INTREG_R3: %ld\n", (*tc).readIntReg(INTREG_R3));

    printf("After injection\n");
    (*tc).setIntReg(INTREG_R3,2000);
    printf("INTREG_R3: %ld\n", (*tc).readIntReg(INTREG_R3));
}

RegisterFault*
RegisterFaultParams::create()
{
    return new RegisterFault(this);
}
