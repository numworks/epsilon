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
  float maxNumberOfUnits = (axis == Axis::X) ? k_maxNumberOfXGridUnits : k_maxNumberOfYGridUnits;
  float minNumberOfUnits = (axis == Axis::X) ? k_minNumberOfXGridUnits : k_minNumberOfYGridUnits;
  constexpr int unitsCount = 3;
  float units[unitsCount] = {k_smallGridUnitMantissa, k_mediumGridUnitMantissa, k_largeGridUnitMantissa};
  for (int k = 0; k < unitsCount; k++) {
    float unit = units[k];
    int b1 = std::floor(std::log10(d/(unit*maxNumberOfUnits)));
    int b2 = std::floor(std::log10(d/(unit*minNumberOfUnits)));
    if (b1 != b2) {
      b = b2;
      a = unit;
    }
  }
  return a*std::pow(10.0f,b);
}

}
