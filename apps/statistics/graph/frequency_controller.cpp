#include "frequency_controller.h"
#include <assert.h>

namespace Statistics {

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

}  // namespace Statistics
