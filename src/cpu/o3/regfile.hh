/*
 * Copyright (c) 2004-2005 The Regents of The University of Michigan
 * Copyright (c) 2013 Advanced Micro Devices, Inc.
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
 *          Gabe Black
 */

#ifndef __CPU_O3_REGFILE_HH__
#define __CPU_O3_REGFILE_HH__

#include <vector>

#include "arch/isa_traits.hh"
#include "arch/kernel_stats.hh"
#include "arch/types.hh"
#include "base/trace.hh"
#include "config/the_isa.hh"
#include "cpu/o3/comm.hh"
#include "debug/IEW.hh"
#include "debug/O3Registers.hh"

class UnifiedFreeList;

/**
 * Simple physical register file class.
 */
class PhysRegFile
{
  private:

    typedef TheISA::IntReg IntReg;
    typedef TheISA::FloatReg FloatReg;
    typedef TheISA::FloatRegBits FloatRegBits;
    typedef TheISA::CCReg CCReg;

    typedef union {
        FloatReg d;
        FloatRegBits q;
    } PhysFloatReg;

    /** Integer register file. */
    std::vector<IntReg> intRegFile;

    /** Integer register file faults*/
    std::vector<IntReg> intRegFileFault;

    /** Integer register file fault mask*/
    std::vector<IntReg> intRegFileMask;

    /** Floating point register file. */
    std::vector<PhysFloatReg> floatRegFile;

    /** Floating point register file fault. */
    std::vector<PhysFloatReg> floatRegFileFault;

    /** Floating point register file fault mask. */
    std::vector<PhysFloatReg> floatRegFileMask;

    /** Condition-code register file. */
    std::vector<CCReg> ccRegFile;

    /** Condition-code register file fault. */
    std::vector<CCReg> ccRegFileFault;

    /** Condition-code register file fault mask. */
    std::vector<CCReg> ccRegFileMask;

    /**
     * The first floating-point physical register index.  The physical
     * register file has a single continuous index space, with the
     * initial indices mapping to the integer registers, followed
     * immediately by the floating-point registers.  Thus the first
     * floating-point index is equal to the number of integer
     * registers.
     *
     * Note that this internal organizational detail on how physical
     * register file indices are ordered should *NOT* be exposed
     * outside of this class.  Other classes can use the is*PhysReg()
     * methods to map from a physical register index to a class
     * without knowing the internal structure of the index map.
     */
    unsigned baseFloatRegIndex;

    /**
     * The first condition-code physical register index.  The
     * condition-code registers follow the floating-point registers.
     */
    unsigned baseCCRegIndex;

    /** Total number of physical registers. */
    unsigned totalNumRegs;

  public:
    /**
     * Constructs a physical register file with the specified amount of
     * integer and floating point registers.
     */
    PhysRegFile(unsigned _numPhysicalIntRegs,
                unsigned _numPhysicalFloatRegs,
                unsigned _numPhysicalCCRegs);

    /**
     * Destructor to free resources
     */
    ~PhysRegFile() {}

    /** Initialize the free list */
    void initFreeList(UnifiedFreeList *freeList);

    /** @return the number of integer physical registers. */
    unsigned numIntPhysRegs() const { return baseFloatRegIndex; }

    /** @return the number of floating-point physical registers. */
    unsigned numFloatPhysRegs() const
    { return baseCCRegIndex - baseFloatRegIndex; }

    /** @return the number of condition-code physical registers. */
    unsigned numCCPhysRegs() const
    { return totalNumRegs - baseCCRegIndex; }

    /** @return the total number of physical registers. */
    unsigned totalNumPhysRegs() const { return totalNumRegs; }

    /**
     * @return true if the specified physical register index
     * corresponds to an integer physical register.
     */
    bool isIntPhysReg(PhysRegIndex reg_idx) const
    {
        return 0 <= reg_idx && reg_idx < baseFloatRegIndex;
    }

    /**
     * @return true if the specified physical register index
     * corresponds to a floating-point physical register.
     */
    bool isFloatPhysReg(PhysRegIndex reg_idx) const
    {
        return (baseFloatRegIndex <= reg_idx && reg_idx < baseCCRegIndex);
    }

    /**
     * Return true if the specified physical register index
     * corresponds to a condition-code physical register.
     */
    bool isCCPhysReg(PhysRegIndex reg_idx)
    {
        return (baseCCRegIndex <= reg_idx && reg_idx < totalNumRegs);
    }

