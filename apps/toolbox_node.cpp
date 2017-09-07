#include "toolbox_node.h"

const I18n::Message *ToolboxNode::text() const {
  return m_text;
}

const I18n::Message *ToolboxNode::insertedText() const {
  return m_insertedText;
}

const Node * ToolboxNode::children(int index) const {
  return &m_children[index];
}
