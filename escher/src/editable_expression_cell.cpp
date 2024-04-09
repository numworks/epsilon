#include <assert.h>
#include <escher/container.h>
#include <escher/editable_expression_cell.h>
#include <escher/palette.h>
#include <poincare/print_float.h>

namespace Escher {

EditableExpressionCell::EditableExpressionCell(
    Responder* parentResponder, LayoutFieldDelegate* layoutDelegate)
    : HighlightCell(),
      Responder(parentResponder),
      m_layoutField(this, layoutDelegate) {
  m_layoutField.setMargins(k_margins);
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
  setChildFrame(&m_layoutField, bounds().trimmedBy(k_separatorThickness),
                force);
}

void EditableExpressionCell::didBecomeFirstResponder() {
  App::app()->setFirstResponder(&m_layoutField);
}

KDSize EditableExpressionCell::minimalSizeForOptimalDisplay() const {
  KDSize size = m_layoutField.minimalSizeForOptimalDisplay();
  return KDSize(size.width(),
                std::clamp(size.height(), k_minimalHeight, k_maximalHeight));
}

}  // namespace Escher
