#ifndef SHARED_CONTINUOUS_FUNCTION_CACHE_H
#define SHARED_CONTINUOUS_FUNCTION_CACHE_H

#include <float.h>
#include <ion/display.h>
#include <poincare/context.h>
#include <poincare/coordinate_2D.h>

namespace Shared {

class ContinuousFunction;

class ContinuousFunctionCache {
 public:
  constexpr static int k_numberOfAvailableCaches = 2;

  static void PrepareForCaching(void* fun, ContinuousFunctionCache* cache,
                                float tMin, float tStep);

  ContinuousFunctionCache() : m_tMin(0) { clear(); }

  float step() const { return m_tStep; }
  void clear();
  Poincare::Coordinate2D<float> valueForParameter(
      const ContinuousFunction* function, Poincare::Context* context, float t,
      int curveIndex);
  // Sets step parameters for non-cartesian curves
  static void ComputeNonCartesianSteps(float* tStep, float* tCacheStep,
                                       float tMax, float tMin);

 private:
  /* The size of the cache is chosen to optimize the display of cartesian
   * functions */
  constexpr static int k_sizeOfCache = Ion::Display::Width;
  /* We need a certain amount of tolerance since we try to evaluate the
   * equality of floats. But the value has to be chosen carefully. Too high of
   * a tolerance causes false positives, which lead to errors in curves
   * (ex : 1/x with a vertical line at 0). Too low of a tolerance causes false
   * negatives, which slows down the drawing.
   *
   * The value 128*FLT_EPSILON has been found to be the lowest for which all
   * indices verify indexForParameter(tMin + index * tStep) = index. */
  constexpr static float k_cacheHitTolerance = 128.0f * FLT_EPSILON;
  /* The step is a fraction of tmax-tmin. We will evaluate the function at
   * every step and if the consecutive dots are close enough, we won't
   * evaluate any more dot within the step. We pick a very strange fraction
   * denominator to avoid evaluating a periodic function periodically. For
   * example, if tstep was (tmax - tmin)/10, the polar function r(θ) = sin(5θ)
   * defined on 0..2π would be evaluated on r(0) = 0, r(π/5) = 0, r(2*π/5) = 0
   * which would lead to no curve at all. With 80.0938275501223, the
   * problematic functions are the functions whose period is proportioned to
   * 80.0938275501223 which are hopefully rare enough.
   * TODO: The drawCurve algorithm should use the derivative function to know
   * how fast the function moves... */
  constexpr static float k_graphStepDenominator = 80.0938275501223f;

  void invalidateBetween(int iInf, int iSup);
  void setRange(float tMin, float tStep);
  int indexForParameter(const ContinuousFunction* function, float t,
                        int curveIndex) const;
  Poincare::Coordinate2D<float> valuesAtIndex(
      const ContinuousFunction* function, Poincare::Context* context, float t,
      int i, int curveIndex);
  void pan(ContinuousFunction* function, float newTMin);

  float m_tMin, m_tStep;
  float m_cache[k_sizeOfCache];
  /* m_startOfCache is used to implement a circular buffer for easy panning
   * with cartesian functions. When dealing with parametric or polar functions,
   * m_startOfCache should be zero.*/
  int m_startOfCache;
};

}  // namespace Shared

#endif
