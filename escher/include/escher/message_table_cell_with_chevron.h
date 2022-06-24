#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_H

#include <escher/message_table_cell.h>
#include <escher/chevron_view.h>

template <class T=MessageTextView>
class MessageTableCellWithChevron : public MessageTableCell<T> {
public:
  MessageTableCellWithChevron(I18n::Message message = (I18n::Message)0, const KDFont * font = KDFont::SmallFont);
  View * accessoryView() const override;
private:
  ChevronView m_accessoryView;
};

#endif
