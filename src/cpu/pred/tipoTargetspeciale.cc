#include "tipoTargetspeciale.hh"

TipoTargetSpeciale::TipoTargetSpeciale(uint64_t numBit,char value)
:TipoTarget()  {
        mask = 1 << numBit;

        faultValue = value;

        if ( value == 0 ) {
                mask = ~mask;
        }

}

TheISA::PCState
TipoTargetSpeciale::getData() {

        TheISA::PCState faultedPC;

        faultedPC = TipoTarget::getData();

        faultedPC.set(
                        faultValue ? faultedPC.instAddr() | mask
                        : faultedPC.instAddr() & mask );

        return faultedPC;

}

void
TipoTargetSpeciale::setData(TheISA::PCState value) {

        TipoTarget::setData(value);

}






