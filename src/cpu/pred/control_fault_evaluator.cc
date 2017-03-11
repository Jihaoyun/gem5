#include "cpu/pred/control_fault_evaluator.hh"

using namespace Evaluator;

bool ControlFaultEvaluator::evaluateTrigger(Addr original_address,
    node_index actual_node) {

  if ( nodes[actual_node].isOp() ) {
    if ( nodes[actual_node].value.compare("&") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].left)
            : nodes[nodes[actual_node].left]
            .extractValue(original_address))
        &&
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare("|") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
          evaluateTrigger(original_address,nodes[actual_node].left)
          :
          nodes[nodes[actual_node].left].extractValue(original_address))
        ||
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare(">") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].left)
            : nodes[nodes[actual_node].left].extractValue(original_address))
        >
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare("<") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].left)
            : nodes[nodes[actual_node].left].extractValue(original_address))
        <
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare(">=") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].left)
            : nodes[nodes[actual_node].left].extractValue(original_address))
        >=
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare("<=") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].left)
            : nodes[nodes[actual_node].left].extractValue(original_address))
        <=
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare("==") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].left)
            : nodes[nodes[actual_node].left].extractValue(original_address))
        &&
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare("!") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            !evaluateTrigger(original_address,nodes[actual_node].left)
            : !nodes[nodes[actual_node].left].extractValue(original_address));
    }
  }

  return false;
}

Addr ControlFaultEvaluator::evaluateAction(Addr original_address,
  node_index actual_node) {

  if ( nodes[actual_node].isOp() ) {
    if ( nodes[actual_node].value.compare("&") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].left)
            : nodes[nodes[actual_node].left]
            .extractValue(original_address))
        &
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare("|") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
          evaluateTrigger(original_address,nodes[actual_node].left)
          :
          nodes[nodes[actual_node].left].extractValue(original_address))
        |
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare(">>") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].left)
            : nodes[nodes[actual_node].left].extractValue(original_address))
        >>
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare("<<") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].left)
            : nodes[nodes[actual_node].left].extractValue(original_address))
        <<
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare("^") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].left)
            : nodes[nodes[actual_node].left].extractValue(original_address))
        ^
        (nodes[nodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,nodes[actual_node].right)
          : nodes[nodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( nodes[actual_node].value.compare("~") == 0 ) {
      return
        (nodes[nodes[actual_node].left].isOp() ?
            ~ evaluateTrigger(original_address,nodes[actual_node].left)
            : ~ nodes[nodes[actual_node].left].extractValue(original_address));
    }
  }

  return false;
}

Addr ControlFaultEvaluator::evaluate(Addr original_address) {
  if (!evaluateTrigger(original_address, 0))
    return original_address;

  return evaluateAction(original_address, 0);
}
