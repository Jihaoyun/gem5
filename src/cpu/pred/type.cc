#include "type.hh"

Type::Type() {
}

Type::~Type(){
}


Type::Type(uint64_t initial_value){
    realdato = initial_value;
}


uint64_t
Type::getData() {

        return realdato;

}


void
Type::setData(uint64_t value) {

        realdato = value;

}






