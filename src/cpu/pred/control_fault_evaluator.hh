#ifndef __CPU_PRED_CONTROL_FAULT_EVALUATOR_HH__
#define __CPU_PRED_CONTROL_FAULT_EVALUATOR_HH__


#include <sstream>
#include <string>
#include <vector>

#include "base/types.hh"

using namespace std;
namespace Evaluator {

enum NodeType { op, constant, index };
typedef int node_index;

class Node
{
    public:
        NodeType type;
        string value;
        node_index left;
        node_index right;

        Node(){}

        Node(NodeType p_type, string p_value):
            type(p_type),
            value(p_value)
        {
            left = -1;
            right = -1;
        }

        bool isOp(){
            return type == NodeType::op;
        }

        int extractValue(Addr original_address) {
            if ( type == NodeType::constant )
                return std::stoi(value);
            else if ( type == NodeType::index )
                return (int) original_address;
            else
                return 0;
        }

};

class ControlFaultEvaluator
{
    public:
        ControlFaultEvaluator(string trigger, string action);
        Addr evaluate(Addr original_address);
    private:
        vector<Node> triggerNodes;
        vector<Node> actionNodes;
        bool evaluateTrigger(Addr original_address, node_index actual_node);
        Addr evaluateAction(Addr original_address, node_index actual_node);
};


}
#endif
