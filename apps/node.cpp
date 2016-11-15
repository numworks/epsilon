#include "node.h"

int Node::numberOfChildren() const {
  return m_numberOfChildren;
}

const char * Node::label() const {
  return m_label;
}

bool Node::isNull() const {
  return (m_label == nullptr);
}
