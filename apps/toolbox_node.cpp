#include "toolbox_node.h"

I18n::Message ToolboxNode::text() const {
  return m_text;
}

I18n::Message ToolboxNode::insertedText() const {
  return m_insertedText;
}

const Node * ToolboxNode::children(int index) const {
  return &m_children[index];
}
