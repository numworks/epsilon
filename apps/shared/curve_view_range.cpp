#include "curve_view_range.h"
#include <cmath>
#include <ion.h>
#include <poincare/ieee754.h>
#include <assert.h>
#include <stddef.h>
#include <float.h>

namespace Shared {

uint32_t CurveViewRange::rangeChecksum() {
  float data[7] = {xMin(), xMax(), yMin(), yMax(), xGridUnit(), yGridUnit(), offscreenYAxis()};
  size_t dataLengthInBytes = sizeof(data);
  // Assert that dataLengthInBytes is a multiple of 4
  assert((dataLengthInBytes & 0x3) == 0);
  return Ion::crc32Word((uint32_t *)data, dataLengthInBytes/sizeof(uint32_t));
}

float CurveViewRange::computeGridUnit(float minNumberOfUnits, float maxNumberOfUnits, float range) const {
  int a = 0;
  int b = 0;
  constexpr int unitsCount = 3;
  float units[unitsCount] = {k_smallGridUnitMantissa, k_mediumGridUnitMantissa, k_largeGridUnitMantissa};
  // An infinite range would return an exponentBase10 of 39 and a null gridUnit
  assert(range > 0.0f && std::isfinite(range));
  for (int k = 0; k < unitsCount; k++) {
    float currentA = units[k];
    int b1 = Poincare::IEEE754<float>::exponentBase10(range/(currentA*maxNumberOfUnits));
    int b2 = Poincare::IEEE754<float>::exponentBase10(range/(currentA*minNumberOfUnits));
    if (b1 != b2) {
      b = b2;
      a = currentA;
    }
  }
  return a*std::pow(10.0f,b);

  // clang-format off
  /* Proof of the algorithm:
   *
   * We want to find gridUnit = a*10^b, with a in {1; 2; 5} and b an integer
   * We want: minNumberOfUnits <= range/gridUnit < maxNumberOfUnits
   *
   * A solution thus needs to verify:
   *
   *        minNumberOfUnits/range <= 1/(a*10^b) < maxNumberOfUnits/range
   * =>     range/minNumberOfUnits >= a*10^b > range/maxNumberOfUnits
   * =>     range/(a*minNumberOfUnits) >= 10^b > range/(a*maxNumberOfUnits)
   * =>     log10(range/(a*minNumberOfUnits)) >= b > log10(range/(a*maxNumberOfUnits))
   * => (1) log10(range/(a*maxNumberOfUnits)) < b <= log10(range/(a*minNumberOfUnits))
   * And, because b must be an integer,
   * =>     floor(log10(range/(a*maxNumberOfUnits))) != floor(log10(range/(a*minNumberOfUnits)))
   * The solution is then b = floor(log10(range/(a*minNumberOfUnits)))
   *
   * Is there always at least one solution ?
   *
   * (1) also gives:
   * E1 = log10(range)-log10(a)-log10(maxNumberOfUnits) < b <= log10(range)-log10(a)-log10(maxNumberOfUnits) = E2
   *
   * Let's compute E2-E1:
   * E2-E1 = log10(maxNumberOfUnits) - log10(minNumberOfUnits)
   * For minNumberOfUnits=7 and maxNumberOfUnits=18, E2-E1 = 0.41...
   * For minNumberOfUnits=5 and maxNumberOfUnits=13, E2-E1 = 0.41...
   *
   * Let's compute the union of the [E1;E2] for a in {1; 2; 5}:
   * [E1;E2 for a=1] U [E1;E2 for a=2] U [E1;E2 for a=5]
   *  = [e1;e2] U [e1-log10(2); e2-log10(2)] U [e1-log10(5); e2-log10(5)]
   *  = [e1;e2] U [e1-0.3; e2-0.3] U [e1-0.7; e2-0.7]
   *  = [e1-0.7; e2-0.7] U [e1-0.3; e2-0.3] U [e1;e2]
   *  = [e1-0.7; e2]  because e2-0.7 > e1-0.3 as e2-e1 > 0.7-0.3 and e2-e1 = E2-E1 = 0.41...
   *                  and  e2-0.3 > e1 as e2-e1 > 0.3
   *
   * The union of the [E1;E2] for a in {1; 2; 5} is an interval of size
   * e2-e1+0.7 = 1.1 > 1.
   * We will thus always have at least one a in {1; 2; 5} for which E1 and E2
   * are on each side of an integer.
   *
   * Let's make a drawing.
   *
   * n        n+1       n+2       n+3       n+4       n+5
   * |.........|.........|.........|.........|.........|...
   *  E1^---^E2
   *    0.41
   *   *
   * To have a solution, we need E1 and E2 to be on each side of an integer.
   *
   * --------------------------------------------------------------------------------------------
   * --------------------------------------------------------------------------------------------
   *
   * If e1 - floor(e1) > 1-(e2-e1) = 0.58..., a=1 is a solution
   *
   *                  n       n+0.2     n+0.4     n+0.6     n+0.8       n+1
   *        .........||....|....|....|....|....|....|....|....|....|....||...  a=1
   *                                                e1^--------------------^e2
   *
   * --------------------------------------------------------------------------------------------
   * --------------------------------------------------------------------------------------------
   *
   * If log10(5)-(e2-e1) = 0.29... < e1 - floor(e1) < log10(5) = 0.69..., a=5 is a solution
   *
   *                  n       n+0.2     n+0.4     n+0.6     n+0.8       n+1
   *        .........||....|....|....|....|....|....|....|....|....|....||...
   *                                    e1^--------------------^e2
   *
   *                  n       n+0.2     n+0.4     n+0.6     n+0.8       n+1
   *        .........||....|....|....|....|....|....|....|....|....|....||...  a=5
   * E1^--------------------^E2                                                <- shift by log10(5) = 0.7
   *
   * --------------------------------------------------------------------------------------------
   * --------------------------------------------------------------------------------------------
   *
   * If e1 - floor(e1) < log10(2) = 0.3..., a=2 is a solution
   *                  n       n+0.2     n+0.4     n+0.6     n+0.8       n+1
   *        .........||....|....|....|....|....|....|....|....|....|....||...
   *                          e1^--------------------^e2
   *
   *                  n       n+0.2     n+0.4     n+0.6     n+0.8       n+1
   *        .........||....|....|....|....|....|....|....|....|....|....||...  a=2
   *           E1^--------------------^E2                                      <- shift by log10(2) = 0.3
   *
   * */
  // clang-format on
}

}
