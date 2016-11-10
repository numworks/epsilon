#include "toolbox_node.h"

const char * ToolboxNode::text() const {
  return m_text;
}

const Node * ToolboxNode::children(int index) const {
  return &m_children[index];
}
