#include "text_field_function_title_cell.h"
#include "storage_list_controller.h"
#include <assert.h>

namespace Graph {

static inline float min(float x, float y) { return (x<y ? x : y); }
static inline float max(float x, float y) { return (x>y ? x : y); }

TextFieldFunctionTitleCell::TextFieldFunctionTitleCell(StorageListController * listController, Orientation orientation, const KDFont * font) :
  Shared::FunctionTitleCell(orientation),
  Responder(listController),
  m_textField(Shared::StorageFunction::k_parenthesedArgumentWithEqualLength, this, m_textFieldBuffer, m_textFieldBuffer, k_textFieldBufferSize, nullptr, listController, false, font, k_horizontalAlignment, 0.5f)
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
  KDRect frame = textFieldFrame();
  m_textField.setFrame(frame);
  KDCoordinate glyphHeight = font()->glyphSize().height();
  float verticalAlignment = max(0.0f, min(1.0f, m_baseline < 0 ? 0.5f : ((float)(m_baseline - glyphHeight/2))/((float)frame.height()+1-glyphHeight)));
  m_textField.setAlignment(k_horizontalAlignment, verticalAlignment);
}

void TextFieldFunctionTitleCell::didBecomeFirstResponder() {
  if (isEditing()) {
    app()->setFirstResponder(&m_textField);
  }
}

KDRect TextFieldFunctionTitleCell::textFieldFrame() const {
  assert(m_orientation == Orientation::VerticalIndicator);
  return KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness, bounds().height()-k_separatorThickness);
}

}
