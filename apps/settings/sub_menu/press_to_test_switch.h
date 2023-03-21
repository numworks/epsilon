#ifndef SETTINGS_MESSAGE_TABLE_CELL_WITH_MESSAGE_WITH_SWITCH_OR_IMAGE_H
#define SETTINGS_MESSAGE_TABLE_CELL_WITH_MESSAGE_WITH_SWITCH_OR_IMAGE_H

#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/switch_view.h>
#include <escher/transparent_image_view.h>

namespace Settings {

class AlternateSwitchAndImage : public Escher::CellWidget {
 public:
  void setDisplayImage(bool state) { m_displayImage = state; }

  Escher::SwitchView* switchView() { return &m_switch; }
  Escher::TransparentImageView* imageView() { return &m_image; }

  // CellWidget
  const Escher::View* view() const override {
    return m_displayImage
               ? (m_switch.state() ? static_cast<const Escher::View*>(&m_image)
                                   : nullptr)
               : static_cast<const Escher::View*>(&m_switch);
  }
  void setHighlighted(bool highlighted) override {
    m_switch.setHighlighted(highlighted);
    m_image.setBackgroundColor(
        Escher::AbstractMenuCell::BackgroundColor(highlighted));
  }
  bool enterOnEvent(Ion::Events::Event event) const override {
    return view() == &m_switch
               ? m_switch.enterOnEvent(event)
               : (view() == &m_image ? m_image.enterOnEvent(event) : false);
  }

 private:
  Escher::SwitchView m_switch;
  Escher::TransparentImageView m_image;
  bool m_displayImage;
};

using PressToTestSwitch =
    Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                     AlternateSwitchAndImage>;

}  // namespace Settings

#endif
