#include "text_field_function_title_cell.h"
#include "storage_list_controller.h"
#include <assert.h>

namespace Graph {

TextFieldFunctionTitleCell::TextFieldFunctionTitleCell(StorageListController * listController, Orientation orientation, const KDFont * font) :
  Shared::FunctionTitleCell(orientation),
  Responder(listController),
  m_textField(Shared::StorageFunction::k_parenthesedArgumentLength, this, m_textFieldBuffer, m_textFieldBuffer, k_textFieldBufferSize, nullptr, listController, false, font, 0.5f, 0.5f)
  {}

void TextFieldFunctionTitleCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_textField.setBackgroundColor(EvenOddCell::backgroundColor());
}

Responder * TextFieldFunctionTitleCell::responder() {
  return isEditing() ? this : nullptr;
}

void TextFieldFunctionTitleCell::setEditing(bool editing) {
  app()->setFirstResponder(&m_textField);
  const char * previousText = m_textField.text();
  m_textField.setEditing(true, false);
  m_textField.setText(previousText);
}

bool TextFieldFunctionTitleCell::isEditing() const {
  return m_textField.isEditing();
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

void TextFieldFunctionTitleCell::didBecomeFirstResponder() {
  if (isEditing()) {
    app()->setFirstResponder(&m_textField);
  }
}

KDRect TextFieldFunctionTitleCell::textFieldFrame() const {
  KDRect textFrame(0, k_colorIndicatorThickness, bounds().width(), bounds().height() - k_colorIndicatorThickness);
  if (m_orientation == Orientation::VerticalIndicator){
    textFrame = KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness-k_separatorThickness, bounds().height()-k_separatorThickness);
  }
  return textFrame;
}

}
