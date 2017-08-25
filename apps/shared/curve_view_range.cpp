#include "curve_view_range.h"
#include <cmath>
#include <ion.h>
#include <assert.h>
#include <stddef.h>
#include <float.h>

namespace Shared {

uint32_t CurveViewRange::rangeChecksum() {
  float data[4] = {xMin(), xMax(), yMin(), yMax()};
  size_t dataLengthInBytes = 4*sizeof(float);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32((uint32_t *)data, dataLengthInBytes/sizeof(uint32_t));
}

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
  float units[3] = {k_smallGridUnitMantissa, k_mediumGridUnitMantissa, k_largeGridUnitMantissa};
  for (int k = 0; k < 3; k++) {
    float unit = units[k];
    if (std::floor(std::log10(d/(unit*maxNumberOfUnits))) != std::floor(std::log10(d/(unit*minNumberOfUnits)))) {
      b = std::floor(std::log10(d/(unit*minNumberOfUnits)));
      a = unit;
    }
  }
  return a*std::pow(10.0f,b);
}

}
