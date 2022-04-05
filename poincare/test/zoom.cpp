#include <poincare/undefined.h>
#include <poincare/zoom.h>
#include <apps/shared/global_context.h>
#include <float.h>

#include "helper.h"

using namespace Poincare;

// When adding the graph window margins, this ratio gives an orthonormal window
constexpr float NormalRatio = 0.442358822;
constexpr float StandardTolerance = 50.f * FLT_EPSILON;

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

float evaluate_expression(float x, Context * context, const void * auxiliary) {
  const ParametersPack * pack = static_cast<const ParametersPack *>(auxiliary);
  return pack->expression().approximateWithValueForSymbol<float>(pack->symbol(), x, context, Real, pack->angleUnit());
}

bool range1D_matches(float min, float max, float targetMin, float targetMax, float tolerance = StandardTolerance) {
  return (roughly_equal(min, targetMin, tolerance) && roughly_equal(max, targetMax, tolerance))
      || (std::isnan(min) && std::isnan(max) && std::isnan(targetMin) && std::isnan(targetMax));
}

bool ranges_match(float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax, float tolerance = StandardTolerance) {
  return range1D_matches(xMin, xMax, targetXMin, targetXMax, tolerance)
      && range1D_matches(yMin, yMax, targetYMin, targetYMax, tolerance);
}

void assert_interesting_range_is(const char * definition, float targetXMin, float targetXMax, float targetYMin, float targetYMax, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  float xMin, xMax, yMin, yMax;
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(definition, &globalContext, false);
  ParametersPack aux(e, symbol, angleUnit);
  Zoom::InterestingRangesForDisplay(evaluate_expression, &xMin, &xMax, &yMin, &yMax, -INFINITY, INFINITY, &globalContext, &aux);
  quiz_assert_print_if_failure(ranges_match(xMin, xMax, yMin, yMax, targetXMin, targetXMax, targetYMin, targetYMax), definition);
}

QUIZ_CASE(poincare_zoom_interesting_ranges) {
  assert_interesting_range_is("0", FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX);
  assert_interesting_range_is("1", FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX);
  assert_interesting_range_is("-100", FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX);
  assert_interesting_range_is("x", -10, 10, 0, 0);
  assert_interesting_range_is("x^2", -10, 10, 0, 0);
  assert_interesting_range_is("-(x^3)", -10, 10, 0, 0);
  assert_interesting_range_is("10×x^4", -10, 10, 0, 0);
  assert_interesting_range_is("e^(-x)", FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX);
  assert_interesting_range_is("√(x^2+1)-x", FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX);
  assert_interesting_range_is("x-21", 9.37962341, 29.3796234, 0, 0);
  assert_interesting_range_is("-11x+100", -1.07708645, 18.9229126, 0, 0);
  assert_interesting_range_is("x^2-1", -7.80982208, 7.80982208, -1, 0);
  assert_interesting_range_is("3x^2+x+10", -10.1693897, 9.83061028, 9.91668892, 9.91668892);
  assert_interesting_range_is("(x+10)(x-10)", -15.5615616, 15.5615616, -100, 0);
  assert_interesting_range_is("x(x-1)(x-2)(x-3)(x-4)(x-5)", -1.46434164, 6.34548044, -16.9008026, 5.02076149);
  assert_interesting_range_is("1/x", -3.5958581, 3.5958581, FLT_MAX, -FLT_MAX);
  assert_interesting_range_is("1/(1-x)", -2.54995537, 4.49294949, 0, 0);
  assert_interesting_range_is("1/(x-10)", 5.98468208, 14.1209784, 0, 0);
  assert_interesting_range_is("√(x)", -1.89636505, 8.21758175, 0, 0);
  assert_interesting_range_is("ln(x)", -1.46434164, 6.34548044, 0, 0);
  assert_interesting_range_is("sin(x)", -13.0978546, 13.0978546, -0.999772608, 0.999772608);
  assert_interesting_range_is("cos(x)", -893.509644, 893.509644, -0.999294519, 0.998674929, Degree);
  assert_interesting_range_is("tan(x)", -13.0978546, 13.0978546, 0, 0);
  assert_interesting_range_is("1/tan(x)", -893.509644, 893.509644, 0, 0, Degree);
  assert_interesting_range_is("asin(x)", -1.65563226, 1.65563226, 0, 0);
  assert_interesting_range_is("acos(x)", -1.65563226, 1.65563226, FLT_MAX, -FLT_MAX);
  assert_interesting_range_is("atan(x)", -3.2989521, 3.2989521, 0, 0);
  assert_interesting_range_is("x×sin(x)", -13.0978546, 13.0978546, -4.81180477, 7.73865843);
  assert_interesting_range_is("x×ln(x)", -0.284799129, 1.23412955, -0.36787945, 0);
  assert_interesting_range_is("root(x^3+1,3)-x", -2.47177649, 2.21971154, 1.58732843, 1.58732843);
  // TODO : Fix Interesting ranges for x^x
  // assert_interesting_range_is("x^x", -0.674223423, 2.92163467, 0.692200601, 0.692200601);
  assert_interesting_range_is("x^x", -9.63138771, 4.98867226, 0.692200601, 0.692200601);
  assert_interesting_range_is("(e^x-1)/(e^x+1)", -3.2989521, 3.2989521, 0, 0);
}


