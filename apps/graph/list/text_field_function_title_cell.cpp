#include "text_field_function_title_cell.h"
#include "list_controller.h"
#include <assert.h>
#include <algorithm>

namespace Graph {

TextFieldFunctionTitleCell::TextFieldFunctionTitleCell(ListController * listController, Orientation orientation, const KDFont * font) :
  Shared::FunctionTitleCell(orientation),
  Responder(listController),
  m_textField(Shared::Function::k_parenthesedThetaArgumentByteLength, this, m_textFieldBuffer, k_textFieldBufferSize, k_textFieldBufferSize, nullptr, listController, font, 1.0f, 0.5f)
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
  Container::activeApp()->setFirstResponder(&m_textField);
  const char * previousText = m_textField.text();
  int extensionLength = UTF8Helper::HasCodePoint(previousText, UCodePointGreekSmallLetterTheta) ? Shared::Function::k_parenthesedThetaArgumentByteLength : Shared::Function::k_parenthesedXNTArgumentByteLength;
  m_textField.setExtensionLength(extensionLength);
  m_textField.setEditing(true);
  m_textField.setText(previousText);
  m_textField.setDraftTextBufferSize(Poincare::SymbolAbstract::k_maxNameSize+extensionLength);
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

void TextFieldFunctionTitleCell::setHorizontalAlignment(float alignment) {
  assert(alignment >= 0.0f && alignment <= 1.0f);
  m_textField.setAlignment(alignment, verticalAlignment());
}

void TextFieldFunctionTitleCell::layoutSubviews(bool force) {
  KDRect frame = subviewFrame();
  m_textField.setFrame(frame, force);
  KDCoordinate maxTextFieldX = frame.width() - m_textField.minimalSizeForOptimalDisplay().width();
  float horizontalAlignment = std::max(
      0.0f,
      std::min(
        1.0f,
        ((float)(maxTextFieldX - k_textFieldRightMargin))/((float)maxTextFieldX)));
  m_textField.setAlignment(horizontalAlignment, verticalAlignment());
}

void TextFieldFunctionTitleCell::didBecomeFirstResponder() {
  if (isEditing()) {
    Container::activeApp()->setFirstResponder(&m_textField);
  }
}

float TextFieldFunctionTitleCell::verticalAlignmentGivenExpressionBaselineAndRowHeight(KDCoordinate expressionBaseline, KDCoordinate rowHeight) const {
  assert(m_orientation == Orientation::VerticalIndicator);
  KDCoordinate glyphHeight = font()->glyphSize().height();
  return ((float)(expressionBaseline - glyphHeight/2))/((float)rowHeight+1-glyphHeight);
}

}
