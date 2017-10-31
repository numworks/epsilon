#include "settings_message_tree.h"

namespace Settings {

const MessageTree * SettingsMessageTree::children(int index) const {
  return &m_children[index];
}

}
