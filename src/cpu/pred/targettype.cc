#include "targettype.hh"

TargetType::TargetType():
target(0) {
}

TargetType::~TargetType(){
}

TargetType::TargetType(TheISA::PCState initial):
target(initial) {
}



TheISA::PCState
TargetType::getData() {

        return target;

}

TheISA::PCState
TargetType::getInitData() {

        return target;

}

void
TargetType::setData(TheISA::PCState value) {

        target = value;

}






