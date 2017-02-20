#include <escher/text_expression_menu_list_cell.h>
#include <assert.h>

TextExpressionMenuListCell::TextExpressionMenuListCell(char * accessoryText) :
  ExpressionMenuListCell(),
  m_accessoryView(PointerTextView(KDText::FontSize::Small, accessoryText, 0.0f, 0.5f, Palette::GreyDark, KDColorWhite))
{
}

void TextExpressionMenuListCell::setHighlighted(bool highlight) {
  ExpressionMenuListCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void TextExpressionMenuListCell::setAccessoryText(const char * text) {
  m_accessoryView.setText(text);
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
