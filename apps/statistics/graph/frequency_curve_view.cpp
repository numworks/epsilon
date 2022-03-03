#include "frequency_curve_view.h"
#include <apps/shared/curve_view.h>
#include <algorithm>
#include <assert.h>
#include <string.h>

namespace Statistics {

// TODO : Always enforce 20%, 40%, 60%, 80% and 100% labels
void FrequencyCurveView::appendLabelSuffix(Axis axis, char * labelBuffer, int maxSize, int glyphLength, int maxGlyphLength) {
  if (axis == Axis::Horizontal) {
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

}
