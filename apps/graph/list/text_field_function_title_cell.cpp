#include "text_field_function_title_cell.h"
#include <assert.h>

namespace Graph {

void TextFieldFunctionTitleCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_textField.setBackgroundColor(EvenOddCell::backgroundColor());
}

void TextFieldFunctionTitleCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_textField.setBackgroundColor(EvenOddCell::backgroundColor());
}

void TextFieldFunctionTitleCell::setColor(KDColor color) {
  FunctionTitleCell::setColor(color);
  m_textField.setTextColor(color);
}

void TextFieldFunctionTitleCell::setText(const char * title) {
  m_textField.setText(title);
}

void TextFieldFunctionTitleCell::layoutSubviews() {
  m_textField.setFrame(textFieldFrame());
}

KDRect TextFieldFunctionTitleCell::textFieldFrame() const {
  KDRect textFrame(0, k_colorIndicatorThickness, bounds().width(), bounds().height() - k_colorIndicatorThickness);
  if (m_orientation == Orientation::VerticalIndicator){
    textFrame = KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness-k_separatorThickness, bounds().height()-k_separatorThickness);
  }
  return textFrame;
}

}
