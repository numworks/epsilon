#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_MESSAGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_MESSAGE_H

#include <escher/message_table_cell.h>

class MessageTableCellWithMessage : public MessageTableCell {
public:
  MessageTableCellWithMessage(const I18n::Message *message = &I18n::NullMessage, Layout layout = Layout::Vertical);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setAccessoryMessage(const I18n::Message *textBody);
  void setTextColor(KDColor color) override;
  void setAccessoryTextColor(KDColor color);
protected:
  MessageTextView m_accessoryView;
};

#endif
