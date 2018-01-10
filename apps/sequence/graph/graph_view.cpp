#include "graph_view.h"
#include <cmath>

using namespace Shared;

namespace Sequence {

GraphView::GraphView(SequenceStore * sequenceStore, InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, BannerView * bannerView, View * cursorView) :
  FunctionGraphView(graphRange, cursor, bannerView, cursorView),
  m_sequenceStore(sequenceStore)
{
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  FunctionGraphView::drawRect(ctx, rect);
  for (int i = 0; i < m_sequenceStore->numberOfActiveFunctions(); i++) {
    Sequence * s = m_sequenceStore->activeFunctionAtIndex(i);
    float rectXMin = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
    rectXMin = rectXMin < 0 ? 0 : rectXMin;
    float rectXMax = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
    /* We draw a dot at every integer if WindowRange/Resolution < 1. Otherwise,
     * we draw a dot at every step where step is an integer wider than 1. */
    float windowRange = pixelToFloat(Axis::Horizontal, bounds().width()) - pixelToFloat(Axis::Horizontal, 0);
    int step = std::ceil(windowRange/resolution());
    for (int x = rectXMin; x < rectXMax; x += step) {
      float y = s->evaluateAtAbscissa((float)x, context());
      if (std::isnan(y)) {
        continue;
      }
      drawDot(ctx, rect, x, y, s->color());
      if (x >= m_highlightedStart && x <= m_highlightedEnd && s == m_selectedFunction) {
        KDColor color = m_shouldColorHighlighted ? s->color() : KDColorBlack;
        if (y >= 0.0f) {
          drawSegment(ctx, rect, Axis::Vertical, x, 0.0f, y, color, 1);
        } else {
          drawSegment(ctx, rect, Axis::Vertical, x, y, 0.0f, color, 1);
        }
      }
    }
  }
}

float GraphView::samplingRatio() const {
  return 5.0f;
}

}
