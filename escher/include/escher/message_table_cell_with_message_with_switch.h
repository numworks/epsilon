#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_MESSAGE_WITH_SWITCH_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_MESSAGE_WITH_SWITCH_H

#include <escher/message_table_cell_with_message.h>
#include <escher/switch_view.h>

namespace Escher {

class MessageTableCellWithMessageWithSwitch : public MessageTableCellWithMessage {
public:
  MessageTableCellWithMessageWithSwitch(I18n::Message message = (I18n::Message)0) : MessageTableCellWithMessage(message), m_accessoryView() {}
  const View * accessoryView() const override { return &m_accessoryView; }
  void setState(bool state) { m_accessoryView.setState(state); }
private:
  SwitchView m_accessoryView;
};

}

#endif
