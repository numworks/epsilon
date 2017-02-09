#include <escher/expression_menu_list_cell.h>
#include <assert.h>

ExpressionMenuListCell::ExpressionMenuListCell() :
  TableViewCell(),
  m_labelExpressionView(ExpressionView(0.0f, 0.5f, KDColorBlack, KDColorWhite))
{
}

void ExpressionMenuListCell::setHighlighted(bool highlight) {
  TableViewCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_labelExpressionView.setBackgroundColor(backgroundColor);
}

void ExpressionMenuListCell::setExpression(Poincare::ExpressionLayout * expressionLayout) {
  m_labelExpressionView.setExpression(expressionLayout);
}

void ExpressionMenuListCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;

  ctx->fillRect(KDRect(k_separatorThickness, k_separatorThickness, width-2*k_separatorThickness, height-k_separatorThickness), backgroundColor);
  ctx->fillRect(KDRect(0, 0, width, k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(0, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(width-k_separatorThickness, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
 }

int ExpressionMenuListCell::numberOfSubviews() const {
  return 1;
}

View * ExpressionMenuListCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_labelExpressionView;
}

void ExpressionMenuListCell::layoutSubviews() {
  m_labelExpressionView.setFrame(bounds());
}
