#ifndef __REGISTER_INTERMITTENT_FAULT_HH__
#define __REGISTER_INTERMITTENT_FAULT_HH__

#include "cpu/thread_context.hh"
#include "debug/RegisterIntermittentFault.hh"
#include "params/RegisterIntermittentFault.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class RegisterIntermittentFault : public SimObject
{
private:
	System* sys;
	std::string faultLabel;
	uint64_t bitPosition;
	uint64_t faultRegister;
	uint64_t registerCategory;
	uint32_t faultStuckBit;
	uint64_t tickBegin;
	uint64_t tickEnd;

public:
	RegisterIntermittentFault(RegisterIntermittentFaultParams *p);

	void faultSimBegin();
	void faultSimEnd();
	EventWrapper<RegisterIntermittentFault, &RegisterIntermittentFault::faultSimBegin> faultBeginEvent;
	EventWrapper<RegisterIntermittentFault, &RegisterIntermittentFault::faultSimEnd> faultEndEvent;
};

#endif