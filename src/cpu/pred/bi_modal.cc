/*
 * Copyright (c) 2014 The Regents of The University of Michigan
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
 * Authors: Anthony Gutierrez
 */

/* @file
 * Implementation of a bi-mode branch predictor
 */

#include "cpu/pred/bi_modal.hh"

#include "base/bitfield.hh"
#include "base/intmath.hh"

BiModalBP::BiModalBP(const BiModalBPParams *params)
    : BPredUnit(params),
      historyRegisters(params->numThreads,0),
      ctrBits(params->ctrBits)
{
    int numThreads = params->numThreads;
    int numberOfTable = pow(2,params->historyBits);
    int predictorBits = ceilLog2(params->predictorSize);

    historyRegisterMask = (1 << params->historyBits) - 1;
    addressBitMask = (1 << predictorBits) - 1;

    counters.resize(numThreads);
    for ( int i = 0; i < numThreads; i++ ) {
        counters[i].resize(numberOfTable);
        for ( int j = 0; j < numberOfTable; j++ ) {
            counters[i][j].resize(params->predictorSize);
            for ( int k = 0; k < params->predictorSize; k++ )
                counters[i][j][k].setBits(ctrBits);
        }
    }

}

/*
 * For an unconditional branch we set its history such that
 * everything is set to taken. I.e., its choice predictor
 * chooses the taken array and the taken array predicts taken.
 */
void
BiModalBP::uncondBranch(ThreadID tid, Addr pc, void * &bpHistory)
{
    BPHistory *history = new BPHistory;
    history->historyRegister = historyRegisters[tid];
    history->finalPred = true;
    bpHistory = static_cast<void*>(history);
    updateGlobalHistReg(tid, true);
}

void
BiModalBP::squash(ThreadID tid, void *bpHistory)
{
    BPHistory *history = static_cast<BPHistory*>(bpHistory);
    historyRegisters[tid] = history->historyRegister;
    delete history;
}

/*
 * Here we lookup the actual branch prediction. We use the PC to
 * identify the bias of a particular branch, which is based on the
 * prediction in the choice array. A hash of the global history
 * register and a branch's PC is used to index into both the taken
 * and not-taken predictors, which both present a prediction. The
 * choice array's prediction is used to select between the two
 * direction predictors for the final branch prediction.
 */
bool
BiModalBP::lookup(ThreadID tid, Addr branchAddr, void * &bpHistory)
{
    unsigned predictorIdx = ((branchAddr >> instShiftAmt)
                                & addressBitMask);
    unsigned historyIdx = historyRegisters[tid] & historyRegisterMask;

    bool finalPrediction =
        (counters[tid][historyIdx][predictorIdx].read() >> (ctrBits - 1) );

    BPHistory *history = new BPHistory;
    history->historyRegister = historyRegisters[tid];
    history->finalPred = finalPrediction;

    bpHistory = static_cast<void*>(history);
    updateGlobalHistReg(tid, finalPrediction);

    return finalPrediction;
}

/* Set the last bit of the history to zero when an entry was not found
 * into the BTB.
 */
void
BiModalBP::btbUpdate(ThreadID tid, Addr branchAddr, void * &bpHistory)
{
    historyRegisters[tid] &= (historyRegisterMask & ~ULL(1));
}

/* Only the selected direction predictor will be updated with the final
 * outcome; the status of the unselected one will not be altered. The choice
 * predictor is always updated with the branch outcome, except when the
 * choice is opposite to the branch outcome but the selected counter of
 * the direction predictors makes a correct final prediction.
 */
void
BiModalBP::update(ThreadID tid, Addr branchAddr, bool taken, void *bpHistory,
                 bool squashed)
{
    if (bpHistory) {
        BPHistory *history = static_cast<BPHistory*>(bpHistory);

        unsigned predictorIdx = ((branchAddr >> instShiftAmt)
                                & addressBitMask);
        unsigned historyIdx = historyRegisters[tid] & historyRegisterMask;

        if ( taken )
            counters[tid][historyIdx][predictorIdx].increment();
        else
            counters[tid][historyIdx][predictorIdx].decrement();

        if (squashed) {
            if (taken) {
                historyRegisters[tid] = (history->historyRegister << 1) | 1;
            } else {
                historyRegisters[tid] = (history->historyRegister << 1);
            }
            historyRegisters[tid] &= historyRegisterMask;
        } else {
            delete history;
        }
    }
}

void
BiModalBP::retireSquashed(ThreadID tid, void *bp_history)
{
    BPHistory *history = static_cast<BPHistory*>(bp_history);
    delete history;
}

unsigned
BiModalBP::getGHR(ThreadID tid, void *bp_history) const
{
    return static_cast<BPHistory*>(bp_history)->historyRegister;
}

void
BiModalBP::updateGlobalHistReg(ThreadID tid, bool taken)
{
    historyRegisters[tid] = taken ? (historyRegisters[tid] << 1) | 1 :
                               (historyRegisters[tid] << 1);
    historyRegisters[tid] &= historyRegisterMask;
}

BiModalBP*
BiModalBPParams::create()
{
    return new BiModalBP(this);
}
