#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_MESSAGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_MESSAGE_H

#include <escher/message_table_cell_with_chevron.h>

namespace Escher {

class MessageTableCellWithChevronAndMessage
    : public MessageTableCellWithChevron {
 public:
  MessageTableCellWithChevronAndMessage(
      I18n::Message label = (I18n::Message)0,
      I18n::Message subtitle = (I18n::Message)0);
  const View* subLabelView() const override { return &m_subtitleView; }
  void setHighlighted(bool highlight) override;
  void setSubtitle(I18n::Message text);

 private:
  MessageTextView m_subtitleView;
};

}  // namespace Escher

#endif
