#ifndef ESCHER_MESSAGE_TREE_H
#define ESCHER_MESSAGE_TREE_H

#include <escher/i18n.h>

class MessageTree {
public:
  constexpr MessageTree(I18n::Message label = (I18n::Message)0, int numberOfChildren = 0) :
    m_label(label),
    m_numberOfChildren(numberOfChildren)
  {
  };
  virtual const MessageTree * children(int index) const = 0;
  I18n::Message label() const;
  int numberOfChildren() const;
  bool isNull() const;
protected:
  I18n::Message m_label;
  int m_numberOfChildren;
};

#endif

