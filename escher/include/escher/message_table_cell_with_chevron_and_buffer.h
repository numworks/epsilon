#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_BUFFER_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_BUFFER_H

#include <escher/message_table_cell_with_chevron.h>
#include <escher/buffer_text_view.h>

class MessageTableCellWithChevronAndBuffer : public MessageTableCellWithChevron<> {
public:
  MessageTableCellWithChevronAndBuffer(const KDFont * labelFont = KDFont::SmallFont, const KDFont * subAccessoryFont = KDFont::SmallFont);
  View * subAccessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setAccessoryText(const char * textBody);
private:
  BufferTextView m_subAccessoryView;
};

#endif
