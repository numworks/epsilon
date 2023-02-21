#include <assert.h>
#include <escher/container.h>
#include <escher/editable_expression_cell.h>
#include <escher/palette.h>
#include <poincare/print_float.h>

namespace Escher {

EditableExpressionCell::EditableExpressionCell(
    Responder* parentResponder,
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    LayoutFieldDelegate* layoutDelegate)
    : HighlightCell(),
      Responder(parentResponder),
      m_expressionField(this, inputEventHandlerDelegate, layoutDelegate) {
  m_expressionField.setMargins(k_topMargin, k_margin, k_margin, k_margin);
  m_expressionBody[0] = 0;
}

void EditableExpressionCell::drawRect(KDContext* ctx, KDRect rect) const {
  drawBorderOfRect(ctx, bounds(), Palette::GrayBright);
}

void EditableExpressionCell::setHighlighted(bool highlight) {
  // We want the background to stay white to improve visibility
  return;
}

void EditableExpressionCell::layoutSubviews(bool force) {
  m_expressionField.setFrame(bounds().trimmedBy(k_separatorThickness), force);
}

void EditableExpressionCell::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_expressionField);
}

KDSize EditableExpressionCell::minimalSizeForOptimalDisplay() const {
  KDSize size = m_expressionField.minimalSizeForOptimalDisplay();
  return KDSize(size.width(), std::max(size.height(), k_minimalHeigh));
}

}  // namespace Escher
