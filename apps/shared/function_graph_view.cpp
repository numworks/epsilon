#include "function_graph_view.h"
#include <assert.h>
#include <cmath>
#include <float.h>
using namespace Poincare;

namespace Shared {

FunctionGraphView::FunctionGraphView(InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, BannerView * bannerView, CursorView * cursorView) :
  LabeledCurveView(graphRange, cursor, bannerView, cursorView),
  m_highlightedStart(NAN),
  m_highlightedEnd(NAN),
  m_shouldColorHighlighted(false),
  m_functionsInterrupted(0),
  m_context(nullptr)
{
}

void FunctionGraphView::reload(bool resetInterrupted, bool force) {
  if (force || resetInterrupted) {
    resetCurvesInterrupted();
  }
  LabeledCurveView::reload(resetInterrupted, force);
}

void FunctionGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  simpleDrawBothAxesLabels(ctx, rect);
}

void FunctionGraphView::setContext(Context * context) {
  m_context = context;
}

Context * FunctionGraphView::context() const {
  return m_context;
}

void FunctionGraphView::selectRecord(Ion::Storage::Record record) {
  if (m_selectedRecord != record) {
    m_selectedRecord = record;
    reloadBetweenBounds(m_highlightedStart, m_highlightedEnd);
  }
}

void FunctionGraphView::selectSecondRecord(Ion::Storage::Record record) {
  if (m_secondSelectedRecord != record) {
    m_secondSelectedRecord = record;
    reloadBetweenBounds(m_highlightedStart, m_highlightedEnd);
  }
}

void FunctionGraphView::setAreaHighlight(float start, float end) {
  if (m_highlightedStart != start || m_highlightedEnd != end) {
    float dirtyStart = start > m_highlightedStart ? m_highlightedStart : start;
    float dirtyEnd = start > m_highlightedStart ? start : m_highlightedStart;
    reloadBetweenBounds(dirtyStart, dirtyEnd);
    dirtyStart = end > m_highlightedEnd ? m_highlightedEnd : end;
    dirtyEnd = end > m_highlightedEnd ? end : m_highlightedEnd;
    reloadBetweenBounds(dirtyStart, dirtyEnd);
    if (std::isnan(start) || std::isnan(end)) {
      reloadBetweenBounds(m_highlightedStart, m_highlightedEnd);
    }
    if (std::isnan(m_highlightedStart) || std::isnan(m_highlightedEnd)) {
      reloadBetweenBounds(start, end);
    }
    m_highlightedStart = start;
    m_highlightedEnd = end;
  }
}

void FunctionGraphView::setAreaHighlightColor(bool highlightColor) {
  if (m_shouldColorHighlighted != highlightColor) {
    reloadBetweenBounds(m_highlightedStart, m_highlightedEnd);
    m_shouldColorHighlighted = highlightColor;
  }
}

void FunctionGraphView::reloadBetweenBounds(float start, float end) {
  if (start == end) {
    return;
  }
  float pixelLowerBound = floatToPixel(Axis::Horizontal, start) - 2.0f;
  float pixelUpperBound = floatToPixel(Axis::Horizontal, end) + 4.0f;
  /* We exclude the banner frame from the dirty zone to avoid unnecessary
   * redrawing */
  KDRect dirtyZone(KDRect(pixelLowerBound, 0, pixelUpperBound-pixelLowerBound,
        bounds().height()-m_bannerView->bounds().height()));
  markRectAsDirty(dirtyZone);
}

bool FunctionGraphView::allFunctionsInterrupted(int numberOfFunctions) const {
  /* The number of functions displayed at the same time is theoretically unbounded, but we only store the status of 32 functions. */
  if (numberOfFunctions <= 0 || static_cast<size_t>(numberOfFunctions) > 8 * sizeof(m_functionsInterrupted)) {
    return false;
  }
  return m_functionsInterrupted == static_cast<uint32_t>((1 << numberOfFunctions) - 1);
}

bool FunctionGraphView::functionWasInterrupted(int index) const {
  if (index < 0 || static_cast<size_t>(index) >= 8 * sizeof(m_functionsInterrupted)) {
    return false;
  }
  return (1 << index) & m_functionsInterrupted;
}

void FunctionGraphView::setFunctionInterrupted(int index) const {
  if (index >= 0 && static_cast<size_t>(index) < 8 * sizeof(m_functionsInterrupted)) {
    m_functionsInterrupted |= 1 << index;
  }
}

}
