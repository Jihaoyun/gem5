/*
 * Copyright (c) 2004-2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Kevin Lim
 */

#include "base/intmath.hh"
#include "base/misc.hh"
#include "base/trace.hh"
#include "cpu/pred/2bit_local.hh"
#include "debug/Fetch.hh"

LocalBP::LocalBP(const LocalBPParams *params)
    : BPredUnit(params),
      localPredictorSize(params->localPredictorSize),
      localCtrBits(params->localCtrBits)
{
    if (!isPowerOf2(localPredictorSize)) {
        fatal("Invalid local predictor size!\n");
    }

    localPredictorSets = localPredictorSize / localCtrBits;

    if (!isPowerOf2(localPredictorSets)) {
        fatal("Invalid number of local predictor sets! Check localCtrBits.\n");
    }
    // Setup the index mask.
    indexMask = localPredictorSets - 1;

    DPRINTF(Fetch, "index mask: %#x\n", indexMask);

    // Setup the array of counters for the local predictor.
    localCtrs.resize(localPredictorSets);

    for (unsigned i = 0; i < localPredictorSets; ++i)
        localCtrs[i].setBits(localCtrBits);

    if (  params->faultEnabled &&
          params->faultField == 3 &&
          params->faultTickEnd == -1) {
        if ( params->faultEntry >= localPredictorSets )
          fatal("BP: FaultEntry exceeds"
              "dimension of the saturating counter array");
        localCtrs[params->faultEntry].setFaulted(
            params->faultBitPosition,params->faultStuckBit);
    }


    DPRINTF(Fetch, "local predictor size: %i\n",
            localPredictorSize);

    DPRINTF(Fetch, "local counter bits: %i\n", localCtrBits);

    DPRINTF(Fetch, "instruction shift amount: %i\n",
            instShiftAmt);
}

void
LocalBP::setFault(struct FaultBPU::injFault f_parameters,bool faultEnd)
{

    if (  f_parameters.field != 3 )
        return;
    if ( f_parameters.entry >= localPredictorSets )
        fatal("BP: FaultEntry exceeds"
              "dimension of the saturating counter array");
        localCtrs[f_parameters.entry].setTranFaulted(
            f_parameters.bitPosition);

}

void 
LocalBP::setInterFault(struct FaultBPU::injFault f_parameters,bool faultEnd)
{

    if (  f_parameters.field != 3 )
        return;
    if ( f_parameters.entry >= localPredictorSets )
        fatal("BP: FaultEntry exceeds"
              "dimension of the saturating counter array");
        localCtrs[f_parameters.entry].setInterFaulted(
            f_parameters.bitPosition, f_parameters.stuckBit);

}

void
LocalBP::resetInterFault(struct FaultBPU::injFault f_parameters,bool faultEnd)
{

    if (  f_parameters.field != 3 )
        return;
    if ( f_parameters.entry >= localPredictorSets )
        fatal("BP: FaultEntry exceeds"
              "dimension of the saturating counter array");
        localCtrs[f_parameters.entry].setOriginal();
   
}

void
LocalBP::reset()
{
    for (unsigned i = 0; i < localPredictorSets; ++i) {
        localCtrs[i].reset();
    }
}

void
LocalBP::btbUpdate(ThreadID tid, Addr branch_addr, void * &bp_history)
{

    update(tid,branch_addr,false,bp_history,true);

}


bool
LocalBP::lookup(ThreadID tid, Addr branch_addr, void * &bp_history)
{
    bool taken;
    uint8_t counter_val;
    unsigned local_predictor_idx = getLocalIndex(branch_addr);

    DPRINTF(Fetch, "Looking up index %#x\n",
            local_predictor_idx);

    counter_val = localCtrs[local_predictor_idx].read();

    DPRINTF(Fetch, "BHT lookup: prediction is %i.\n",
            (int)counter_val);

    taken = getPrediction(counter_val);


    return taken;
}

void
LocalBP::update(ThreadID tid, Addr branch_addr, bool taken, void *bp_history,
                bool squashed)
{
    assert(bp_history == NULL);
    unsigned local_predictor_idx;

    // Update the local predictor.
    local_predictor_idx = getLocalIndex(branch_addr);

    DPRINTF(Fetch, "BHT update:Looking up index %#x\n", local_predictor_idx);

    if (taken) {
        DPRINTF(Fetch, "BHT update: Branch updated as taken.\n");
        localCtrs[local_predictor_idx].increment();
    } else {
        DPRINTF(Fetch, "BHT update: Branch updated as not taken.\n");
        localCtrs[local_predictor_idx].decrement();
    }
}

inline
bool
LocalBP::getPrediction(uint8_t &count)
{
    // Get the MSB of the counter
    return (count >> (localCtrBits - 1));
}

inline
unsigned
LocalBP::getLocalIndex(Addr &branch_addr)
{
    return (branch_addr >> instShiftAmt) & indexMask;
}

void
LocalBP::uncondBranch(ThreadID tid, Addr pc, void *&bp_history)
{
}

LocalBP*
LocalBPParams::create()
{
    return new LocalBP(this);
}
