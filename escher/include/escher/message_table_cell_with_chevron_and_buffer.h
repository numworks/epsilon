#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_BUFFER_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_BUFFER_H

#include <escher/buffer_text_view.h>
#include <escher/message_table_cell_with_chevron.h>

namespace Escher {

class MessageTableCellWithChevronAndBuffer
    : public MessageTableCellWithChevron {
 public:
  MessageTableCellWithChevronAndBuffer();
  const View* subLabelView() const override { return &m_subLabelView; }
  void setHighlighted(bool highlight) override;
  void setSubLabelText(const char* textBody);

 private:
  BufferTextView m_subLabelView;
};

}  // namespace Escher

#endif
