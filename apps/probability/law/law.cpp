#include "law.h"
#include <math.h>
#include <float.h>

namespace Probability {

float Law::gridUnit() {
  int a = 0;
  int b = 0;
  float d = xMax() - xMin();
  float units[3] = {k_oneUnit, k_twoUnit, k_fiveUnit};
  for (int k = 0; k < 3; k++) {
    float unit = units[k];
    if (floorf(log10f(d/(unit*k_maxNumberOfXGridUnits))) != floorf(log10f(d/(unit*k_minNumberOfXGridUnits)))) {
      b = floorf(log10f(d/(unit*k_minNumberOfXGridUnits)));
      a = unit;
    }
  }
  return a*powf(10,b);
}

float Law::cumulativeDistributiveFunctionAtAbscissa(float x) const {
  if (!isContinuous()) {
    int end = x;
    float result = 0.0f;
    for (int k = 0; k <=end; k++) {
      result += evaluateAtAbscissa(k);
    }
    return result;
  }
  return 0.0f;
}

float Law::rightIntegralFromAbscissa(float x) const {
  if (isContinuous()) {
    return 1.0f - cumulativeDistributiveFunctionAtAbscissa(x);
  }
  int start = ceilf(x);
  return 1.0f - cumulativeDistributiveFunctionAtAbscissa(start - 1);
}

float Law::finiteIntegralBetweenAbscissas(float a, float b) const {
  if (b < a) {
    return 0.0f;
  }
  if (isContinuous())
  {
    return cumulativeDistributiveFunctionAtAbscissa(b) - cumulativeDistributiveFunctionAtAbscissa(a);
  }
  int start = ceilf(a);
  int end = floorf(b);
  float result = 0.0f;
  for (int k = start; k <=end; k++) {
    result += evaluateAtAbscissa(k);
  }
  return result;
}

float Law::cumulativeDistributiveInverseForProbability(float * probability) {
  if (*probability >= 1.0f) {
    return INFINITY;
  }
  if (!isContinuous()) {
    if (*probability <= 0.0f) {
      return 0.0f;
    }
    float p = 0.0f;
    int k = 0;
    while (p < *probability) {
      p += evaluateAtAbscissa(k++);
    }
    *probability = p;
    return k-1;
  }
  return 0.0f;
}

float Law::rightIntegralInverseForProbability(float * probability) {
  if (isContinuous()) {
    float f = 1.0f - *probability;
    return cumulativeDistributiveInverseForProbability(&f);
  }
  float p = 0.0f;
  int k = 0;
  while (p < 1.0f - *probability) {
    p += evaluateAtAbscissa(k++);
  }
  *probability = 1.0f - (p - evaluateAtAbscissa(k-1));
  return k-1;
}

}
