#ifndef __MULTI_REGISTER_FAULT_HH__
#define __MULTI_REGISTER_FAULT_HH__

#include "cpu/thread_context.hh"
#include "debug/MultiRegisterFault.hh"
#include "params/MultiRegisterFault.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class MultiRegisterFault : public SimObject
{
private:
	System* sys;
	std::string faultLabel;
	uint64_t bitPosition;
	uint64_t faultRegister;
	uint64_t registerCategory;
	uint32_t faultStuckBit;
	uint64_t tick;
	uint64_t operation;

public:
	MultiRegisterFault(MultiRegisterFaultParams *p);

	void faultSimOperation();
	EventWrapper<MultiRegisterFault, &MultiRegisterFault::faultSimOperation> faultEvent;
};

#endif