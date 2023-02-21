#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_BUFFER_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_BUFFER_H

#include <escher/buffer_text_view.h>
#include <escher/message_table_cell.h>

namespace Escher {

class MessageTableCellWithBuffer : public MessageTableCell {
 public:
  MessageTableCellWithBuffer(I18n::Message message = (I18n::Message)0);
  const View* subLabelView() const override { return &m_subLabelView; }
  void setHighlighted(bool highlight) override;
  void setSubLabelText(const char* textBody);
  const char* subLabelText();
  const char* text() const override { return m_subLabelView.text(); }

 protected:
  BufferTextView m_subLabelView;
};

}  // namespace Escher

#endif
