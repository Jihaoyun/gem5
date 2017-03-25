/*
 * Copyright (c) 2005-2006 The Regents of The University of Michigan
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

#ifndef __CPU_PRED_SAT_COUNTER_HH__
#define __CPU_PRED_SAT_COUNTER_HH__

#include "base/misc.hh"
#include "base/types.hh"
#include "cpu/pred/faultedtype.hh"
#include "cpu/pred/type.hh"

/**
 * Private counter class for the internal saturating counters.
 * Implements an n bit saturating counter and provides methods to
 * increment, decrement, and read it.
 * @todo Consider making this something that more closely mimics a
 * built in class so you can use ++ or --.
 */
class SatCounter
{
  public:
    /**
     * Constructor for the counter.
     */
    SatCounter()
        : initialVal(0)
    {
      counter = new Type(0);
    }

    /**
     * Constructor for the counter.
     * @param bits How many bits the counter will have.
     */
    SatCounter(unsigned bits)
        : initialVal(0), maxVal((1 << bits) - 1)
    {
      counter = new Type(0);
    }

    /**
     * Constructor for the counter.
     * @param bits How many bits the counter will have.
     * @param initial_val Starting value for each counter.
     */
    SatCounter(unsigned bits, uint8_t initial_val)
        : initialVal(initial_val), maxVal((1 << bits) - 1)
    {
        // Check to make sure initial value doesn't exceed the max
        // counter value.
        if (initial_val > maxVal) {
            fatal("BP: Initial counter value exceeds max size.");
        }
        counter = new Type(initial_val);
    }

    /**
     * Sets the number of bits.
     */
    void setBits(unsigned bits) { maxVal = (1 << bits) - 1; }

    void reset() { counter->setData(initialVal); }

    /**
     * Increments the counter's current value.
     */
    void increment()
    {
        uint8_t counter_value = (*counter).getData();
        if ( counter_value < maxVal) {
            ++counter_value;
        }
        (*counter).setData(counter_value);
    }

    /**
     * Decrements the counter's current value.
     */
    void decrement()
    {
        uint8_t counter_value = (*counter).getData();
        if ( counter_value > 0) {
            --counter_value;
        }
        (*counter).setData(counter_value);
    }

    /**
     * Read the counter's value.
     */
    uint8_t read() const
    { return (uint8_t) (*counter).getData(); }

    void setFaulted(uint8_t numBit, uint8_t value)
    {
        Type* actual_ptr = counter;
        counter = new FaultedType(numBit,value,(*counter).getData());
        free(actual_ptr);
    }

    void setOriginal()
    {
        Type* actual_ptr = counter;
        counter = new Type((*counter).getData());
        free(actual_ptr);
    }

  private:
    uint8_t initialVal;
    uint8_t maxVal;
    Type* counter;
};

#endif // __CPU_PRED_SAT_COUNTER_HH__
