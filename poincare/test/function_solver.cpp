#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

enum class PointOfInterestType {
  Maximum,
  Minimum,
  Root,
  Intersection,
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

void assert_points_of_interest_are(
    PointOfInterestType type,
    int numberOfPointsOfInterest,
    Coordinate2D<double> * pointsOfInterest,
    const char * expression1,
    const char * expression2,
    const char * symbol,
    double start,
    double step,
    double max,
    Preferences::ComplexFormat complexFormat = Preferences::ComplexFormat::Real,
    Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree)
{
  Shared::GlobalContext context;
  Poincare::Expression e1 = parse_expression(expression1, &context, false);
  Poincare::Expression e2;
  if (expression2) {
    assert(type == PointOfInterestType::Intersection);
    e2 = parse_expression(expression2, &context, false);
  }
  for (int i = 0; i < numberOfPointsOfInterest; i++) {
    quiz_assert_log_if_failure(!std::isnan(start), e1);
    Coordinate2D<double> nextPointOfInterest;
    if (type == PointOfInterestType::Maximum) {
      nextPointOfInterest = e1.nextMaximum(symbol, start, step, max, &context, complexFormat, angleUnit);
    } else if (type == PointOfInterestType::Minimum) {
      nextPointOfInterest = e1.nextMinimum(symbol, start, step, max, &context, complexFormat, angleUnit);
    } else if (type == PointOfInterestType::Root) {
      nextPointOfInterest = Coordinate2D<double>(e1.nextRoot(symbol, start, step, max, &context, complexFormat, angleUnit), 0.0);
    } else if (type == PointOfInterestType::Intersection) {
      nextPointOfInterest = e1.nextIntersection(symbol, start, step, max, &context, complexFormat, angleUnit, e2);
    }
    quiz_assert_log_if_failure(
        doubles_are_approximately_equal(pointsOfInterest[i].x1(), nextPointOfInterest.x1()) &&
        doubles_are_approximately_equal(pointsOfInterest[i].x2(), nextPointOfInterest.x2()),
        e1);
    start = nextPointOfInterest.x1() + step;
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
      assert_points_of_interest_are(PointOfInterestType::Maximum, numberOfMaxima, maxima, "cos(a)", nullptr, "a", -1.0, 0.1, 500.0);
    }
    {
      constexpr int numberOfMaxima = 3;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(360.0, 1.0),
        Coordinate2D<double>(0.0, 1.0),
        Coordinate2D<double>(NAN, NAN)};
      assert_points_of_interest_are(PointOfInterestType::Maximum, numberOfMaxima, maxima, "cos(a)", nullptr, "a", 500.0, -0.1, -1.0);
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(180.0, -1.0)};
      assert_points_of_interest_are(PointOfInterestType::Minimum, numberOfMinima, minima, "cos(a)", nullptr, "a", 0.0, 0.1, 300.0);
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(180.0, -1.0)};
      assert_points_of_interest_are(PointOfInterestType::Minimum, numberOfMinima, minima, "cos(a)", nullptr, "a", 300.0, -0.1, 0.0);
    }
  }
  {
    {
      constexpr int numberOfMaxima = 1;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(NAN, NAN)};
      assert_points_of_interest_are(PointOfInterestType::Maximum, numberOfMaxima, maxima, "a^2", nullptr, "a", -1.0, 0.1, 100.0);
    }
    {
      constexpr int numberOfMaxima = 1;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(NAN, NAN)};
      assert_points_of_interest_are(PointOfInterestType::Maximum, numberOfMaxima, maxima, "a^2", nullptr, "a", 100.0, -0.1, -1.0);
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(0.0, 0.0)};
      assert_points_of_interest_are(PointOfInterestType::Minimum, numberOfMinima, minima, "a^2", nullptr, "a", -1.0, 0.1, 100.0);
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(0.0, 0.0)};
      assert_points_of_interest_are(PointOfInterestType::Minimum, numberOfMinima, minima, "a^2", nullptr, "a", 100.0, -0.1, -1.0);
    }
  }
  {
    {
      constexpr int numberOfMaxima = 1;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(NAN, 3.0)};
      assert_points_of_interest_are(PointOfInterestType::Maximum, numberOfMaxima, maxima, "3", nullptr, "a", -1.0, 0.1, 100.0);
    }
    {
      constexpr int numberOfMaxima = 1;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(NAN, 3.0)};
      assert_points_of_interest_are(PointOfInterestType::Maximum, numberOfMaxima, maxima, "3", nullptr, "a", 100.0, -0.1, -1.0);
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(NAN, 3.0)};
      assert_points_of_interest_are(PointOfInterestType::Minimum, numberOfMinima, minima, "3", nullptr, "a", -1.0, 0.1, 100.0);
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(NAN, 3.0)};
      assert_points_of_interest_are(PointOfInterestType::Minimum, numberOfMinima, minima, "3", nullptr, "a", 100.0, -0.1, -1.0);
    }
  }
  {
    {
      constexpr int numberOfMaxima = 1;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(NAN, 0.0)};
      assert_points_of_interest_are(PointOfInterestType::Maximum, numberOfMaxima, maxima, "0", nullptr, "a", -1.0, 0.1, 100.0);
    }
    {
      constexpr int numberOfMaxima = 1;
      Coordinate2D<double> maxima[numberOfMaxima] = {
        Coordinate2D<double>(NAN, 0.0)};
      assert_points_of_interest_are(PointOfInterestType::Maximum, numberOfMaxima, maxima, "0", nullptr, "a", 100.0, -0.1, -1.0);
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(NAN, 0.0)};
      assert_points_of_interest_are(PointOfInterestType::Minimum, numberOfMinima, minima, "0", nullptr, "a", -1.0, 0.1, 100.0);
    }
    {
      constexpr int numberOfMinima = 1;
      Coordinate2D<double> minima[numberOfMinima] = {
        Coordinate2D<double>(NAN, 0.0)};
      assert_points_of_interest_are(PointOfInterestType::Minimum, numberOfMinima, minima, "0", nullptr, "a", 100.0, -0.1, -1.0);
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
    assert_points_of_interest_are(PointOfInterestType::Root, numberOfRoots, roots, "cos(a)", nullptr, "a", 0.0, 0.1, 500.0);
  }
  {
    constexpr int numberOfRoots = 3;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(450.0, 0.0),
      Coordinate2D<double>(270.0, 0.0),
      Coordinate2D<double>(90.0, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Root, numberOfRoots, roots, "cos(a)", nullptr, "a", 500.0, -0.1, 0.0);
  }
  {
    constexpr int numberOfRoots = 1;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(0.0, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Root, numberOfRoots, roots, "a^2", nullptr, "a", -1.0, 0.1, 100.0);
  }
  {
    constexpr int numberOfRoots = 1;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(0.0, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Root, numberOfRoots, roots, "a^2", nullptr, "a", 100.0, -0.1, -1.0);
  }
  {
    constexpr int numberOfRoots = 2;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(-2.0, 0.0),
      Coordinate2D<double>(2.0, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Root, numberOfRoots, roots, "a^2-4", nullptr, "a", -5.0, 0.1, 100.0);
  }
  {
    constexpr int numberOfRoots = 2;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(2.0, 0.0),
      Coordinate2D<double>(-2.0, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Root, numberOfRoots, roots, "a^2-4", nullptr, "a", 100.0, -0.1, -5.0);
  }
  {
    constexpr int numberOfRoots = 1;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(NAN, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Root, numberOfRoots, roots, "3", nullptr, "a", -1.0, 0.1, 100.0);
  }
  {
    constexpr int numberOfRoots = 1;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(NAN, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Root, numberOfRoots, roots, "3", nullptr, "a", 100.0, -0.1, -1.0);
  }
  {
    constexpr int numberOfRoots = 1;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(-0.9, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Root, numberOfRoots, roots, "0", nullptr, "a", -1.0, 0.1, 100.0);
  }
  {
    constexpr int numberOfRoots = 1;
    Coordinate2D<double> roots[numberOfRoots] = {
      Coordinate2D<double>(99.8, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Root, numberOfRoots, roots, "0", nullptr, "a", 100.0, -0.1, -1.0);
  }
}

