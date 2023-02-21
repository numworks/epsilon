#ifndef SHARED_SETTINGS_MESSAGE_TREE_H
#define SHARED_SETTINGS_MESSAGE_TREE_H

#include <apps/i18n.h>
#include <escher/message_tree.h>

namespace Settings {

class MessageTree : public Escher::MessageTree {
 public:
  constexpr MessageTree(I18n::Message label = I18n::Message::Default)
      : Escher::MessageTree(label, 0), m_children(nullptr) {}

  template <int N>
  constexpr MessageTree(I18n::Message label,
                        const MessageTree (&children)[N] = nullptr)
      : Escher::MessageTree(label, N), m_children(children) {}

  const MessageTree* childAtIndex(int index) const override {
    assert(index < m_numberOfChildren);
    return &m_children[index];
  }

 private:
  const MessageTree* m_children;
};

}  // namespace Settings

#endif
