#ifndef ESCHER_MESSAGE_TABLE_CELL_H
#define ESCHER_MESSAGE_TABLE_CELL_H

#include <escher/message_text_view.h>
#include <escher/i18n.h>
#include <escher/table_cell.h>

class MessageTableCell : public TableCell {
public:
  MessageTableCell(I18n::Message label = (I18n::Message)0, const KDFont * font = KDFont::SmallFont, Layout layout = Layout::HorizontalLeftOverlap);
  View * labelView() const override;
  void setHighlighted(bool highlight) override;
  void setMessage(I18n::Message message);
  virtual void setTextColor(KDColor color);
  void setMessageFont(const KDFont * font);
private:
  MessageTextView m_messageTextView;
};

#endif
