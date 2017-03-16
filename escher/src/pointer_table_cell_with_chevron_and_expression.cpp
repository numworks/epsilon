#include <escher/pointer_table_cell_with_chevron_and_expression.h>
#include <escher/palette.h>

PointerTableCellWithChevronAndExpression::PointerTableCellWithChevronAndExpression(I18n::Message message, KDText::FontSize size) :
  PointerTableCellWithChevron(message, size),
  m_subtitleView(1.0f, 0.5f, Palette::GreyDark)
{
}

View * PointerTableCellWithChevronAndExpression::subAccessoryView() const {
  return (View *)&m_subtitleView;
}

void PointerTableCellWithChevronAndExpression::setHighlighted(bool highlight) {
  PointerTableCellWithChevron::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void PointerTableCellWithChevronAndExpression::setExpression(Poincare::ExpressionLayout * expressionLayout) {
  m_subtitleView.setExpression(expressionLayout);
  reloadCell();
  layoutSubviews();
}
