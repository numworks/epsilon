#include <escher/pointer_table_cell_with_chevron.h>

PointerTableCellWithChevron::PointerTableCellWithChevron(char * label, KDText::FontSize size) :
  PointerTableCell(label, size),
  m_accessoryView(ChevronView())
{
}

View * PointerTableCellWithChevron::accessoryView() const {
  return (View *)&m_accessoryView;
}

void PointerTableCellWithChevron::setHighlighted(bool highlight) {
  PointerTableCell::setHighlighted(highlight);
  m_accessoryView.setHighlighted(isHighlighted());
}
