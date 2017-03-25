#ifndef __CPU_PRED_FAULTEDTARGETTYPE_HH__
#define __CPU_PRED_FAULTEDTARGETTYPE_HH__


#include <stdint.h>

#include "targettype.hh"

class FaultedTargetType : public TargetType
{
        public:

        FaultedTargetType(uint64_t numBit, char value);
        FaultedTargetType(uint64_t numBit, char value,
                TheISA::PCState initial);
        virtual TheISA::PCState getData();


        void setData(TheISA::PCState value);

        private:
        uint64_t mask;
        char faultValue;
};

#endif
