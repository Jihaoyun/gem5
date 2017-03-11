#include <iostream>       // std::cout

#include "tipospeciale.hh"

Tipospeciale::Tipospeciale(uint64_t numBit,char value) : Tipo()  {

        mask = 1 << numBit;

        faultValue = value;

        if ( value == 0 ) {
                mask = ~mask;
        }

}

Tipospeciale::Tipospeciale(uint64_t numBit,char value,
        uint64_t initial_value) : Tipo(initial_value)  {

        mask = 1 << numBit;

        faultValue = value;

        if ( value == 0 ) {
                mask = ~mask;
        }

}


uint64_t
Tipospeciale::getData() {
        // prende i valori da vectorRead;
        uint64_t value;

        if ( faultValue == 0 )
                value =  (Tipo::getData() & mask);
        else
                value =  (Tipo::getData() | mask);


        return value;


}

void
Tipospeciale::setData(uint64_t value) {

        Tipo::setData(value);
}






