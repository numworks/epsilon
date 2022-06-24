#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_BUFFER_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_BUFFER_H

#include <escher/message_table_cell.h>
#include <escher/buffer_text_view.h>

class MessageTableCellWithBuffer : public MessageTableCell<> {
public:
  MessageTableCellWithBuffer(I18n::Message message = (I18n::Message)0, const KDFont * font = KDFont::SmallFont, const KDFont * accessoryFont = KDFont::LargeFont, KDColor accessoryTextColor = Palette::PrimaryText);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setAccessoryText(const char * textBody);
  const char * accessoryText();
  void setTextColor(KDColor color) override;
  void setAccessoryTextColor(KDColor color) {
    m_accessoryView.setTextColor(color);
  }
  void setAccessoryFont(const KDFont * font) {
    m_accessoryView.setFont(font);
  }
  const char * text() const override {
    return m_accessoryView.text();
  }
protected:
  BufferTextView m_accessoryView;
};

#endif
