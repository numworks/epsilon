#ifndef SETTINGS_MESSAGE_TABLE_CELL_WITH_MESSAGE_WITH_SWITCH_OR_IMAGE_H
#define SETTINGS_MESSAGE_TABLE_CELL_WITH_MESSAGE_WITH_SWITCH_OR_IMAGE_H

#include <escher/image_view.h>
#include <escher/message_table_cell_with_message_with_switch.h>

namespace Settings {

class PressToTestSwitch : public Escher::MessageTableCellWithMessageWithSwitch {
public:
  PressToTestSwitch(I18n::Message message = (I18n::Message)0);
  const Escher::View * accessoryView() const override;
  void setDisplayImage(bool state) { m_displayImage = state; }
private:
  Escher::ImageView m_accessoryView;
  bool m_displayImage;
};

}  // namespace Settings

#endif
