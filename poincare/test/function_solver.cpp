#include "helper.h"
#include <apps/shared/global_context.h>
#include <poincare/solver.h>

using namespace Poincare;

typedef Solver<double>::Interest Interest;

void assert_next_solution_is(const char * expression, Context * context, Solver<double> * solver, Coordinate2D<double> expected, Interest interest, const char * otherExpression) {
  assert(std::isnan(expected.x1()) == std::isnan(expected.x2()));

  Expression e = parse_expression(expression, context, false);

  Coordinate2D<double> observed;
  switch (interest) {
  case Interest::Root:
    observed = solver->nextRoot(e);
    break;
  case Interest::LocalMinimum:
    observed = solver->nextMinimum(e);
    break;
  case Interest::LocalMaximum:
    observed = solver->nextMaximum(e);
    break;
  default:
    assert(interest == Interest::Intersection);
    assert(otherExpression);
    Expression e2 = parse_expression(otherExpression, context, false);
    observed = solver->nextIntersection(e, e2);
  }

  constexpr double relativePrecision = 2. * Helpers::SquareRoot(2. * Float<double>::Epsilon());
  quiz_assert_print_if_failure(std::isnan(observed.x1()) == std::isnan(expected.x1()), expression);
  if (std::isfinite(expected.x1())) {
    quiz_assert_print_if_failure(Helpers::RelativelyEqual(observed.x1(), expected.x1(), relativePrecision), expression);
    quiz_assert_print_if_failure(Helpers::RelativelyEqual(observed.x2(), expected.x2(), relativePrecision), expression);
  }
}

void assert_solutions_are(const char * expression, double start, double end, std::initializer_list<Coordinate2D<double>> expected, Interest interest, Preferences::AngleUnit angleUnit, const char * otherExpression) {
  Shared::GlobalContext context;
  Solver<double> solver(start, end, "x", &context, Real, angleUnit);
  for (Coordinate2D<double> c : expected) {
    assert_next_solution_is(expression, &context, &solver, c, interest, otherExpression);
  }
  assert_next_solution_is(expression, &context, &solver, Coordinate2D<double>(NAN, NAN), interest, otherExpression);
}

void assert_roots_are(const char * expression, double start, double end, std::initializer_list<Coordinate2D<double>> expected, Preferences::AngleUnit angleUnit = Degree) {
  assert_solutions_are(expression, start, end, expected, Interest::Root, angleUnit, nullptr);
}
void assert_minima_are(const char * expression, double start, double end, std::initializer_list<Coordinate2D<double>> expected, Preferences::AngleUnit angleUnit = Degree) {
  assert_solutions_are(expression, start, end, expected, Interest::LocalMinimum, angleUnit, nullptr);
}
void assert_maxima_are(const char * expression, double start, double end, std::initializer_list<Coordinate2D<double>> expected, Preferences::AngleUnit angleUnit = Degree) {
  assert_solutions_are(expression, start, end, expected, Interest::LocalMaximum, angleUnit, nullptr);
}
void assert_intersections_are(const char * expression1, const char * expression2, double start, double end, std::initializer_list<Coordinate2D<double>> expected, Preferences::AngleUnit angleUnit = Degree) {
  assert_solutions_are(expression1, start, end, expected, Interest::Intersection, angleUnit, expression2);
}

Coordinate2D<double> R(double x) { return Coordinate2D<double>(x, 0.); }
Coordinate2D<double> XY(double x, double y) { return Coordinate2D<double>(x, y); }

