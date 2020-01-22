#ifndef __MULTI_BPU_FAULT_LIST_HH__
#define __MULTI_BPU_FAULT_LIST_HH__

#include <vector>
#include "cpu/thread_context.hh"
#include "debug/MultiBpuFaultList.hh"
#include "params/MultiBpuFaultList.hh"
#include "pred/bpred_unit.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class MultiBpuFaultList : public SimObject
{
private:
	BPredUnit* bpu;
	std::string faultLabel;
	std::vector<uint32_t> bitPositionVector;
	std::vector<uint32_t> entryVector;
	std::vector<uint32_t> fieldVector;
	std::vector<uint32_t> faultStuckBitVector;
	std::vector<uint64_t> tickVector;
	std::vector<uint64_t> operationVector;
	bool faultEnd;

	int timesLeft;

public:
	MultiBpuFaultList(MultiBpuFaultListParams *p);

	void faultSimOperation();
	EventWrapper<MultiBpuFaultList, &MultiBpuFaultList::faultSimOperation> faultEvent;
};

#endif