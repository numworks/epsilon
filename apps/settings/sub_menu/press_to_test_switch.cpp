#include "press_to_test_switch.h"

#include "press_to_test_success.h"

using namespace Escher;

namespace Settings {

const View* AlternateSwitchAndImage::view() const {
  return m_displayImage
             ? (m_switch.state() ? static_cast<const View*>(&m_image) : nullptr)
             : static_cast<const View*>(&m_switch);
}

PressToTestSwitch::PressToTestSwitch(I18n::Message message) {
  m_accessory.setDisplayImage(false);
  m_accessory.imageView()->setImage(ImageStore::PressToTestSuccess);
  m_accessory.imageView()->setBackgroundColor(KDColorWhite);
  // TODO: DefaultInitialization
  m_subLabel.setFont(KDFont::Size::Small);
  m_subLabel.setAlignment(KDContext::k_alignLeft, KDContext::k_alignCenter);
  m_subLabel.setTextColor(Palette::GrayDark);
}

}  // namespace Settings
