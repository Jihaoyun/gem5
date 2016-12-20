#ifndef __CPU_PRED_GSHARE_PRED_HH__
#define __CPU_PRED_GSHARE_PRED_HH__

#include "cpu/pred/bpred_unit.hh"
#include "cpu/pred/sat_counter.hh"
#include "params/GShareBP.hh"

class GShareBP : public BPredUnit
{
  public:
    GShareBP(const GShareBPParams *params);
    void uncondBranch(ThreadID tid, Addr pc, void * &bp_history);
    void squash(ThreadID tid, void *bp_history);
    bool lookup(ThreadID tid, Addr branch_addr, void * &bp_history);
    void btbUpdate(ThreadID tid, Addr branch_addr, void * &bp_history);
    void update(ThreadID tid, Addr branch_addr, bool taken, void *bp_history,
                bool squashed);
    void retireSquashed(ThreadID tid, void *bp_history);
    unsigned getGHR(ThreadID tid, void *bp_history) const;

  private:
    void updateGlobalHistReg(ThreadID tid, bool taken);

    struct BPHistory {
        unsigned historyRegister;
        bool finalPred;
    };

   // Each thread has its counters
    std::vector<std::vector<std::vector<SatCounter>>> counters;
   // Each thread has its own history register
   std::vector<unsigned> historyRegisters;
   // Mask whose value depends on the number of bit of the history register
   unsigned historyRegisterMask;
   unsigned historyBits;
   // Mask whose value depends on the number of bit
   // of the address used as index
   unsigned addressBitMask;
   // Number of bit of each saturating counter
   unsigned ctrBits;
};

#endif // __CPU_PRED_GSHARE_PRED_HH__
