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
  const Escher::View* view() const override;
  void setHighlighted(bool highlighted) override {
    m_switch.setHighlighted(highlighted);
    m_image.setBackgroundColor(
        Escher::AbstractMenuCell::BackgroundColor(highlighted));
  }

 private:
  Escher::SwitchView m_switch;
  Escher::TransparentImageView m_image;
  bool m_displayImage;
};

class PressToTestSwitch
    : public Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                              AlternateSwitchAndImage> {
 public:
  PressToTestSwitch(I18n::Message message = (I18n::Message)0);
};

}  // namespace Settings

#endif
