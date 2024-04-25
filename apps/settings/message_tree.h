#ifndef SHARED_SETTINGS_MESSAGE_TREE_H
#define SHARED_SETTINGS_MESSAGE_TREE_H

#include <apps/i18n.h>
#include <escher/message_tree.h>

namespace Settings {

class MessageTree : public Escher::MessageTree {
 public:
  constexpr MessageTree(I18n::Message label = I18n::Message::Default)
      : Escher::MessageTree(label),
        m_numberOfChildren(0),
        m_children(nullptr) {}

  template <int N>
  constexpr MessageTree(I18n::Message label,
                        const MessageTree (&children)[N] = nullptr)
      : Escher::MessageTree(label),
        m_numberOfChildren(N),
        m_children(children) {}

  const MessageTree* childAtIndex(int index) const override {
    assert(index < m_numberOfChildren);
    return &m_children[index];
  }

  int numberOfChildren() const override { return m_numberOfChildren; }

 private:
  const uint16_t m_numberOfChildren;
  const MessageTree* m_children;
};

}  // namespace Settings

#endif
