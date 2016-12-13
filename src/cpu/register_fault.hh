#ifndef __REGISTER_FAULT_HH__
#define __REGISTER_FAULT_HH__

#include "params/RegisterFault.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"

class RegisterFault : public SimObject
{
      public:
        RegisterFault(RegisterFaultParams *p);

        void fault();
        EventWrapper<RegisterFault, &RegisterFault::fault> faultEvent;
};

#endif
