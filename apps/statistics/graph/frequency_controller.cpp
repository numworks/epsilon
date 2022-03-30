#include "frequency_controller.h"
#include <assert.h>

namespace Statistics {

void FrequencyController::viewWillAppearBeforeReload() {
  // Setup the adequate cursor before reloading the curve view
  if (m_continuousCursor) {
    m_roundCursorView.setColor(Shared::DoublePairStore::colorOfSeriesAtIndex(selectedSeriesIndex()));
  }
  #ifdef GRAPH_CURSOR_SPEEDUP
  if (m_continuousCursor) {
    m_roundCursorView.resetMemoization();
  }
  #endif
  m_curveView.setCursorView(m_continuousCursor ? &m_roundCursorView : &m_cursorView);
  PlotController::viewWillAppearBeforeReload();
}

void FrequencyController::computeYBounds(float * yMin, float *yMax) const {
  // Frequency curve is always bounded between 0 and 100
  *yMin = 0.0f;
  *yMax = 100.0f;
}

void FrequencyController::computeXBounds(float * xMin, float *xMax) const {
  *xMin = m_store->minValueForAllSeries(true);
  *xMax = m_store->maxValueForAllSeries(true);
}

void FrequencyController::appendLabelSuffix(Shared::CurveView::Axis axis, char * labelBuffer, int maxSize, int glyphLength, int maxGlyphLength) const {
  if (axis == Shared::CurveView::Axis::Horizontal) {
    return;
  }
  int length = strlen(labelBuffer);
  if (glyphLength >= maxGlyphLength || length + 1 >= maxSize) {
    // Suffix must fit, empty the label.
    labelBuffer[0] = 0;
    return;
  }
  assert(labelBuffer[length-1] != '%');
  labelBuffer[length] = '%';
  labelBuffer[length+1] = 0;
}

int FrequencyController::getNextIndex(int series, int totValues, int startIndex, int direction, double * x) const {
  assert(direction != 0);
  int index = SanitizeIndex(startIndex, totValues);
  /* If directions is > 0, find the first index of value strictly above x
   * Otherwise, find the first index of value equal or under x */
  while ((direction > 0) == (*x >= valueAtIndex(series, index))) {
    int nextIndex = SanitizeIndex(index + direction, totValues);
    if (nextIndex == index) {
      // Minimum reached, cap cursor's position.
      *x = valueAtIndex(series, index);
      break;
    }
    index = nextIndex;
  }
  return index;
}

bool FrequencyController::moveSelectionHorizontally(int deltaIndex) {
  if (!m_continuousCursor) {
    return PlotController::moveSelectionHorizontally(deltaIndex);
  }
  int series = selectedSeriesIndex();
  assert(series >= 0);
  int totValues = totalValues(series);
  if (totValues <= 1) {
    return false;
  }
  // Increment cursor's position
  double step = deltaIndex * static_cast<double>(m_graphRange.xGridUnit())/static_cast<double>(k_numberOfCursorStepsInGradUnit);
  double x = m_cursor.x() + step;

  // Find an index of value under x.
  int index = getNextIndex(series, totValues, *m_selectedBarIndex, -1, &x);

  // Find the first index of value strictly above x.
  int nextIndex = getNextIndex(series, totValues, index + 1, 1, &x);

  // Set the selected index
  *m_selectedBarIndex = index = SanitizeIndex(nextIndex - 1, totValues);
  assert(index != nextIndex);

  // Compute the cursor's position on the segment between the tow indexes
  double xIndex = valueAtIndex(series, index);
  double yIndex = resultAtIndex(series, index);
  double xNextIndex = valueAtIndex(series, nextIndex);
  double yNextIndex = resultAtIndex(series, nextIndex);
  double y = yIndex + (yNextIndex - yIndex) * ((x - xIndex) / (xNextIndex - xIndex));
  m_cursor.moveTo(x, x, y);
  m_curveView.reload();
  return true;
}

bool FrequencyController::moveSelectionVertically(int direction) {
  /* Move selection to another curve (or to tab headers) when :
   * - Continuous cursor is selected, and selection is going down (direction==1)
   * - Continuous cursor isn't selected, and selection is going up
   * */
  if (m_continuousCursor == (direction == 1)) {
    int previousSeries = selectedSeriesIndex();
    bool result = PlotController::moveSelectionVertically(direction);
    if (result && previousSeries != selectedSeriesIndex()) {
      // Cursor has been moved into another curve, cursor must be switched
      switchCursor(true);
    }
    return result;
  }
  // Otherwise, just switch to the other cursor.
  switchCursor(false);
  return true;
}

void FrequencyController::switchCursor(bool seriesChanged) {
  int series = selectedSeriesIndex();
  m_continuousCursor = !m_continuousCursor;
  if (m_continuousCursor) {
    m_roundCursorView.setColor(Shared::DoublePairStore::colorOfSeriesAtIndex(series));
  }
  #ifdef GRAPH_CURSOR_SPEEDUP
  m_roundCursorView.resetMemoization();
  #endif
  m_curveView.setCursorView(m_continuousCursor ? &m_roundCursorView : &m_cursorView);
  if (!m_continuousCursor || seriesChanged) {
    // Cursor must be repositionned
    double x = valueAtIndex(series, *m_selectedBarIndex);
    double y = resultAtIndex(series, *m_selectedBarIndex);
    m_cursor.moveTo(x, x, y);
    m_curveView.reload();
  }
}

}  // namespace Statistics