    /** Reads an integer register. */
    uint64_t readIntReg(PhysRegIndex reg_idx) const
    {
        assert(isIntPhysReg(reg_idx));

        DPRINTF(IEW, "RegFile: Access to int register %i, has data "
                "%#x\n", int(reg_idx), intRegFile[reg_idx]);
        DPRINTF(O3Registers, "RegFile: Access to int register %i, has data "
                "%#x\n", int(reg_idx), intRegFile[reg_idx]);
        return intRegFile[reg_idx];
    }

    /** Reads an integer register with fault. */
    uint64_t readIntRegWithFault(PhysRegIndex reg_idx) const
    {
        assert(isIntPhysReg(reg_idx));

        uint64_t faultyRegFile = (intRegFile[reg_idx] & ~intRegFileMask[reg_idx]) | 
                (intRegFileFault[reg_idx] & intRegFileMask[reg_idx]);

        DPRINTF(IEW, "RegFile: Access to int register %i, has data "
                "%#x and faulty value %#x\n", int(reg_idx), intRegFile[reg_idx], faultyRegFile);

        DPRINTF(O3Registers, "RegFile: Access to int register %i, has data "
                "%#x and faulty value %#x\n", int(reg_idx), intRegFile[reg_idx], faultyRegFile);

        return (intRegFile[reg_idx] & ~intRegFileMask[reg_idx]) | (intRegFileFault[reg_idx] & intRegFileMask[reg_idx]);
    }

    /** Reads a floating point register (double precision). */
    FloatReg readFloatReg(PhysRegIndex reg_idx) const
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        DPRINTF(IEW, "RegFile: Access to float register %i, has "
                "data %#x\n", int(reg_idx), floatRegFile[reg_offset].q);

        DPRINTF(O3Registers, "RegFile: Access to float register %i, has "
                "data %#x\n", int(reg_idx), floatRegFile[reg_offset].q);

