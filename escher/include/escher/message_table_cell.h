#ifndef ESCHER_MESSAGE_TABLE_CELL_H
#define ESCHER_MESSAGE_TABLE_CELL_H

#include <escher/message_text_view.h>
#include <escher/i18n.h>
#include <escher/table_cell.h>

namespace Escher {

class MessageTableCell : public TableCell {
public:
  MessageTableCell(I18n::Message label = (I18n::Message)0);
  View * labelView() const override;
  void setHighlighted(bool highlight) override;
  void setMessage(I18n::Message message);
  void setBackgroundColor(KDColor color);
protected:
  KDColor backgroundColor() const override { return m_backgroundColor; }
private:
  MessageTextView m_messageTextView;
  KDColor m_backgroundColor;
};

}

#endif
