#include "continuous_function_cache.h"
#include "continuous_function.h"
#include <limits.h>

namespace Shared {

constexpr int ContinuousFunctionCache::k_sizeOfCache;
constexpr float ContinuousFunctionCache::k_cacheHitTolerance;
constexpr int ContinuousFunctionCache::k_numberOfAvailableCaches;

// public
void ContinuousFunctionCache::PrepareForCaching(void * fun, ContinuousFunctionCache * cache, float tMin, float tStep) {
  if (!cache) {
    /* ContinuousFunctionStore::cacheAtIndex has returned a nullptr : the index
     * of the function we are trying to draw is greater than the number of
     * available caches, so we do nothing.*/
    return;
  }

  ContinuousFunction * function = static_cast<ContinuousFunction *>(fun);
  if (function->cache() != cache) {
    cache->clear();
    function->setCache(cache);
  }

  if (function->plotType() == ContinuousFunction::PlotType::Cartesian && tStep != 0) {
    function->cache()->pan(function, tMin);
  }
  function->cache()->setRange(function, tMin, tStep);
}

void ContinuousFunctionCache::clear() {
  m_startOfCache = 0;
  m_tStep = 0;
  invalidateBetween(0, k_sizeOfCache);
}

Poincare::Coordinate2D<float> ContinuousFunctionCache::valueForParameter(const ContinuousFunction * function, Poincare::Context * context, float t) {
  int resIndex = indexForParameter(function, t);
  if (resIndex < 0) {
    return function->privateEvaluateXYAtParameter(t, context);
  }
  return valuesAtIndex(function, context, t, resIndex);
}

// private
float ContinuousFunctionCache::StepFactor(ContinuousFunction * function) {
  /* When drawing a parametric or polar curve, the range is first divided by
   * ~10,9, creating 11 intervals which are filled by dichotomy.
   * We memoize 16 values for each of the 10 big intervals. */
  return (function->plotType() == ContinuousFunction::PlotType::Cartesian) ? 1.f : 16.f;
}

void ContinuousFunctionCache::invalidateBetween(int iInf, int iSup) {
  for (int i = iInf; i < iSup; i++) {
    m_cache[i] = NAN;
  }
}

void ContinuousFunctionCache::setRange(ContinuousFunction * function, float tMin, float tStep) {
  m_tMin = tMin;
  m_tStep = tStep / StepFactor(function);
}

int ContinuousFunctionCache::indexForParameter(const ContinuousFunction * function, float t) const {
  float delta = (t - m_tMin) / m_tStep;
  if (delta < 0 || delta > INT_MAX) {
    return -1;
  }
  int res = std::round(delta);
  assert(res >= 0);
  if ((res >= k_sizeOfCache && function->plotType() == ContinuousFunction::PlotType::Cartesian)
   || (res >= k_sizeOfCache / 2 && function->plotType() != ContinuousFunction::PlotType::Cartesian)
   || std::abs(res - delta) > k_cacheHitTolerance) {
    return -1;
  }
  assert(function->plotType() == ContinuousFunction::PlotType::Cartesian || m_startOfCache == 0);
  return (res + m_startOfCache) % k_sizeOfCache;
}

Poincare::Coordinate2D<float> ContinuousFunctionCache::valuesAtIndex(const ContinuousFunction * function, Poincare::Context * context, float t, int i) {
  if (function->plotType() == ContinuousFunction::PlotType::Cartesian) {
    if (std::isnan(m_cache[i])) {
      m_cache[i] = function->privateEvaluateXYAtParameter(t, context).x2();
    }
    return Poincare::Coordinate2D<float>(t, m_cache[i]);
  }
  if (std::isnan(m_cache[2 * i]) || std::isnan(m_cache[2 * i + 1])) {
    Poincare::Coordinate2D<float> res = function->privateEvaluateXYAtParameter(t, context);
    m_cache[2 * i] = res.x1();
    m_cache[2 * i + 1] = res.x2();
  }
  return Poincare::Coordinate2D<float>(m_cache[2 * i], m_cache[2 * i + 1]);
}

void ContinuousFunctionCache::pan(ContinuousFunction * function, float newTMin) {
  assert(function->plotType() == ContinuousFunction::PlotType::Cartesian);
  if (newTMin == m_tMin) {
    return;
  }

  float dT = (newTMin - m_tMin) / m_tStep;
  m_tMin = newTMin;
  if (std::abs(dT) > INT_MAX) {
    clear();
    return;
  }
  int dI = std::round(dT);
  if (dI >= k_sizeOfCache || dI <= -k_sizeOfCache || std::abs(dT - dI) > k_cacheHitTolerance) {
    clear();
    return;
  }

  int oldStart = m_startOfCache;
  m_startOfCache = (m_startOfCache + dI) % k_sizeOfCache;
  if (m_startOfCache < 0) {
    m_startOfCache += k_sizeOfCache;
  }
  if (dI > 0) {
    if (m_startOfCache > oldStart) {
      invalidateBetween(oldStart, m_startOfCache);
    } else {
      invalidateBetween(oldStart, k_sizeOfCache);
      invalidateBetween(0, m_startOfCache);
    }
  } else {
    if (m_startOfCache > oldStart) {
      invalidateBetween(m_startOfCache, k_sizeOfCache);
      invalidateBetween(0, oldStart);
    } else {
      invalidateBetween(m_startOfCache, oldStart);
    }
  }
}

}
