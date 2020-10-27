#include "column_title_cell.h"

using namespace Escher;

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

void ColumnTitleCell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width() - Metric::TableSeparatorThickness;
  KDCoordinate height = bounds().height();
  m_firstBufferTextView.setFrame(KDRect(Metric::TableSeparatorThickness, k_colorIndicatorThickness, width/2, height - k_colorIndicatorThickness), force);
  m_secondBufferTextView.setFrame(KDRect(Metric::TableSeparatorThickness + width/2, k_colorIndicatorThickness, width - width/2, height - k_colorIndicatorThickness), force);
}

}
