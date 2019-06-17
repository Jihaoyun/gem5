#ifndef __PHYSICAL_REGISTER_TRANSIENT_FAULT_HH__
#define __PHYSICAL_REGISTER_TRANSIENT_FAULT_HH__

#include "cpu/thread_context.hh"
#include "debug/PhysicalRegisterTransientFault.hh"
#include "params/PhysicalRegisterTransientFault.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class PhysicalRegisterTransientFault : public SimObject
{
private:
	System* sys;
	std::string faultLabel;
	uint64_t bitPosition;
	uint64_t faultRegister;
	uint64_t registerCategory;
	uint64_t tickBegin;

public:
	PhysicalRegisterTransientFault(PhysicalRegisterTransientFaultParams *p);

	void fault();
	EventWrapper<PhysicalRegisterTransientFault, &PhysicalRegisterTransientFault::fault> faultEvent;
};

#endif