void assert_refined_range_is(const char * definition, float xMin, float xMax, float targetYMin, float targetYMax, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  float yMin = FLT_MAX, yMax = -FLT_MAX;
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(definition, &globalContext, false);
  ParametersPack aux(e, symbol, angleUnit);
  Zoom::RefinedYRangeForDisplay(evaluate_expression, xMin, xMax, &yMin, &yMax, &globalContext, &aux);
  quiz_assert_print_if_failure(range1D_matches(yMin, yMax, targetYMin, targetYMax), definition);
}

QUIZ_CASE(poincare_zoom_refined_range) {
  assert_refined_range_is("1", -10, 10, 1, 1);
  assert_refined_range_is("-100", -10, 10, -100, -100);
  assert_refined_range_is("x", -10, 10, -9.74683571, 9.74683571);
  assert_refined_range_is("x^2", -10, 10, 0, 36.5035477);
  assert_refined_range_is("-(x^3)", -10, 10, -133.769241, 133.769241);
  assert_refined_range_is("10×x^4", -10, 10, 0, 4902.04102);
  assert_refined_range_is("e^(-x)", -10, 10, 0, 2.71828127);
  assert_refined_range_is("x-21", 19.1269588, 21.9576702, -1.48373008, 0.92183876);
  assert_refined_range_is("-11x+100", 8.80657959, 10.1099186, -8.44783878, 2.94615173);
  assert_refined_range_is("x^2-1", -8.63486099, 8.63486099, -0.988053143, 26.5802021);
  assert_refined_range_is("(x+10)(x-10)", -17.2055073, 17.2055073, -99.9525681, 156.399399);
  assert_refined_range_is("x(x-1)(x-2)(x-3)(x-4)(x-5)", -1.61903656, 7.01582479, -16.8871994, 67.6706848);
  assert_refined_range_is("1/x", -3.97572827, 3.97572827, -1.86576664, 1.86576664);
  assert_refined_range_is("1/(x-10)", 5.72560453, 15.8184233, -1.45247293, 1.45247293);
  assert_refined_range_is("√(x)", -2.09669948, 9.08569717, 0, 2.99067688);
  assert_refined_range_is("ln(x)", -1.61903656, 7.01582479, -2.63196707, 1.93246615);
  assert_refined_range_is("sin(x)", -13.2858067, 13.2858067, -0.998738647, 0.998738587);
  assert_refined_range_is("cos(x)", -906.33136, 906.33136, -0.999904931, 0.998831093, Degree);
  assert_refined_range_is("tan(x)", -14.4815292, 14.4815292, -2.86643958, 2.86643958);
  assert_refined_range_is("asin(x)", -1.67939043, 1.67939043, -1.131253, 1.131253);
  assert_refined_range_is("acos(x)", -1.67939043, 1.67939043, 0, 177.611237, Degree);
  assert_refined_range_is("atan(x)", -3.34629107, 3.34629107, -1.27329516, 1.27329516);
  assert_refined_range_is("x×sin(x)", -14.4815292, 14.4815292, -7.37234354, 7.37234354);
  assert_refined_range_is("x×ln(x)", -0.314885706, 1.36450469, -0.367870897, 0.396377981);
  assert_refined_range_is("x!", -10, 10, FLT_MAX, -FLT_MAX);
  assert_refined_range_is("xe^(1/x)", -1.29999995, 2.4000001, -0.564221799, 5.58451653);
}

