#include "settings_message_tree.h"

namespace Shared {

const MessageTree * SettingsMessageTree::children(int index) const {
  return &m_children[index];
}

}
