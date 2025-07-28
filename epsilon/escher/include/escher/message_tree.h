#ifndef ESCHER_MESSAGE_TREE_H
#define ESCHER_MESSAGE_TREE_H

#include <escher/i18n.h>

namespace Escher {

class MessageTree {
 public:
  constexpr MessageTree(I18n::Message label = (I18n::Message)0)
      : m_label(label){};
  virtual const MessageTree* childAtIndex(int index) const = 0;
  constexpr I18n::Message label() const { return m_label; }
  virtual int numberOfChildren() const { return 0; }
  bool isNull() const { return (m_label == (I18n::Message)0); }

 protected:
  I18n::Message m_label;
};

}  // namespace Escher

#endif
