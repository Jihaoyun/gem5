/*
 * Copyright (c) 2011-2012 ARM Limited
 * Copyright (c) 2013 Advanced Micro Devices, Inc.
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2001-2006 The Regents of The University of Michigan
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
 * Authors: Steve Reinhardt
 *          Nathan Binkert
 */

#ifndef __CPU_SIMPLE_THREAD_HH__
#define __CPU_SIMPLE_THREAD_HH__

#include "arch/decoder.hh"
#include "arch/isa.hh"
#include "arch/isa_traits.hh"
#include "arch/registers.hh"
#include "arch/tlb.hh"
#include "arch/types.hh"
#include "base/types.hh"
#include "config/the_isa.hh"
#include "cpu/thread_context.hh"
#include "cpu/thread_state.hh"
#include "debug/CCRegs.hh"
#include "debug/FloatRegs.hh"
#include "debug/IntRegs.hh"
#include "mem/page_table.hh"
#include "mem/request.hh"
#include "sim/byteswap.hh"
#include "sim/eventq.hh"
#include "sim/process.hh"
#include "sim/serialize.hh"
#include "sim/system.hh"

class BaseCPU;
class CheckerCPU;

class FunctionProfile;
class ProfileNode;

namespace TheISA {
    namespace Kernel {
        class Statistics;
    }
}

/**
 * The SimpleThread object provides a combination of the ThreadState
 * object and the ThreadContext interface. It implements the
 * ThreadContext interface so that a ProxyThreadContext class can be
 * made using SimpleThread as the template parameter (see
 * thread_context.hh). It adds to the ThreadState object by adding all
 * the objects needed for simple functional execution, including a
 * simple architectural register file, and pointers to the ITB and DTB
 * in full system mode. For CPU models that do not need more advanced
 * ways to hold state (i.e. a separate physical register file, or
 * separate fetch and commit PC's), this SimpleThread class provides
 * all the necessary state for full architecture-level functional
 * simulation.  See the AtomicSimpleCPU or TimingSimpleCPU for
 * examples.
 */

class SimpleThread : public ThreadState
{
  protected:
    typedef TheISA::MachInst MachInst;
    typedef TheISA::MiscReg MiscReg;
    typedef TheISA::FloatReg FloatReg;
    typedef TheISA::FloatRegBits FloatRegBits;
    typedef TheISA::CCReg CCReg;
  public:
    typedef ThreadContext::Status Status;

  protected:
    union {
        FloatReg f[TheISA::NumFloatRegs];
        FloatRegBits i[TheISA::NumFloatRegs];
    } floatRegs, floatRegsFault, floatRegsMask;
    TheISA::IntReg intRegs[TheISA::NumIntRegs];
    TheISA::IntReg intRegsFault[TheISA::NumIntRegs];
    TheISA::IntReg intRegsMask[TheISA::NumIntRegs];
#ifdef ISA_HAS_CC_REGS
    TheISA::CCReg ccRegs[TheISA::NumCCRegs];
    TheISA::CCReg ccRegsFault[TheISA::NumCCRegs];
    TheISA::CCReg ccRegsMask[TheISA::NumCCRegs];
#endif
    TheISA::ISA *const isa;    // one "instance" of the current ISA.

    TheISA::PCState _pcState;

    /** Did this instruction execute or is it predicated false */
    bool predicate;

  public:
    std::string name() const
    {
        return csprintf("%s.[tid:%i]", baseCpu->name(), tc->threadId());
    }

    ProxyThreadContext<SimpleThread> *tc;

    System *system;

    TheISA::TLB *itb;
    TheISA::TLB *dtb;

    TheISA::Decoder decoder;

    // constructor: initialize SimpleThread from given process structure
    // FS
    SimpleThread(BaseCPU *_cpu, int _thread_num, System *_system,
                 TheISA::TLB *_itb, TheISA::TLB *_dtb, TheISA::ISA *_isa,
                 bool use_kernel_stats = true);
    // SE
    SimpleThread(BaseCPU *_cpu, int _thread_num, System *_system,
                 Process *_process, TheISA::TLB *_itb, TheISA::TLB *_dtb,
                 TheISA::ISA *_isa);

    virtual ~SimpleThread();

    virtual void takeOverFrom(ThreadContext *oldContext);

    void regStats(const std::string &name);

    void copyState(ThreadContext *oldContext);

