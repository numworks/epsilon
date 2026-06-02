#include <apps/global_preferences.h>
#include <apps/shared/continuous_function.h>
#include <apps/shared/continuous_function_cache.h>
#include <apps/shared/curve_view_cursor.h>
#include <apps/shared/global_store.h>
#include <apps/shared/interactive_curve_view_range.h>
#include <poincare/test/helper.h>
#include <poincare/test/helpers/float.h>
#include <quiz.h>

#include <cmath>

#include "helper.h"

using namespace Poincare;
using namespace Shared;

namespace Shared {

// Helper class friend of ContinuousFunctionCache to access its private methods
class CacheTestHelper {
 public:
  static bool HasValueInCache(Ion::Storage::Record record,
                              const ContinuousFunctionCache* cache, float t) {
    ContinuousFunctionProperties properties =
        GlobalContextAccessor::ContinuousFunctionStore()
            .modelForRecord(record)
            ->properties();
    return cache->indexForParameter(properties, t, 0) >= 0;
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
    Ion::Storage::Record record, ContinuousFunctionCache* cache, float tMin) {
  OMG::ExpiringPointer<ContinuousFunction> function =
      GlobalContextAccessor::ContinuousFunctionStore().modelForRecord(record);
  assert(cache);
  QUIZ_ASSERT(function->cache() == cache);
  QUIZ_ASSERT(cache->step() != 0);
  /* We set the cache to nullptr to force the evaluation (otherwise we would be
   * comparing the cache against itself). */
  function->tidyCache();

  float t;
  for (int i = 0; i < Ion::Display::Width; i++) {
    t = tMin + i * cache->step();
    if (!CacheTestHelper::HasValueInCache(record, cache, t)) {
      /* TODO: This test actually encounters many values not in cache, it should
       * be reworked. */
      continue;
    }
    Coordinate2D<float> cacheValues =
        cache->valueForParameter(function.pointer(), t, 0);
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
    Ion::Storage::Record record, InteractiveCurveViewRange* range,
    CurveViewCursor* cursor, float step) {
  ContinuousFunctionCache* cache =
      GlobalContextAccessor::ContinuousFunctionContext().cacheAtIndex(0);
  assert(cache);

  float tMin, tStep;
  constexpr float margin = 0.04f;
  constexpr int numberOfMoves = 30;
  for (int i = 0; i < numberOfMoves; i++) {
    /* Re-fetch each iteration: assert_check_cartesian_cache_against_function
     * calls modelForRecord, which would invalidate an outer ExpiringPointer. */
    OMG::ExpiringPointer<ContinuousFunction> function =
        GlobalContextAccessor::ContinuousFunctionStore().modelForRecord(record);
    cursor->moveTo(cursor->t() + step, cursor->x() + step,
                   function->evaluateXYAtParameter(cursor->x() + step).y());
    range->panToMakePointVisible(
        cursor->x(), cursor->y(), margin, margin, margin, margin,
        (range->xMax() - range->xMin()) / (Ion::Display::Width - 1));
    tMin = range->xMin();
    tStep = (range->xMax() - range->xMin()) / (Ion::Display::Width - 1);
    ContinuousFunctionCache::PrepareForCaching(record, cache, tMin, tStep);
    assert_check_cartesian_cache_against_function(record, cache, tMin);
  }
}

void assert_check_polar_cache_against_function(
    Ion::Storage::Record record, InteractiveCurveViewRange* range) {
  OMG::ExpiringPointer<ContinuousFunction> function =
      GlobalContextAccessor::ContinuousFunctionStore().modelForRecord(record);
  ContinuousFunctionCache* cache =
      GlobalContextAccessor::ContinuousFunctionContext().cacheAtIndex(0);
  assert(cache);

  float tMin = range->xMin();
  float tMax = range->xMax();

  float tStep, tCacheStep;
  ContinuousFunctionCache::ComputeNonCartesianSteps(&tStep, &tCacheStep, tMax,
                                                    tMin);

  ContinuousFunctionCache::PrepareForCaching(record, cache, tMin, tCacheStep);
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
    QUIZ_ASSERT(CacheTestHelper::HasValueInCache(record, cache, t));
    Coordinate2D<float> cacheValues =
        cache->valueForParameter(function.pointer(), t, 0);
    Coordinate2D<float> functionValues = function->evaluateXYAtParameter(t);
    assert_float_equals(cacheValues.x(), functionValues.x());
    assert_float_equals(cacheValues.y(), functionValues.y());
  }
}

void assert_cache_stays_valid(const char* definition, float rangeXMin = -5,
                              float rangeXMax = 5) {
  ContinuousFunctionStore& store =
      GlobalContextAccessor::ContinuousFunctionStore();
  CachesContainer cachesContainer;
  store.setCachesContainer(&cachesContainer);
  InteractiveCurveViewRange graphRange;
  graphRange.setXRange(rangeXMin, rangeXMax);
  graphRange.setYRange(-3.f, 3.f);

  CurveViewCursor cursor;
  Ion::Storage::Record record = AddFunction(definition);
  OMG::ExpiringPointer<ContinuousFunction> function =
      store.modelForRecord(record);
  Coordinate2D<float> origin = function->evaluateXYAtParameter(0.f, 0);
  cursor.moveTo(0.f, origin.x(), origin.y());

  if (function->properties().isCartesian()) {
    assert_cartesian_cache_stays_valid_while_panning(record, &graphRange,
                                                     &cursor, 2.f);
    assert_cartesian_cache_stays_valid_while_panning(record, &graphRange,
                                                     &cursor, -0.4f);
  } else {
    assert(function->properties().isPolar());
    assert_check_polar_cache_against_function(record, &graphRange);
  }
  store.removeAll();
}

QUIZ_CASE(graph_caching) {
  AngleUnit previousAngleUnit = SharedGlobalPrefs().angleUnit();
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
