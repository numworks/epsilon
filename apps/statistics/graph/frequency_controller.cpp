#include "frequency_controller.h"
#include <assert.h>
#include <poincare/ieee754.h>

namespace Statistics {

void FrequencyController::viewWillAppearBeforeReload() {
  // Setup the adequate cursor before reloading the curve view
  if (m_continuousCursor) {
    m_roundCursorView.setColor(Shared::DoublePairStore::colorOfSeriesAtIndex(m_selectedSeries));
  }
  #ifdef GRAPH_CURSOR_SPEEDUP
  if (m_continuousCursor) {
    m_roundCursorView.resetMemoization();
  }
  #endif
  m_curveView.setCursorView(m_continuousCursor ? &m_roundCursorView : &m_cursorView);
  PlotController::viewWillAppearBeforeReload();
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

void FrequencyController::moveCursorVertically(bool seriesChanged) {
  m_continuousCursor = !m_continuousCursor;
  if (m_continuousCursor) {
    m_roundCursorView.setColor(Shared::DoublePairStore::colorOfSeriesAtIndex(m_selectedSeries));
  }
  #ifdef GRAPH_CURSOR_SPEEDUP
  m_roundCursorView.resetMemoization();
  #endif
  m_curveView.setCursorView(m_continuousCursor ? &m_roundCursorView : &m_cursorView);
  PlotController::moveCursorVertically(!m_continuousCursor || seriesChanged);
}

bool FrequencyController::moveSelectionHorizontally(int deltaIndex) {
  if (!m_continuousCursor) {
    return PlotController::moveSelectionHorizontally(deltaIndex);
  }
  assert(m_selectedSeries >= 0);
  int totValues = totalValues(m_selectedSeries);
  if (totValues <= 1) {
    return false;
  }
  // Increment cursor's position
  double step = deltaIndex * static_cast<double>(m_graphRange.xGridUnit())/static_cast<double>(k_numberOfCursorStepsInGradUnit);
  double x = m_cursor.x() + step;

  // Find an index of value under x.
  int index = getNextIndex(m_selectedSeries, totValues, m_selectedIndex, -1, &x);

  // Find the first index of value strictly above x.
  int nextIndex = getNextIndex(m_selectedSeries, totValues, index + 1, 1, &x);

  if (x == m_cursor.x()) {
    // Cursor did not move
    return false;
  }

  // Set the selected index
  m_selectedIndex = index = SanitizeIndex(nextIndex - 1, totValues);
  assert(index != nextIndex);

  double xIndex = valueAtIndex(m_selectedSeries, index);
  double xNextIndex = valueAtIndex(m_selectedSeries, nextIndex);

  double precision = std::fabs(step / 2.0);

  // Round cursor's position to closest interesting value
  if (std::fabs(x - xIndex) < precision) {
    x = xIndex;
  } else if (std::fabs(x - xNextIndex) < precision) {
    x = xNextIndex;
    // Update index values
    nextIndex = SanitizeIndex(nextIndex + 1, totValues);
    index = SanitizeIndex(nextIndex - 1, totValues);
    xIndex = valueAtIndex(m_selectedSeries, index);
    xNextIndex = valueAtIndex(m_selectedSeries, nextIndex);
  } else if (std::fabs(x) < precision) {
    // Round the cursor to 0 if it is close to it
    x = 0.0;
  } else {
    // Simplify the cursor's position while staying at the same pixel.
    assert(precision >= m_curveView.pixelWidth());
    double magnitude = std::pow(10.0, Poincare::IEEE754<double>::exponentBase10(m_curveView.pixelWidth()) - 1.0);
    x = magnitude * std::round(x / magnitude);
  }

  assert(x >= xIndex && x <= xNextIndex && nextIndex >= index && nextIndex - index <= 1);

  double yIndex = resultAtIndex(m_selectedSeries, index);
  double yNextIndex = resultAtIndex(m_selectedSeries, nextIndex);

  // Compute the cursor's position on the segment between the two indexes
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
    return PlotController::moveSelectionVertically(direction);
  }
  // Otherwise, just switch to the other cursor.
  moveCursorVertically(false);
  return true;
}

void FrequencyController::computeYBounds(float * yMin, float *yMax) const {
  // Frequency curve is always bounded between 0 and 100
  *yMin = 0.0f;
  *yMax = 100.0f;
}

}  // namespace Statistics
