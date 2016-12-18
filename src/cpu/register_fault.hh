#ifndef __REGISTER_FAULT_HH__
#define __REGISTER_FAULT_HH__

#include "arch/arm/intregs.hh"
#include "cpu/thread_context.hh"
#include "params/RegisterFault.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class RegisterFault : public SimObject
{
      private:
        System* sys;
        ThreadContext* tc;
      public:
        RegisterFault(RegisterFaultParams *p);

        void fault();
        EventWrapper<RegisterFault, &RegisterFault::fault> faultEvent;
};

#endif