void assert_orthonormal_range_is(const char * definition, float xMin, float xMax, float targetYMin, float targetYMax, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  assert(std::isfinite(xMin) && std::isfinite(xMax) && xMin <= xMax);
  assert((targetYMin == FLT_MAX && targetYMax == -FLT_MAX) || roughly_equal((targetYMax - targetYMin) / (xMax - xMin), NormalRatio, StandardTolerance));
  float yMin, yMax;
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(definition, &globalContext, false);
  ParametersPack aux(e, symbol, angleUnit);
  Zoom::RangeWithRatioForDisplay(evaluate_expression, NormalRatio, xMin, xMax, FLT_MAX, -FLT_MAX, &yMin, &yMax, &globalContext, &aux);
  quiz_assert_print_if_failure(range1D_matches(yMin, yMax, targetYMin, targetYMax), definition);
}

QUIZ_CASE(poincare_zoom_range_with_ratio) {
  assert_orthonormal_range_is("1", 0, 0, FLT_MAX, -FLT_MAX);
  assert_orthonormal_range_is("x", -10, 10, -4.36069489, 4.48648167);
  assert_orthonormal_range_is("x^2", -10, 10, -0.0527148247, 8.7944622);
  assert_orthonormal_range_is("x^3", -10, 10, -3.91881895, 4.9283576);
  assert_orthonormal_range_is("e^x", -10, 10, -0.439413071, 8.40776348);
  assert_orthonormal_range_is("e^x+4", -10, 10, 3.56058741, 12.4077644);
}

void assert_full_range_is(const char * definition, float xMin, float xMax, float targetYMin, float targetYMax, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(definition, &globalContext, false);
  float yMin, yMax;
  constexpr float stepDivisor = Ion::Display::Width;
  const float step = (xMax - xMin) / stepDivisor;
  ParametersPack aux(e, symbol, angleUnit);
  Zoom::FullRange(&evaluate_expression, xMin, xMax, step, &yMin, &yMax, &globalContext, &aux);
  quiz_assert_print_if_failure(range1D_matches(yMin, yMax, targetYMin, targetYMax), definition);
}

QUIZ_CASE(poincare_zoom_full_range) {
  assert_full_range_is("1", -10, 10, 1, 1);
  assert_full_range_is("x", -10, 10, -10, 10);
  assert_full_range_is("x-3", -10, 10, -13, 7);
  assert_full_range_is("-6x", -10, 10, -60, 60);
  assert_full_range_is("x^2", -10, 10, 0, 100);
  assert_full_range_is("e^x", -10, 10, 0.0000453999419, 22026.459);
  assert_full_range_is("sin(x)", -3600, 3600, -1, 1, Degree);
  assert_full_range_is("acos(x)", -10, 10, 0, 3.1415925);
}

void assert_ranges_combine_to(int length, float * mins, float * maxs, float targetMin, float targetMax) {
  float resMin = NAN, resMax = NAN;
  for (int i = 0; i < length; i++) {
    Zoom::CombineRanges(mins[i], maxs[i], resMin, resMax, &resMin, &resMax);
  }
  quiz_assert(resMin == targetMin && resMax == targetMax);
}

void assert_sanitized_range_is(float min, float max, float targetMin, float targetMax) {
  Zoom::SanitizeRangeForDisplay(&min, &max);
  quiz_assert(min == targetMin && max == targetMax);
}

