#include "register_fault.hh"

RegisterFault::RegisterFault(RegisterFaultParams *p):
    SimObject(p),
    faultEvent(this, false)
{
    schedule(faultEvent, p->startTick);
}

void
RegisterFault::fault()
{
    printf("Ok\n");
}

RegisterFault*
RegisterFaultParams::create()
{
    return new RegisterFault(this);
}
