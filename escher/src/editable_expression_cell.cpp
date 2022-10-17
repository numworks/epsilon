#include <escher/editable_expression_cell.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <poincare/print_float.h>
#include <assert.h>

namespace Escher {

EditableExpressionCell::EditableExpressionCell(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textDelegate, LayoutFieldDelegate * layoutDelegate) :
  HighlightCell(),
  Responder(parentResponder),
  m_expressionField(this, inputEventHandlerDelegate, textDelegate, layoutDelegate)
{
  m_expressionBody[0] = 0;
}

void EditableExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  drawBorderOfRect(ctx, bounds(), Palette::GrayBright);
}

void EditableExpressionCell::setHighlighted(bool highlight) {
  // We want the background to stay white to improve visibility
  return;
}

void EditableExpressionCell::layoutSubviews(bool force) {
  KDRect cellBounds = bounds();
  m_expressionField.setFrame(KDRect(cellBounds.x() + k_separatorThickness,
                                    cellBounds.y(),
                                    cellBounds.width() - 2 * k_separatorThickness,
                                    cellBounds.height() - k_separatorThickness),
                             force);
}

void EditableExpressionCell::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_expressionField);
}

}
