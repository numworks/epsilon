#ifndef SETTINGS_MESSAGE_TREE_H
#define SETTINGS_MESSAGE_TREE_H
#include <escher/message_tree.h>
#include <apps/i18n.h>

namespace Settings {

class SettingsMessageTree : public MessageTree {
public:
  constexpr SettingsMessageTree(I18n::Message label = I18n::Message::Default, const SettingsMessageTree * children = nullptr, int numberOfChildren = 0) :
    MessageTree(label, numberOfChildren),
    m_children(children)
  {
  };
  const MessageTree * children(int index) const override;
private:
  const SettingsMessageTree * m_children;
};

}

#endif