    void serialize(CheckpointOut &cp) const override;
    void unserialize(CheckpointIn &cp) override;
    void startup();

    /***************************************************************
     *  SimpleThread functions to provide CPU with access to various
     *  state.
     **************************************************************/

    /** Returns the pointer to this SimpleThread's ThreadContext. Used
     *  when a ThreadContext must be passed to objects outside of the
     *  CPU.
     */
    ThreadContext *getTC() { return tc; }

    void demapPage(Addr vaddr, uint64_t asn)
    {
        itb->demapPage(vaddr, asn);
        dtb->demapPage(vaddr, asn);
    }

    void demapInstPage(Addr vaddr, uint64_t asn)
    {
        itb->demapPage(vaddr, asn);
    }

    void demapDataPage(Addr vaddr, uint64_t asn)
    {
        dtb->demapPage(vaddr, asn);
    }

    void dumpFuncProfile();

    Fault hwrei();

    bool simPalCheck(int palFunc);

    /*******************************************
     * ThreadContext interface functions.
     ******************************************/

    BaseCPU *getCpuPtr() { return baseCpu; }

    TheISA::TLB *getITBPtr() { return itb; }

    TheISA::TLB *getDTBPtr() { return dtb; }

    CheckerCPU *getCheckerCpuPtr() { return NULL; }

    TheISA::Decoder *getDecoderPtr() { return &decoder; }

    System *getSystemPtr() { return system; }

    Status status() const { return _status; }

    void setStatus(Status newStatus) { _status = newStatus; }

    /// Set the status to Active.
    void activate();

    /// Set the status to Suspended.
    void suspend();

    /// Set the status to Halted.
    void halt();

    void copyArchRegs(ThreadContext *tc);

    void clearArchRegs()
    {
        _pcState = 0;
        memset(intRegs, 0, sizeof(intRegs));
        memset(intRegsMask, 0, sizeof(intRegs));
        memset(intRegsFault, 0, sizeof(intRegs));
        memset(floatRegs.i, 0, sizeof(floatRegs.i));
        memset(floatRegsMask.i, 0, sizeof(floatRegs.i));
        memset(floatRegsFault.i, 0, sizeof(floatRegs.i));
#ifdef ISA_HAS_CC_REGS
        memset(ccRegs, 0, sizeof(ccRegs));
        memset(ccRegsMask, 0, sizeof(ccRegs));
        memset(ccRegsFault, 0, sizeof(ccRegs));
#endif
        isa->clear();
    }

    //
    // New accessors for new decoder.
    //
    uint64_t readIntReg(int reg_idx)
    {
        int flatIndex = isa->flattenIntIndex(reg_idx);
        assert(flatIndex < TheISA::NumIntRegs);
        uint64_t regVal(readIntRegWithFaultFlat(flatIndex));
        DPRINTF(IntRegs, "Reading int reg %d (%d) as %#x.\n",
                reg_idx, flatIndex, regVal);
        return regVal;
    }

    FloatReg readFloatReg(int reg_idx)
    {
        int flatIndex = isa->flattenFloatIndex(reg_idx);
        assert(flatIndex < TheISA::NumFloatRegs);
        FloatReg regVal(readFloatRegWithFaultFlat(flatIndex));
        DPRINTF(FloatRegs, "Reading float reg %d (%d) as %f, %#x.\n",
                reg_idx, flatIndex, regVal, floatRegs.i[flatIndex]);
        return regVal;
    }

    FloatRegBits readFloatRegBits(int reg_idx)
    {
        int flatIndex = isa->flattenFloatIndex(reg_idx);
        assert(flatIndex < TheISA::NumFloatRegs);
        FloatRegBits regVal(readFloatRegBitsWithFaultFlat(flatIndex));
        DPRINTF(FloatRegs, "Reading float reg %d (%d) bits as %#x, %f.\n",
                reg_idx, flatIndex, regVal, floatRegs.f[flatIndex]);
        return regVal;
    }

    CCReg readCCReg(int reg_idx)
    {
#ifdef ISA_HAS_CC_REGS
        int flatIndex = isa->flattenCCIndex(reg_idx);
        assert(0 <= flatIndex);
        assert(flatIndex < TheISA::NumCCRegs);
        uint64_t regVal(readCCRegWithFaultFlat(flatIndex));
        DPRINTF(CCRegs, "Reading CC reg %d (%d) as %#x.\n",
                reg_idx, flatIndex, regVal);
        return regVal;
#else
        panic("Tried to read a CC register.");
        return 0;
#endif
    }

