#include <escher/i18n.h>

namespace I18n {
UniversalMessage::UniversalMessage(const char *msg) : m_msg(msg) {}

const char * UniversalMessage::translate(Language l) const {
  return m_msg;
}

const UniversalMessage NullMessage("");
}