QUIZ_CASE(poincare_solver_roots) {
  assert_roots_are("cos(x)", 0., 500., { R(90.), R(270.), R(450.) });
  assert_roots_are("cos(x)", 500., 0., { R(450.), R(270.), R(90.) });
  assert_roots_are("x^2", -1., 100., { R(0.) });
  assert_roots_are("x^2", 100., -1., { R(0.) });
  assert_roots_are("x^2-4", -5., 100., { R(-2.), R(2.) });
  assert_roots_are("x^2-4", 100., -5, { R(2.), R(-2.) });
  // TODO assert_roots_are("0", -1., 100., { ... });
  assert_roots_are("log(x^2/(0.01-x))+4.8", -10., 10., { R(-0.00040611049837290978), R(0.00039026156644829873) });
  assert_roots_are("csc(x)+tan(2×x)", 6., -6., { R(5.127216097883478), R(3.687536352063443), R(2.5956489551161432), R(1.1559692092961089), R(-1.1559692092961089), R(-2.5956489551161432), R(-3.687536352063443), R(-5.127216097883478) }, Radian);
  assert_roots_are("3", -1., 100., {});
  assert_roots_are("e^x", -1000., -800., {});
  assert_roots_are("x", 1e208, 1e208, {});
  // TODO assert_roots_are("1/(x-100)^2-10000", 0., 10000., { R(99.99), R(100.01) });
}

QUIZ_CASE(poincare_solver_minima) {
  assert_minima_are("cos(x)", 0., 300., { XY(180., -1.) });
  assert_minima_are("cos(x)", 300., 0., { XY(180., -1.) });
  assert_minima_are("x^2", -1e-3, 100., { XY(0., 0.) });
  assert_minima_are("3", -1e28, 1e28, {});
  assert_minima_are("csc(x)+tan(2×x)", -6., 6., { XY(-5.8419120302571166, 3.5573771431792993), XY(0.44127324410824942, 3.5573771431792993) }, Radian);
  assert_minima_are("1/(x-3)", 7., -1., {});
  assert_minima_are("5+4/sin(x)-2/tan(x)", 10., -10., { XY(7.3303828071957247, 8.4641016151377606), XY(1.0471975566950564, 8.4641016151377606), XY(-5.2359877779725741, 8.4641016151377606) }, Radian);
  assert_minima_are("(x^2+x+1)/x", -2., 2., { XY(1., 3.) });
  assert_minima_are("0.05/x+1.44×x^2", 7., -7., { XY(0.25893604049110841, 0.2896468153816889) });
  assert_minima_are("x^3+200/x", -6., 6., { XY(2.8574404375160141, 93.323613642148885)});
  assert_minima_are("16π/x+8πx", -2., 2., { XY(1.4142135565624723, 71.086127010533858) });
}

QUIZ_CASE(poincare_solver_maxima) {
  assert_maxima_are("cos(x)", -1., 500., { XY(0., 1.), XY(360., 1.) });
  assert_maxima_are("cos(x)", 500., -1., { XY(360., 1.), XY(0., 1.) });
  assert_maxima_are("x^2", -1e-3, 100., {});
  assert_maxima_are("3", -1e28, 1e28, {});
  assert_maxima_are("csc(x)+tan(2×x)", 6., -6., { XY(5.8419120302571166, -3.5573771431792993), XY(-0.44127324410824942, -3.5573771431792993) }, Radian);
  assert_maxima_are("1/(x-3)", -1., 7., {});
  assert_maxima_are("5+4/sin(x)-2/tan(x)", -10., 10., { XY(-7.3303828071957247, 1.5358983848622398), XY(-1.0471975566950564, 1.5358983848622398), XY(5.2359877779725741, 1.5358983848622398) }, Radian);
  assert_maxima_are("(x^2+x+1)/x", -2., 2., { XY(-1., -1.) });
  assert_maxima_are("0.05/x+1.44×x^2", 7., -7., {});
}

QUIZ_CASE(poincare_solver_intersections) {
  assert_intersections_are("cos(x)", "2", -1., 500., {});
  assert_intersections_are("cos(x)", "1", 500., -1, { XY(360., 1.), XY(0., 1.) });
  assert_intersections_are("x", "x^3", -1e-2, 2., { XY(0., 0.), XY(1., 1.) });
  // TODO assert_intersections_are("x", "√(x)", -666., 666., { XY(0., 0.), XY(1., 1.) });
  // TODO assert_intersections_are("x^2-3x-2", "log(x^2-2x)", -8., 10., { XY(-0.609961198731614, 0.2019362602), XY(i-0.00516870705322244, -1.984467163), XY(2.00005000450738i, -3.999949993), XY(3.75110444134456, 0.8174712058) });
}
