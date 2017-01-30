#include "law.h"
#include <math.h>
#include <float.h>

namespace Probability {

float Law::xGridUnit() {
  return computeGridUnit(Axis::X, xMin(), xMax());
}

float Law::cumulativeDistributiveFunctionAtAbscissa(float x) const {
  if (!isContinuous()) {
    int end = floorf(x);
    float result = 0.0f;
    for (int k = 0; k <=end; k++) {
      result += evaluateAtAbscissa(k);
    }
    return result;
  }
  return 0.0f;
}

float Law::rightIntegralFromAbscissa(float x) const {
  return 1.0f - cumulativeDistributiveFunctionAtAbscissa(x);
}

float Law::finiteIntegralBetweenAbscissas(float a, float b) const {
  if (b < a) {
    return 0.0f;
  }
  if (isContinuous()) {
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
  if (isContinuous()) {
    return 0.0f;
  }
  if (*probability <= 0.0f) {
    return 0.0f;
  }
  float p = 0.0f;
  int k = 0;
  while (p < *probability && k < k_maxNumberOfOperations) {
    p += evaluateAtAbscissa(k++);
  }
  if (k == k_maxNumberOfOperations) {
    *probability = 1.0f;
    return INFINITY;
  }
  *probability = p;
  if (isnan(*probability)) {
    return NAN;
  }
  return k-1;
}

float Law::rightIntegralInverseForProbability(float * probability) {
  if (isContinuous()) {
    float f = 1.0f - *probability;
    return cumulativeDistributiveInverseForProbability(&f);
  }
  if (*probability >= 1.0f) {
    return 0.0f;
  }
  if (*probability <= 0.0f) {
    return INFINITY;
  }
  float p = 0.0f;
  int k = 0;
  while (p < 1.0f - *probability && k < k_maxNumberOfOperations) {
    p += evaluateAtAbscissa(k++);
  }
  if (k == k_maxNumberOfOperations) {
    *probability = 1.0f;
    return INFINITY;
  }
  *probability = 1.0f - (p - evaluateAtAbscissa(k-1));
  if (isnan(*probability)) {
    return NAN;
  }
  return k-1;
}

}
