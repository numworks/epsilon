#include <assert.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/theme/color_picker.h>

using namespace Poincare;

namespace Escher {

EvenOddExpressionCell::EvenOddExpressionCell(const TextFormat& format)
    : EvenOddCell(), m_layoutView(format), m_margins() {}

void EvenOddExpressionCell::setLayout(const Layout& layoutR) {
  m_layoutView.setLayout(layoutR);
}

void EvenOddExpressionCell::setBackgroundColor(ThemeComponent backgroundColor) {
  m_layoutView.setBackgroundColor(backgroundColor);
}

void EvenOddExpressionCell::setTextColor(ThemeComponent textColor) {
  m_layoutView.setTextColor(textColor);
}

KDSize EvenOddExpressionCell::minimalSizeForOptimalDisplay() const {
  KDSize layoutSize = m_layoutView.minimalSizeForOptimalDisplay();
  return KDSize(m_margins.width() + layoutSize.width(), layoutSize.height());
}

void EvenOddExpressionCell::setMargins(KDHorizontalMargins margins) {
  m_margins = margins;
  layoutSubviews();
}

void EvenOddExpressionCell::drawRect(KDContext* ctx, KDRect rect) const {
  // Color the margins
  ctx->fillRect(KDRect(0, 0, m_margins.left(), bounds().height()),
                ColorPicker::Resolve(backgroundColor()));
  ctx->fillRect(KDRect(bounds().width() - m_margins.right(), 0,
                       m_margins.right(), bounds().height()),
                ColorPicker::Resolve(backgroundColor()));
}

void EvenOddExpressionCell::updateSubviewsBackgroundAfterChangingState() {
  m_layoutView.setBackgroundColor(backgroundColor());
}

int EvenOddExpressionCell::numberOfSubviews() const { return 1; }

View* EvenOddExpressionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_layoutView;
}

void EvenOddExpressionCell::layoutSubviews(bool force) {
  setChildFrame(&m_layoutView,
                KDRect(m_margins.left(), 0,
                       bounds().width() - m_margins.width(), bounds().height()),
                force);
}

}  // namespace Escher
