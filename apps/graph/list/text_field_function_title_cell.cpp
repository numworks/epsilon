#include "text_field_function_title_cell.h"
#include "storage_list_controller.h"
#include <assert.h>

namespace Graph {

TextFieldFunctionTitleCell::TextFieldFunctionTitleCell(StorageListController * listController, Orientation orientation, KDText::FontSize size) :
  Shared::FunctionTitleCell(orientation),
  Responder(listController),
  m_textField(this, m_textFieldBuffer, m_textFieldBuffer, k_textFieldBufferSize, listController, false, size, 0.5f, 0.5f)
  {}

void TextFieldFunctionTitleCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_textField.setBackgroundColor(EvenOddCell::backgroundColor());
}

void TextFieldFunctionTitleCell::setEditing(bool editing) {
  app()->setFirstResponder(&m_textField);
  const char * previousText = m_textField.text();
  m_textField.setEditing(true, false);
  m_textField.setText(previousText);
  m_textField.setCursorLocation(strlen(previousText) - strlen("(x)"));
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
