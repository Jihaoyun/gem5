#ifndef __REGISTER_PERMANENT_FAULT_HH__
#define __REGISTER_PERMANENT_FAULT_HH__

#include "cpu/thread_context.hh"
#include "debug/RegisterPermanentFault.hh"
#include "params/RegisterPermanentFault.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class RegisterPermanentFault : public SimObject
{
private:
	System* sys;
	std::string faultLabel;
	uint64_t bitPosition;
	uint64_t faultRegister;
	uint64_t registerCategory;
	uint32_t faultStuckBit;

public:
	RegisterPermanentFault(RegisterPermanentFaultParams *p);

	void fault();
	EventWrapper<RegisterPermanentFault, &RegisterPermanentFault::fault> faultEvent;
};

#endif