#include "tool_box_node.h"

const char * ToolBoxNode::text() const {
  return m_text;
}

const Node * ToolBoxNode::children(int index) const {
  return &m_children[index];
}
