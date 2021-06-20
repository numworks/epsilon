#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_COLOR_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_COLOR_H

#include <escher/message_table_cell.h>
#include <escher/color_view.h>

class MessageTableCellWithColor : public MessageTableCell {
public:
  MessageTableCellWithColor(I18n::Message message = (I18n::Message)0, const KDFont * font = KDFont::SmallFont);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
private:
  ColorView m_accessoryView;
};

#endif
