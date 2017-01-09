#include "curve_view_range.h"
#include <math.h>
#include <float.h>

float CurveViewRange::yGridUnit() {
  return 0.0f;
}

float CurveViewRange::computeGridUnit(Axis axis, float min, float max) {
  int a = 0;
  int b = 0;
  float d = max - min;
  float maxNumberOfUnits = k_maxNumberOfXGridUnits;
  float minNumberOfUnits = k_minNumberOfXGridUnits;
  if (axis == Axis::Y) {
    maxNumberOfUnits = k_maxNumberOfYGridUnits;
    minNumberOfUnits = k_minNumberOfYGridUnits;
  }
  float units[3] = {k_oneUnit, k_twoUnit, k_fiveUnit};
  for (int k = 0; k < 3; k++) {
    float unit = units[k];
    if (floorf(log10f(d/(unit*maxNumberOfUnits))) != floorf(log10f(d/(unit*minNumberOfUnits)))) {
      b = floorf(log10f(d/(unit*minNumberOfUnits)));
      a = unit;
    }
  }
  return a*powf(10,b);
}
