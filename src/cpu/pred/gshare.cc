#include "cpu/pred/gshare.hh"

#include "base/bitfield.hh"
#include "base/intmath.hh"

GShareBP::GShareBP(const GShareBPParams *params)
    : BPredUnit(params),
      historyRegisters(params->numThreads, 0),
      historyBits(params->historyBits),
      ctrBits(params->ctrBits) {
    int numThreads = params->numThreads;
    int numberOfTable = pow(2,historyBits);
    int predictorSize = ceilLog2(params->predictorSize);

    historyRegisterMask = 0;
    for ( int i = 0; i < historyBits; i++ )
        historyRegisterMask = (historyRegisterMask << 1) | 1;

    addressBitMask = 0;
    for ( int i = 0; i < predictorSize ; i++ )
        addressBitMask = (addressBitMask << 1 ) | 1;

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
GShareBP::uncondBranch(ThreadID tid, Addr pc, void * &bpHistory) {
    BPHistory *history = new BPHistory;
    history->historyRegister = historyRegisters[tid];
    history->finalPred = true;
    bpHistory = static_cast<void*>(history);
    updateGlobalHistReg(tid, true);
}

void
GShareBP::squash(ThreadID tid, void *bpHistory) {
    BPHistory *history = static_cast<BPHistory*>(bpHistory);
    historyRegisters[tid] = history->historyRegister;
    delete history;
}


bool
GShareBP::lookup(ThreadID tid, Addr branchAddr, void * &bpHistory) {
    unsigned historyIdx = historyRegisters[tid] & historyRegisterMask;



    unsigned predictorIdx = (((branchAddr >> instShiftAmt)
                                xor historyRegisters[tid])
                                & addressBitMask);

    bool finalPrediction =
        (counters[tid][historyIdx][predictorIdx].read() >> (ctrBits - 1) );

    BPHistory *history = new BPHistory;
    history->historyRegister = historyRegisters[tid];
    history->finalPred = finalPrediction;

    bpHistory = static_cast<void*>(history);
    updateGlobalHistReg(tid, finalPrediction);

    return finalPrediction;
}

void
GShareBP::btbUpdate(ThreadID tid, Addr branchAddr, void * &bpHistory) {
    historyRegisters[tid] &= (historyRegisterMask & ~ULL(1));
}

/* Only the selected direction predictor will be updated with the final
 * outcome; the status of the unselected one will not be altered. The choice
 * predictor is always updated with the branch outcome, except when the
 * choice is opposite to the branch outcome but the selected counter of
 * the direction predictors makes a correct final prediction.
 */
void
GShareBP::update(ThreadID tid, Addr branchAddr, bool taken, void *bpHistory,
                 bool squashed)
{
    if (bpHistory) {
        BPHistory *history = static_cast<BPHistory*>(bpHistory);

        unsigned predictorIdx = (((branchAddr >> instShiftAmt)
                                xor historyRegisters[tid])
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
GShareBP::retireSquashed(ThreadID tid, void *bp_history)
{
    BPHistory *history = static_cast<BPHistory*>(bp_history);
    delete history;
}

unsigned
GShareBP::getGHR(ThreadID tid, void *bp_history) const
{
    return static_cast<BPHistory*>(bp_history)->historyRegister;
}

void
GShareBP::updateGlobalHistReg(ThreadID tid, bool taken)
{
    historyRegisters[tid] = taken ? (historyRegisters[tid] << 1) | 1 :
                               (historyRegisters[tid] << 1);
    historyRegisters[tid] &= historyRegisterMask;
}

GShareBP*
GShareBPParams::create()
{
    return new GShareBP(this);
}
