#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_MESSAGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_MESSAGE_H

#include <escher/message_table_cell_with_chevron.h>

class MessageTableCellWithChevronAndMessage : public MessageTableCellWithChevron {
public:
  MessageTableCellWithChevronAndMessage(KDText::FontSize labelSize = KDText::FontSize::Small, KDText::FontSize contentSize = KDText::FontSize::Small);
  View * subAccessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setSubtitle(I18n::Message text);
private:
  MessageTextView m_subtitleView;
};

#endif
