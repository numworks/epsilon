#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_SWITCH_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_SWITCH_H

#include <escher/message_table_cell.h>
#include <escher/switch_view.h>

class MessageTableCellWithSwitch : public MessageTableCell {
public:
  MessageTableCellWithSwitch(const I18n::Message *message = &I18n::NullMessage, KDText::FontSize size = KDText::FontSize::Small);
  View * accessoryView() const override;
private:
  SwitchView m_accessoryView;
};

#endif
