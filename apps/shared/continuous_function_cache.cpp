#include "continuous_function_cache.h"
#include "continuous_function.h"

namespace Shared {

constexpr int ContinuousFunctionCache::k_sizeOfCache;
constexpr float ContinuousFunctionCache::k_cacheHitTolerance;
constexpr int ContinuousFunctionCache::k_numberOfAvailableCaches;

// public
void ContinuousFunctionCache::PrepareCache(void * f, void * ctx, void * cch, float tMin, float tStep) {
  if (!cch) {
    return;
  }
  ContinuousFunction * function = (ContinuousFunction *)f;
  Poincare::Context * context = (Poincare::Context *)ctx;
  if (!function->cache()) {
    ContinuousFunctionCache * cache = (ContinuousFunctionCache *)cch;
    cache->clear();
    function->setCache(cache);
  }
  if (function->cache()->filled() && tStep / StepFactor(function) == function->cache()->step()) {
    if (function->plotType() == ContinuousFunction::PlotType::Cartesian) {
      function->cache()->pan(function, context, tMin);
    }
    return;
  }
  function->cache()->setRange(function, tMin, tStep);
  function->cache()->memoize(function, context);
}

void ContinuousFunctionCache::clear() {
  m_filled = false;
  m_startOfCache = 0;
}

Poincare::Coordinate2D<float> ContinuousFunctionCache::valueForParameter(const ContinuousFunction * function, float t) const {
  int iRes = indexForParameter(function, t);
  /* If t does not map to an index, iRes is -1 */
  if (iRes < 0) {
    return Poincare::Coordinate2D<float>(NAN, NAN);
  }
  if (function->plotType() == ContinuousFunction::PlotType::Cartesian) {
    return Poincare::Coordinate2D<float>(t, m_cache[iRes]);
  }
  assert(m_startOfCache == 0);
  return Poincare::Coordinate2D<float>(m_cache[2*iRes], m_cache[2*iRes+1]);
}

// private
float ContinuousFunctionCache::StepFactor(ContinuousFunction * function) {
  /* When drawing a parametric or polar curve, the range is first divided by
   * ~10,9, creating 11 intervals which are filled by dichotomy.
   * We memoize 16 values for each of the 10 big intervals. */
  return (function->plotType() == ContinuousFunction::PlotType::Cartesian) ? 1.f : 16.f;
}

void ContinuousFunctionCache::setRange(ContinuousFunction * function, float tMin, float tStep) {
  m_tMin = tMin;
  m_tStep = tStep / StepFactor(function);
}

void ContinuousFunctionCache::memoize(ContinuousFunction * function, Poincare::Context * context) {
  m_filled = true;
  m_startOfCache = 0;
  if (function->plotType() == ContinuousFunction::PlotType::Cartesian) {
    memoizeYForX(function, context);
    return;
  }
  memoizeXYForT(function, context);
}

void ContinuousFunctionCache::memoizeYForX(ContinuousFunction * function, Poincare::Context * context) {
  memoizeYForXBetweenIndices(function, context, 0, k_sizeOfCache);
}

void ContinuousFunctionCache::memoizeYForXBetweenIndices(ContinuousFunction * function, Poincare::Context * context, int iInf, int iSup) {
  assert(function->plotType() == ContinuousFunction::PlotType::Cartesian);
  for (int i = iInf; i < iSup; i++) {
    m_cache[i] = function->privateEvaluateXYAtParameter(parameterForIndex(i), context).x2();
  }
}

void ContinuousFunctionCache::memoizeXYForT(ContinuousFunction * function, Poincare::Context * context) {
  assert(function->plotType() != ContinuousFunction::PlotType::Cartesian);
  for (int i = 1; i < k_sizeOfCache; i += 2) {
    Poincare::Coordinate2D<float> res = function->privateEvaluateXYAtParameter(parameterForIndex(i/2), context);
    m_cache[i - 1] = res.x1();
    m_cache[i] = res.x2();
  }
}

float ContinuousFunctionCache::parameterForIndex(int i) const {
  if (i < m_startOfCache) {
    i += k_sizeOfCache;
  }
  return m_tMin + m_tStep * (i - m_startOfCache);
}

int ContinuousFunctionCache::indexForParameter(const ContinuousFunction * function, float t) const {
  float delta = (t - m_tMin) / m_tStep;
  if (delta < 0 || delta > INT_MAX) {
    return -1;
  }
  int res = std::round(delta);
  assert(res >= 0);
  if (res >= k_sizeOfCache || std::abs(res - delta) > k_cacheHitTolerance) {
    return -1;
  }
  assert(function->plotType() == ContinuousFunction::PlotType::Cartesian || m_startOfCache == 0);
  return (res + m_startOfCache) % k_sizeOfCache;
}

void ContinuousFunctionCache::pan(ContinuousFunction * function, Poincare::Context * context, float newTMin) {
  assert(function->plotType() == ContinuousFunction::PlotType::Cartesian);
  if (newTMin == m_tMin) {
    return;
  }

  float dT = (newTMin - m_tMin) / m_tStep;
  m_tMin = newTMin;
  if (std::abs(dT) > INT_MAX) {
    memoize(function, context);
    return;
  }
  int dI = std::round(dT);
  if (dI >= k_sizeOfCache || dI <= -k_sizeOfCache || std::abs(dT - dI) > k_cacheHitTolerance) {
    memoize(function, context);
    return;
  }

  int oldStart = m_startOfCache;
  m_startOfCache = (m_startOfCache + dI) % k_sizeOfCache;
  if (m_startOfCache < 0) {
    m_startOfCache += k_sizeOfCache;
  }
  if (dI > 0) {
    if (m_startOfCache > oldStart) {
      memoizeYForXBetweenIndices(function, context, oldStart, m_startOfCache);
    } else {
      memoizeYForXBetweenIndices(function, context, oldStart, k_sizeOfCache);
      memoizeYForXBetweenIndices(function, context, 0, m_startOfCache);
    }
  } else {
    if (m_startOfCache > oldStart) {
      memoizeYForXBetweenIndices(function, context, m_startOfCache, k_sizeOfCache);
      memoizeYForXBetweenIndices(function, context, 0, oldStart);
    } else {
      memoizeYForXBetweenIndices(function, context, m_startOfCache, oldStart);
    }
  }
}

}
