#ifndef __CPU_PRED_TIPOSPECIALE_HH__
#define __CPU_PRED_TIPOSPECIALE_HH__


#include <stdint.h>

#include "tipo.hh"

class Tipospeciale : public Tipo
{
        public:

        Tipospeciale(uint64_t numBit, char value);

        virtual uint64_t getData();


        void setData(uint64_t value);

        private:
        uint64_t mask;
        char faultValue;
};

#endif