    void setIntReg(int reg_idx, uint64_t val)
    {
        int flatIndex = isa->flattenIntIndex(reg_idx);
        assert(flatIndex < TheISA::NumIntRegs);
        DPRINTF(IntRegs, "Setting int reg %d (%d) to %#x.\n",
                reg_idx, flatIndex, val);
        setIntRegFlat(flatIndex, val);
    }

    void setIntRegFault(int reg_idx, uint64_t numBit, char value)
    {
        int flatIndex = isa->flattenIntIndex(reg_idx);
        assert(flatIndex < TheISA::NumIntRegs);
        assert(value == 0 || value == 1);
        DPRINTF(IntRegs, "Setting fault in int reg %d (%d) at bit %d to %#x.\n",
                reg_idx, flatIndex, numBit, value);
        setIntRegFaultFlat(flatIndex, numBit, value);
    }

    void resetIntRegFault(int reg_idx, uint64_t numBit)
    {
        int flatIndex = isa->flattenIntIndex(reg_idx);
        assert(flatIndex < TheISA::NumIntRegs);
        DPRINTF(IntRegs, "Resetting fault in int reg %d (%d) at bit %d.\n",
                reg_idx, flatIndex, numBit);
        resetIntRegFaultFlat(flatIndex, numBit);
    }

    void setIntRegTransFault(int reg_idx, uint64_t numBit)
    {
        int flatIndex = isa->flattenIntIndex(reg_idx);
        uint64_t registerBits = readIntRegWithFaultFlat(flatIndex);
        assert(flatIndex < TheISA::NumIntRegs);
        DPRINTF(IntRegs, "Transient fault in int reg %d (%d) at bit %d, before fault: %#x, ",
                reg_idx, flatIndex, numBit, registerBits);
        setIntRegTransFaultFlat(flatIndex, numBit);
        registerBits = readIntRegWithFaultFlat(flatIndex);
        DPRINTF(IntRegs, "after fault: %#x\n",
                registerBits);
    }

    void setFloatReg(int reg_idx, FloatReg val)
    {
        int flatIndex = isa->flattenFloatIndex(reg_idx);
        assert(flatIndex < TheISA::NumFloatRegs);
        setFloatRegFlat(flatIndex, val);
        DPRINTF(FloatRegs, "Setting float reg %d (%d) to %f, %#x.\n",
                reg_idx, flatIndex, val, floatRegs.i[flatIndex]);
    }

    void setFloatRegFault(int reg_idx, uint64_t numBit, char value)
    {
        int flatIndex = isa->flattenFloatIndex(reg_idx);
        assert(flatIndex < TheISA::NumFloatRegs);
        assert(value == 0 || value == 1);
        setFloatRegFaultFlat(flatIndex, numBit, value);
        DPRINTF(FloatRegs, "Setting fault in float reg %d (%d) at bit %d to %f, %#x.\n",
                reg_idx, flatIndex, numBit, value, floatRegs.i[flatIndex]);
    }

    void resetFloatRegFault(int reg_idx, uint64_t numBit)
    {
        int flatIndex = isa->flattenFloatIndex(reg_idx);
        assert(flatIndex < TheISA::NumFloatRegs);
        DPRINTF(FloatRegs, "Resetting fault in float reg %d (%d) at bit %d.\n",
                reg_idx, flatIndex, numBit);
        resetFloatRegFaultFlat(flatIndex, numBit);
    }

    void setFloatRegTransFault(int reg_idx, uint64_t numBit)
    {
        int flatIndex = isa->flattenFloatIndex(reg_idx);
        uint64_t registerBits = readFloatRegBitsWithFaultFlat(flatIndex);
        assert(flatIndex < TheISA::NumFloatRegs);
        DPRINTF(FloatRegs, "Transient fault in float reg %d (%d) at bit %d, before fault: %#x, ",
                reg_idx, flatIndex, numBit, registerBits);
        setFloatRegTransFaultFlat(flatIndex, numBit);
        registerBits = readFloatRegBitsWithFaultFlat(flatIndex);
        DPRINTF(FloatRegs, "after fault: %#x\n",
                registerBits);
    }

