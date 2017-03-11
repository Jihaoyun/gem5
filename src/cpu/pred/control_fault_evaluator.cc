#include "cpu/pred/control_fault_evaluator.hh"

using namespace Evaluator;

bool ControlFaultEvaluator::evaluateTrigger(Addr original_address,
    node_index actual_node) {

  if ( triggerNodes[actual_node].isOp() ) {
    if ( triggerNodes[actual_node].value.compare("&") == 0 ) {
      return
        (triggerNodes[triggerNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].left)
            : triggerNodes[triggerNodes[actual_node].left]
            .extractValue(original_address))
        &&
        (triggerNodes[triggerNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].right)
          : triggerNodes[triggerNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( triggerNodes[actual_node].value.compare("|") == 0 ) {
      return
        (triggerNodes[triggerNodes[actual_node].left].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].left)
          :
          triggerNodes[triggerNodes[actual_node].left]
            .extractValue(original_address))
        ||
        (triggerNodes[triggerNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].right)
          : triggerNodes[triggerNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( triggerNodes[actual_node].value.compare(">") == 0 ) {
      return
        (triggerNodes[triggerNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].left)
            : triggerNodes[triggerNodes[actual_node].left]
              .extractValue(original_address))
        >
        (triggerNodes[triggerNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].right)
          : triggerNodes[triggerNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( triggerNodes[actual_node].value.compare("<") == 0 ) {
      return
        (triggerNodes[triggerNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].left)
            : triggerNodes[triggerNodes[actual_node].left]
              .extractValue(original_address))
        <
        (triggerNodes[triggerNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].right)
          : triggerNodes[triggerNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( triggerNodes[actual_node].value.compare(">=") == 0 ) {
      return
        (triggerNodes[triggerNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].left)
            : triggerNodes[triggerNodes[actual_node].left]
              .extractValue(original_address))
        >=
        (triggerNodes[triggerNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].right)
          : triggerNodes[triggerNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( triggerNodes[actual_node].value.compare("<=") == 0 ) {
      return
        (triggerNodes[triggerNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].left)
            : triggerNodes[triggerNodes[actual_node].left]
              .extractValue(original_address))
        <=
        (triggerNodes[triggerNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].right)
          : triggerNodes[triggerNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( triggerNodes[actual_node].value.compare("==") == 0 ) {
      return
        (triggerNodes[triggerNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].left)
            : triggerNodes[triggerNodes[actual_node].left]
              .extractValue(original_address))
        &&
        (triggerNodes[triggerNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].right)
          : triggerNodes[triggerNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( triggerNodes[actual_node].value.compare("!") == 0 ) {
      return
        (triggerNodes[triggerNodes[actual_node].left].isOp() ?
            !evaluateTrigger(original_address,triggerNodes[actual_node].left)
            : !triggerNodes[triggerNodes[actual_node].left]
              .extractValue(original_address));
    }
  }

  return false;
}

Addr ControlFaultEvaluator::evaluateAction(Addr original_address,
  node_index actual_node) {

  if ( actionNodes[actual_node].isOp() ) {
    if ( actionNodes[actual_node].value.compare("&") == 0 ) {
      return
        (actionNodes[actionNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,actionNodes[actual_node].left)
            : actionNodes[actionNodes[actual_node].left]
            .extractValue(original_address))
        &
        (actionNodes[actionNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,actionNodes[actual_node].right)
          : actionNodes[actionNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( actionNodes[actual_node].value.compare("|") == 0 ) {
      return
        (actionNodes[actionNodes[actual_node].left].isOp() ?
          evaluateTrigger(original_address,actionNodes[actual_node].left)
          :
          actionNodes[actionNodes[actual_node].left]
            .extractValue(original_address))
        |
        (actionNodes[actionNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,actionNodes[actual_node].right)
          : actionNodes[actionNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( actionNodes[actual_node].value.compare(">>") == 0 ) {
      return
        (actionNodes[actionNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,actionNodes[actual_node].left)
            : actionNodes[actionNodes[actual_node].left]
              .extractValue(original_address))
        >>
        (actionNodes[actionNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,actionNodes[actual_node].right)
          : actionNodes[actionNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( actionNodes[actual_node].value.compare("<<") == 0 ) {
      return
        (actionNodes[actionNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,actionNodes[actual_node].left)
            : actionNodes[actionNodes[actual_node].left]
              .extractValue(original_address))
        <<
        (actionNodes[actionNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,actionNodes[actual_node].right)
          : actionNodes[actionNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( actionNodes[actual_node].value.compare("^") == 0 ) {
      return
        (actionNodes[actionNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,actionNodes[actual_node].left)
            : actionNodes[actionNodes[actual_node].left]
              .extractValue(original_address))
        ^
        (actionNodes[actionNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,actionNodes[actual_node].right)
          : actionNodes[actionNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( actionNodes[actual_node].value.compare("~") == 0 ) {
      return
        (actionNodes[actionNodes[actual_node].left].isOp() ?
            ~ evaluateTrigger(original_address,actionNodes[actual_node].left)
            : ~ actionNodes[actionNodes[actual_node].left]
              .extractValue(original_address));
    }
  }

  return false;
}

Addr ControlFaultEvaluator::evaluate(Addr original_address) {
  if (!evaluateTrigger(original_address, 0))
    return original_address;

  return evaluateAction(original_address, 0);
}
