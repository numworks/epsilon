#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_BUFFER_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_BUFFER_H

#include <escher/message_table_cell.h>
#include <escher/buffer_text_view.h>

class MessageTableCellWithBuffer : public MessageTableCell {
public:
  MessageTableCellWithBuffer(I18n::Message message = (I18n::Message)0, KDText::FontSize fontSize = KDText::FontSize::Small, KDText::FontSize accessoryFontSize = KDText::FontSize::Large, KDColor accessoryTextColor = KDColorBlack);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setAccessoryText(const char * textBody);
  const char * accessoryText();
  void setTextColor(KDColor color) override;
  void setAccessoryTextColor(KDColor color);
  void setAccessoryFontSize(KDText::FontSize fontSize);
protected:
  BufferTextView m_accessoryView;
};

#endif
