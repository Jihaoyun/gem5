#include "tipoTarget.hh"

TipoTarget::TipoTarget():
target(0) {
}


TheISA::PCState
TipoTarget::getData() {

        return target;

}


void
TipoTarget::setData(TheISA::PCState value) {

        target = value;

}






