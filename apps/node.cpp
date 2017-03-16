#include "node.h"

int Node::numberOfChildren() const {
  return m_numberOfChildren;
}

I18n::Message Node::label() const {
  return m_label;
}

bool Node::isNull() const {
  return (m_label == I18n::Message::Default);
}
