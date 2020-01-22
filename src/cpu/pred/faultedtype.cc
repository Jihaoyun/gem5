#include <fstream>
#include <iomanip>
#include <iostream>       // std::cout

#include "debug/MyFaultedType.hh"
#include "faultedtype.hh"

FaultedType::FaultedType(uint64_t numBit,char value) : Type()  {

        mask = 1 << numBit;

        faultValue = value;

        if ( value == 0 ) {
                mask = ~mask;
        }

}

FaultedType::FaultedType(uint64_t numBit,char value,
        uint64_t initial_value) : Type(initial_value)  {

        mask = 1 << numBit;

        faultValue = value;

        if ( value == 0 ) {
                mask = ~mask;
        }

}


uint64_t
FaultedType::getData() {
        // prende i valori da vectorRead;
        uint64_t value;

        if ( faultValue == 0 )
                value =  (Type::getData() & mask);
        else
                value =  (Type::getData() | mask);


        return value;


}

uint64_t 
FaultedType::getInitData() {
        uint64_t value;
        value =  Type::getData();
        return value;
}

void
FaultedType::setData(uint64_t value) {

        Type::setData(value);
}






