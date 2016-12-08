#ifndef __CPU_PRED__FAULTINJECTED_HH__
#define __CPU_PRED__FAULTINJECTED_HH__



namespace FaultBPU {

struct injFault {
        /** Whether the fault is enabled or not */
        bool enabled;
        /** Injected fault label */
        std::string label;
        /** Faulted value */
        unsigned stuckBit;
        /** 0 = Tag, 1 = Target, 2 = Valid */
        unsigned field;
        /** The entry number where we are injecting the fault */
        unsigned entry;
        /** The bit target of the fault */
        unsigned bitPosition;
        /** Whether the fault is permantent or not.
            If it is permanent we ignore tickBegin and tickEnd */
        bool permanent;
        /** Fault begin time */
        int64_t tickBegin;
        /** Fault end time */
        int64_t tickEnd;

        injFault(bool _enabled, std::string _label, unsigned
                _stuckBit, unsigned _field, unsigned _entry,
                unsigned _bitPosition, bool _permanent,
                unsigned _tickBegin, unsigned _tickEnd) :
            enabled(_enabled),
            label(_label),
            stuckBit(_stuckBit),
            field(_field),
            entry(_entry),
            bitPosition(_bitPosition),
            permanent(_permanent),
            tickBegin(_tickBegin),
            tickEnd(_tickEnd)
             {}
};

}

#endif
