#include <apps/shared/global_context.h>
#include <poincare/numeric/zoom.h>
#include <poincare/src/expression/simplification.h>

#include <cmath>

#include "../helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

// When adding the graph window margins, this ratio gives an orthonormal window
constexpr float k_normalRatio = 0.442358822;
constexpr float k_rangeTolerance = 0.05f;
constexpr float k_maxFloat = 1e8f;

/* Class befriended by Poincare::Zoom to be able to read its members. */
class ZoomTest {
 public:
  ZoomTest(Range1D<float> bounds)
      : m_zoom(bounds.min(), bounds.max(), k_normalRatio,
               Range1D<float>::k_maxFloat) {}

  Zoom* zoom() { return &m_zoom; }
  Range2D<float> interestingRange() const { return m_zoom.m_interestingRange; }

 private:
  Zoom m_zoom;
};

void assert_ranges_equal(Range1D<float> observed, Range1D<float> expected,
                         const char* errorMessage = "") {
  /* The range computed by Poincare::Zoom can differ from the ideal range
   * because:
   * - the function is only sampled a finite number of times, and can miss the
   *   peaks and valleys.
   * - searching for points of interest is done in single precision, without
   *   the use of Brent's methods to refine the points. */
  quiz_assert_print_if_failure(observed.isNan() == expected.isNan(),
                               errorMessage);
  if (expected.isNan()) {
    return;
  }
  float dl = std::max(k_rangeTolerance * std::max(expected.length(),
                                                  std::fabs(expected.center())),
                      FLT_EPSILON);
  quiz_assert_print_if_failure(std::fabs(observed.min() - expected.min()) <= dl,
                               errorMessage);
  quiz_assert_print_if_failure(std::fabs(observed.max() - expected.max()) <= dl,
                               errorMessage);
}

void assert_ranges_equal(Range2D<float> observed, Range2D<float> expected,
                         const char* errorMessage = "") {
  assert_ranges_equal(*observed.x(), *expected.x(), errorMessage);
  assert_ranges_equal(*observed.y(), *expected.y(), errorMessage);
}

template <typename T>
Coordinate2D<T> expressionEvaluator(T t, const void* model) {
  const Tree* e = static_cast<const Tree*>(model);
  // TODO_PCJ : Handle matrixes
#if 0
  ApproximationContext approximationContext(context, Real, Radian);
  if (e->otype() == ExpressionNode::Type::OMatrix) {
    return Coordinate2D<T>(
        e->childAtIndex(0).approximateToScalarWithValueForSymbol(
            k_symbol, t, approximationContext),
        e->childAtIndex(1).approximateToScalarWithValueForSymbol(
            k_symbol, t, approximationContext));
  }
  return Coordinate2D<T>(t, e->approximateToScalarWithValueForSymbol(
                                k_symbol, t, approximationContext));
#endif
  return Coordinate2D<T>(
      t, Approximation::To<T>(e, t, Approximation::Parameters{.isRoot = true}));
}

void assert_points_of_interest_range_is(const char* expression,
                                        Range2D<float> expectedRange) {
  Shared::GlobalContext context;
  Tree* e =
      parseAndPrepareForApproximation(expression, {.m_context = &context});
  ZoomTest zoom(Range1D<float>(-k_maxFloat, k_maxFloat));
  zoom.zoom()->fitPointsOfInterest(expressionEvaluator<float>, e, false,
                                   expressionEvaluator<double>);
  e->removeTree();
  assert_ranges_equal(zoom.interestingRange(), expectedRange, expression);
}

