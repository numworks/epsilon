#include <assert.h>
#include <escher/editable_expression_model_cell.h>

namespace Escher {

EditableExpressionModelCell::EditableExpressionModelCell(
    Responder* parentResponder, LayoutFieldDelegate* layoutDelegate,
    KDGlyph::Format format)
    : HighlightCell(),
      m_layoutField(parentResponder, layoutDelegate, format),
      m_margins() {
  m_layoutField.margins()->setLeft(Metric::EditableExpressionAdditionalMargin);
}

KDSize EditableExpressionModelCell::minimalSizeForOptimalDisplay() const {
  KDSize layoutSize = m_layoutField.minimalSizeForOptimalDisplay();
  return KDSize(m_margins.width() + layoutSize.width(), layoutSize.height());
}

void EditableExpressionModelCell::setMargins(KDHorizontalMargins margins) {
  m_margins = margins;
  layoutSubviews();
}

void EditableExpressionModelCell::drawRect(KDContext* ctx, KDRect rect) const {
  // Color the margins
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  ctx->fillRect(KDRect(0, 0, m_margins.left(), bounds().height()),
                backgroundColor);
  ctx->fillRect(KDRect(bounds().width() - m_margins.right(), 0,
                       m_margins.right(), bounds().height()),
                backgroundColor);
}

void EditableExpressionModelCell::layoutSubviews(bool force) {
  setChildFrame(&m_layoutField,
                KDRect(m_margins.left(), 0,
                       bounds().width() - m_margins.width(), bounds().height()),
                force);
}

}  // namespace Escher
