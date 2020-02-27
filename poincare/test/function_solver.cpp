#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

enum class ExtremumType : uint8_t {
  Maximum,
  Minimum,
  Root
};

bool doubles_are_approximately_equal(double d1, double d2) {
  bool d2IsNaN = std::isnan(d2);
  if (std::isnan(d1)) {
    return d2IsNaN;
  }
  if (d2IsNaN) {
    return false;
  }
  return std::abs(d1-d2) < 0.00001;
}

void assert_next_extrema_are(
    ExtremumType extremumType,
    int numberOfExtrema,
    Coordinate2D<double> * extrema,
    const char * expression,
    const char * symbol,
    double start = -1.0,
    double step = 0.1,
    double max = 100.0,
    Preferences::ComplexFormat complexFormat = Preferences::ComplexFormat::Real,
    Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree)
{
  Shared::GlobalContext context;
  Poincare::Expression e = parse_expression(expression, &context, false);
  double currentStart = start;
  for (int i = 0; i < numberOfExtrema; i++) {
    quiz_assert_log_if_failure(!std::isnan(currentStart), e);
    Coordinate2D<double> nextExtrema;
    if (extremumType == ExtremumType::Maximum) {
      nextExtrema = e.nextMaximum(symbol, currentStart, step, max, &context, complexFormat, angleUnit);
    } else if (extremumType == ExtremumType::Minimum) {
      nextExtrema = e.nextMinimum(symbol, currentStart, step, max, &context, complexFormat, angleUnit);
    } else if (extremumType == ExtremumType::Root) {
      nextExtrema = Coordinate2D<double>(e.nextRoot(symbol, currentStart, step, max, &context, complexFormat, angleUnit), 0.0 );
    }
    currentStart = nextExtrema.x1() + step;
    quiz_assert_log_if_failure(
        (doubles_are_approximately_equal(extrema[i].x1(), nextExtrema.x1()))
        && (doubles_are_approximately_equal(extrema[i].x2(), nextExtrema.x2())),
        e);
  }
}

QUIZ_CASE(poincare_function_extremum) {
  {
    {
      constexpr int numberOfMaxima = 3;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(0.0, 1.0),
        Coordinate2D<double>(360.0, 1.0),
        Coordinate2D<double>(NAN, NAN)};
      assert_next_extrema_are(ExtremumType::Maximum, numberOfMaxima, maxima, "cos(a)", "a", -1.0, 0.1, 500.0);
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(180.0, -1.0)};
      assert_next_extrema_are(ExtremumType::Minimum, numberOfMinima, minima, "cos(a)", "a", 0.0, 0.1, 300.0);
    }
  }
  {
    {
      constexpr int numberOfMaxima = 1;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(NAN, NAN)};
      assert_next_extrema_are(ExtremumType::Maximum, numberOfMaxima, maxima, "a^2", "a");
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(0.0, 0.0)};
      assert_next_extrema_are(ExtremumType::Minimum, numberOfMinima, minima, "a^2", "a");
    }
  }
  {
    {
      constexpr int numberOfMaxima = 1;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(NAN, 3.0)};
      assert_next_extrema_are(ExtremumType::Maximum, numberOfMaxima, maxima, "3", "a");
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(NAN, 3.0)};
      assert_next_extrema_are(ExtremumType::Minimum, numberOfMinima, minima, "3", "a");
    }
  }
  {
    {
      constexpr int numberOfMaxima = 1;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(NAN, 0.0)};
      assert_next_extrema_are(ExtremumType::Maximum, numberOfMaxima, maxima, "0", "a");
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(NAN, 0.0)};
      assert_next_extrema_are(ExtremumType::Minimum, numberOfMinima, minima, "0", "a");
    }
  }
}

QUIZ_CASE(poincare_function_root) {
  {
    constexpr int numberOfRoots = 3;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(90.0, 0.0),
      Coordinate2D<double>(270.0, 0.0),
      Coordinate2D<double>(450.0, 0.0)};
    assert_next_extrema_are(ExtremumType::Root, numberOfRoots, roots, "cos(a)", "a", 0.0, 0.1, 500.0);
  }
  {
    constexpr int numberOfRoots = 1;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(0.0, 0.0)};
    assert_next_extrema_are(ExtremumType::Root, numberOfRoots, roots, "a^2", "a");
  }
  {
    constexpr int numberOfRoots = 2;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(-2.0, 0.0),
      Coordinate2D<double>(2.0, 0.0)};
    assert_next_extrema_are(ExtremumType::Root, numberOfRoots, roots, "a^2-4", "a", -5.0);
  }
  {
    constexpr int numberOfRoots = 1;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(NAN, 0.0)};
    assert_next_extrema_are(ExtremumType::Root, numberOfRoots, roots, "3", "a");
  }
  {
    constexpr int numberOfRoots = 1;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(-0.9, 0.0)};
    assert_next_extrema_are(ExtremumType::Root, numberOfRoots, roots, "0", "a");
  }
}

void assert_next_intersections_are(
    const char * otherExpression,
    int numberOfIntersections,
    Coordinate2D<double> * intersections,
    const char * expression,
    const char * symbol,
    double start = -1.0,
    double step = 0.1,
    double max = 500.0,
    Preferences::ComplexFormat complexFormat = Preferences::ComplexFormat::Real,
    Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree)
{
  Shared::GlobalContext context;
  Poincare::Expression e = parse_expression(expression, &context, false);
  Poincare::Expression other = parse_expression(otherExpression, &context, false);
  double currentStart = start;
  for (int i = 0; i < numberOfIntersections; i++) {
    quiz_assert_log_if_failure(!std::isnan(currentStart), e);
    Coordinate2D<double> nextIntersection = e.nextIntersection(symbol, currentStart, step, max, &context, complexFormat, angleUnit, other);
    currentStart = nextIntersection.x1() + step;
    quiz_assert_log_if_failure(
        (doubles_are_approximately_equal(intersections[i].x1(), nextIntersection.x1()))
        && (doubles_are_approximately_equal(intersections[i].x2(), nextIntersection.x2())),
        e);
  }
}

QUIZ_CASE(poincare_function_intersection) {
  {
    constexpr int numberOfIntersections = 1;
    Coordinate2D<double> intersections[numberOfIntersections] = {
      Coordinate2D<double>(NAN, NAN)};
    assert_next_intersections_are("2", numberOfIntersections, intersections, "cos(a)", "a");
  }
  {
    constexpr int numberOfIntersections = 2;
    Coordinate2D<double> intersections[numberOfIntersections] = {
      Coordinate2D<double>(0.0, 1.0),
      Coordinate2D<double>(360.0, 1.0)};
    assert_next_intersections_are("1", numberOfIntersections, intersections, "cos(a)", "a");
  }
  {
    constexpr int numberOfIntersections = 3;
    Coordinate2D<double> intersections[numberOfIntersections] = {
      Coordinate2D<double>(90.0, 0.0),
      Coordinate2D<double>(270.0, 0.0),
      Coordinate2D<double>(450.0, 0.0)};
    assert_next_intersections_are("0", numberOfIntersections, intersections, "cos(a)", "a");
  }
}