QUIZ_CASE(poincare_zoom_fit_points_of_interest) {
  assert_points_of_interest_range_is("1", Range2D<float>());
  assert_points_of_interest_range_is("x", Range2D<float>(0, 0, 0, 0));
  assert_points_of_interest_range_is("x-30", Range2D<float>(30, 30, 0, 0));
  assert_points_of_interest_range_is("-11x+100",
                                     Range2D<float>(9.091, 9.091, 0, 0));
  assert_points_of_interest_range_is("x^2", Range2D<float>(0, 0, 0, 0));
  assert_points_of_interest_range_is("(x-10)(x+10)",
                                     Range2D<float>(-10, 10, -100, 0));
  assert_points_of_interest_range_is(
      "e^(-x)", Range2D<float>(1.803, 1.803, 0.165, 0.165));
  assert_points_of_interest_range_is(
      "√(x^2+1)-x", Range2D<float>(1.483, 1.483, 0.306, 0.306));
  assert_points_of_interest_range_is("x(x-1)(x-2)(x-3)(x-4)(x-5)",
                                     Range2D<float>(0, 5, -16.901, 5.046));
  assert_points_of_interest_range_is(
      "1/x", Range2D<float>(-2.416, 2.416, -0.414, 0.414));
  assert_points_of_interest_range_is(
      "1+(x+1)^(-4)", Range2D<float>(-3.073, 0.911, 1.053, 1.075));
  assert_points_of_interest_range_is(
      "2+1/(x^2+x+1)", Range2D<float>(-2.663, 1.558, 2.184, 3.333));
  assert_points_of_interest_range_is("√(x)",
                                     Range2D<float>(0, 7.422, 0, 2.724));
  assert_points_of_interest_range_is(
      "√(x+1)-2", Range2D<float>(-1.000, 6.237, 0.000, 0.690));
  assert_points_of_interest_range_is("ln(x)",
                                     Range2D<float>(0, 5.792, 0, 1.756));
  assert_points_of_interest_range_is("sin(x)",
                                     Range2D<float>(-8.028, 8.028, -1, 1));
  assert_points_of_interest_range_is("sin(π*x/180)",
                                     Range2D<float>(-450, 450, -1, 1));
  assert_points_of_interest_range_is("cos(x+1)+2",
                                     Range2D<float>(-7.287, 8.432, 1, 3));
  assert_points_of_interest_range_is("x*ln(x)",
                                     Range2D<float>(0, 1, -0.368, 0));
  assert_points_of_interest_range_is(
      "(e^x-1)/(e^x+1)", Range2D<float>(-2.309, 2.309, -0.819, 0.819));
  assert_points_of_interest_range_is(
      "(e^x-1)*(e^x+1)^(-1)", Range2D<float>(-2.309, 2.309, -0.819, 0.819));
  // TODO_PCJ: this test fails with emscripten
  // assert_points_of_interest_range_is("10-1/(3^x)",
  // Range2D<float>(-2.096, 1.723, 0, 9.849));
  // FIXME assert_points_of_interest_range_is("x^x", Range2D<float>());
  // TODO_PCJ: assert_points_of_interest_range_is(
  // "root(x^3+1,3)-x", Range2D<float>(-1.733, 1.478, 0.119, 1.587));
  assert_points_of_interest_range_is(
      "sum((((-1)^k)*(x^(2k+1)))/((2k+1)!),k,0,4)",
      Range2D<float>(-4.968, 4.968, -1, 1));
  assert_points_of_interest_range_is(
      "200e^(x/5)(7+e^(x/5))^(-1)",
      Range2D<float>(-18.68, 38.84, 0.6786, 199.4));
}

void assert_intersections_range_is(const char* expression1,
                                   const char* expression2,
                                   Range2D<float> expectedRange) {
  Shared::GlobalContext context;
  TreeRef e1 =
      parseAndPrepareForApproximation(expression1, {.m_context = &context});
  TreeRef e2 =
      parseAndPrepareForApproximation(expression2, {.m_context = &context});
  ZoomTest zoom(Range1D<float>(-k_maxFloat, k_maxFloat));
  zoom.zoom()->fitIntersections(expressionEvaluator, e1, expressionEvaluator,
                                e2);
  e1->removeTree();
  e2->removeTree();
  assert_ranges_equal(zoom.interestingRange(), expectedRange, expression1);
}

QUIZ_CASE(poincare_zoom_fit_intersections) {
  assert_intersections_range_is("x/2+2", "2x-1", Range2D<float>(2, 2, 3, 3));
  assert_intersections_range_is("x^2", "-x^2/3+x",
                                Range2D<float>(0, 0.75, 0, 0.5631));
}

void assert_sanitized_range_is(Range2D<float> inputRange,
                               Range2D<float> expectedRange) {
  assert_ranges_equal(Zoom::Sanitize(inputRange, k_normalRatio, k_maxFloat),
                      expectedRange);
}

QUIZ_CASE(poincare_zoom_sanitation) {
  assert_ranges_equal(Zoom::DefaultRange(k_normalRatio, k_maxFloat),
                      Range2D<float>(-10, 10, -4.42358822, 4.42358822));
  assert_ranges_equal(Zoom::DefaultRange(1, k_maxFloat),
                      Range2D<float>(-10, 10, -10, 10));

  assert_sanitized_range_is(
      Range2D<float>(Range1D<float>(-5, 5), Range1D<float>()),
      Range2D<float>(-5, 5, -2.211794, 2.211794));
  assert_sanitized_range_is(Range2D<float>(),
                            Zoom::DefaultRange(k_normalRatio, k_maxFloat));
  assert_sanitized_range_is(Range2D<float>(0, 0, 0, 0),
                            Zoom::DefaultRange(k_normalRatio, k_maxFloat));
}
