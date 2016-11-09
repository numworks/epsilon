#include "variable_box_node.h"

const Node * VariableBoxNode::children(int index) const {
  return &m_children[index];
}
