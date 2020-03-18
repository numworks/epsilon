#include "graph_view.h"
#include <cmath>

using namespace Shared;

namespace Sequence {

GraphView::GraphView(SequenceStore * sequenceStore, InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, BannerView * bannerView, CursorView * cursorView) :
  FunctionGraphView(graphRange, cursor, bannerView, cursorView),
  m_sequenceStore(sequenceStore)
{
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  FunctionGraphView::drawRect(ctx, rect);
  /* A dot is drawn at every step where step is larger than 1
   * and than a pixel's width. */
  const int step = std::ceil(pixelWidth());
  for (int i = 0; i < m_sequenceStore->numberOfActiveFunctions(); i++) {
    Ion::Storage::Record record = m_sequenceStore->activeRecordAtIndex(i);
    Sequence * s = m_sequenceStore->modelForRecord(record);;
    float rectXMin = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
    rectXMin = rectXMin < 0 ? 0 : rectXMin;
    float rectXMax = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
    for (int x = rectXMin; x < rectXMax; x += step) {
      float y = s->evaluateXYAtParameter((float)x, context()).x2();
      if (std::isnan(y)) {
        continue;
      }
      drawDot(ctx, rect, x, y, s->color());
      if (x >= m_highlightedStart && x <= m_highlightedEnd && record == m_selectedRecord) {
        KDColor color = m_shouldColorHighlighted ? s->color() : KDColorBlack;
        if (y >= 0.0f) {
          drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, x, 0.0f, y, color, 1);
        } else {
          drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, x, y, 0.0f, color, 1);
        }
      }
    }
  }
}

}
