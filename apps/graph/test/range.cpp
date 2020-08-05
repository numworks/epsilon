#include <quiz.h>
#include "helper.h"
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Graph {

void assert_range_inclusion_predicate(const char * definition, ContinuousFunction::PlotType type, const float * xList, size_t length, bool testInclusion) {
  /* Test that all points (x, f(x)) for x in xList are either included in the
   * range if testIncluded is true, or exculded from the range if testInclusion
   * is false.
   * If f(x) is not finite, only the presence of x in the horizontal range is
   * tested. */
  GlobalContext globalContext;
  ContinuousFunctionStore functionStore;
  ContinuousFunction * f = addFunction(definition, type, &functionStore, &globalContext);

  float xMin = FLT_MAX, xMax = - FLT_MAX, yMin = FLT_MAX, yMax = - FLT_MAX;
  f->rangeForDisplay(&xMin, &xMax, &yMin, &yMax, &globalContext);
  assert(xMin <= xMax && yMin <= yMax);

  for (size_t i = 0; i < length; i++) {
    float x = xList[i];
    float y = f->evaluateXYAtParameter(x, &globalContext).x2();
    assert(std::isfinite(x));
    if (!testInclusion) {
      quiz_assert(xMin > x || x > xMax || (std::isfinite(y) && (yMin > y || y > yMax)));
    } else {
      /* The program can miss the exact abscissa of an extremum, resulting in
       * bounds that are close from its value but that do not encompass it. We
       * thus test the inclusion of (x, f(x)) along with two neighbouring
       * points. */
      float dx = (xMax - xMin) / (Ion::Display::Width / 2);
      float y1 = f->evaluateXYAtParameter(x - dx, &globalContext).x2(), y2 = f->evaluateXYAtParameter(x + dx, &globalContext).x2();
      quiz_assert(xMin <= x
               && x <= xMax
               && (!std::isfinite(y)
                || (yMin <= y && y <= yMax)
                || (yMin <= y1 && y1 <= yMax)
                || (yMin <= y2 && y2 <= yMax)));
    }
  }
}

void assert_range_includes_points(const char * definition, ContinuousFunction::PlotType type, const float * xList, size_t length) { assert_range_inclusion_predicate(definition, type, xList, length, true); }
void assert_range_excludes_points(const char * definition, ContinuousFunction::PlotType type, const float * xList, size_t length) { assert_range_inclusion_predicate(definition, type, xList, length, false); }

void assert_range_includes_single_point(const char * definition, ContinuousFunction::PlotType type, float x) { assert_range_includes_points(definition, type, &x, 1); }
void assert_range_excludes_single_point(const char * definition, ContinuousFunction::PlotType type, float x) { assert_range_excludes_points(definition, type, &x, 1); }

void assert_range_includes_and_bounds_asymptotes(const char * definition, const float * asymptotesXList, size_t length) {
  /* The value for delta is the step the old algorithm used to sample a
   * cartesian function, causing functions such as 1/x to be evaluated too
   * close to 0. */
  constexpr float delta = 1.f / 32.f;
  for (size_t i = 0; i < length; i++) {
    float x = asymptotesXList[i];
    assert_range_includes_single_point(definition, Cartesian, x);
    assert_range_excludes_single_point(definition, Cartesian, x - delta);
    assert_range_excludes_single_point(definition, Cartesian, x + delta);
  }
}

void assert_range_shows_enough_periods(const char * definition, float period, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree) {
  /* The graph should display at least 3 periods, but no more than 7. */
  constexpr int lowNumberOfPeriods = 3, highNumberOfPeriods = 8;

  GlobalContext globalContext;
  ContinuousFunctionStore functionStore;
  ContinuousFunction * f = addFunction(definition, Cartesian, &functionStore, &globalContext);

  Preferences::sharedPreferences()->setAngleUnit(angleUnit);
  if (angleUnit != Preferences::AngleUnit::Degree) {
    f->setPlotType(Cartesian, angleUnit, &globalContext);
  }

  float xMin = FLT_MAX, xMax = - FLT_MAX, yMin = FLT_MAX, yMax = - FLT_MAX;
  f->rangeForDisplay(&xMin, &xMax, &yMin, &yMax, &globalContext);
  assert(xMin <= xMax && yMin <= yMax);
  float numberOfPeriods = (xMax - xMin) / period;

  quiz_assert(lowNumberOfPeriods <= numberOfPeriods && numberOfPeriods <= highNumberOfPeriods);
}

void assert_range_displays_entire_polar_function(const char * definition) {
  GlobalContext globalContext;
  ContinuousFunctionStore functionStore;
  ContinuousFunction * f = addFunction(definition, Polar, &functionStore, &globalContext);

  float xMin = FLT_MAX, xMax = - FLT_MAX, yMin = FLT_MAX, yMax = - FLT_MAX;
  f->rangeForDisplay(&xMin, &xMax, &yMin, &yMax, &globalContext);
  assert(xMin <= xMax && yMin <= yMax);

  for (float t = f->tMin(); t < f->tMax(); t += f->rangeStep()) {
    const Coordinate2D<float> xy = f->evaluateXYAtParameter(t, &globalContext);
    if (!std::isfinite(xy.x1()) || !std::isfinite(xy.x2())) {
      continue;
    }
    quiz_assert(xMin <= xy.x1() && xy.x1() <= xMax && yMin <= xy.x2() && xy.x2() <= yMax);
  }
}

QUIZ_CASE(graph_range_polynomes) {
  assert_range_includes_single_point("37", Cartesian, 0.f);
  assert_range_includes_single_point("x-1", Cartesian, 1.f);
  assert_range_includes_single_point("100+x", Cartesian, -100.f);
  assert_range_includes_single_point("x^2-20*x+101", Cartesian, 10.f);

  constexpr float array1[2] = {-2.f, 3.f};
  assert_range_includes_points("(x+2)*(x-3)", Cartesian, array1, 2);

  constexpr float array2[2] = {-1.f, 0.5f};
  assert_range_includes_points("2*x^2+x-1", Cartesian, array2, 2);

  constexpr float array3[6] = {-3.f, 3.f, -2.f, 2.f, -1.f, 1.f};
  assert_range_includes_points("(x+3)(x+2)(x+1)(x-1)(x-2)(x-3)", Cartesian, array3, 6);

  constexpr float array4[3] = {0.f, 4.f, 5.f};
  assert_range_includes_points("x^5-5*x^4", Cartesian, array4, 3);
}

QUIZ_CASE(graph_range_exponential) {
  assert_range_includes_single_point("ℯ^x", Cartesian, 0.f);
  assert_range_excludes_single_point("ℯ^x", Cartesian, 4.f);

  assert_range_includes_single_point("ℯ^(-x)", Cartesian, 0.f);
  assert_range_excludes_single_point("ℯ^(-x)", Cartesian, -4.f);

  constexpr float array1[3] = {1.f, 5.f, 1e-1f};
  assert_range_includes_points("ln(x)", Cartesian, array1, 3);
  assert_range_excludes_single_point("ln(x)", Cartesian, 1e-3f);

  constexpr float array2[2] = {-1.f, 3.f};
  assert_range_includes_points("2-ℯ^(-x)", Cartesian, array2, 2);
  assert_range_excludes_single_point("2-ℯ^(-x)", Cartesian, 50.f);
  assert_range_excludes_single_point("2-ℯ^(-x)", Cartesian, -10.f);

  assert_range_includes_single_point("x^x", Cartesian, 0.5f);
  assert_range_excludes_single_point("x^x", Cartesian, -5.f);
  assert_range_includes_single_point("x^(1/x)", Cartesian, 1e-3f);
}

QUIZ_CASE(graph_range_fractions) {
  constexpr float array1[1] = {0.f};
  assert_range_includes_and_bounds_asymptotes("1/x", array1, 1);
  assert_range_includes_and_bounds_asymptotes("1/x^2", array1, 1);

  constexpr float array2[2] = {-2.f, 2.f};
  assert_range_includes_and_bounds_asymptotes("1/(x^2-4)", array2, 2);

  constexpr float array3[1] = {-100.f};
  assert_range_includes_and_bounds_asymptotes("1/(x+100)", array3, 1);
}

QUIZ_CASE(graph_range_periodic) {
  assert_range_shows_enough_periods("sin(x)", 2*M_PI, Preferences::AngleUnit::Radian);
  assert_range_shows_enough_periods("sin(x)", 360.f);
  assert_range_shows_enough_periods("sin(x)+10", 2*M_PI, Preferences::AngleUnit::Radian);
  assert_range_shows_enough_periods("sin(x)+10", 360.f);
  assert_range_shows_enough_periods("cos(x)", 2*M_PI, Preferences::AngleUnit::Radian);
  assert_range_shows_enough_periods("cos(x)", 360.f);
  assert_range_shows_enough_periods("sin(x)/x", 2*M_PI, Preferences::AngleUnit::Radian);
  assert_range_shows_enough_periods("sin(x)/x", 360.f);
  assert_range_shows_enough_periods("x*sin(x)", 2*M_PI, Preferences::AngleUnit::Radian);
  assert_range_shows_enough_periods("x*sin(x)", 360.f);
  assert_range_shows_enough_periods("ln(sin(x))", 2*M_PI, Preferences::AngleUnit::Radian);
  assert_range_shows_enough_periods("ln(sin(x))", 360.f);
  constexpr float array1[2] = {-1.f, 1.f};
  assert_range_includes_points("x*sin(1/x)", Cartesian, array1, 2);
}

QUIZ_CASE(graph_range_polar) {
  assert_range_displays_entire_polar_function("1");
  assert_range_displays_entire_polar_function("θ");
  assert_range_displays_entire_polar_function("sin(θ)");
  assert_range_displays_entire_polar_function("sin(10θ)");
  assert_range_displays_entire_polar_function("ln(θ)");
}

}
