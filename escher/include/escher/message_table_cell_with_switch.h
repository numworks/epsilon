#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_SWITCH_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_SWITCH_H

#include <escher/message_table_cell.h>
#include <escher/switch_view.h>

namespace Escher {

class MessageTableCellWithSwitch : public MessageTableCell {
public:
  MessageTableCellWithSwitch(I18n::Message message = (I18n::Message)0);
  const View * accessoryView() const override { return &m_accessoryView; }
private:
  SwitchView m_accessoryView;
};

}

#endif
