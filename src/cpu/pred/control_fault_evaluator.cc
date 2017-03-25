#include "cpu/pred/control_fault_evaluator.hh"

using namespace Evaluator;
using namespace std;

ControlFaultEvaluator::ControlFaultEvaluator(string trigger,string action,
  bool _isEnabled) :
  isEnabled(_isEnabled)
  {

    if (!isEnabled)
      return;

    int n_nodes, n_edges;
    stringstream trigger_stream(trigger);
    stringstream action_stream(action);

    trigger_stream >> n_nodes;
    trigger_stream >> n_edges;

    triggerNodes.resize(n_nodes);
    for ( int i = 0; i < n_nodes; i++ ) {
        int number;
        trigger_stream >> number;
        char type;
        NodeType n_type;
        trigger_stream >> type;
        if ( type == 'i' )
            n_type = NodeType::index;
        else if ( type == 'c' )
            n_type = NodeType::constant;
        else
            n_type = NodeType::op;

        string value;
        trigger_stream >> value;
        triggerNodes[number].type= n_type;
        triggerNodes[number].value = value;
    }
    for ( int i = 0; i < n_edges; i++ ) {
        int father;
        int child;
        trigger_stream >> father;
        trigger_stream >> child;
        if ( triggerNodes[father].left == -1 )
            triggerNodes[father].left = child;
        else
            triggerNodes[father].right = child;
    }

    action_stream >> n_nodes;
    action_stream >> n_edges;

    actionNodes.resize(n_nodes);
    for ( int i = 0; i < n_nodes; i++ ) {
        int number;
        action_stream >> number;
        char type;
        NodeType n_type;
        action_stream >> type;
        if ( type == 'i' )
            n_type = NodeType::index;
        else if ( type == 'c' )
            n_type = NodeType::constant;
        else
            n_type = NodeType::op;

        string value;
        action_stream >> value;
        actionNodes[number] = Node(n_type,value);
    }
    for ( int i = 0; i < n_edges; i++ ) {
        int father;
        int child;
        action_stream >> father;
        action_stream >> child;
        if ( actionNodes[father].left == -1 )
            actionNodes[father].left = child;
        else
            actionNodes[father].right = child;
    }

}


Addr ControlFaultEvaluator::evaluateTrigger(Addr original_address,
    node_index actual_node) {

  //original_address %= 128;

  if ( triggerNodes[actual_node].isOp() ) {
    if ( triggerNodes[actual_node].value.compare("&") == 0 ) {
      return
        (triggerNodes[triggerNodes[actual_node].left].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].left)
            : triggerNodes[triggerNodes[actual_node].left]
            .extractValue(original_address))
        &
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
        |
        (triggerNodes[triggerNodes[actual_node].right].isOp() ?
            evaluateTrigger(original_address,triggerNodes[actual_node].right)
          : triggerNodes[triggerNodes[actual_node].right]
          .extractValue(original_address));
    }
    else if ( triggerNodes[actual_node].value.compare(">") == 0 ) {
      long int op1 = (triggerNodes[triggerNodes[actual_node].left].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].left)
          : triggerNodes[triggerNodes[actual_node].left]
            .extractValue(original_address));
      long int op2 = (triggerNodes[triggerNodes[actual_node].right].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].right)
        : triggerNodes[triggerNodes[actual_node].right]
        .extractValue(original_address));
      if (op1 > op2)
        return 1;
      else
        return 0;
    }
    else if ( triggerNodes[actual_node].value.compare("<") == 0 ) {
      long int op1 = (triggerNodes[triggerNodes[actual_node].left].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].left)
          : triggerNodes[triggerNodes[actual_node].left]
            .extractValue(original_address));
      long int op2 = (triggerNodes[triggerNodes[actual_node].right].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].right)
        : triggerNodes[triggerNodes[actual_node].right]
        .extractValue(original_address));
      if (op1 < op2)
        return 1;
      else
        return 0;
    }
    else if ( triggerNodes[actual_node].value.compare(">=") == 0 ) {
      long int op1 = (triggerNodes[triggerNodes[actual_node].left].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].left)
          : triggerNodes[triggerNodes[actual_node].left]
            .extractValue(original_address));
      long int op2 = (triggerNodes[triggerNodes[actual_node].right].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].right)
        : triggerNodes[triggerNodes[actual_node].right]
        .extractValue(original_address));
      if (op1 >= op2)
        return 1;
      else
        return 0;
    }
    else if ( triggerNodes[actual_node].value.compare("<=") == 0 ) {
      long int op1 = (triggerNodes[triggerNodes[actual_node].left].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].left)
          : triggerNodes[triggerNodes[actual_node].left]
            .extractValue(original_address));
      long int op2 = (triggerNodes[triggerNodes[actual_node].right].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].right)
        : triggerNodes[triggerNodes[actual_node].right]
        .extractValue(original_address));
      if (op1 <= op2)
        return 1;
      else
        return 0;
    }
    else if ( triggerNodes[actual_node].value.compare("==") == 0 ) {
      long int op1 = (triggerNodes[triggerNodes[actual_node].left].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].left)
          : triggerNodes[triggerNodes[actual_node].left]
            .extractValue(original_address));
      long int op2 = (triggerNodes[triggerNodes[actual_node].right].isOp() ?
          evaluateTrigger(original_address,triggerNodes[actual_node].right)
        : triggerNodes[triggerNodes[actual_node].right]
        .extractValue(original_address));
      if (op1 == op2)
        return 1;
      else
        return 0;
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
  return actionNodes[actual_node].extractValue(original_address);
}

Addr ControlFaultEvaluator::evaluate(Addr original_address) {
  if (!isEnabled)
    return original_address;

  if (!evaluateTrigger(original_address, 0))
    return original_address;

  return evaluateAction(original_address, 0);
}
