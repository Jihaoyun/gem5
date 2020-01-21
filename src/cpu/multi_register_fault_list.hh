#ifndef __MULTI_REGISTER_FAULT_LIST_HH__
#define __MULTI_REGISTER_FAULT_LIST_HH__

#include <vector>
#include "cpu/thread_context.hh"
#include "debug/MultiRegisterFaultList.hh"
#include "params/MultiRegisterFaultList.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class MultiRegisterFaultList : public SimObject
{
private:
	System* sys;
	std::string faultLabel;
	std::vector<uint64_t> bitPositionVector;
	std::vector<uint64_t> faultRegisterVector;
	std::vector<uint64_t> registerCategoryVector;
	std::vector<uint32_t> faultStuckBitVector;
	std::vector<uint64_t> tickVector;
	std::vector<uint64_t> operationVector;

	int timesLeft;

public:
	MultiRegisterFaultList(MultiRegisterFaultListParams *p);

	void faultSimOperation();
	EventWrapper<MultiRegisterFaultList, &MultiRegisterFaultList::faultSimOperation> faultEvent;
};

#endif