#include "pattern_view.h"

namespace HardwareTest {

PatternView::PatternView() :
  m_pattern(Pattern::patternAtIndex(0))
{
}

void PatternView::setPattern(Pattern p) {
  m_pattern = p;
  markRectAsDirty(bounds());
}

void PatternView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_pattern.outlineColor());
  ctx->fillRect(KDRect(k_outlineThickness, k_outlineThickness, bounds().width()-2*k_outlineThickness, bounds().height()-2*k_outlineThickness), m_pattern.fillColor());
}

}
