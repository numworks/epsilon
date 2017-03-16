#include "toolbox_node.h"

I18n::Message ToolboxNode::text() const {
  return m_text;
}

const Node * ToolboxNode::children(int index) const {
  return &m_children[index];
}
