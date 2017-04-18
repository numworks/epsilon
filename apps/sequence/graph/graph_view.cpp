#include "graph_view.h"

using namespace Shared;

namespace Sequence {

GraphView::GraphView(SequenceStore * sequenceStore, InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, BannerView * bannerView, View * cursorView) :
  FunctionGraphView(graphRange, cursor, bannerView, cursorView),
  m_sequenceStore(sequenceStore),
  m_verticalCursor(false),
  m_highlightedDotStart(-1),
  m_highlightedDotEnd(-1),
  m_shouldColorHighlighted(false),
  m_selectedSequence(nullptr)
{
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  FunctionGraphView::drawRect(ctx, rect);
  for (int i = 0; i < m_sequenceStore->numberOfActiveFunctions(); i++) {
    Sequence * s = m_sequenceStore->activeFunctionAtIndex(i);
    float rectMin = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
    float rectMax = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
    /* We draw a dot at every integer WindowRange/Resolution < 1. Otherwise, we
     * draw a dot every step where step is an integer. */
    int step = ceilf((rectMax - rectMin)/resolution());
    for (int x = rectMin; x < rectMax; x += step) {
      float y = evaluateModelWithParameter(s, x);
      if (isnan(y)) {
        continue;
      }
      drawDot(ctx, rect, x, y, s->color());
      if (x >= m_highlightedDotStart && x <= m_highlightedDotEnd && s == m_selectedSequence) {
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

void GraphView::setVerticalCursor(bool verticalCursor) {
  m_verticalCursor = verticalCursor;
}

void GraphView::reload() {
  FunctionGraphView::reload();
  if (m_highlightedDotStart >= 0) {
    float pixelLowerBound = floatToPixel(Axis::Horizontal, m_highlightedDotStart)-1;
    float pixelUpperBound = floatToPixel(Axis::Horizontal, m_highlightedDotEnd)+2;
    KDRect dirtyZone(KDRect(pixelLowerBound, 0, pixelUpperBound-pixelLowerBound,
    bounds().height()));
    markRectAsDirty(dirtyZone);
  }
}

void GraphView::selectSequence(Sequence * sequence) {
  if (m_selectedSequence != sequence) {
    reload();
    m_selectedSequence = sequence;
    reload();
  }
}

void GraphView::setHighlight(int start, int end) {
  if (m_highlightedDotStart != start || m_highlightedDotEnd != end) {
    reload();
    m_highlightedDotStart = start;
    m_highlightedDotEnd = end;
    reload();
  }
}

void GraphView::setHighlightColor(bool highlightColor) {
  if (m_shouldColorHighlighted != highlightColor) {
    reload();
    m_shouldColorHighlighted = highlightColor;
    reload();
  }
}

float GraphView::samplingRatio() const {
  return 0.8f;
}

float GraphView::evaluateModelWithParameter(Model * curve, float abscissa) const {
  Sequence * s = (Sequence *)curve;
  return s->evaluateAtAbscissa(abscissa, context());
}

KDSize GraphView::cursorSize() {
  if (m_verticalCursor) {
    return KDSize(1, 2*bounds().height());
  }
  return CurveView::cursorSize();
}

}
