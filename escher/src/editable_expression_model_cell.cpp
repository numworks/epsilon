#include <assert.h>
#include <escher/editable_expression_model_cell.h>

namespace Escher {

EditableExpressionModelCell::EditableExpressionModelCell(
    Responder* parentResponder,
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    LayoutFieldDelegate* layoutDelegate, float horizontalAlignment,
    float verticalAlignment, KDColor textColor, KDColor backgroundColor,
    KDFont::Size font)
    : HighlightCell(),
      m_expressionField(parentResponder, inputEventHandlerDelegate,
                        layoutDelegate, horizontalAlignment, verticalAlignment),
      m_leftMargin(0),
      m_rightMargin(0) {
  m_expressionField.setLeftMargin(Metric::EditableExpressionAdditionalMargin);
}

KDSize EditableExpressionModelCell::minimalSizeForOptimalDisplay() const {
  KDSize expressionSize = m_expressionField.minimalSizeForOptimalDisplay();
  return KDSize(m_leftMargin + expressionSize.width() + m_rightMargin,
                expressionSize.height());
}

void EditableExpressionModelCell::setMargins(KDCoordinate leftMargin,
                                             KDCoordinate rightMargin) {
  m_leftMargin = leftMargin;
  m_rightMargin = rightMargin;
  layoutSubviews();
}

void EditableExpressionModelCell::drawRect(KDContext* ctx, KDRect rect) const {
  // Color the margins
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  ctx->fillRect(KDRect(0, 0, m_leftMargin, bounds().height()), backgroundColor);
  ctx->fillRect(KDRect(bounds().width() - m_rightMargin, 0, m_rightMargin,
                       bounds().height()),
                backgroundColor);
}

void EditableExpressionModelCell::layoutSubviews(bool force) {
  m_expressionField.setFrame(
      KDRect(m_leftMargin, 0, bounds().width() - m_leftMargin - m_rightMargin,
             bounds().height()),
      force);
}

}  // namespace Escher
