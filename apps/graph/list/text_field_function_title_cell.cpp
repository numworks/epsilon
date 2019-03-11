#include "text_field_function_title_cell.h"
#include "storage_list_controller.h"
#include <assert.h>

namespace Graph {

static inline float minFloat(float x, float y) { return x < y ? x : y; }
static inline float maxFloat(float x, float y) { return x > y ? x : y; }

TextFieldFunctionTitleCell::TextFieldFunctionTitleCell(StorageListController * listController, Orientation orientation, const KDFont * font) :
  Shared::FunctionTitleCell(orientation),
  Responder(listController),
  m_textField(Shared::Function::k_parenthesedArgumentLength, this, m_textFieldBuffer, m_textFieldBuffer, k_textFieldBufferSize, nullptr, listController, false, font, 1.0f, 0.5f)
{
}

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

void TextFieldFunctionTitleCell::setHorizontalAlignment(float alignment) {
  assert(alignment >= 0.0f && alignment <= 1.0f);
  m_textField.setAlignment(alignment, verticalAlignment());
}

void TextFieldFunctionTitleCell::layoutSubviews() {
  KDRect frame = subviewFrame();
  m_textField.setFrame(frame);
  KDCoordinate maxTextFieldX = frame.width() - m_textField.minimalSizeForOptimalDisplay().width();
  float horizontalAlignment = maxFloat(
      0.0f,
      minFloat(
        1.0f,
        ((float)(maxTextFieldX - k_textFieldRightMargin))/((float)maxTextFieldX)));
  m_textField.setAlignment(horizontalAlignment, verticalAlignment());
}

void TextFieldFunctionTitleCell::didBecomeFirstResponder() {
  if (isEditing()) {
    app()->setFirstResponder(&m_textField);
  }
}

float TextFieldFunctionTitleCell::verticalAlignmentGivenExpressionBaselineAndRowHeight(KDCoordinate expressionBaseline, KDCoordinate rowHeight) const {
  assert(m_orientation == Orientation::VerticalIndicator);
  KDCoordinate glyphHeight = font()->glyphSize().height();
  return ((float)(expressionBaseline - glyphHeight/2))/((float)rowHeight+1-glyphHeight);
}

}