        return floatRegFile[reg_offset].d;
    }

    /** Reads a floating point register (double precision) with fault. */
    FloatReg readFloatRegWithFault(PhysRegIndex reg_idx) const
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        PhysFloatReg valueWithFault;

        valueWithFault.q = (floatRegFile[reg_offset].q & ~floatRegFileMask[reg_offset].q) |
            (floatRegFileFault[reg_offset].q & floatRegFileMask[reg_offset].q);

        DPRINTF(IEW, "RegFile: Access to float register %i, has "
                "data %#x and faulty value %#x\n", int(reg_idx), floatRegFile[reg_offset].q, 
                valueWithFault.q);

        DPRINTF(O3Registers, "RegFile: Access to float register %i, has "
                "data %#x and faulty value %#x\n", int(reg_idx), floatRegFile[reg_offset].q, 
                valueWithFault.q);

        return valueWithFault.d;
    }

    FloatRegBits readFloatRegBits(PhysRegIndex reg_idx) const
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        FloatRegBits floatRegBits = floatRegFile[reg_offset].q;

        DPRINTF(IEW, "RegFile: Access to float register %i as int, "
                "has data %#x\n", int(reg_idx), (uint64_t)floatRegBits);

        DPRINTF(O3Registers, "RegFile: Access to float register %i as int, "
                "has data %#x\n", int(reg_idx), (uint64_t)floatRegBits);

        return floatRegBits;
    }

    FloatRegBits readFloatRegBitsWithFault(PhysRegIndex reg_idx) const
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        FloatRegBits floatRegBits = (floatRegFile[reg_offset].q & ~floatRegFileMask[reg_offset].q) |
            (floatRegFileFault[reg_offset].q & floatRegFileMask[reg_offset].q);

        DPRINTF(IEW, "RegFile: Access to float register %i as int, "
                "has data %#x and faulty value %#x\n", int(reg_idx), 
                (uint64_t)floatRegFile[reg_offset].q, (uint64_t)floatRegBits);

        DPRINTF(O3Registers, "RegFile: Access to float register %i as int, "
                "has data %#x and faulty value %#x\n", int(reg_idx), 
                (uint64_t)floatRegFile[reg_offset].q, (uint64_t)floatRegBits);

        return floatRegBits;
    }

    /** Reads a condition-code register. */
    CCReg readCCReg(PhysRegIndex reg_idx)
    {
        assert(isCCPhysReg(reg_idx));

        // Remove the base CC reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseCCRegIndex;

        DPRINTF(IEW, "RegFile: Access to cc register %i, has "
                "data %#x\n", int(reg_idx), ccRegFile[reg_offset]);

        DPRINTF(O3Registers, "RegFile: Access to cc register %i, has "
                "data %#x\n", int(reg_idx), ccRegFile[reg_offset]);

        return ccRegFile[reg_offset];
    }

    /** Reads a condition-code register with fault. */
    CCReg readCCRegWithFault(PhysRegIndex reg_idx)
    {
        assert(isCCPhysReg(reg_idx));

        // Remove the base CC reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseCCRegIndex;

        CCReg faultyRegFile = (ccRegFile[reg_offset] & ~ccRegFileMask[reg_offset]) | 
            (ccRegFileFault[reg_offset] & ccRegFileMask[reg_offset]);

        DPRINTF(IEW, "RegFile: Access to cc register %i, has "
                "data %#x and faulty value %#x\n", int(reg_idx), ccRegFile[reg_offset],
                faultyRegFile);

        DPRINTF(O3Registers, "RegFile: Access to cc register %i, has "
                "data %#x and faulty value %#x\n", int(reg_idx), ccRegFile[reg_offset],
                faultyRegFile);

        return (ccRegFile[reg_offset] & ~ccRegFileMask[reg_offset]) | 
            (ccRegFileFault[reg_offset] & ccRegFileMask[reg_offset]);
    }

    /** Sets an integer register to the given value. */
    void setIntReg(PhysRegIndex reg_idx, uint64_t val)
    {
        assert(isIntPhysReg(reg_idx));

        DPRINTF(IEW, "RegFile: Setting int register %i to %#x\n",
                int(reg_idx), val);

        DPRINTF(O3Registers, "RegFile: Setting int register %i to %#x\n",
                int(reg_idx), val);

        if (reg_idx != TheISA::ZeroReg)
            intRegFile[reg_idx] = val;
    }

    /** Sets a fault in an integer register. */
    void setIntRegFault(PhysRegIndex reg_idx, uint64_t numBit, char value)
    {
        assert(isIntPhysReg(reg_idx));

        DPRINTF(IEW, "RegFile: Setting fault in int register %i at bit %i to %#x\n",
                int(reg_idx), int(numBit), (uint64_t)value);

        DPRINTF(O3Registers, "RegFile: Setting fault in int register %i at bit %i to %#x\n",
                int(reg_idx), int(numBit), (uint64_t)value);

        int oldMask = (intRegFileMask[reg_idx] >> numBit) % 2;

        assert(oldMask == 0);

        if (reg_idx != TheISA::ZeroReg) {
            intRegFileMask[reg_idx] += 1 << numBit;
            intRegFileFault[reg_idx] += value << numBit;
        }
    }

    /** Resets a fault in an integer register. */
    void resetIntRegFault(PhysRegIndex reg_idx, uint64_t numBit)
    {
        assert(isIntPhysReg(reg_idx));

        DPRINTF(IEW, "RegFile: Resetting fault in int register %i at bit %i \n",
                int(reg_idx), int(numBit));

        DPRINTF(O3Registers, "RegFile: Resetting fault in int register %i at bit %i \n",
                int(reg_idx), int(numBit));

        int mask = (intRegFileMask[reg_idx] >> numBit) % 2;
        int faultValue = (intRegFileFault[reg_idx] >> numBit) % 2;

        assert(mask == 1);

        if (reg_idx != TheISA::ZeroReg) {
            intRegFileMask[reg_idx] -= 1 << numBit;
            if (faultValue)
                intRegFileFault[reg_idx] -= faultValue << numBit;
        }
    }

    /** Inject a transient fault in physical reg file (flip a bit in the reg). */
    void setIntRegTransFault(PhysRegIndex reg_idx, uint64_t numBit)
    {
        assert(isIntPhysReg(reg_idx));

        DPRINTF(IEW, "RegFile: Transient fault in int register %i at bit %i \n",
                int(reg_idx), int(numBit));

        DPRINTF(O3Registers, "RegFile: Transient fault in int register %i at bit %i, before fault: %#x, ",
                int(reg_idx), int(numBit), uint64_t(intRegFile[reg_idx]));

        uint64_t mask = 1 << numBit;

        if (reg_idx != TheISA::ZeroReg) {
            intRegFile[reg_idx] = intRegFile[reg_idx] ^ mask;
        }

        DPRINTF(O3Registers, "after fault: %#x\n",
                uint64_t(intRegFile[reg_idx]));
    }

    /** Sets a double precision floating point register to the given value. */
    void setFloatReg(PhysRegIndex reg_idx, FloatReg val)
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        DPRINTF(IEW, "RegFile: Setting float register %i to %#x\n",
                int(reg_idx), (uint64_t)val);

        DPRINTF(O3Registers, "RegFile: Setting float register %i to %#x\n",
                int(reg_idx), (uint64_t)val);

