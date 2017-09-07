#ifndef ESCHER_MESSAGE_TABLE_CELL_H
#define ESCHER_MESSAGE_TABLE_CELL_H

#include <escher/message_text_view.h>
#include <escher/i18n.h>
#include <escher/table_cell.h>

class MessageTableCell : public TableCell {
public:
  MessageTableCell(const I18n::Message *label = &I18n::NullMessage, KDText::FontSize size = KDText::FontSize::Small, Layout layout = Layout::Horizontal);
  View * labelView() const override;
  virtual void setHighlighted(bool highlight) override;
  void setMessage(const I18n::Message *message);
  virtual void setTextColor(KDColor color);
  void setMessageFontSize(KDText::FontSize fontSize);
private:
  MessageTextView m_messageTextView;
};

#endif
