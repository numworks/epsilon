#include "helper.h"
#include <apps/shared/global_context.h>
#include <poincare/zoom.h>
#include <cmath>

using namespace Poincare;

// When adding the graph window margins, this ratio gives an orthonormal window
constexpr float k_normalRatio = 0.442358822;
constexpr const char * k_symbol = "x";
constexpr float k_rangeTolerance = 0.05f;
constexpr float k_maxFloat = 1e8f;

/* Class befriended by Poincare::Zoom to be able to read its members. */
class ZoomTest {
public:
  ZoomTest(Range1D bounds, Context * context) : m_zoom(bounds.min(), bounds.max(), k_normalRatio, context, Range1D::k_maxFloat) {}

  Zoom * zoom() { return &m_zoom; }
  Range2D interestingRange() const { return m_zoom.m_interestingRange; }

private:
  Zoom m_zoom;
};

void assert_ranges_equal(Range1D observed, Range1D expected, const char * errorMessage = "") {
  /* The range computed by Poincare::Zoom can differ from the ideal range
   * because:
   * - the function is only sampled a finite number of times, and can miss the
   *   peaks and valleys.
   * - searching for points of interest is done in single precision, without
   *   the use of Brent's methods to refine the points.*/
  quiz_assert_print_if_failure(observed.isValid() == expected.isValid(), errorMessage);
  if (expected.isValid()) {
    float dl = std::max(k_rangeTolerance * std::max(expected.length(), std::fabs(expected.center())), FLT_EPSILON);
    quiz_assert_print_if_failure(std::fabs(observed.min() - expected.min()) <= dl, errorMessage);
    quiz_assert_print_if_failure(std::fabs(observed.max() - expected.max()) <= dl, errorMessage);
  }
}

void assert_ranges_equal(Range2D observed, Range2D expected, const char * errorMessage = "") {
  assert_ranges_equal(*observed.x(), *expected.x(), errorMessage);
  assert_ranges_equal(*observed.y(), *expected.y(), errorMessage);
}

template <typename T> Coordinate2D<T> expressionEvaluator(T t, const void * model, Context * context) {
  const Expression * e = static_cast<const Expression *>(model);
  if (e->type() == ExpressionNode::Type::Matrix) {
    return Coordinate2D<T>(e->childAtIndex(0).approximateWithValueForSymbol(k_symbol, t, context, Real, Radian), e->childAtIndex(1).approximateWithValueForSymbol(k_symbol, t, context, Real, Radian));
  }
  return Coordinate2D<T>(t, e->approximateWithValueForSymbol(k_symbol, t, context, Real, Radian));
}

void assert_full_function_range_is(const char * expression, Range1D bounds, Range2D expectedRange) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression, &context, false);
  ZoomTest zoom(bounds, &context);
  zoom.zoom()->fitFullFunction(expressionEvaluator<float>, &e);
  assert_ranges_equal(zoom.interestingRange(), expectedRange, expression);
}

QUIZ_CASE(poincare_zoom_fit_full_function) {
  assert_full_function_range_is("1", Range1D(-10, 10), Range2D(-10, 10, 1, 1));
  assert_full_function_range_is("x", Range1D(-10, 10), Range2D(-10, 10, -10, 10));
  assert_full_function_range_is("e^x", Range1D(-10, 10), Range2D(-10, 10, std::exp(-10.f), std::exp(10.f)));
  assert_full_function_range_is("1+1/x", Range1D(0.1, 2), Range2D(0.1, 2, 1.5, 11));
  assert_full_function_range_is("[[cos(x)][sin(x)]]", Range1D(0, 2 * M_PI), Range2D(-1, 1, -1, 1));
  assert_full_function_range_is("[[-x^2+1][-2x]]", Range1D(0, 1), Range2D(0, 1, -2, 0));
  assert_full_function_range_is("[[acos(x)][asin(x)]]", Range1D(0, 1), Range2D(0, M_PI/2, 0, M_PI/2));
}

void assert_points_of_interest_range_is(const char * expression, Range2D expectedRange) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression, &context, false);
  ZoomTest zoom(Range1D(-k_maxFloat, k_maxFloat), &context);
  zoom.zoom()->fitPointsOfInterest(expressionEvaluator<float>, &e, false, expressionEvaluator<double>);
  assert_ranges_equal(zoom.interestingRange(), expectedRange, expression);
}

