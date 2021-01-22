#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_MESSAGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_MESSAGE_H

#include <escher/message_table_cell.h>

namespace Escher {

class MessageTableCellWithMessage : public MessageTableCell {
public:
  MessageTableCellWithMessage(I18n::Message message = (I18n::Message)0);
  View * subLabelView() const override;
  void setHighlighted(bool highlight) override;
  void setSubLabelMessage(I18n::Message textBody);
protected:
  MessageTextView m_subLabelView;
};

}

#endif
