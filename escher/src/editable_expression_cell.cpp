#include <escher/editable_expression_cell.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <poincare/print_float.h>
#include <assert.h>

namespace Escher {

EditableExpressionCell::EditableExpressionCell(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textDelegate, LayoutFieldDelegate * layoutDelegate,
   KDFont::Size font, float horizontalAlignment, float verticalAlignment, KDColor expressionColor, KDColor backgroundColor, KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  HighlightCell(),
  Responder(parentResponder),
  m_expressionField(this, inputEventHandlerDelegate, textDelegate, layoutDelegate),//m_expressionBody, Poincare::PrintFloat::k_maxFloatCharSize, ExpressionField::maxBufferSize(), inputEventHandlerDelegate, textDelegate, font, horizontalAlignment, verticalAlignment, expressionColor, backgroundColor),
  m_topMargin(topMargin),
  m_rightMargin(rightMargin),
  m_bottomMargin(bottomMargin),
  m_leftMargin(leftMargin)
{
  m_expressionBody[0] = 0;
}

void EditableExpressionCell::setMargins(KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) {
  m_topMargin = topMargin;
  m_rightMargin = rightMargin;
  m_bottomMargin = bottomMargin;
  m_leftMargin = leftMargin;
}

ExpressionField * EditableExpressionCell::expressionField() {
  return &m_expressionField;
}

void EditableExpressionCell::setHighlighted(bool highlight) {
  // We want the background to stay white to improve visibility
  return;
}

int EditableExpressionCell::numberOfSubviews() const {
  return 1;
}

View * EditableExpressionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_expressionField;
}

void EditableExpressionCell::layoutSubviews(bool force) {
  KDRect cellBounds = bounds();
  m_expressionField.setFrame(KDRect(cellBounds.x() + m_leftMargin,
                                    cellBounds.y() + m_topMargin,
                                    cellBounds.width() - m_leftMargin - m_rightMargin,
                                    cellBounds.height() - m_topMargin - m_bottomMargin),
                             force);
}

void EditableExpressionCell::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_expressionField);
}

KDSize EditableExpressionCell::minimalSizeForOptimalDisplay() const {
  return m_expressionField.minimalSizeForOptimalDisplay();
}

}
