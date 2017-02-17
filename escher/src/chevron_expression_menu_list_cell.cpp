#include <escher/chevron_expression_menu_list_cell.h>

ChevronExpressionMenuListCell::ChevronExpressionMenuListCell(char * label) :
  ChevronMenuListCell(label),
  m_subtitleView(1.0f, 0.5f, Palette::GreyDark)
{
}

void ChevronExpressionMenuListCell::setHighlighted(bool highlight) {
  ChevronMenuListCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void ChevronExpressionMenuListCell::setExpression(Poincare::ExpressionLayout * expressionLayout) {
  m_subtitleView.setExpression(expressionLayout);
  markRectAsDirty(bounds());
  layoutSubviews();
}

int ChevronExpressionMenuListCell::numberOfSubviews() const {
  return 3;
}

View * ChevronExpressionMenuListCell::subviewAtIndex(int index) {
  if (index == 0 || index == 1) {
    return ChevronMenuListCell::subviewAtIndex(index);
  }
  return &m_subtitleView;
}

void ChevronExpressionMenuListCell::layoutSubviews() {
  ChevronMenuListCell::layoutSubviews();
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDSize subtitleSize = m_subtitleView.minimalSizeForOptimalDisplay();
  KDSize chevronSize = accessoryView()->minimalSizeForOptimalDisplay();
  m_subtitleView.setFrame(KDRect(width-chevronSize.width()-subtitleSize.width()-k_margin, k_separatorThickness, subtitleSize.width(), height - 2*k_separatorThickness));
}
