#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_MESSAGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_MESSAGE_H

#include <escher/message_table_cell.h>

namespace Escher {

class MessageTableCellWithMessage : public MessageTableCell {
 public:
  MessageTableCellWithMessage(I18n::Message message = (I18n::Message)0,
                              I18n::Message subLabelMessage = (I18n::Message)0);
  const View* subLabelView() const override;
  void setHighlighted(bool highlight) override;
  void setSubLabelMessage(I18n::Message textBody);
  void setSublabelTextColor(KDColor color) {
    m_subLabelView.setTextColor(color);
  }

 protected:
  MessageTextView m_subLabelView;
};

}  // namespace Escher

#endif
