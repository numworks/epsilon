#include "node.h"

int Node::numberOfChildren() const {
  return m_numberOfChildren;
}

const I18n::Message *Node::label() const {
  return m_label;
}

bool Node::isNull() const {
  return (m_label == &I18n::Common::Default);
}