    void setFloatRegBits(int reg_idx, FloatRegBits val)
    {
        int flatIndex = isa->flattenFloatIndex(reg_idx);
        assert(flatIndex < TheISA::NumFloatRegs);
        // XXX: Fix array out of bounds compiler error for gem5.fast
        // when checkercpu enabled
        if (flatIndex < TheISA::NumFloatRegs)
            setFloatRegBitsFlat(flatIndex, val);
        DPRINTF(FloatRegs, "Setting float reg %d (%d) bits to %#x, %#f.\n",
                reg_idx, flatIndex, val, floatRegs.f[flatIndex]);
    }

    void setFloatRegBitsFault(int reg_idx, uint64_t numBit, char value)
    {
        int flatIndex = isa->flattenFloatIndex(reg_idx);
        assert(flatIndex < TheISA::NumFloatRegs);
        assert(value == 0 || value == 1);
        // XXX: Fix array out of bounds compiler error for gem5.fast
        // when checkercpu enabled
        if (flatIndex < TheISA::NumFloatRegs)
            setFloatRegBitsFaultFlat(flatIndex, numBit, value);
        DPRINTF(FloatRegs, "Setting fault in float reg %d (%d) bits at bit %d to %#x, %#f.\n",
                reg_idx, flatIndex, numBit, value, floatRegs.f[flatIndex]);
    }

    void resetFloatRegBitsFault(int reg_idx, uint64_t numBit)
    {
        int flatIndex = isa->flattenFloatIndex(reg_idx);
        assert(flatIndex < TheISA::NumFloatRegs);
        DPRINTF(FloatRegs, "Resetting fault in float reg %d (%d) at bit %d.\n",
                reg_idx, flatIndex, numBit);
        if (flatIndex < TheISA::NumFloatRegs)
            resetFloatRegBitsFaultFlat(flatIndex, numBit);
    }

    void setFloatRegBitsTransFault(int reg_idx, uint64_t numBit)
    {
        int flatIndex = isa->flattenFloatIndex(reg_idx);
        uint64_t registerBits = readFloatRegBitsWithFaultFlat(flatIndex);
        assert(flatIndex < TheISA::NumFloatRegs);
        DPRINTF(FloatRegs, "Transient fault in float reg %d (%d) at bit %d, before fault: %#x, ",
                reg_idx, flatIndex, numBit, registerBits);
        setFloatRegBitsTransFaultFlat(flatIndex, numBit);
        registerBits = readFloatRegBitsWithFaultFlat(flatIndex);
        DPRINTF(FloatRegs, "after fault: %#x\n",
                registerBits);
    }

    void setCCReg(int reg_idx, CCReg val)
    {
#ifdef ISA_HAS_CC_REGS
        int flatIndex = isa->flattenCCIndex(reg_idx);
        assert(flatIndex < TheISA::NumCCRegs);
        DPRINTF(CCRegs, "Setting CC reg %d (%d) to %#x.\n",
                reg_idx, flatIndex, val);
        setCCRegFlat(flatIndex, val);
#else
        panic("Tried to set a CC register.");
#endif
    }

    void setCCRegFault(int reg_idx, uint64_t numBit, char value)
    {
#ifdef ISA_HAS_CC_REGS
        int flatIndex = isa->flattenCCIndex(reg_idx);
        assert(flatIndex < TheISA::NumCCRegs);
        assert(value == 0 || value == 1);
        DPRINTF(CCRegs, "Setting fault in CC reg %d (%d) at bit %d to %#x.\n",
                reg_idx, flatIndex, numBit, value);
        setCCRegFaultFlat(flatIndex, numBit, value);
#else
        panic("Tried to set a CC register.");
#endif
    }

    void resetCCRegFault(int reg_idx, uint64_t numBit)
    {
#ifdef ISA_HAS_CC_REGS
        int flatIndex = isa->flattenCCIndex(reg_idx);
        assert(flatIndex < TheISA::NumCCRegs);
        DPRINTF(CCRegs, "Resetting fault in CC reg %d (%d) at bit %d.\n",
                reg_idx, flatIndex, numBit);
        resetCCRegFaultFlat(flatIndex, numBit);
#else
        panic("Tried to set a CC register.");
#endif
    }

