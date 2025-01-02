#include "function_graph_view.h"

#include <apps/shared/global_context.h>
#include <assert.h>
#include <float.h>
#include <poincare/old/circuit_breaker_checkpoint.h>

#include <cmath>

using namespace Poincare;

namespace Shared {

// FunctionGraphPolicy

void FunctionGraphPolicy::drawPlot(const AbstractPlotView* plotView,
                                   KDContext* ctx, KDRect rect) const {
  int n = numberOfDrawnRecords();
  int selectedIndex = selectedRecordIndex();
  bool firstDrawnRecord = true;
  for (int i = 0; i <= n; i++) {
    // Draw the selected record last so that it's on top of others
    if (i == selectedIndex) {
      continue;
    }
    int index = i;
    if (i == n) {
      index = selectedIndex;
      if (index < 0 || index >= n) {
        break;
      }
    }
    if (functionWasInterrupted(index)) {
      continue;
    }

    /* In case it's not already memoized, init the model in the function store
     * before the checkpoint because it can change the pool. */
    Ion::Storage::Record record = initModelBeforeDrawingPlot(index);

    CircuitBreakerCheckpoint checkpoint(
        Ion::CircuitBreaker::CheckpointType::Back);
    if (CircuitBreakerRun(checkpoint)) {
      drawRecord(record, index, ctx, rect, firstDrawnRecord);
    } else {
      setFunctionInterrupted(index);
      tidyModel(index, checkpoint.endOfPoolBeforeCheckpoint());
      functionStore()->tidyDownstreamPoolFrom(
          checkpoint.endOfPoolBeforeCheckpoint());
      GlobalContext::s_sequenceStore->tidyDownstreamPoolFrom(
          checkpoint.endOfPoolBeforeCheckpoint());
    }
    firstDrawnRecord = false;
  }
}

Ion::Storage::Record FunctionGraphPolicy::initModelBeforeDrawingPlot(
    int modelIndex) const {
  Ion::Storage::Record record =
      functionStore()->activeRecordAtIndex(modelIndex);
  functionStore()->modelForRecord(record);
  return record;
}

bool FunctionGraphPolicy::functionWasInterrupted(int index) const {
  if (index < 0 || static_cast<size_t>(index) >=
                       OMG::BitHelper::numberOfBitsIn(m_functionsInterrupted)) {
    return false;
  }
  return OMG::BitHelper::bitAtIndex(m_functionsInterrupted, index);
}

void FunctionGraphPolicy::setFunctionInterrupted(int index) const {
  if (index >= 0 && static_cast<size_t>(index) < OMG::BitHelper::numberOfBitsIn(
                                                     m_functionsInterrupted)) {
    OMG::BitHelper::setBitAtIndex(m_functionsInterrupted, index, true);
  }
}

// FunctionGraphView

FunctionGraphView::FunctionGraphView(InteractiveCurveViewRange* range,
                                     CurveViewCursor* cursor,
                                     BannerView* bannerView,
                                     CursorView* cursorView)
    : PlotView(range),
      m_highlightedStart(NAN),
      m_highlightedEnd(NAN),
      m_shouldColorHighlighted(false) {
  // FunctionGraphPolicy
  m_functionsInterrupted = 0;
  m_context = nullptr;
  // WithBanner
  m_banner = bannerView;
  // WithCursor
  m_cursor = cursor;
  m_cursorView = cursorView;
}

void FunctionGraphView::reload(bool resetInterrupted, bool force) {
  if (force || resetInterrupted) {
    resetInterruption();
  }
  AbstractPlotView::reload(resetInterrupted, force);
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
  KDCoordinate pixelLowerBound =
      floatToKDCoordinatePixel(OMG::Axis::Horizontal, start) - 2;
  KDCoordinate pixelUpperBound =
      floatToKDCoordinatePixel(OMG::Axis::Horizontal, end) + 4;
  /* We exclude the banner frame from the dirty zone to avoid unnecessary
   * redrawing */
  KDRect dirtyZone(KDRect(pixelLowerBound, 0, pixelUpperBound - pixelLowerBound,
                          bounds().height() - m_banner->bounds().height()));
  markRectAsDirty(dirtyZone);
}

Ion::Storage::Record FunctionGraphView::initModelBeforeDrawingPlot(
    int modelIndex) const {
  Ion::Storage::Record record =
      FunctionGraphPolicy::initModelBeforeDrawingPlot(modelIndex);
  if (record == m_selectedRecord && !m_secondSelectedRecord.isNull()) {
    functionStore()->modelForRecord(m_secondSelectedRecord);
  }
  return record;
}

}  // namespace Shared
