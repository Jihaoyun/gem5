#ifndef __CPU_PRED_TARGETTIPE_HH__
#define __CPU_PRED_TARGETTIPE_HH__



#include <stdint.h>

#include "arch/types.hh"
#include "base/misc.hh"
#include "base/types.hh"
#include "config/the_isa.hh"

class TargetType
{
        public:

        TargetType();
        ~TargetType();
        TargetType(TheISA::PCState initial);
        virtual TheISA::PCState getData();
        TheISA::PCState getInitData();

        void setData(TheISA::PCState value);

        private:

        TheISA::PCState target;

};

#endif
