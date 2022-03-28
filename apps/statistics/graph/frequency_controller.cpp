#include "frequency_controller.h"
#include <assert.h>

namespace Statistics {

void FrequencyController::viewWillAppear() {
  PlotController::viewWillAppear();
  // Setup the adequate cursor
  // TODO : Set it up before reloading curve view, but after sanitizing series
  if (m_continuousCursor) {
    m_roundCursorView.setColor(Shared::DoublePairStore::colorOfSeriesAtIndex(selectedSeriesIndex()));
  }
  m_curveView.setCursorView(m_continuousCursor ? &m_roundCursorView : &m_cursorView);
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
  int index = *m_selectedBarIndex;
  while (x < valueAtIndex(series, index)) {
    int previousIndex = SanitizeIndex(index - 1, totValues);
    if (previousIndex == index) {
      // Minimum reached, cap cursor's position.
      x = valueAtIndex(series, index);
      break;
    }
    index = previousIndex;
  }

  // Find the first index of value strictly above x.
  int nextIndex = SanitizeIndex(index + 1, totValues);
  while (x >= valueAtIndex(series, nextIndex)) {
    int nextNextIndex = SanitizeIndex(nextIndex + 1, totValues);
    if (nextNextIndex == nextIndex) {
      // Maximum reached, cap cursor's position.
      x = valueAtIndex(series, nextIndex);
      break;
    }
    nextIndex = nextNextIndex;
  }

  // Set the selected index
  index = SanitizeIndex(nextIndex - 1, totValues);
  *m_selectedBarIndex = index;

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
  // When going down, we first use the discreet cursor, then the continuous one
  if (m_continuousCursor == (direction == -1)) {
    int previousSeries = selectedSeriesIndex();
    bool result = PlotController::moveSelectionVertically(direction);
    if (result && previousSeries != selectedSeriesIndex()) {
      // Cursor has been moved into another curve
      switchCursor(true);
    }
    return result;
  }
  switchCursor(false);
  return true;
}

// TODO : Fix a bug where the previous cursor would not properly dirty itself.
void FrequencyController::switchCursor(bool seriesChanged) {
  int series = selectedSeriesIndex();
  m_continuousCursor = !m_continuousCursor;
  if (m_continuousCursor) {
    m_roundCursorView.setColor(Shared::DoublePairStore::colorOfSeriesAtIndex(series));
  }
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
