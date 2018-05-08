#include <escher/toolbox_message_tree.h>

I18n::Message ToolboxMessageTree::text() const {
  return m_text;
}

I18n::Message ToolboxMessageTree::insertedText() const {
  return m_insertedText;
}

const MessageTree * ToolboxMessageTree::children(int index) const {
  return &m_children[index];
}
