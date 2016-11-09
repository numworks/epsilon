#include "node.h"

int Node::numberOfChildren() {
  return m_numberOfChildren;
}

const char * Node::label() const {
  return m_label;
}

bool Node::isNull() {
  return (m_label == nullptr);
}
