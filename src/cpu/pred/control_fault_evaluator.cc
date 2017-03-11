#include "cpu/pred/control_fault_evaluator.hh"

using namespace Evaluator;

bool ControlFaultEvaluator::evaluateTrigger(Addr original_address,
        node_index actual_node) {

    if ( actual_node.isOp() ) {
        if ( strcmp(actual_node.value,"&") == 0 ) {
            return
                (nodes[nodes[actual_node].left].isOp() ?
                    evaluateTrigger(original_address,actual_node.left)
                    : nodes[nodes[actual_node].left]
                    .extractValue(original_address))
                &&
                (nodes[actual_node.right].isOp() ?
                    evaluateTrigger(original_address,actual_node.right));
        }
        else if ( strcmp(actual_node.value,"|") == 0 ) {
            return
                (nodes[actual_node.left].isOp() ?
                    evaluateTrigger(original_address,actual_node.left)
                    : nodes[actual_node.left].extractValue(original_address))
                ||
                (nodes[actual_node.right].isOp() ?
                    evaluateTrigger(original_address,actual_node.right));
        }
        else if ( strcmp(actual_node.value,">") == 0 ) {
            return
                (nodes[actual_node.left].isOp() ?
                    evaluateTrigger(original_address,actual_node.left)
                    : nodes[actual_node.left].extractValue(original_address))
                >
                (nodes[actual_node.right].isOp() ?
                    evaluateTrigger(original_address,actual_node.right));
        }
        else if ( strcmp(actual_node.value,"<") == 0 ) {
            return
                (nodes[actual_node.left].isOp() ?
                    evaluateTrigger(original_address,actual_node.left)
                    : nodes[actual_node.left].extractValue(original_address))
                <
                (nodes[actual_node.right].isOp() ?
                    evaluateTrigger(original_address,actual_node.right));
        }
        else if ( strcmp(actual_node.value,">=") == 0 ) {
            return
                (nodes[actual_node.left].isOp() ?
                    evaluateTrigger(original_address,actual_node.left)
                    : nodes[actual_node.left].extractValue(original_address))
                >=
                (nodes[actual_node.right].isOp() ?
                    evaluateTrigger(original_address,actual_node.right));
        }
        else if ( strcmp(actual_node.value,"<=") == 0 ) {
            return
                (nodes[actual_node.left].isOp() ?
                    evaluateTrigger(original_address,actual_node.left)
                    : nodes[actual_node.left].extractValue(original_address))
                <=
                (nodes[actual_node.right].isOp() ?
                    evaluateTrigger(original_address,actual_node.right));
        }
        else if ( strcmp(actual_node.value,"==") == 0 ) {
            return
                (nodes[actual_node.left].isOp() ?
                    evaluateTrigger(original_address,actual_node.left)
                    : nodes[actual_node.left].extractValue(original_address))
                &&
                (nodes[actual_node.right].isOp() ?
                    evaluateTrigger(original_address,actual_node.right));
        }
        else if ( strcmp(actual_node.value,"!") == 0 ) {
            return
                (nodes[actual_node.left].isOp() ?
                    !evaluateTrigger(original_address,actual_node.left)
                    : !nodes[actual_node.left].extractValue(original_address));
        }
    }

    return false;

}
