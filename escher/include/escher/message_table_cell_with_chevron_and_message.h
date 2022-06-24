#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_MESSAGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_MESSAGE_H

#include <escher/message_table_cell_with_chevron.h>

class MessageTableCellWithChevronAndMessage : public MessageTableCellWithChevron<> {
public:
  MessageTableCellWithChevronAndMessage(const KDFont * labelFont = KDFont::SmallFont, const KDFont * contentFont = KDFont::SmallFont);
  View * subAccessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setSubtitle(I18n::Message text);
private:
  MessageTextView m_subtitleView;
};

#endif
