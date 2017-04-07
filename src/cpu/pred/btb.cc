/*
 * Copyright (c) 2004-2005 The Regents of The University of Michigan
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
#include "base/trace.hh"
#include "cpu/pred/btb.hh"
#include "debug/Fetch.hh"

DefaultBTB::DefaultBTB(unsigned _numEntries,
                       unsigned _tagBits,
                       unsigned _instShiftAmt,
                       unsigned _num_threads)
    : numEntries(_numEntries),
      tagBits(_tagBits),
      instShiftAmt(_instShiftAmt),
      log2NumThreads(floorLog2(_num_threads))
{
    DPRINTF(Fetch, "BTB: Creating BTB object.\n");

    if (!isPowerOf2(numEntries)) {
        fatal("BTB entries is not a power of 2!");
    }

    btb.resize(numEntries);

    for (unsigned i = 0; i < numEntries; ++i) {
        btb[i].setValid(false);
    }

    idxMask = numEntries - 1;

    tagMask = (1 << tagBits) - 1;

    tagShiftAmt = instShiftAmt + floorLog2(numEntries);
}


DefaultBTB::DefaultBTB(unsigned _numEntries,
                       unsigned _tagBits,
                       unsigned _instShiftAmt,
                       unsigned _num_threads,
                       FaultBPU::injFault f_parameters)
    : numEntries(_numEntries),
      tagBits(_tagBits),
      instShiftAmt(_instShiftAmt),
      log2NumThreads(floorLog2(_num_threads))
{
    DPRINTF(Fetch, "BTB: Creating BTB object.\n");

    if (!isPowerOf2(numEntries)) {
        fatal("BTB entries is not a power of 2!");
    }

        // TODO: check if the entry is in the range of the BTB
        btb.resize(numEntries);
        if ( f_parameters.enabled && f_parameters.tickEnd == -1 ) {
                if (f_parameters.entry >= numEntries)
                        fatal("BP: FaultEntry exceeds dimension of the BTB");
                btb[f_parameters.entry].setFaulted(
                                f_parameters.field,
                f_parameters.bitPosition,
                f_parameters.stuckBit);
        }

    for (unsigned i = 0; i < numEntries; ++i) {
        btb[i].setValid(false);
    }

    idxMask = numEntries - 1;

    tagMask = (1 << tagBits) - 1;

    tagShiftAmt = instShiftAmt + floorLog2(numEntries);

}


void
DefaultBTB::setFault(struct FaultBPU::injFault f_parameters,bool faultEnd) {

  btb[f_parameters.entry].setTranFaulted(
          f_parameters.field,
          f_parameters.bitPosition);
}



void
DefaultBTB::reset()
{
    for (unsigned i = 0; i < numEntries; ++i) {
        btb[i].setValid(false);
    }
}


unsigned
DefaultBTB::getIndex(Addr instPC, ThreadID tid)
{
    // Need to shift PC over by the word offset.
    return ((instPC >> instShiftAmt)
            ^ (tid << (tagShiftAmt - instShiftAmt - log2NumThreads)))
            & idxMask;
}

inline
Addr
DefaultBTB::getTag(Addr instPC)
{
    return (instPC >> tagShiftAmt) & tagMask;
}

bool
DefaultBTB::valid(Addr instPC, ThreadID tid)
{
    unsigned btb_idx = getIndex(instPC, tid);

    Addr inst_tag = getTag(instPC);


    assert(btb_idx < numEntries);

    if (btb[btb_idx].getValid()
        && inst_tag == btb[btb_idx].getTag()
        && btb[btb_idx].getTid() == tid) {
        return true;
    } else {
        return false;
    }
}

// @todo Create some sort of return struct that has both whether or not the
// address is valid, and also the address.  For now will just use addr = 0 to
// represent invalid entry.
TheISA::PCState
DefaultBTB::lookup(Addr instPC, ThreadID tid)
{
    unsigned btb_idx = getIndex(instPC, tid);

    Addr inst_tag = getTag(instPC);

    assert(btb_idx < numEntries);


    if ( btb[btb_idx].getValid()
        && inst_tag == btb[btb_idx].getTag()
        && btb[btb_idx].getTid() == tid) {
        return btb[btb_idx].getTarget();
    } else {
        return 0;
    }
}

void
DefaultBTB::update(Addr instPC,const TheISA::PCState &target, ThreadID tid)
{
    unsigned btb_idx = getIndex(instPC, tid);

    assert(btb_idx < numEntries);

    btb[btb_idx].setTid(tid);
    btb[btb_idx].setValid(true);
    btb[btb_idx].setTarget( target );
    btb[btb_idx].setTag(getTag(instPC));
}


DefaultBTB::~DefaultBTB(){
    printf("\nSperiamo che me la cavi\n");
}





