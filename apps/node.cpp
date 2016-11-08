#include "node.h"

const Node * Node::children(int index) const {
  return &m_children[index];
}

int Node::numberOfChildren() {
  return m_numberOfChildren;
}

const char * Node::label() const {
  return m_label;
}

const char * Node::text() const {
  return m_text;
}

bool Node::isNull() {
  return (m_label == nullptr);
}
