#include "vertical_sequence_title_cell.h"
#include <poincare/layout.h>
#include <algorithm>

namespace Sequence {

VerticalSequenceTitleCell::VerticalSequenceTitleCell() :
    SequenceTitleCell(k_font),
    m_connectColorIndicator(false)
{
  /* We do not care here about the vertical alignment, it will be set properly
   * in layoutSubviews */
  m_titleTextView.setAlignment(k_verticalOrientationHorizontalAlignment, k_verticalOrientationHorizontalAlignment);
}

void VerticalSequenceTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor separatorColor = m_even ? Escher::Palette::WallScreen : KDColorWhite;
  KDColor backgroundColor = m_even ? KDColorWhite : Escher::Palette::WallScreen;
  // Draw the color indicator
  ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, bounds().height() - (m_connectColorIndicator ? 0 : k_separatorThickness)), m_functionColor);
  // Draw the horizontal separator
  ctx->fillRect(KDRect((m_connectColorIndicator ? k_colorIndicatorThickness : 0), bounds().height() - k_separatorThickness, bounds().width() - (m_connectColorIndicator ? k_colorIndicatorThickness : 0), k_separatorThickness), separatorColor);
  // Draw some background
  ctx->fillRect(KDRect(bounds().width() - k_equalWidthWithMargins, 0, k_equalWidthWithMargins, bounds().height() - k_separatorThickness), backgroundColor);
  // Draw '='
  KDPoint p = KDPoint(bounds().width() - k_equalWidthWithMargins, m_baseline - KDFont::Font(k_font)->glyphSize().height()/2 - 1); // -1 is visually needed
  ctx->drawString("=", p, k_font, m_functionColor, backgroundColor);
}

KDRect VerticalSequenceTitleCell::subviewFrame() const {
  return KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness - k_equalWidthWithMargins, bounds().height() - k_separatorThickness);
}

void VerticalSequenceTitleCell::layoutSubviews(bool force) {
  m_titleTextView.setAlignment(k_verticalOrientationHorizontalAlignment, verticalAlignment());
  SequenceTitleCell::layoutSubviews(force);
}

float VerticalSequenceTitleCell::verticalAlignment() const {
  return std::max(0.0f, std::min(1.0f, m_baseline < 0 ? KDContext::k_alignCenter : verticalAlignmentGivenExpressionBaselineAndRowHeight(m_baseline, subviewFrame().height())));
}

float VerticalSequenceTitleCell::verticalAlignmentGivenExpressionBaselineAndRowHeight(KDCoordinate expressionBaseline, KDCoordinate rowHeight) const {
  Poincare::Layout l = layout();
  return ((float)(expressionBaseline - l.baseline(k_font))) / ((float)rowHeight - l.layoutSize(k_font).height());
}

}  // namespace Sequence
