#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_BUFFER_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_BUFFER_H

#include <escher/message_table_cell.h>
#include <escher/buffer_text_view.h>

namespace Escher {

class MessageTableCellWithBuffer : public MessageTableCell {
public:
  MessageTableCellWithBuffer(I18n::Message message = (I18n::Message)0);
  View * subLabelView() const override;
  void setHighlighted(bool highlight) override;
  void setSubLabelText(const char * textBody);
  const char * subLabelText();
  // void setTextColor(KDColor color) override;
  // void setSubLabelTextColor(KDColor color) {
  //   m_subLabelView.setTextColor(color);
  // }
  // void setSubLabelFont(const KDFont * font) {
  //   m_subLabelView.setFont(font);
  // }
  const char * text() const override {
    return m_subLabelView.text();
  }
protected:
  BufferTextView m_subLabelView;
};

}

#endif
