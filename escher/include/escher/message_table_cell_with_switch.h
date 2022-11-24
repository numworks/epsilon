#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_SWITCH_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_SWITCH_H

#include <escher/message_table_cell.h>
#include <escher/switch_view.h>

namespace Escher {

class MessageTableCellWithSwitch : public MessageTableCell {
public:
  using MessageTableCell::MessageTableCell;
  const View * accessoryView() const override { return &m_accessoryView; }
  void setState(bool state) { m_accessoryView.setState(state); }
  bool cellTypeIsStorable() const override { return false; }
private:
  SwitchView m_accessoryView;
};

}

#endif