QUIZ_CASE(poincare_function_intersection) {
  {
    constexpr int numberOfIntersections = 1;
    Coordinate2D<double> intersections[numberOfIntersections] = {
      Coordinate2D<double>(NAN, NAN)};
    assert_points_of_interest_are(PointOfInterestType::Intersection, numberOfIntersections, intersections, "cos(a)", "2", "a", -1.0, 0.1, 500.0);
  }
  {
    constexpr int numberOfIntersections = 1;
    Coordinate2D<double> intersections[numberOfIntersections] = {
      Coordinate2D<double>(NAN, NAN)};
    assert_points_of_interest_are(PointOfInterestType::Intersection, numberOfIntersections, intersections, "cos(a)", "2", "a", 500.0, -0.1, -1.0);
  }
  {
    constexpr int numberOfIntersections = 2;
    Coordinate2D<double> intersections[numberOfIntersections] = {
      Coordinate2D<double>(0.0, 1.0),
      Coordinate2D<double>(360.0, 1.0)};
    assert_points_of_interest_are(PointOfInterestType::Intersection, numberOfIntersections, intersections, "cos(a)", "1", "a", -1.0, 0.1, 500.0);
  }
  {
    constexpr int numberOfIntersections = 2;
    Coordinate2D<double> intersections[numberOfIntersections] = {
      Coordinate2D<double>(360.0, 1.0),
      Coordinate2D<double>(0.0, 1.0)};
    assert_points_of_interest_are(PointOfInterestType::Intersection, numberOfIntersections, intersections, "cos(a)", "1", "a", 500.0, -0.1, -1.0);
  }
  {
    constexpr int numberOfIntersections = 3;
    Coordinate2D<double> intersections[numberOfIntersections] = {
      Coordinate2D<double>(90.0, 0.0),
      Coordinate2D<double>(270.0, 0.0),
      Coordinate2D<double>(450.0, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Intersection, numberOfIntersections, intersections, "cos(a)", "0", "a", -1.0, 0.1, 500.0);
  }
  {
    constexpr int numberOfIntersections = 3;
    Coordinate2D<double> intersections[numberOfIntersections] = {
      Coordinate2D<double>(450.0, 0.0),
      Coordinate2D<double>(270.0, 0.0),
      Coordinate2D<double>(90.0, 0.0)};
    assert_points_of_interest_are(PointOfInterestType::Intersection, numberOfIntersections, intersections, "cos(a)", "0", "a", 500.0, -0.1, -1.0);
  }
}
