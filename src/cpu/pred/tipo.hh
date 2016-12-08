#ifndef __CPU_PRED_TIPO_HH__
#define __CPU_PRED_TIPO_HH__



#include <stdint.h>

class Tipo
{
        public:

        Tipo();

        virtual uint64_t getData();

        void setData(uint64_t value);

        private:

        uint64_t realdato;

};

#endif
