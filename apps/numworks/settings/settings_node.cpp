#include "settings_node.h"

namespace Settings {

const Node * SettingsNode::children(int index) const {
  return &m_children[index];
}

}
