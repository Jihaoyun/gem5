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

#ifndef __CPU_PRED_BTB_HH__
#define __CPU_PRED_BTB_HH__

#include "arch/types.hh"
#include "base/misc.hh"
#include "base/statistics.hh"
#include "base/types.hh"
#include "config/the_isa.hh"
#include "cpu/pred/faultInjected.hh"
#include "faultedtargettype.hh"
#include "faultedtype.hh"
#include "sim/sim_object.hh"
#include "debug/Fetch.hh"

class BTBEntry
{
        public:

        BTBEntry()
        {
                tag = new Type();
                target = new TargetType();
                valid = new Type(0);
        }

        void setFaulted(int field, int numBit, char value) {
                if ( field == 0 ) {
                    tag = new FaultedType(numBit,value);
                }
                if ( field == 1 ) {
                    target = new FaultedTargetType(numBit,value);
                }
                if (field == 2 ) {
                    valid = new FaultedType(numBit,value);
                }
        }

         void setOriginal(int field, int numBit, char value) {

              DPRINTF(Fetch, "BTB: Before fault reset, tag: 0x%x, target: 0x%x, valid: 0x%x", 
                  tag->getData(), target->getData().pc(), valid->getData());

                if ( field == 0 ) {
                    //Type* old_tag = tag;
                    tag = new Type(tag->getData());
                    //delete old_tag;
                }

                if ( field == 1 ) {
                    //TargetType* old_target = target;
                    target = new TargetType(target->getData());
                    //delete old_target;
                }

                if ( field == 2 ) {
                    //Type* old_valid = valid;
                    valid = new Type(valid->getData());
                    //delete old_valid;
                }

                DPRINTF(Fetch, "BTB: After fault reset, tag: 0x%x, target: 0x%x, valid: 0x%x", 
                  tag->getData(), target->getData().pc(), valid->getData());
        }

        void setInterFaulted(int field, int numBit, char value) {

          DPRINTF(Fetch, "BTB: Before fault set, tag: 0x%x, target: 0x%x, valid: 0x%x", 
                  tag->getData(), target->getData().pc(), valid->getData());

            if (field == 0) {
                uint64_t old_value = tag->getData();
                tag = new FaultedType(numBit, value);
                tag->setData(old_value);
            }
            if (field == 1) {
                uint64_t old_value = target->getData().pc();
                target = new FaultedTargetType(numBit, value);
                target->getData().set(old_value);
            }
            if (field == 2) {
                uint64_t old_value = valid->getData();
                valid = new FaultedType(numBit, value);
                valid->setData(old_value);
            }

            DPRINTF(Fetch, "BTB: After fault set, tag: 0x%x, target: 0x%x, valid: 0x%x", 
                  tag->getData(), target->getData().pc(), valid->getData());
        }

        void setTranFaulted(int field, int numBit) {

          DPRINTF(Fetch, "BTB: Before transient fault, tag: 0x%x, target: 0x%x, valid: 0x%x", 
                  tag->getData(), target->getData().pc(), valid->getData());

            uint64_t mask = 1 << numBit;
            if ( field == 0 ) {
              tag->setData(tag->getData() ^ mask );
            }
            if ( field == 1 ) {
              target->getData().set(target->getData().pc() ^ mask);
            }
            if ( field == 2 ) {
              valid->setData(valid->getData() ^ mask );;
            }

            DPRINTF(Fetch, "BTB: After transient fault, tag: 0x%x, target: 0x%x, valid: 0x%x", 
                  tag->getData(), target->getData().pc(), valid->getData());

        }

        bool getValid() {
                return valid->getData() & 0x1;
        }

        void setValid(bool value) {
                valid->setData(value);
        }

        uint64_t getTag() {
                return tag->getData();
        }

        void setTag(uint64_t value) {
                tag->setData(value);
        }

        TheISA::PCState getTarget() {
                return target->getData();
        }

        void setTarget(const TheISA::PCState value) {
                target->setData(value);
        }

        ThreadID getTid() {
                return tid;
        }

        void setTid(ThreadID ntid) {
                tid = ntid;
        }

        private:
        Type* tag;
        TargetType* target;
        Type* valid;
        ThreadID tid;

};





class DefaultBTB
{
    private:

        public:
    /** Creates a BTB with the given number of entries, number of bits per
     *  tag, and instruction offset amount.
     *  @param numEntries Number of entries for the BTB.
     *  @param tagBits Number of bits for each tag in the BTB.
     *  @param instShiftAmt Offset amount for instructions to ignore alignment.
     */
    DefaultBTB(unsigned numEntries, unsigned tagBits,
               unsigned instShiftAmt, unsigned numThreads);

        DefaultBTB(unsigned numEntries, unsigned tagBits,
               unsigned instShiftAmt, unsigned numThreads,
                           FaultBPU::injFault f_parameters);

        ~DefaultBTB();
    void reset();

    /** Looks up an address in the BTB. Must call valid() first on the address.
     *  @param inst_PC The address of the branch to look up.
     *  @param tid The thread id.
     *  @return Returns the target of the branch.
     */
    TheISA::PCState lookup(Addr instPC, ThreadID tid);

    /** Checks if a branch is in the BTB.
     *  @param inst_PC The address of the branch to look up.
     *  @param tid The thread id.
     *  @return Whether or not the branch exists in the BTB.
     */
    bool valid(Addr instPC, ThreadID tid);

    /** Updates the BTB with the target of a branch.
     *  @param inst_PC The address of the branch being updated.
     *  @param target_PC The target address of the branch.
     *  @param tid The thread id.
     */
    void update(Addr instPC, const TheISA::PCState &targetPC,
                ThreadID tid);

    unsigned getIndex(Addr instPC, ThreadID tid);

    void setFault(struct FaultBPU::injFault f_parameters, bool faultEnd);

    void setInterFault(struct FaultBPU::injFault f_parameters, bool faultEnd);

    void resetInterFault(struct FaultBPU::injFault f_parameters, bool faultEnd);

  private:
    /** Returns the index into the BTB, based on the branch's PC.
     *  @param inst_PC The branch to look up.
     *  @return Returns the index into the BTB.
     */



            /** Returns the tag bits of a given address.
     *  @param inst_PC The branch's address.
     *  @return Returns the tag bits.
     */
    inline Addr getTag(Addr instPC);

    /** The actual BTB. */

        std::vector<BTBEntry> btb;


    /** The number of entries in the BTB. */
    unsigned numEntries;

    /** The index mask. */
    unsigned idxMask;

    /** The number of tag bits per entry. */
    unsigned tagBits;

    /** The tag mask. */
    unsigned tagMask;

    /** Number of bits to shift PC when calculating index. */
    unsigned instShiftAmt;

    /** Number of bits to shift PC when calculating tag. */
    unsigned tagShiftAmt;

    /** Log2 NumThreads used for hashing threadid */
    unsigned log2NumThreads;
};

#endif // __CPU_PRED_BTB_HH__
