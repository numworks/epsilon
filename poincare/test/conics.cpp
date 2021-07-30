#include <apps/shared/global_context.h>
#include <poincare/conic.h>

#include "helper.h"

using namespace Poincare;

Conic getConicFromExpression(const char * expression) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  Preferences::ComplexFormat complexFormat =
      Preferences::ComplexFormat::Cartesian;
  Preferences::AngleUnit angleUnit =
      Preferences::sharedPreferences()->angleUnit();
  Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric;
  e = e.reduce(ExpressionNode::ReductionContext(
      &globalContext, complexFormat, angleUnit, unitFormat,
      ExpressionNode::ReductionTarget::SystemForApproximation));
  return Conic(e, &globalContext);
}

void quiz_assert_undefined(const char * expression) {
  Conic conic = getConicFromExpression(expression);
  quiz_assert(conic.getConicType() == Conic::Type::Undefined);
}

void quiz_assert_conic_parameter_is(double observed, double expected) {
  quiz_assert(IsApproximatelyEqual(observed, expected, 1e-5, FLT_EPSILON));
}

void quiz_assert_circle(const char * expression, double radius, double cx = 0.0,
                        double cy = 0.0) {
  Conic conic = getConicFromExpression(expression);
  quiz_assert(conic.getConicType() == Conic::Type::Circle);
  double x, y;
  conic.getCenter(&x, &y);
  // TODO Hugo : Improve precision for center
  quiz_assert_conic_parameter_is(x, cx);
  quiz_assert_conic_parameter_is(y, cy);
  quiz_assert_conic_parameter_is(conic.getEccentricity(), 0.0);
  quiz_assert_conic_parameter_is(conic.getRadius(), radius);
}

void quiz_assert_ellipse(const char * expression, double eccentricity,
                         double linearEccentricity, double semiMajorAxis,
                         double semiMinorAxis, double cx = 0.0,
                         double cy = 0.0) {
  Conic conic = getConicFromExpression(expression);
  quiz_assert(conic.getConicType() == Conic::Type::Ellipse);
  double x, y;
  conic.getCenter(&x, &y);
  quiz_assert_conic_parameter_is(x, cx);
  quiz_assert_conic_parameter_is(y, cy);
  quiz_assert_conic_parameter_is(conic.getEccentricity(), eccentricity);
  quiz_assert_conic_parameter_is(conic.getLinearEccentricity(),
                                 linearEccentricity);
  quiz_assert_conic_parameter_is(conic.getSemiMajorAxis(), semiMajorAxis);
  quiz_assert_conic_parameter_is(conic.getSemiMinorAxis(), semiMinorAxis);
}

void quiz_assert_hyperbola(const char * expression, double eccentricity,
                           double linearEccentricity, double semiMajorAxis,
                           double semiMinorAxis, double cx = 0.0,
                           double cy = 0.0) {
  Conic conic = getConicFromExpression(expression);
  quiz_assert(conic.getConicType() == Conic::Type::Hyperbola);
  double x, y;
  conic.getCenter(&x, &y);
  quiz_assert_conic_parameter_is(x, cx);
  quiz_assert_conic_parameter_is(y, cy);
  quiz_assert_conic_parameter_is(conic.getEccentricity(), eccentricity);
  quiz_assert_conic_parameter_is(conic.getLinearEccentricity(),
                                 linearEccentricity);
  quiz_assert_conic_parameter_is(conic.getSemiMajorAxis(), semiMajorAxis);
  quiz_assert_conic_parameter_is(conic.getSemiMinorAxis(), semiMinorAxis);
}

void quiz_assert_parabola(const char * expression, double parameter,
                          double sx = 0.0, double sy = 0.0) {
  Conic conic = getConicFromExpression(expression);
  quiz_assert(conic.getConicType() == Conic::Type::Parabola);
  double x, y;
  conic.getSummit(&x, &y);
  quiz_assert_conic_parameter_is(x, sx);
  quiz_assert_conic_parameter_is(y, sy);
  quiz_assert_conic_parameter_is(conic.getEccentricity(), 1.0);
  quiz_assert_conic_parameter_is(conic.getParameter(), parameter);
}

QUIZ_CASE(poincare_conics_canonic) {
  quiz_assert_circle("y^2+x^2-1", 1.0);
  quiz_assert_ellipse("4*y^2+x^2-1", 0.866025, 0.866025, 1.0, 0.5);
  quiz_assert_hyperbola("y^2-x^2-1", 1.41421, 1.41421, 1.0, 1.0);
  quiz_assert_parabola("y^2-4x", 2.0);
}

QUIZ_CASE(poincare_conics_invalid) {
  quiz_assert_undefined("y+x+1");
  quiz_assert_undefined("y^2+y+1");
  quiz_assert_undefined("y^2+x^2-ln(x)");
  quiz_assert_undefined("y^3+y^2+x^2");
  quiz_assert_undefined("x*y^2+x^2");
}

QUIZ_CASE(poincare_conics_general) {
  quiz_assert_circle("x^2+y^2-x+y", 0.707107, 0.5, -0.5);  // 1/sqrt(2)
  quiz_assert_ellipse("x^2+y^2+x*y+x+y", 0.816497, 0.816497 * 0.816497, 0.816497, 0.471405, -0.333333, -0.333333);
  quiz_assert_ellipse("x^2+0.25*y^2-1", 0.866025, 0.866025 * 2.0, 2.0, 1.0);
  quiz_assert_hyperbola("x^2+2*x*y-4y^2+2x+y-16", 1.13334, 1.13334 * 3.76999,
                        3.76999, 2.01069, -0.9, -0.1);  // 0.946213
  quiz_assert_ellipse("3x^2-2x*y+4y^2+x-y-7", 0.695847, 0.695847 * 1.72814,
                      1.72814, 1.24113, -0.136364, 0.0909091);  // 1.28098
  quiz_assert_parabola("4x^2-4x*y+y^2-x-2*y+2", 0.223607, 0.4, 0.8);
}