#include "press_to_test_switch.h"
#include "press_to_test_success.h"

namespace Settings {

PressToTestSwitch::PressToTestSwitch(I18n::Message message) :
    Escher::MessageTableCellWithMessageWithSwitch(message),
    m_displayImage(false) {
  m_accessoryView.setImage(ImageStore::PressToTestSuccess);
  m_accessoryView.setBackgroundColor(KDColorWhite);
}

const Escher::View * PressToTestSwitch::accessoryView() const {
  return (m_displayImage
              ? (state() ? &m_accessoryView : nullptr)
              : Escher::MessageTableCellWithMessageWithSwitch::accessoryView());
}

void PressToTestSwitch::setHighlighted(bool highlighted) {
  MessageTableCellWithMessageWithSwitch::setHighlighted(highlighted);
  if (m_displayImage && state()) {
    m_accessoryView.setBackgroundColor(highlighted ? Escher::Palette::Select : KDColorWhite);
  }
}

}