QUIZ_CASE(poincare_zoom_fit_points_of_interest) {
  assert_points_of_interest_range_is("1", Range2D());
  assert_points_of_interest_range_is("x", Range2D(0, 0, 0, 0));
  assert_points_of_interest_range_is("x-30", Range2D(30, 30, 0, 0));
  assert_points_of_interest_range_is("-11x+100", Range2D(9.091, 9.091, 0, 0));
  assert_points_of_interest_range_is("x^2", Range2D(0, 0, 0, 0));
  assert_points_of_interest_range_is("(x-10)(x+10)", Range2D(-10, 10, -100, 0));
  assert_points_of_interest_range_is("e^(-x)", Range2D(1.803, 1.803, 0.165, 0.165));
  assert_points_of_interest_range_is("√(x^2+1)-x", Range2D(1.483, 1.483, 0.306, 0.306));
  assert_points_of_interest_range_is("x(x-1)(x-2)(x-3)(x-4)(x-5)", Range2D(0, 5, -16.901, 5.046));
  assert_points_of_interest_range_is("1/x", Range2D(-2.416, 2.416, -0.414, 0.414));
  assert_points_of_interest_range_is("1+(x+1)^(-4)", Range2D(-3.073, 0.911, 1.053, 1.075));
  assert_points_of_interest_range_is("2+1/(x^2+x+1)", Range2D(-2.663, 1.558, 2.184, 3.333));
  assert_points_of_interest_range_is("√(x)", Range2D(0, 7.422, 0, 2.724));
  assert_points_of_interest_range_is("√(x+1)-2", Range2D(-1.000, 6.237, 0.000, 0.690));
  assert_points_of_interest_range_is("ln(x)", Range2D(0, 5.792, 0, 1.756));
  assert_points_of_interest_range_is("sin(x)", Range2D(-8.028, 8.028, -1, 1));
  assert_points_of_interest_range_is("sin(π*x/180)", Range2D(-450, 450, -1, 1));
  assert_points_of_interest_range_is("cos(x+1)+2", Range2D(-13.850, 15.240, 1, 3));
  assert_points_of_interest_range_is("x*ln(x)", Range2D(0, 1, -0.368, 0));
  assert_points_of_interest_range_is("(e^x-1)/(e^x+1)", Range2D(-2.309, 2.309, -0.819, 0.819));
  assert_points_of_interest_range_is("(e^x-1)*(e^x+1)^(-1)", Range2D(-2.309, 2.309, -0.819, 0.819));
  assert_points_of_interest_range_is("10-1/(3^x)", Range2D(-2.096, 1.723, 0, 9.849));
  // FIXME assert_points_of_interest_range_is("x^x", Range2D());
  assert_points_of_interest_range_is("root(x^3+1,3)-x", Range2D(-1.733, 1.478, 0.119, 1.587));
  assert_points_of_interest_range_is("sum((((-1)^k)*(x^(2k+1)))/((2k+1)!),k,0,4)", Range2D(-4.968, 4.968, -1, 1));
}

void assert_intersections_range_is(const char * expression1, const char * expression2, Range2D expectedRange) {
  Shared::GlobalContext context;
  Expression e1 = parse_expression(expression1, &context, false);
  Expression e2 = parse_expression(expression2, &context, false);
  ZoomTest zoom(Range1D(-k_maxFloat, k_maxFloat), &context);
  zoom.zoom()->fitIntersections(expressionEvaluator, &e1, expressionEvaluator, &e2);
  assert_ranges_equal(zoom.interestingRange(), expectedRange, expression1);
}

QUIZ_CASE(poincare_zoom_fit_intersections) {
  /* Intersection ranges are more tentative as the search for intersections
   * uses less refinements. */
  assert_intersections_range_is("sin(x)", "cos(x)", Range2D(-7.413, 7.413, -0.904, 0.904));
  assert_intersections_range_is("x/2+2", "2x-1", Range2D(2, 2, 3, 3));
  assert_intersections_range_is("x^2", "-x^2/3+x", Range2D(0, 0.78, 0, 0.608));
}

void assert_sanitized_range_is(Range2D inputRange, Range2D expectedRange) {
  assert_ranges_equal(Zoom::Sanitize(inputRange, k_normalRatio, k_maxFloat), expectedRange);
}

QUIZ_CASE(poincare_zoom_sanitation) {
  assert_ranges_equal(Zoom::DefaultRange(k_normalRatio, k_maxFloat), Range2D(-10, 10, -4.42358822, 4.42358822));
  assert_ranges_equal(Zoom::DefaultRange(1, k_maxFloat), Range2D(-10, 10, -10, 10));

  assert_sanitized_range_is(Range2D(Range1D(-5, 5), Range1D()), Range2D(-5, 5, -2.211794, 2.211794));
  assert_sanitized_range_is(Range2D(), Zoom::DefaultRange(k_normalRatio, k_maxFloat));
  assert_sanitized_range_is(Range2D(0, 0, 0, 0), Zoom::DefaultRange(k_normalRatio, k_maxFloat));
}
