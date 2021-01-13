#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_BUFFER_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_BUFFER_H

#include <escher/message_table_cell_with_chevron.h>
#include <escher/buffer_text_view.h>

namespace Escher {

class MessageTableCellWithChevronAndBuffer : public MessageTableCellWithChevron {
public:
  MessageTableCellWithChevronAndBuffer();
  View * subLabelView() const override;
  void setHighlighted(bool highlight) override;
  void setSubLabelText(const char * textBody);
private:
  BufferTextView m_subLabelView;
};

}

#endif
