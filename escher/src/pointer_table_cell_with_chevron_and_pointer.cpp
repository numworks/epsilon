#include <escher/pointer_table_cell_with_chevron_and_pointer.h>
#include <escher/palette.h>

PointerTableCellWithChevronAndPointer::PointerTableCellWithChevronAndPointer(KDText::FontSize labelSize, KDText::FontSize contentSize) :
  PointerTableCellWithChevron(nullptr, labelSize),
  m_subtitleView(contentSize, "", 1.0f, 0.5f, Palette::GreyDark)
{
}

View * PointerTableCellWithChevronAndPointer::subAccessoryView() const {
  return (View *)&m_subtitleView;
}

void PointerTableCellWithChevronAndPointer::setHighlighted(bool highlight) {
  PointerTableCellWithChevron::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void PointerTableCellWithChevronAndPointer::setSubtitle(const char * text) {
  m_subtitleView.setText(text);
  reloadCell();
  layoutSubviews();
}
