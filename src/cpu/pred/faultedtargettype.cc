#include "faultedtargettype.hh"

FaultedTargetType::FaultedTargetType(uint64_t numBit,char value,
        TheISA::PCState initial)
:TargetType(initial)  {
        mask = 1 << numBit;

        faultValue = value;

        if ( value == 0 ) {
                mask = ~mask;
        }

}

FaultedTargetType::FaultedTargetType(uint64_t numBit,char value)
:TargetType()  {
        mask = 1 << numBit;

        faultValue = value;

        if ( value == 0 ) {
                mask = ~mask;
        }

}

TheISA::PCState
FaultedTargetType::getData() {

        TheISA::PCState faultedPC;

        faultedPC = TargetType::getData();

        faultedPC.set(
                        faultValue ? faultedPC.instAddr() | mask
                        : faultedPC.instAddr() & mask );

        return faultedPC;

}

TheISA::PCState 
FaultedTargetType::getInitData()
{
        TheISA::PCState faultedPC;
        faultedPC = TargetType::getData();
        return faultedPC;
}

void
FaultedTargetType::setData(TheISA::PCState value) {

        TargetType::setData(value);

}