void assert_ratio_is_set_to(float yxRatio, float xMin, float xMax, float yMin, float yMax, bool shrink, float targetXMin, float targetXMax, float targetYMin, float targetYMax) {
  float tempXMin = xMin, tempXMax = xMax, tempYMin = yMin, tempYMax = yMax;
  assert(yxRatio == (targetYMax - targetYMin) / (targetXMax - targetXMin));
  Zoom::SetToRatio(yxRatio, &tempXMin, &tempXMax, &tempYMin, &tempYMax, shrink);
  quiz_assert(ranges_match(tempXMin, tempXMax, tempYMin, tempYMax, targetXMin, targetXMax, targetYMin, targetYMax));
}

void assert_shrinks_to(float yxRatio, float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax) {
  assert_ratio_is_set_to(yxRatio, xMin, xMax, yMin, yMax, true, targetXMin, targetXMax, targetYMin, targetYMax);
}

void assert_expands_to(float yxRatio, float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax) {
  assert_ratio_is_set_to(yxRatio, xMin, xMax, yMin, yMax, false, targetXMin, targetXMax, targetYMin, targetYMax);
}

void assert_zooms_to(float ratio, float xCenter, float yCenter, float xMin, float xMax, float yMin, float yMax, float targetXMin, float targetXMax, float targetYMin, float targetYMax) {
  float tempXMin = xMin, tempXMax = xMax, tempYMin = yMin, tempYMax = yMax;
  Zoom::SetZoom(ratio, xCenter, yCenter, &tempXMin, &tempXMax, &tempYMin, &tempYMax);
  quiz_assert(ranges_match(tempXMin, tempXMax, tempYMin, tempYMax, targetXMin, targetXMax, targetYMin, targetYMax));
}

QUIZ_CASE(poincare_zoom_utility) {
  // Ranges combinations
  {
    constexpr int length = 1;
    float mins[length] = {-10};
    float maxs[length] = {10};
    assert_ranges_combine_to(length, mins, maxs, -10, 10);
  }
  {
    constexpr int length = 2;
    float mins[length] = {-1, -2};
    float maxs[length] = {1, 2};
    assert_ranges_combine_to(length, mins, maxs, -2, 2);
  }
  {
    constexpr int length = 2;
    float mins[length] = {-1, 9};
    float maxs[length] = {1, 11};
    assert_ranges_combine_to(length, mins, maxs, -1, 11);
  }
  {
    constexpr int length = 3;
    float mins[length] = {-3, -2, -1};
    float maxs[length] = {1, 2, 3};
    assert_ranges_combine_to(length, mins, maxs, -3, 3);
  }

  // Range sanitation
  assert_sanitized_range_is(
      -10, 10,
      -10, 10);
  assert_sanitized_range_is(
      100, 100,
      90, 110);
  assert_sanitized_range_is(
      3, -3,
      -10, 10);
  assert_sanitized_range_is(
      NAN, NAN,
      -10, 10);

  // Ratio
  assert_shrinks_to(1.f,
      -10, 10, -10, 10,
      -10, 10, -10, 10);
  assert_expands_to(1.f,
      -10, 10, -10, 10,
      -10, 10, -10, 10);
  assert_shrinks_to(0.5f,
      -10, 10, -10, 10,
      -10, 10, -5, 5);
  assert_expands_to(0.5f,
      -10, 10, -10, 10,
      -20, 20, -10, 10);
  assert_shrinks_to(1.33f,
      -10, 10, -10, 10,
      -7.518797, 7.518797, -10, 10);
  assert_expands_to(1.33f,
      -10, 10, -10, 10,
      -10, 10, -13.3, 13.3);

  // Zoom
  assert_zooms_to(1.f, 0, 0,
      -10, 10, -10, 10,
      -10, 10, -10, 10);
  assert_zooms_to(0.5f, 0, 0,
      -10, 10, -5, 5,
      -5, 5, -2.5, 2.5);
  assert_zooms_to(3.f, 0, 0,
      -10, 10, -5, 5,
      -30, 30, -15, 15);
  assert_zooms_to(1.5f, 10, 5,
      -10, 10, -5, 5,
      -20, 10, -10, 5);
  assert_zooms_to(0.25f, 2, -2,
      -10, 10, -5, 5,
      -1, 4, -2.75, -0.25);
}
