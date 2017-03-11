#include "tipo.hh"

Tipo::Tipo() {
}

Tipo::~Tipo(){
}


Tipo::Tipo(uint64_t initial_value){
    realdato = initial_value;
}


uint64_t
Tipo::getData() {

        return realdato;

}


void
Tipo::setData(uint64_t value) {

        realdato = value;

}






