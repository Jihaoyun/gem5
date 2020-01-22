#ifndef __CPU_PRED_TYPE_HH__
#define __CPU_PRED_TYPE_HH__



#include <stdint.h>

class Type
{
        public:

        Type();
        ~Type();
        Type(uint64_t initial_value);
        virtual uint64_t getData();
        uint64_t getInitData();

        void setData(uint64_t value);

        private:

        uint64_t realdato;

};

#endif
