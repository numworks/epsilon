#include <poincare/zoom.h>
#include "helper.h"
#include <apps/shared/global_context.h>
#include <float.h>

using namespace Poincare;

class ParametersPack {
public:
  ParametersPack(Expression expression, const char * symbol, Preferences::AngleUnit angleUnit) :
    m_expression(expression),
    m_symbol(symbol),
    m_angleUnit(angleUnit)
  {}

  Expression expression() const { return m_expression; }
  const char * symbol() const { return m_symbol; }
  Preferences::AngleUnit angleUnit() const { return m_angleUnit; }
private:
  Expression m_expression;
  const char * m_symbol;
  Preferences::AngleUnit m_angleUnit;
};

constexpr float NormalRatio = 306.f / 576.f;

float evaluate_expression(float x, Context * context, const void * auxiliary) {
  const ParametersPack * pack = static_cast<const ParametersPack *>(auxiliary);
  return pack->expression().approximateWithValueForSymbol<float>(pack->symbol(), x, context, Real, pack->angleUnit());
}

bool bracket(float a, float b, float d) {
  assert(std::isfinite(a) && std::isfinite(b) && std::isfinite(d));
  return a - d <= b && b <= a + d;
}

bool range_matches(float min, float max, float targetMin, float targetMax, float tolerance) {
  assert(std::isfinite(targetMin) == std::isfinite(targetMax));

  const float rangeTolerance = tolerance * (targetMax - targetMin);

  if (std::isfinite(targetMin) && (targetMin <= targetMax)) {
    return  std::isfinite(min) && std::isfinite(max)
         && bracket(min, targetMin, rangeTolerance) && bracket(max, targetMax, rangeTolerance);
  } else {
    return (!std::isfinite(min) && !std::isfinite(max)) || (max < min);
  }
}

bool window_is_similar(float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax, bool addXMargin = false, float tolerance = 0.1f) {
  assert(std::isfinite(targetXMin) == std::isfinite(targetXMax) && std::isfinite(targetYMin) == std::isfinite(targetYMax));

  /* The target window given in the test should reflect the points of
   * interest we try to display, and should not take into account the
   * breathing room added by the algorithm. We handle it here. */
  constexpr float margin = 0.3f; // Zoom::k_breathingRoom;
  float xDelta = addXMargin ? (xMax - xMin) * margin : 0.f;

  return range_matches(xMin, xMax, targetXMin - xDelta, targetXMax + xDelta, tolerance)
      && range_matches(yMin, yMax, targetYMin, targetYMax, tolerance);
}

void assert_interesting_range_is(const char * definition, float targetXMin, float targetXMax, float targetYMin = FLT_MAX, float targetYMax = -FLT_MAX, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(definition, &globalContext, false);
  float xMin, xMax, yMin, yMax;
  ParametersPack aux(e, symbol, angleUnit);
  Zoom::InterestingRangesForDisplay(&evaluate_expression, &xMin, &xMax, &yMin, &yMax, -INFINITY, INFINITY, &globalContext, &aux);

  bool test = window_is_similar(xMin, xMax, yMin, yMax, targetXMin, targetXMax, targetYMin, targetYMax, true);
  quiz_assert_print_if_failure(test, definition);
}

void assert_has_no_interesting_range(const char * definition, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  assert_interesting_range_is(definition, NAN, NAN, NAN, NAN, angleUnit, symbol);
}

void breaker() {}

