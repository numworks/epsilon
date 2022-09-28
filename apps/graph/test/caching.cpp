#include <quiz.h>
#include "helper.h"
#include <cmath>
#include <apps/shared/global_context.h>

using namespace Poincare;
using namespace Shared;

namespace Graph {

void assert_float_equals(float a, float b, float tolerance = 1.f/static_cast<float>(Ion::Display::Height)) {
  /* The default value for the tolerance is chosen so that the error introduced
   * by caching would not typically be visible on screen. */
  assert_roughly_equal(a, b, tolerance, true);
}

void assert_check_cartesian_cache_against_function(ContinuousFunction * function, ContinuousFunctionCache * cache, Context * context, float tMin) {
  /* We set the cache to nullptr to force the evaluation (otherwise we would be
   * comparing the cache against itself). */
  function->setCache(nullptr);

  float t;
  for (int i = 0; i < Ion::Display::Width; i++) {
    t = tMin + i*cache->step();
    Coordinate2D<float> cacheValues = cache->valueForParameter(function, context, t, 0);
    Coordinate2D<float> functionValues = function->evaluateXYAtParameter(t, context);
    assert_float_equals(t, cacheValues.x1());
    assert_float_equals(t, functionValues.x1());
    assert_float_equals(cacheValues.x2(), functionValues.x2());
  }
  /* We set back the cache, so that it will not be invalidated in
   * PrepareForCaching later. */
  function->setCache(cache);
}

void assert_cartesian_cache_stays_valid_while_panning(ContinuousFunction * function, Context * context, InteractiveCurveViewRange * range, CurveViewCursor * cursor, ContinuousFunctionStore * store, float step) {
  ContinuousFunctionCache * cache = store->cacheAtIndex(0);
  assert(cache);

  float tMin, tStep;
  constexpr float margin = 0.04f;
  constexpr int numberOfMoves = 30;
  for (int i = 0; i < numberOfMoves; i++) {
    cursor->moveTo(cursor->t() + step, cursor->x() + step, function->evaluateXYAtParameter(cursor->x() + step, context).x2());
    range->panToMakePointVisible(cursor->x(), cursor->y(), margin, margin, margin, margin, (range->xMax() - range->xMin()) / (Ion::Display::Width - 1));
    tMin = range->xMin();
    tStep = (range->xMax() - range->xMin()) / (Ion::Display::Width - 1);
    ContinuousFunctionCache::PrepareForCaching(function, cache, tMin, tStep);
    assert_check_cartesian_cache_against_function(function, cache, context, tMin);
  }
}

void assert_check_polar_cache_against_function(ContinuousFunction * function, Context * context, InteractiveCurveViewRange * range, ContinuousFunctionStore * store) {
  ContinuousFunctionCache * cache = store->cacheAtIndex(0);
  assert(cache);

  float tMin = range->xMin();
  float tMax = range->xMax();

  float tStep, tCacheStep;
  ContinuousFunctionCache::ComputeNonCartesianSteps(&tStep, &tCacheStep, tMax, tMin);

  ContinuousFunctionCache::PrepareForCaching(function, cache, tMin, tCacheStep);

  // Fill the cache
  float t;
  for (int i = 0; i < Ion::Display::Width / 2; i++) {
    t = tMin + i*cache->step();
    function->evaluateXYAtParameter(t, context);
  }

  function->setCache(nullptr);
  for (int i = 0; i < Ion::Display::Width / 2; i++) {
    t = tMin + i*cache->step();
    Coordinate2D<float> cacheValues = cache->valueForParameter(function, context, t, 0);
    Coordinate2D<float> functionValues = function->evaluateXYAtParameter(t, context);
    assert_float_equals(cacheValues.x1(), functionValues.x1());
    assert_float_equals(cacheValues.x2(), functionValues.x2());
  }
}

void assert_cache_stays_valid(ContinuousFunction::PlotType type, const char * definition, float rangeXMin = -5, float rangeXMax = 5) {
  GlobalContext globalContext;
  ContinuousFunctionStore functionStore;

  InteractiveCurveViewRange graphRange;
  graphRange.setXMin(rangeXMin);
  graphRange.setXMax(rangeXMax);
  graphRange.setYMin(-3.f);
  graphRange.setYMax(3.f);

  CurveViewCursor cursor;
  ContinuousFunction * function = addFunction(definition, type, &functionStore, &globalContext);
  Coordinate2D<float> origin = function->evaluateXYAtParameter(0.f, &globalContext, 0);
  cursor.moveTo(0.f, origin.x1(), origin.x2());

  if (type == Cartesian) {
    assert_cartesian_cache_stays_valid_while_panning(function, &globalContext, &graphRange, &cursor, &functionStore, 2.f);
    assert_cartesian_cache_stays_valid_while_panning(function, &globalContext, &graphRange, &cursor, &functionStore, -0.4f);
  } else {
    assert_check_polar_cache_against_function(function, &globalContext, &graphRange, &functionStore);
  }

  functionStore.removeAll();
}

QUIZ_CASE(graph_caching) {
  Preferences::AngleUnit previousAngleUnit = Preferences::sharedPreferences()->angleUnit();
  Preferences::sharedPreferences()->setAngleUnit(Preferences::AngleUnit::Degree);
  assert_cache_stays_valid(Cartesian, "f(x)=x");
  assert_cache_stays_valid(Cartesian, "f(x)=x^2");
  assert_cache_stays_valid(Cartesian, "f(x)=sin(x)");
  assert_cache_stays_valid(Cartesian, "f(x)=sin(x)", -1e6f, 2e8f);
  assert_cache_stays_valid(Cartesian, "f(x)=sin(x^2)");
  assert_cache_stays_valid(Cartesian, "f(x)=1/x");
  assert_cache_stays_valid(Cartesian, "f(x)=1/x", -5e-5f, 5e-5f);
  assert_cache_stays_valid(Cartesian, "f(x)=-e^x");

  assert_cache_stays_valid(Polar, "r=1", 0.f, 360.f);
  assert_cache_stays_valid(Polar, "r=θ", 0.f, 360.f);
  assert_cache_stays_valid(Polar, "r=sin(θ)", 0.f, 360.f);
  assert_cache_stays_valid(Polar, "r=sin(θ)", 2e-4f, 1e-3f);
  assert_cache_stays_valid(Polar, "r=cos(5θ)", 0.f, 360.f);
  assert_cache_stays_valid(Polar, "r=cos(5θ)", -1e8f, 1e8f);

  Preferences::sharedPreferences()->setAngleUnit(previousAngleUnit);
}

QUIZ_CASE(graph_caching_signaling_nan) {
  quiz_assert(ContinuousFunctionCache::IsSignalingNan(ContinuousFunctionCache::SignalingNan()));
  quiz_assert(!ContinuousFunctionCache::IsSignalingNan(NAN));
}

}
