#include "layout_junior_view.h"

using namespace CalculationJunior;

void LayoutJuniorView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  if (m_layout.isInitialized()) {
    // Center the layout
    KDSize LayoutSize = m_layout.size(m_font);
    KDCoordinate x = width / 2 - LayoutSize.width() / 2;
    KDCoordinate y = height / 2 - LayoutSize.height() / 2;
    m_layout.draw(ctx, KDPoint(x, y), m_font);
  }
  ctx->fillRect(KDRect(0, 0, width, 1), KDColorBlack);
  ctx->fillRect(KDRect(0, height - 1, width, 1), KDColorBlack);
}

void LayoutJuniorView::setLayout(PoincareJ::Layout layout) {
  m_layout = layout;
  markRectAsDirty(bounds());
}
