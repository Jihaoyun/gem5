#ifndef __CPU_PRED_TIPOTARGETSPECIALE_HH__
#define __CPU_PRED_TIPOTARGETSPECIALE_HH__


#include <stdint.h>

#include "tipoTarget.hh"

class TipoTargetSpeciale : public TipoTarget
{
        public:

        TipoTargetSpeciale(uint64_t numBit, char value);

        virtual TheISA::PCState getData();


        void setData(TheISA::PCState value);

        private:
        uint64_t mask;
        char faultValue;
};

#endif
