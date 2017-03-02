#ifndef ESCHER_POINTER_TABLE_CELL_WITH_CHEVRON_H
#define ESCHER_POINTER_TABLE_CELL_WITH_CHEVRON_H

#include <escher/pointer_table_cell.h>
#include <escher/chevron_view.h>

class PointerTableCellWithChevron : public PointerTableCell {
public:
  PointerTableCellWithChevron(char * label = nullptr, KDText::FontSize size = KDText::FontSize::Small);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
private:
  ChevronView m_accessoryView;
};

#endif