QUIZ_CASE(poincare_zoom_interesting_ranges) {
  assert_has_no_interesting_range(Undefined::Name());
  assert_has_no_interesting_range("0");
  assert_has_no_interesting_range("1");
  assert_has_no_interesting_range("-100");
  assert_has_no_interesting_range("x");
  assert_has_no_interesting_range("x^2");
  assert_has_no_interesting_range("-(x^3)");
  assert_has_no_interesting_range("10×x^4");
  assert_has_no_interesting_range("ℯ^(-x)");

  assert_interesting_range_is("x-21", 20.5, 22.5);
  assert_interesting_range_is("-11x+100", 8.7, 9.5);
  assert_interesting_range_is("x^2-1", -5, 5, -1, -1);
  assert_interesting_range_is("(x+10)(x-10)", -10.5, 10.5, -100, -100);
  assert_interesting_range_is("x(x-1)(x-2)(x-3)(x-4)(x-5)", 0, 5, -16, 5);
  assert_interesting_range_is("1/x", -2.5, 2.5);
  assert_interesting_range_is("1/(x-10)", 8, 14);
  assert_interesting_range_is("√(x)", 0, 7);
  assert_interesting_range_is("ln(x)", 0, 5);
  assert_interesting_range_is("sin(x)", -9, 9, -1, 1);
  assert_interesting_range_is("sin(x)", -480, 480, -1, 1, Degree);
  assert_interesting_range_is("cos(x)", -10, 10, -1, 1);
  assert_interesting_range_is("cos(x)", -580, 580, -1, 1, Degree);
  assert_interesting_range_is("tan(x)", -9, 9);
  assert_interesting_range_is("tan(x)", -500, 500, FLT_MAX, -FLT_MAX, Degree);
  assert_interesting_range_is("asin(x)", -1, 1);
  assert_interesting_range_is("acos(x)", -1, 1, FLT_MAX, -FLT_MAX, Degree);
  assert_interesting_range_is("atan(x)", -2, 2);
  assert_interesting_range_is("sin(x)/x", -12, 12, -0.2, 1);
  assert_interesting_range_is("x×sin(x)", -9, 9, -5, 8);
}

void assert_refined_range_is(const char * definition, float targetXMin, float targetXMax, float targetYMin, float targetYMax, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(definition, &globalContext, false);
  float yMin, yMax;
  ParametersPack aux(e, symbol, angleUnit);
  Zoom::RefinedYRangeForDisplay(&evaluate_expression, targetXMin, targetXMax, &yMin, &yMax, &globalContext, &aux);

  bool test = window_is_similar(targetXMin, targetXMax, yMin, yMax, targetXMin, targetXMax, targetYMin, targetYMax);
  quiz_assert_print_if_failure(test, definition);
}

QUIZ_CASE(poincare_zoom_refined_ranges) {
  assert_refined_range_is(Undefined::Name(), NAN, NAN, NAN, NAN);
  assert_refined_range_is("0", NAN, NAN, NAN, NAN);

  assert_refined_range_is("1", -10, 10, 1, 1);
  assert_refined_range_is("x", -10, 10, -10, 10);
  assert_refined_range_is("x^2", -10, 10, 0, 37);
  assert_refined_range_is("1/x", -10, 10, -0.73, 0.73);
  assert_refined_range_is("(x-100)(x+100)", -120, 120, -10000, 4400);
  assert_refined_range_is("sin(x)", -300, 300, -1, 1, Degree);
  assert_refined_range_is("ℯ^x", -10, 10, 0, 3);
  assert_refined_range_is("atan(x)", -100, 100, -90, 90, Degree);
}

bool can_find_normal_range(const char * definition, Preferences::AngleUnit angleUnit, const char * symbol) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(definition, &globalContext, false);
  float xMin, xMax, yMin, yMax;
  ParametersPack aux(e, symbol, angleUnit);
  Zoom::RangeWithRatioForDisplay(&evaluate_expression, NormalRatio, &xMin, &xMax, &yMin, &yMax, &globalContext, &aux);
  return std::isfinite(xMin) && std::isfinite(xMax) && std::isfinite(yMin) && std::isfinite(yMax);
}

void assert_range_is_normalized(const char * definition, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  quiz_assert_print_if_failure(can_find_normal_range(definition, angleUnit, symbol), definition);
}

QUIZ_CASE(poincare_zoom_ratio_ranges) {
  assert_range_is_normalized("x");
  assert_range_is_normalized("x^2");
  assert_range_is_normalized("-(x^3)");
  assert_range_is_normalized("ℯ^x");
  assert_range_is_normalized("ℯ^(-x)");
}

void assert_full_range_is(const char * definition, float targetXMin, float targetXMax, float targetYMin, float targetYMax, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(definition, &globalContext, false);
  float yMin, yMax;
  constexpr float stepDivisor = Ion::Display::Width;
  const float step = (targetXMax - targetXMin) / stepDivisor;
  ParametersPack aux(e, symbol, angleUnit);
  Zoom::FullRange(&evaluate_expression, targetXMin, targetXMax, step, &yMin, &yMax, &globalContext, &aux);
  quiz_assert_print_if_failure(window_is_similar(targetXMin, targetXMax, yMin, yMax, targetXMin, targetXMax, targetYMin, targetYMax), definition);
}

