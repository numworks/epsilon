#include "column_title_cell.h"

namespace Regression {

void ColumnTitleCell::setColor(KDColor color) {
  m_functionColor = color;
  m_firstBufferTextView.setTextColor(color);
  m_secondBufferTextView.setTextColor(color);
  reloadCell();
}

void ColumnTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddDoubleBufferTextCellWithSeparator::drawRect(ctx, rect);
  ctx->fillRect(KDRect(Metric::TableSeparatorThickness, 0, bounds().width(), k_colorIndicatorThickness), m_functionColor);
}

void ColumnTitleCell::layoutSubviews() {
  KDCoordinate width = bounds().width() - Metric::TableSeparatorThickness;
  KDCoordinate height = bounds().height();
  m_firstBufferTextView.setFrame(KDRect(Metric::TableSeparatorThickness, k_colorIndicatorThickness, width/2, height - k_colorIndicatorThickness));
  m_secondBufferTextView.setFrame(KDRect(Metric::TableSeparatorThickness + width/2, k_colorIndicatorThickness, width - width/2, height - k_colorIndicatorThickness));
}

}
