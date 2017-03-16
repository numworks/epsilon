#ifndef ESCHER_POINTER_TABLE_CELL_WITH_POINTER_H
#define ESCHER_POINTER_TABLE_CELL_WITH_POINTER_H

#include <escher/pointer_table_cell.h>

class PointerTableCellWithPointer : public PointerTableCell {
public:
  PointerTableCellWithPointer(I18n::Message message = (I18n::Message)0, Layout layout = Layout::Vertical);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setAccessoryMessage(I18n::Message textBody);
  void setTextColor(KDColor color) override;
  void setAccessoryTextColor(KDColor color);
protected:
  PointerTextView m_accessoryView;
};

#endif