QUIZ_CASE(poincare_zoom_full_ranges) {
  assert_full_range_is("1", -10, 10, 1, 1);
  assert_full_range_is("x", -10, 10, -10, 10);
  assert_full_range_is("x-3", -10, 10, -13, 7);
  assert_full_range_is("-6x", -10, 10, -60, 60);
  assert_full_range_is("x^2", -10, 10, 0, 100);
  assert_full_range_is("ℯ^x", -10, 10, 0, 22000);
  assert_full_range_is("sin(x)", -3600, 3600, -1, 1, Degree);
  assert_full_range_is("acos(x)", -10, 10, 0, 3.14);
}

void assert_ranges_combine(int length, float * mins, float * maxs, float targetMin, float targetMax) {
  float resMin, resMax;
  Zoom::CombineRanges(length, mins, maxs, &resMin, &resMax);
  quiz_assert(resMin == targetMin && resMax == targetMax);
}

void assert_sanitized_range_is(float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax) {
  Zoom::SanitizeRange(&xMin, &xMax, &yMin, &yMax, NormalRatio);
  quiz_assert(xMin == targetXMin && xMax == targetXMax && yMin == targetYMin && yMax == targetYMax);
}

void assert_ratio_is_set(float xMin, float xMax, float yMin, float yMax, float yxRatio) {
  {
    float tempXMin = xMin, tempXMax = xMax, tempYMin = yMin, tempYMax = yMax;
    Zoom::SetToRatio(yxRatio, &tempXMin, &tempXMax, &tempYMin, &tempYMax, false);
    quiz_assert((tempYMax - tempYMin) / (tempXMax - tempXMin) == yxRatio);
    quiz_assert((tempYMax - tempYMin) > (yMax - yMin) || (tempXMax - tempXMin) > (xMax - xMin));
  }
  {
    float tempXMin = xMin, tempXMax = xMax, tempYMin = yMin, tempYMax = yMax;
    Zoom::SetToRatio(yxRatio, &tempXMin, &tempXMax, &tempYMin, &tempYMax, true);
    quiz_assert((tempYMax - tempYMin) / (tempXMax - tempXMin) == yxRatio);
    quiz_assert((tempYMax - tempYMin) < (yMax - yMin) || (tempXMax - tempXMin) < (xMax - xMin));
  }
}

QUIZ_CASE(poincare_zoom_utility) {
  // Ranges combinations
  {
    constexpr int length = 1;
    float mins[length] = {-10};
    float maxs[length] = {10};
    assert_ranges_combine(length, mins, maxs, -10, 10);
  }
  {
    constexpr int length = 2;
    float mins[length] = {-1, -2};
    float maxs[length] = {1, 2};
    assert_ranges_combine(length, mins, maxs, -2, 2);
  }
  {
    constexpr int length = 2;
    float mins[length] = {-1, 9};
    float maxs[length] = {1, 11};
    assert_ranges_combine(length, mins, maxs, -1, 11);
  }
  {
    constexpr int length = 3;
    float mins[length] = {-3, -2, -1};
    float maxs[length] = {1, 2, 3};
    assert_ranges_combine(length, mins, maxs, -3, 3);
  }

  // Range sanitation
  assert_sanitized_range_is(-10, 10, -10, 10, -10, 10, -10, 10);
  assert_sanitized_range_is(-10, 10, 100, 100, -10, 10, 94.6875, 105.3125);
  assert_sanitized_range_is(3, -3, -10, 10, -18.8235302, 18.8235302, -10, 10);
  assert_sanitized_range_is(3, -3, 2, 2, -10, 10, -3.3125, 7.3125);
  assert_sanitized_range_is(NAN, NAN, NAN, NAN, -10, 10, -5.3125, 5.3125);

  // Ratio
  assert_ratio_is_set(-10, 10, -10, 10, 0.1);
  assert_ratio_is_set(-10, 10, -10, 10, 0.5);
  assert_ratio_is_set(-10, 10, -10, 10, NormalRatio);
  assert_ratio_is_set(-10, 10, -10, 10, 3);


}
