#include "graph_view.h"
#include <poincare/circuit_breaker_checkpoint.h>
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
  int numberOfActiveFunctions = m_sequenceStore->numberOfActiveFunctions();
  if (allFunctionsInterrupted(numberOfActiveFunctions)) {
    // All functions were interrupted, do not draw anything.
    return;
  }

  FunctionGraphView::drawRect(ctx, rect);
  /* A dot is drawn at every step where step is larger than 1
   * and than a pixel's width. */
  const int step = std::ceil(pixelWidth());
  float rectXMin = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
  rectXMin = rectXMin < 0 ? 0 : rectXMin;
  float rectXMax = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
  for (int i = 0; i < numberOfActiveFunctions; i++) {
    if (functionWasInterrupted(i)) {
      continue;
    }
    Ion::Storage::Record record = m_sequenceStore->activeRecordAtIndex(i);
    Shared::Sequence * s = m_sequenceStore->modelForRecord(record);
    Poincare::UserCircuitBreakerCheckpoint checkpoint;
    if (CircuitBreakerRun(checkpoint)) {
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
    } else {
      setFunctionInterrupted(i);
      s->tidyDownstreamPoolFrom();
      m_context->tidyDownstreamPoolFrom();
    }
  }
}

}
