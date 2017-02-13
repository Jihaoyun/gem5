#ifndef __REGISTER_FAULT_HH__
#define __REGISTER_FAULT_HH__

//#include "arch/arm/intregs.hh"
#include "cpu/thread_context.hh"
#include "debug/BpuTransientFault.hh"
#include "params/BpuTransientFault.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class BpuTransientFault : public SimObject
{
  private:
    System* sys;
    uint64_t tick;
    uint32_t faultField;
    uint32_t faultEntry;
    uint32_t faultBitPosition;
    uint32_t faultStuckBit;

  public:
    BpuTransientFault(BpuTransientFaultParams *p);

    void fault();
    EventWrapper<BpuTransientFault, &BpuTransientFault::fault> faultEvent;
};

#endif
