#include <apps/global_preferences.h>
#include <apps/shared/global_store.h>
#include <poincare/test/helper.h>
#include <quiz.h>

#include <cmath>

#include "helper.h"

using namespace Poincare;
using namespace Shared;

namespace Shared {

// Helper class friend of ContinuousFunctionCache to access its private methods
class CacheTestHelper {
 public:
  static bool HasValueInCache(ContinuousFunction* function,
                              ContinuousFunctionCache* cache, float t) {
    return cache->indexForParameter(function, t, 0) >= 0;
  }
};

}  // namespace Shared

namespace Graph {

void assert_float_equals(
    float a, float b,
    float tolerance = 1.f / static_cast<float>(Ion::Display::Height)) {
  /* The default value for the tolerance is chosen so that the error introduced
   * by caching would not typically be visible on screen. */
  assert_roughly_equal(a, b, tolerance, true);
}

void assert_check_cartesian_cache_against_function(
    ContinuousFunction* function, ContinuousFunctionCache* cache, float tMin) {
  assert(cache);
  QUIZ_ASSERT(function->cache() == cache);
  QUIZ_ASSERT(cache->step() != 0);
  /* We set the cache to nullptr to force the evaluation (otherwise we would be
   * comparing the cache against itself). */
  function->tidyCache();

  float t;
  for (int i = 0; i < Ion::Display::Width; i++) {
    t = tMin + i * cache->step();
    if (!CacheTestHelper::HasValueInCache(function, cache, t)) {
      /* TODO: This test actually encounters many values not in cache, it should
       * be reworked. */
      continue;
    }
    Coordinate2D<float> cacheValues = cache->valueForParameter(function, t, 0);
    Coordinate2D<float> functionValues = function->evaluateXYAtParameter(t);
    assert_float_equals(t, cacheValues.x());
    assert_float_equals(t, functionValues.x());
    assert_float_equals(cacheValues.y(), functionValues.y());
  }
  /* We set back the cache, so that it will not be invalidated in
   * PrepareForCaching later. */
  function->setCache(cache);
}

void assert_cartesian_cache_stays_valid_while_panning(
    ContinuousFunction* function, InteractiveCurveViewRange* range,
    CurveViewCursor* cursor, float step) {
  ContinuousFunctionCache* cache =
      GlobalContextAccessor::ContinuousFunctionContext().cacheAtIndex(0);
  assert(cache);

  float tMin, tStep;
  constexpr float margin = 0.04f;
  constexpr int numberOfMoves = 30;
  for (int i = 0; i < numberOfMoves; i++) {
    cursor->moveTo(cursor->t() + step, cursor->x() + step,
                   function->evaluateXYAtParameter(cursor->x() + step).y());
    range->panToMakePointVisible(
        cursor->x(), cursor->y(), margin, margin, margin, margin,
        (range->xMax() - range->xMin()) / (Ion::Display::Width - 1));
    tMin = range->xMin();
    tStep = (range->xMax() - range->xMin()) / (Ion::Display::Width - 1);
    ContinuousFunctionCache::PrepareForCaching(function, cache, tMin, tStep);
    assert_check_cartesian_cache_against_function(function, cache, tMin);
  }
}

void assert_check_polar_cache_against_function(
    ContinuousFunction* function, InteractiveCurveViewRange* range) {
  ContinuousFunctionCache* cache =
      GlobalContextAccessor::ContinuousFunctionContext().cacheAtIndex(0);
  assert(cache);

  float tMin = range->xMin();
  float tMax = range->xMax();

  float tStep, tCacheStep;
  ContinuousFunctionCache::ComputeNonCartesianSteps(&tStep, &tCacheStep, tMax,
                                                    tMin);

  ContinuousFunctionCache::PrepareForCaching(function, cache, tMin, tCacheStep);
  assert(cache);
  QUIZ_ASSERT(function->cache() == cache);
  QUIZ_ASSERT(cache->step() != 0);
  // Fill the cache
  float t;
  for (int i = 0; i < Ion::Display::Width / 2; i++) {
    t = tMin + i * cache->step();
    function->evaluateXYAtParameter(t);
  }

  function->tidyCache();
  for (int i = 0; i < Ion::Display::Width / 2; i++) {
    t = tMin + i * cache->step();
    /* Otherwise, cache->valueForParameter would fallback on
     * function->evaluateXYAtParameter. */
    QUIZ_ASSERT(CacheTestHelper::HasValueInCache(function, cache, t));
    Coordinate2D<float> cacheValues = cache->valueForParameter(function, t, 0);
    Coordinate2D<float> functionValues = function->evaluateXYAtParameter(t);
    assert_float_equals(cacheValues.x(), functionValues.x());
    assert_float_equals(cacheValues.y(), functionValues.y());
  }
}

void assert_cache_stays_valid(const char* definition, float rangeXMin = -5,
                              float rangeXMax = 5) {
  CachesContainer cachesContainer;
  GlobalContextAccessor::ContinuousFunctionStore().setCachesContainer(
      &cachesContainer);
  InteractiveCurveViewRange graphRange;
  graphRange.setXRange(rangeXMin, rangeXMax);
  graphRange.setYRange(-3.f, 3.f);

  CurveViewCursor cursor;
  ContinuousFunction* function = AddFunction(definition);
  Coordinate2D<float> origin = function->evaluateXYAtParameter(0.f, 0);
  cursor.moveTo(0.f, origin.x(), origin.y());

  if (function->properties().isCartesian()) {
    assert_cartesian_cache_stays_valid_while_panning(function, &graphRange,
                                                     &cursor, 2.f);
    assert_cartesian_cache_stays_valid_while_panning(function, &graphRange,
                                                     &cursor, -0.4f);
  } else {
    assert(function->properties().isPolar());
    assert_check_polar_cache_against_function(function, &graphRange);
  }
  GlobalContextAccessor::ContinuousFunctionStore().removeAll();
}

QUIZ_CASE(graph_caching) {
  AngleUnit previousAngleUnit =
      GlobalPreferences::SharedGlobalPreferences()->angleUnit();
  GlobalPreferences::SharedGlobalPreferences()->setAngleUnit(AngleUnit::Degree);
  assert_cache_stays_valid("f(x)=x");
  assert_cache_stays_valid("f(x)=x^2");
  assert_cache_stays_valid("f(x)=sin(x)");
  assert_cache_stays_valid("f(x)=sin(x)", -1e6f, 2e8f);
  assert_cache_stays_valid("f(x)=sin(x^2)");
  assert_cache_stays_valid("f(x)=1/x");
  assert_cache_stays_valid("f(x)=-e^x");

  assert_cache_stays_valid("r=1", 0.f, 360.f);
  assert_cache_stays_valid("r=θ", 0.f, 360.f);
  assert_cache_stays_valid("r=sin(θ)", 0.f, 360.f);
  assert_cache_stays_valid("r=cos(5θ)", 0.f, 360.f);
  assert_cache_stays_valid("r=cos(5θ)", -1e8f, 1e8f);

  GlobalPreferences::SharedGlobalPreferences()->setAngleUnit(previousAngleUnit);
}

}  // namespace Graph
