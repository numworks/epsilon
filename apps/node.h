#ifndef APPS_NODE_H
#define APPS_NODE_H

#include "i18n.h"

class Node {
public:
  constexpr Node(I18n::Message label = I18n::Message::Default, int numberOfChildren = 0) :
    m_label(label),
    m_numberOfChildren(numberOfChildren)
  {
  };
  virtual const Node * children(int index) const = 0;
  I18n::Message label() const;
  int numberOfChildren() const;
  bool isNull() const;
protected:
  I18n::Message m_label;
  int m_numberOfChildren;
};

#endif

