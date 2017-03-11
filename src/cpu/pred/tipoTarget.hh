#ifndef __CPU_PRED_TIPOTARGET_HH__
#define __CPU_PRED_TIPOTARGET_HH__



#include <stdint.h>

#include "arch/types.hh"
#include "base/misc.hh"
#include "base/types.hh"
#include "config/the_isa.hh"

class TipoTarget
{
        public:

        TipoTarget();
        ~TipoTarget();
        TipoTarget(TheISA::PCState initial);
        virtual TheISA::PCState getData();

        void setData(TheISA::PCState value);

        private:

        TheISA::PCState target;

};

#endif
