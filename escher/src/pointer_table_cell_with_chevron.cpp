#include <escher/pointer_table_cell_with_chevron.h>

PointerTableCellWithChevron::PointerTableCellWithChevron(I18n::Message message, KDText::FontSize size) :
  PointerTableCell(message, size),
  m_accessoryView(ChevronView())
{
}

View * PointerTableCellWithChevron::accessoryView() const {
  return (View *)&m_accessoryView;
}