    void setCCRegTransFault(int reg_idx, uint64_t numBit)
    {
#ifdef ISA_HAS_CC_REGS
        int flatIndex = isa->flattenCCIndex(reg_idx);
        uint64_t registerBits = readCCRegWithFaultFlat(flatIndex);
        assert(flatIndex < TheISA::NumCCRegs);
        DPRINTF(CCRegs, "Transient fault in CC reg %d (%d) at bit %d, before fault: %#x, ",
                reg_idx, flatIndex, numBit, registerBits);
        setCCRegTransFaultFlat(flatIndex, numBit);
        registerBits = readCCRegWithFaultFlat(flatIndex);
        DPRINTF(CCRegs, "after fault: %#x, ",
                registerBits);
#else
        panic("Tried to set a CC register.");
#endif
    }

    TheISA::PCState
    pcState()
    {
        return _pcState;
    }

    void
    pcState(const TheISA::PCState &val)
    {
        _pcState = val;
    }

    void
    pcStateNoRecord(const TheISA::PCState &val)
    {
        _pcState = val;
    }

    Addr
    instAddr()
    {
        return _pcState.instAddr();
    }

    Addr
    nextInstAddr()
    {
        return _pcState.nextInstAddr();
    }

    MicroPC
    microPC()
    {
        return _pcState.microPC();
    }

    bool readPredicate()
    {
        return predicate;
    }

    void setPredicate(bool val)
    {
        predicate = val;
    }

    MiscReg
    readMiscRegNoEffect(int misc_reg, ThreadID tid = 0) const
    {
        return isa->readMiscRegNoEffect(misc_reg);
    }

    MiscReg
    readMiscReg(int misc_reg, ThreadID tid = 0)
    {
        return isa->readMiscReg(misc_reg, tc);
    }

    void
    setMiscRegNoEffect(int misc_reg, const MiscReg &val, ThreadID tid = 0)
    {
        return isa->setMiscRegNoEffect(misc_reg, val);
    }

    void
    setMiscReg(int misc_reg, const MiscReg &val, ThreadID tid = 0)
    {
        return isa->setMiscReg(misc_reg, val, tc);
    }

    int
    flattenIntIndex(int reg)
    {
        return isa->flattenIntIndex(reg);
    }

    int
    flattenFloatIndex(int reg)
    {
        return isa->flattenFloatIndex(reg);
    }

    int
    flattenCCIndex(int reg)
    {
        return isa->flattenCCIndex(reg);
    }

    int
    flattenMiscIndex(int reg)
    {
        return isa->flattenMiscIndex(reg);
    }

    unsigned readStCondFailures() { return storeCondFailures; }

    void setStCondFailures(unsigned sc_failures)
    { storeCondFailures = sc_failures; }

    void syscall(int64_t callnum)
    {
        process->syscall(callnum, tc);
    }

    uint64_t readIntRegFlat(int idx) { return intRegs[idx]; }
    uint64_t readIntRegWithFaultFlat(int idx) 
    { 
        return (intRegs[idx] & ~intRegsMask[idx]) | ((intRegsFault[idx] & intRegsMask[idx])); 
    }
    void setIntRegFlat(int idx, uint64_t val) { intRegs[idx] = val; }
    void setIntRegFaultFlat(int idx, uint64_t numBit, char value) 
    { 
        intRegsMask[idx] += 1 << numBit;
        intRegsFault[idx] += value << numBit; 
    }
    void resetIntRegFaultFlat(int idx, uint64_t numBit)
    {
        int mask = (intRegsMask[idx] >> numBit) % 2;
        int faultValue = (intRegsFault[idx] >> numBit) % 2;

        assert(mask == 1);

        intRegsMask[idx] -= 1 << numBit;
        if (faultValue)
            intRegsFault[idx] -= faultValue << numBit;
    }
    void setIntRegTransFaultFlat(int idx, uint64_t numBit)
    {
        uint64_t mask = 1 << numBit;

        intRegs[idx] = intRegs[idx] ^ mask;
    }

