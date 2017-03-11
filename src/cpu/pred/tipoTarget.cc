#include "tipoTarget.hh"

TipoTarget::TipoTarget():
target(0) {
}

TipoTarget::~TipoTarget(){
}

TipoTarget::TipoTarget(TheISA::PCState initial):
target(initial) {
}



TheISA::PCState
TipoTarget::getData() {

        return target;

}


void
TipoTarget::setData(TheISA::PCState value) {

        target = value;

}






