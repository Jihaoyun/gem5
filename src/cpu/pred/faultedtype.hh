#ifndef __CPU_PRED_FAULTEDTYPE_HH__
#define __CPU_PRED_FAULTEDTYPE_HH__


#include <stdint.h>

#include "type.hh"

class FaultedType : public Type
{
        public:

        FaultedType(uint64_t numBit, char value);
        FaultedType(uint64_t numBit,char value,
        uint64_t initial_value);


        virtual uint64_t getData();
        uint64_t getInitData();

        void setData(uint64_t value);

        private:
        uint64_t mask;
        char faultValue;
};

#endif
