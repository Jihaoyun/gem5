#ifndef __INTERMITTENT_FAULT_HH__
#define __INTERMITTENT_FAULT_HH__

#include "cpu/thread_context.hh"
#include "debug/BpuIntermittentFault.hh"
#include "params/BpuIntermittentFault.hh"
#include "pred/bpred_unit.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"
#include "sim/system.hh"

class BpuIntermittentFault : public SimObject 
{
private:
	BPredUnit* bpu;
	uint64_t tickBegin;
	uint64_t tickEnd;
	std::string faultLabel;
	uint32_t faultField;
	uint32_t faultEntry;
	uint32_t faultBitPosition;
	uint32_t faultStuckBit;
	bool faultEnd;

public:
	BpuIntermittentFault(BpuIntermittentFaultParams *p);

	void faultSimBegin();
	void faultSimEnd();
	EventWrapper<BpuIntermittentFault, &BpuIntermittentFault::faultSimBegin> faultBeginEvent;
	EventWrapper<BpuIntermittentFault, &BpuIntermittentFault::faultSimEnd> faultEndEvent;
};

#endif