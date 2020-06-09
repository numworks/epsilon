#ifndef SHARED_CONTINUOUS_FUNCTION_CACHE_H
#define SHARED_CONTINUOUS_FUNCTION_CACHE_H

#include <ion/display.h>
#include <poincare/context.h>
#include <poincare/coordinate_2D.h>

namespace Shared {

class ContinuousFunction;

class ContinuousFunctionCache {
public:
  /* The size of the cache is chosen to optimize the display of cartesian
   * function */
  static constexpr int k_numberOfAvailableCaches = 2;

  static void PrepareCache(void * f, void * ctx, void * cch, float tMin, float tStep);

  float step() const { return m_tStep; }
  bool filled() const { return m_filled; }
  void clear();
  Poincare::Coordinate2D<float> valueForParameter(const ContinuousFunction * function, float t) const;
private:
  /* The size of the cache is chosen to optimize the display of cartesian
   * function */
  static constexpr int k_sizeOfCache = Ion::Display::Width;
  static constexpr float k_cacheHitTolerance = 1e-3;

  static float StepFactor(ContinuousFunction * function);

  void setRange(ContinuousFunction * function, float tMin, float tStep);
  void memoize(ContinuousFunction * function, Poincare::Context * context);
  void memoizeYForX(ContinuousFunction * function, Poincare::Context * context);
  void memoizeYForXBetweenIndices(ContinuousFunction * function, Poincare::Context * context, int iInf, int iSup);
  void memoizeXYForT(ContinuousFunction * function, Poincare::Context * context);
  float parameterForIndex(int i) const;
  int indexForParameter(const ContinuousFunction * function, float t) const;
  void pan(ContinuousFunction * function, Poincare::Context * context, float newTMin);

  float m_tMin, m_tStep;
  float m_cache[k_sizeOfCache];
  /* m_startOfCache is used to implement a circular buffer for easy panning
   * with cartesian functions. When dealing with parametric or polar functions,
   * m_startOfCache should be zero.*/
  int m_startOfCache;
  bool m_filled;
};

}

#endif
