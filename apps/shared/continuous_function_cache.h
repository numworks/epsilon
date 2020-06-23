#ifndef SHARED_CONTINUOUS_FUNCTION_CACHE_H
#define SHARED_CONTINUOUS_FUNCTION_CACHE_H

#include "../graph/graph/graph_view.h"
#include <ion/display.h>
#include <poincare/context.h>
#include <poincare/coordinate_2D.h>

namespace Shared {

class ContinuousFunction;

class ContinuousFunctionCache {
private:
  /* The size of the cache is chosen to optimize the display of cartesian
   * functions */
   static constexpr int k_sizeOfCache = Ion::Display::Width;
public:
  static constexpr int k_numberOfAvailableCaches = 2;
  static constexpr int k_parametricStepFactor = k_sizeOfCache / int(Graph::GraphView::k_graphStepDenominator);

  static void PrepareForCaching(void * fun, ContinuousFunctionCache * cache, float tMin, float tStep);

  ContinuousFunctionCache() { clear(); }

  float step() const { return m_tStep; }
  void clear();
  Poincare::Coordinate2D<float> valueForParameter(const ContinuousFunction * function, Poincare::Context * context, float t);
private:
  /* We need a certain amount of tolerance since we try to evaluate the
   * equality of floats. But the value has to be chosen carefully. Too high of
   * a tolerance causes false positives, which lead to errors in curves
   * (ex : 1/x with a vertical line at 0). Too low of a tolerance causes false
   * negatives, which slows down the drawing.
   *
   * The value 128*FLT_EPSILON has been found to be the lowest for which all
   * indices verify indexForParameter(tMin + index * tStep) = index. */
  static constexpr float k_cacheHitTolerance = 128 * FLT_EPSILON;

  void invalidateBetween(int iInf, int iSup);
  void setRange(ContinuousFunction * function, float tMin, float tStep);
  int indexForParameter(const ContinuousFunction * function, float t) const;
  Poincare::Coordinate2D<float> valuesAtIndex(const ContinuousFunction * function, Poincare::Context * context, float t, int i);
  void pan(ContinuousFunction * function, float newTMin);

  float m_tMin, m_tStep;
  float m_cache[k_sizeOfCache];
  /* m_startOfCache is used to implement a circular buffer for easy panning
   * with cartesian functions. When dealing with parametric or polar functions,
   * m_startOfCache should be zero.*/
  int m_startOfCache;
};

}

#endif