    FloatReg readFloatRegFlat(int idx) { return floatRegs.f[idx]; }
    FloatReg readFloatRegWithFaultFlat(int idx) 
    { 
        union {
            FloatReg f;
            FloatRegBits i;
        } valueWithFault;
        valueWithFault.i = (floatRegs.i[idx] & ~floatRegsMask.i[idx]) | (floatRegsFault.i[idx] & floatRegsMask.i[idx]);
        return valueWithFault.f; 
    }
    void setFloatRegFlat(int idx, FloatReg val) { floatRegs.f[idx] = val; }
    void setFloatRegFaultFlat(int idx, uint64_t numBit, char value) 
    { 
        floatRegsMask.i[idx] += 1 << numBit;
        floatRegsFault.i[idx] += value << numBit; 
    }
    void resetFloatRegFaultFlat(int idx, uint64_t numBit)
    {
        int mask = (floatRegsMask.i[idx] >> numBit) % 2;
        int faultValue = (floatRegsFault.i[idx] >> numBit) % 2;

        assert(mask == 1);

        floatRegsMask.i[idx] -= 1 << numBit;
        if (faultValue)
            floatRegsFault.i[idx] -= faultValue << numBit;
    }
    void setFloatRegTransFaultFlat(int idx, uint64_t numBit)
    {
        uint64_t mask = 1 << numBit;

        floatRegs.i[idx] = floatRegs.i[idx] ^ mask;
    }

    FloatRegBits readFloatRegBitsFlat(int idx) { return floatRegs.i[idx]; }
    FloatRegBits readFloatRegBitsWithFaultFlat(int idx) 
    { 
        return (floatRegs.i[idx] & ~floatRegsMask.i[idx]) | (floatRegsFault.i[idx] & floatRegsMask.i[idx]); 
    }
    void setFloatRegBitsFlat(int idx, FloatRegBits val) {
        floatRegs.i[idx] = val;
    }
    void setFloatRegBitsFaultFlat(int idx, uint64_t numBit, char value) {
        floatRegsMask.i[idx] += 1 << numBit;
        floatRegsFault.i[idx] += value << numBit;
    }
    void resetFloatRegBitsFaultFlat(int idx, uint64_t numBit)
    {
        int mask = (floatRegsMask.i[idx] >> numBit) % 2;
        int faultValue = (floatRegsFault.i[idx] >> numBit) % 2;

        assert(mask == 1);

        floatRegsMask.i[idx] -= 1 << numBit;
        if (faultValue)
            floatRegsFault.i[idx] -= faultValue << numBit;
    }
    void setFloatRegBitsTransFaultFlat(int idx, uint64_t numBit)
    {
        uint64_t mask = 1 << numBit;
        
        floatRegs.i[idx] = floatRegs.i[idx] ^ mask;
    }

#ifdef ISA_HAS_CC_REGS
    CCReg readCCRegFlat(int idx) { return ccRegs[idx]; }
    CCReg readCCRegWithFaultFlat(int idx) 
    { 
        return (ccRegs[idx] & ~ccRegsMask[idx]) | (ccRegsFault[idx] & ccRegsMask[idx]); 
    }
    void setCCRegFlat(int idx, CCReg val) { ccRegs[idx] = val; }
    void setCCRegFaultFlat(int idx, uint64_t numBit, char value) 
    { 
        ccRegsMask[idx] += 1 << numBit;
        ccRegsFault[idx] += value << numBit; 
    }
    void resetCCRegFaultFlat(int idx, uint64_t numBit)
    {
        int mask = (ccRegsMask[idx] >> numBit) % 2;
        int faultValue = (ccRegsFault[idx] >> numBit) % 2;

        assert(mask == 1);

        ccRegsMask[idx] -= 1 << numBit;
        if (faultValue)
            ccRegsFault[idx] -= faultValue << numBit;
    }
    void setCCRegTransFaultFlat(int idx, uint64_t numBit)
    {
        uint64_t mask = 1 << numBit;
        
        ccRegs[idx] = ccRegs[idx] ^ mask;
    }
#else
    CCReg readCCRegFlat(int idx)
    { panic("readCCRegFlat w/no CC regs!\n"); }

    CCReg readCCRegWithFaultFlat(int idx)
    { panic("readCCRegFlat w/no CC regs!\n"); }

    void setCCRegFlat(int idx, CCReg val)
    { panic("setCCRegFlat w/no CC regs!\n"); }

    void setCCRegFaultFlat(int idx, uint64_t numBit, char value)
    { panic("setCCRegFlat w/no CC regs!\n"); }

    void resetCCRegFaultFlat(int idx, uint64_t numBit)
    { panic("setCCRegFlat w/no CC regs!\n"); }

    void setCCRegTransFaultFlat(int idx, uint64_t numBit)
    { panic("setCCRegFlat w/no CC regs!\n"); }
#endif
};


#endif // __CPU_CPU_EXEC_CONTEXT_HH__
