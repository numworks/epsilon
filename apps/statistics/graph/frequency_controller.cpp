#include "frequency_controller.h"
#include <assert.h>
#include <poincare/ieee754.h>

namespace Statistics {

FrequencyController::FrequencyController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Escher::Responder * tabController, Escher::StackViewController * stackViewController, Escher::ViewController * typeViewController, Store * store) :
    PlotController(parentResponder, header, tabController, stackViewController, typeViewController, store) {
  m_curveView.setCursorView(&m_roundCursorView);
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

void FrequencyController::moveCursorToSelectedIndex() {
  m_roundCursorView.setColor(Escher::Palette::YellowDark);
  PlotController::moveCursorToSelectedIndex();
}

bool FrequencyController::moveSelectionHorizontally(int deltaIndex) {
  assert(m_selectedSeries >= 0);
  int totValues = totalValues(m_selectedSeries);
  if (totValues <= 1) {
    return false;
  }

  // Compute cursor step
  double step = deltaIndex * static_cast<double>(m_graphRange.xGridUnit())/static_cast<double>(k_numberOfCursorStepsInGradUnit);
  assert(step != 0.0);

  double x = m_cursor.x();

  /* Virtually increase the step so that if the cursor were to move very close
   * to a significant value, it will instead snap on this value. */
  double snapFactor = 1.5;

  assert(x >= valueAtIndex(m_selectedSeries, m_selectedIndex));
  int index = (step > 0.0 || x > valueAtIndex(m_selectedSeries, m_selectedIndex)) ? m_selectedIndex : m_selectedIndex - 1;
  int nextIndex = index + 1;
  if (index < 0 || (step > 0.0 && nextIndex >= totValues)) {
    // Cursor cannot move further
    return false;
  }
  double xTarget = x + step * snapFactor;
  double xIndex = valueAtIndex(m_selectedSeries, index);
  double xNextIndex = valueAtIndex(m_selectedSeries, nextIndex);
  if (xTarget <= xIndex || xTarget >= xNextIndex) {
    assert((xTarget <= xIndex) == (step < 0.0));
    assert((xTarget >= xNextIndex) == (step > 0.0));
    // Step is too big, snap on the next interesting value
    m_selectedIndex = (step > 0.0) ? nextIndex : index;
    moveCursorToSelectedIndex();
    return true;
  }

  // Apply step
  x += step;

  // Update the newly selected index
  m_selectedIndex = index;

  // Simplify cursor's position
  if (std::fabs(x) <= std::fabs(step / 2.0)) {
    // At half a step of 0.0
    x = 0.0;
  } else {
    /* Using pixelWidth(), simplify the cursor's position value while staying at
     * the same pixel. */
    assert(std::fabs(step / 2.0) >= m_curveView.pixelWidth());
    double magnitude = std::pow(10.0, Poincare::IEEE754<double>::exponentBase10(m_curveView.pixelWidth()) - 1.0);
    x = magnitude * std::round(x / magnitude);
  }

  // Compute start and end of the segment on which the cursor is
  assert(x > xIndex && x < xNextIndex && nextIndex == index + 1);
  double yIndex = resultAtIndex(m_selectedSeries, index);
  double yNextIndex = resultAtIndex(m_selectedSeries, nextIndex);

  // Compute the cursor's position
  double y = yIndex + (yNextIndex - yIndex) * ((x - xIndex) / (xNextIndex - xIndex));
  m_roundCursorView.setColor(Shared::DoublePairStore::colorOfSeriesAtIndex(m_selectedSeries));
  m_cursor.moveTo(x, x, y);
  m_curveView.reload();
  return true;
}

void FrequencyController::computeYBounds(float * yMin, float *yMax) const {
  // Frequency curve is always bounded between 0 and 100
  *yMin = 0.0f;
  *yMax = 100.0f;
}

}  // namespace Statistics
