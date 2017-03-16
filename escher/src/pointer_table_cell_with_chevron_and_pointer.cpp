#include <escher/pointer_table_cell_with_chevron_and_pointer.h>
#include <escher/palette.h>

PointerTableCellWithChevronAndPointer::PointerTableCellWithChevronAndPointer(KDText::FontSize labelSize, KDText::FontSize contentSize) :
  PointerTableCellWithChevron((I18n::Message)0, labelSize),
  m_subtitleView(contentSize, (I18n::Message)0, 1.0f, 0.5f, Palette::GreyDark)
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

void PointerTableCellWithChevronAndPointer::setSubtitle(I18n::Message text) {
  m_subtitleView.setMessage(text);
  reloadCell();
  layoutSubviews();
}
