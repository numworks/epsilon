#include <escher/message_tree.h>

int MessageTree::numberOfChildren() const {
  return m_numberOfChildren;
}

I18n::Message MessageTree::label() const {
  return m_label;
}

bool MessageTree::isNull() const {
  return (m_label == (I18n::Message)0);
}
