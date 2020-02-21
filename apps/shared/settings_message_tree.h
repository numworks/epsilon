#ifndef SHARED_SETTINGS_MESSAGE_TREE_H
#define SHARED_SETTINGS_MESSAGE_TREE_H

#include <escher/message_tree.h>
#include <apps/i18n.h>

namespace Shared {

class SettingsMessageTree : public MessageTree {
public:
  constexpr SettingsMessageTree(I18n::Message label = I18n::Message::Default) :
    MessageTree(label, 0),
    m_children(nullptr)
  {}

  template <int N>
  constexpr SettingsMessageTree(I18n::Message label, const SettingsMessageTree (&children)[N] = nullptr) :
    MessageTree(label, N),
    m_children(children)
  {}

  const MessageTree * children(int index) const override { return &m_children[index]; }

private:
  const SettingsMessageTree * m_children;
};

}

#endif

