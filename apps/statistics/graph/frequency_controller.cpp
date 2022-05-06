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

bool FrequencyController::getNextCursorPosition(int totValues, double step, double * x, double * y) {
  /* Virtually increase the step so that if the cursor were to move very close
   * to a significant value, it will instead snap on this value. */
  double latchFactor = 1.5;

  int index, nextIndex;
  if (step > 0.0) {
    index = m_selectedIndex;
    nextIndex = m_selectedIndex + 1;
    if (nextIndex >= totValues) {
      // Cursor cannot move further
      return false;
    }
    double xNextIndex = valueAtIndex(m_selectedSeries, nextIndex);
    if (*x + step * latchFactor >= xNextIndex) {
      // Step is too big, snap on the next interesting value
      *x = xNextIndex;
      m_selectedIndex = nextIndex;
      *y = resultAtIndex(m_selectedSeries, m_selectedIndex);
      return true;
    }
  } else {
    double xSelectedIndex = valueAtIndex(m_selectedSeries, m_selectedIndex);
    assert(*x >= xSelectedIndex);
    index = (*x == xSelectedIndex) ? m_selectedIndex - 1 : m_selectedIndex;
    nextIndex = index + 1;
    if (index < 0) {
      // Cursor cannot move further
      return false;
    }
    double xIndex = valueAtIndex(m_selectedSeries, index);
    if (*x + step * latchFactor <= xIndex) {
      // Step is too big, snap on the previous interesting value
      *x = xIndex;
      m_selectedIndex = index;
      *y = resultAtIndex(m_selectedSeries, m_selectedIndex);
      return true;
    }
  }

  // Apply step
  *x += step;

  // Update the newly selected index
  m_selectedIndex = index;

  // Simplify cursor's position
  if (std::fabs(*x) <= std::fabs(step / 2.0)) {
    // At half a step of 0.0
    *x = 0.0;
  } else {
    /* Using pixelWidth(), simplify the cursor's position value while staying at
     * the same pixel. */
    assert(std::fabs(step / 2.0) >= m_curveView.pixelWidth());
    double magnitude = std::pow(10.0, Poincare::IEEE754<double>::exponentBase10(m_curveView.pixelWidth()) - 1.0);
    *x = magnitude * std::round(*x / magnitude);
  }

  // Compute start and end of the segment on which the cursor is
  double xIndex = valueAtIndex(m_selectedSeries, index);
  double xNextIndex = valueAtIndex(m_selectedSeries, nextIndex);
  assert(*x > xIndex && *x < xNextIndex && nextIndex == index + 1);
  double yIndex = resultAtIndex(m_selectedSeries, index);
  double yNextIndex = resultAtIndex(m_selectedSeries, nextIndex);

  // Compute the cursor's position
  *y = yIndex + (yNextIndex - yIndex) * ((*x - xIndex) / (xNextIndex - xIndex));
  return true;
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

  double x = m_cursor.x();
  double y;
  // Compute cursor step
  double step = deltaIndex * static_cast<double>(m_graphRange.xGridUnit())/static_cast<double>(k_numberOfCursorStepsInGradUnit);
  assert(step != 0.0);

  bool cursorHasMoved = getNextCursorPosition(totValues, step, &x, &y);
  if (cursorHasMoved) {
    m_cursor.moveTo(x, x, y);
    m_curveView.reload();
  }
  return cursorHasMoved;
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
