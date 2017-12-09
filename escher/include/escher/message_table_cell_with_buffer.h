#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_BUFFER_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_BUFFER_H

#include <escher/message_table_cell.h>
#include <escher/buffer_text_view.h>

class MessageTableCellWithBuffer final : public MessageTableCell {
public:
  MessageTableCellWithBuffer(I18n::Message message = (I18n::Message)0, KDText::FontSize fontSize = KDText::FontSize::Small, KDText::FontSize accessoryFontSize = KDText::FontSize::Large, KDColor accessoryTextColor = KDColorBlack);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setAccessoryText(const char * textBody);
  const char * accessoryText() { return m_accessoryView.text(); }
  void setTextColor(KDColor color) override;
  void setAccessoryTextColor(KDColor color) { m_accessoryView.setTextColor(color); }
  void setAccessoryFontSize(KDText::FontSize fontSize) { m_accessoryView.setFontSize(fontSize); }
protected:
  BufferTextView m_accessoryView;
};

#endif
