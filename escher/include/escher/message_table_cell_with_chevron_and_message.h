#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_MESSAGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_MESSAGE_H

#include <escher/message_table_cell_with_chevron.h>

namespace Escher {

class MessageTableCellWithChevronAndMessage : public MessageTableCellWithChevron {
public:
  MessageTableCellWithChevronAndMessage();
  View * subLabelView() const override;
  void setHighlighted(bool highlight) override;
  void setSubtitle(I18n::Message text);
private:
  MessageTextView m_subtitleView;
};

}

#endif