#if THE_ISA == ALPHA_ISA
        if (reg_offset != TheISA::ZeroReg)
#endif
            floatRegFile[reg_offset].d = val;
    }

    /** Sets a fault in a double precision floating point register. */
    void setFloatRegFault(PhysRegIndex reg_idx, uint64_t numBit, char value)
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        DPRINTF(IEW, "RegFile: Setting fault in float register %i at bit %i to %#x\n",
                int(reg_idx), int(numBit), (uint64_t)value);

        DPRINTF(O3Registers, "RegFile: Setting fault in float register %i at bit %i to %#x\n",
                int(reg_idx), int(numBit), (uint64_t)value);

        int oldMask = (floatRegFileMask[reg_offset].q >> numBit) % 2;

        assert(oldMask == 0);

        floatRegFileMask[reg_offset].q += 1 << numBit;
        floatRegFileFault[reg_offset].q += value << numBit;
    }

    /** Resets a fault in a double precision floating point register. */
    void resetFloatRegFault(PhysRegIndex reg_idx, uint64_t numBit)
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        DPRINTF(IEW, "RegFile: Resetting fault in float register %i at bit %i \n",
                int(reg_idx), int(numBit));

        DPRINTF(O3Registers, "RegFile: Resetting fault in float register %i at bit %i \n",
                int(reg_idx), int(numBit));

        int mask = (floatRegFileMask[reg_offset].q >> numBit) % 2;
        int faultValue = (floatRegFileFault[reg_offset].q >> numBit) % 2;

        assert(mask == 1);

        floatRegFileMask[reg_offset].q -= 1 << numBit;
        if (faultValue)
            floatRegFileFault[reg_offset].q -= faultValue << numBit;
    }

    /** Inject a transient fault in a double precision floating point register. */
    void setFloatRegTransFault(PhysRegIndex reg_idx, uint64_t numBit)
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        DPRINTF(IEW, "RegFile: Transient fault in float register %i at bit %i \n",
                int(reg_idx), int(numBit));

        DPRINTF(O3Registers, "RegFile: Transient fault in float register %i at bit %i, before fault: %#x, ",
                int(reg_idx), int(numBit), uint64_t(floatRegFile[reg_offset].q));

        uint64_t mask = 1 << numBit;


        floatRegFile[reg_offset].q = floatRegFile[reg_offset].q ^ mask;

        DPRINTF(O3Registers, "after fault: %#x\n",
                uint64_t(floatRegFile[reg_offset].q));
    }

    void setFloatRegBits(PhysRegIndex reg_idx, FloatRegBits val)
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        DPRINTF(IEW, "RegFile: Setting float register %i to %#x\n",
                int(reg_idx), (uint64_t)val);

        DPRINTF(O3Registers, "RegFile: Setting float register %i to %#x\n",
                int(reg_idx), (uint64_t)val);

        floatRegFile[reg_offset].q = val;
    }

    /** Sets a fault in a double precision floating point register. */
    void setFloatRegBitsFault(PhysRegIndex reg_idx, uint64_t numBit, char value)
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        DPRINTF(IEW, "RegFile: Setting fault in float register %i at bit %i to %#x\n",
                int(reg_idx), int(numBit), (uint64_t)value);

        DPRINTF(O3Registers, "RegFile: Setting fault in float register %i at bit %i to %#x\n",
                int(reg_idx), int(numBit), (uint64_t)value);

        int oldMask = (floatRegFileMask[reg_offset].q >> numBit) % 2;

        assert(oldMask == 0);

        floatRegFileMask[reg_offset].q += 1 << numBit;
        floatRegFileFault[reg_offset].q += value << numBit;
    }

    /** Resets a fault in a double precision floating point register. */
    void resetFloatRegBitsFault(PhysRegIndex reg_idx, uint64_t numBit)
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        DPRINTF(IEW, "RegFile: Resetting fault in float register %i at bit %i \n",
                int(reg_idx), int(numBit));

        DPRINTF(O3Registers, "RegFile: Resetting fault in float register %i at bit %i \n",
                int(reg_idx), int(numBit));

        int mask = (floatRegFileMask[reg_offset].q >> numBit) % 2;
        int faultValue = (floatRegFileFault[reg_offset].q >> numBit) % 2;

        assert(mask == 1);

        floatRegFileMask[reg_offset].q -= 1 << numBit;
        if (faultValue)
            floatRegFileFault[reg_offset].q -= faultValue << numBit;
    }

    /** Inject a transient fault in a double precision floating point register. */
    void setFloatRegBitsTransFault(PhysRegIndex reg_idx, uint64_t numBit)
    {
        assert(isFloatPhysReg(reg_idx));

        // Remove the base Float reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseFloatRegIndex;

        DPRINTF(IEW, "RegFile: Transient fault in float register %i at bit %i \n",
                int(reg_idx), int(numBit));

        DPRINTF(O3Registers, "RegFile: Transient fault in float register %i at bit %i, before fault: %#x, ",
                int(reg_idx), int(numBit), uint64_t(floatRegFile[reg_offset].q));

        uint64_t mask = 1 << numBit;


        floatRegFile[reg_offset].q = floatRegFile[reg_offset].q ^ mask;

        DPRINTF(O3Registers, "after fault: %#x\n",
                uint64_t(floatRegFile[reg_offset].q));
    }

    /** Sets a condition-code register to the given value. */
    void setCCReg(PhysRegIndex reg_idx, CCReg val)
    {
        assert(isCCPhysReg(reg_idx));

        // Remove the base CC reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseCCRegIndex;

        DPRINTF(IEW, "RegFile: Setting cc register %i to %#x\n",
                int(reg_idx), (uint64_t)val);

        DPRINTF(O3Registers, "RegFile: Setting cc register %i to %#x\n",
                int(reg_idx), (uint64_t)val);

        ccRegFile[reg_offset] = val;
    }

    /** Sets a fault in a condition-code register. */
    void setCCRegFault(PhysRegIndex reg_idx, uint64_t numBit, char value)
    {
        assert(isCCPhysReg(reg_idx));

        // Remove the base CC reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseCCRegIndex;

        DPRINTF(IEW, "RegFile: Setting fault in cc register %i at bit %i to %#x\n",
                int(reg_idx), int(numBit), (uint64_t)value);

        DPRINTF(O3Registers, "RegFile: Setting fault in cc register %i at bit %i to %#x\n",
                int(reg_idx), int(numBit), (uint64_t)value);

        int oldMask = (ccRegFileMask[reg_offset] >> numBit) % 2;

        assert(oldMask == 0);

        ccRegFileMask[reg_offset] += 1 << numBit;
        ccRegFileFault[reg_offset] += value << numBit;
    }

    /** Resets a fault in a condition-code register. */
    void resetCCRegFault(PhysRegIndex reg_idx, uint64_t numBit)
    {
        assert(isCCPhysReg(reg_idx));

        // Remove the base CC reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseCCRegIndex;

        DPRINTF(IEW, "RegFile: Resetting fault in cc register %i at bit %i \n",
                int(reg_idx), int(numBit));

        DPRINTF(O3Registers, "RegFile: Resetting fault in cc register %i at bit %i \n",
                int(reg_idx), int(numBit));

        int mask = (ccRegFileMask[reg_offset] >> numBit) % 2;
        int faultValue = (ccRegFileFault[reg_offset] >> numBit) % 2;

        assert(mask == 1);

        ccRegFileMask[reg_offset] -= 1 << numBit;
        if (faultValue)
            ccRegFileFault[reg_offset] -= faultValue << numBit;
    }

    /** Inject a transient fault in a condition-code register. */
    void setCCRegTransFault(PhysRegIndex reg_idx, uint64_t numBit)
    {
        assert(isCCPhysReg(reg_idx));

        // Remove the base CC reg dependency.
        PhysRegIndex reg_offset = reg_idx - baseCCRegIndex;

        DPRINTF(IEW, "RegFile: Transient fault in cc register %i at bit %i \n",
                int(reg_idx), int(numBit));

        DPRINTF(O3Registers, "RegFile: Transient fault in cc register %i at bit %i, before fault %#x, ",
                int(reg_idx), int(numBit), uint64_t(ccRegFile[reg_offset]) );

        uint64_t mask = 1 << numBit;
 
        ccRegFile[reg_offset] = ccRegFile[reg_offset] ^ mask;

        DPRINTF(O3Registers, "after fault %#x\n",
                uint64_t(ccRegFile[reg_offset]) );        
    }

};


#endif //__CPU_O3_REGFILE_HH__
