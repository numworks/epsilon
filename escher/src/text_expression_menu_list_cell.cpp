#include <escher/text_expression_menu_list_cell.h>
#include <assert.h>

TextExpressionMenuListCell::TextExpressionMenuListCell(char * accessoryText) :
  TableViewCell(),
  m_labelExpressionView(ExpressionView(0.0f, 0.5f, KDColorBlack, KDColorWhite)),
  m_accessoryView(PointerTextView(KDText::FontSize::Small, accessoryText, 0.0f, 0.5f, Palette::GreyDark, KDColorWhite))
{
}

void TextExpressionMenuListCell::setHighlighted(bool highlight) {
  TableViewCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_labelExpressionView.setBackgroundColor(backgroundColor);
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void TextExpressionMenuListCell::setExpression(Poincare::ExpressionLayout * expressionLayout) {
  m_labelExpressionView.setExpression(expressionLayout);
  markRectAsDirty(bounds());
}

void TextExpressionMenuListCell::setAccessoryText(const char * text) {
  m_accessoryView.setText(text);
}

void TextExpressionMenuListCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;

  ctx->fillRect(KDRect(k_separatorThickness, k_separatorThickness, width-2*k_separatorThickness, height-k_separatorThickness), backgroundColor);
  ctx->fillRect(KDRect(0, 0, width, k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(0, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(width-k_separatorThickness, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
 }

int TextExpressionMenuListCell::numberOfSubviews() const {
  return 2;
}

View * TextExpressionMenuListCell::subviewAtIndex(int index) {
  assert(index == 0 || index == 1);
  if (index == 0) {
    return &m_labelExpressionView;
  }
  return &m_accessoryView;
}

void TextExpressionMenuListCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_labelExpressionView.setFrame(KDRect(k_separatorThickness, k_separatorThickness, width, height/2 - k_separatorThickness));
  m_accessoryView.setFrame(KDRect(k_separatorThickness, height/2, width, height/2 - k_separatorThickness));
}
