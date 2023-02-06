#include <apps/shared/global_context.h>
#include <poincare/conic.h>
#include <ion/storage/file_system.h>

#include "helper.h"

using namespace Poincare;

CartesianConic getConicFromExpression(const char * expression) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  return CartesianConic(e, &globalContext);
}

void quiz_assert_undefined(const char * expression) {
  CartesianConic conic = getConicFromExpression(expression);
  quiz_assert(conic.conicType().shape == Conic::Shape::Undefined);
}

void quiz_assert_conic_parameter_is(double observed, double expected) {
  // Observed should be exactly 0 if expected is null.
  quiz_assert(roughly_equal(observed, expected, 1e-5, false, 0.0));
}

void quiz_assert_circle(const char * expression,
                        double radius,
                        double cx = 0.0,
                        double cy = 0.0) {
  CartesianConic conic = getConicFromExpression(expression);
  quiz_assert(conic.conicType().shape == Conic::Shape::Circle);
  double x, y;
  conic.getCenter(&x, &y);
  quiz_assert_conic_parameter_is(x, cx);
  quiz_assert_conic_parameter_is(y, cy);
  quiz_assert_conic_parameter_is(conic.getEccentricity(), 0.0);
  quiz_assert_conic_parameter_is(conic.getRadius(), radius);
}

void quiz_assert_ellipse(const char * expression,
                         double eccentricity,
                         double linearEccentricity,
                         double semiMajorAxis,
                         double semiMinorAxis,
                         double cx = 0.0,
                         double cy = 0.0) {
  CartesianConic conic = getConicFromExpression(expression);
  quiz_assert(conic.conicType().shape == Conic::Shape::Ellipse);
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

void quiz_assert_hyperbola(const char * expression,
                           double eccentricity,
                           double linearEccentricity,
                           double semiMajorAxis,
                           double semiMinorAxis,
                           double cx = 0.0,
                           double cy = 0.0) {
  CartesianConic conic = getConicFromExpression(expression);
  quiz_assert(conic.conicType().shape == Conic::Shape::Hyperbola);
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

void quiz_assert_parabola(const char * expression,
                          double parameter,
                          double sx = 0.0,
                          double sy = 0.0) {
  CartesianConic conic = getConicFromExpression(expression);
  quiz_assert(conic.conicType().shape == Conic::Shape::Parabola);
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
  quiz_assert_undefined("3*y");
  quiz_assert_undefined("3*y+x");
  quiz_assert_undefined("y+x+1");
  quiz_assert_undefined("y^2+y+1");
  quiz_assert_undefined("y^2+x^2-ln(x)");
  quiz_assert_undefined("y^3+y^2+x^2");
  quiz_assert_undefined("x*y^2+x^2");
  quiz_assert_undefined("y^2+x^2+1");
  quiz_assert_undefined("y^2+x^2+x*y+x+y+1");
  quiz_assert_undefined("y^2+1");
  quiz_assert_undefined("y^2-x^2");
  quiz_assert_undefined("y^2+x^2");
  quiz_assert_undefined("(y-x-1)(y-x+1)");
  quiz_assert_undefined("y*(y-x)");
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a").destroy();
  quiz_assert_undefined("y^2-x^2+a");
  quiz_assert_undefined("x^2+y^2+x*y+x+y-5*10^70");
  quiz_assert_undefined("x^2+y^2+2*x*y+x+y");
}

QUIZ_CASE(poincare_conics_general) {
  quiz_assert_circle("x^2+y^2-x+y", 0.707107, 0.5, -0.5);  // 1/sqrt(2)
  quiz_assert_ellipse("x^2+y^2+x*y+x+y", 0.816497, 0.816497 * 0.816497,
                      0.816497, 0.471405, -0.333333, -0.333333);
  quiz_assert_ellipse("x^2+0.25*y^2-1", 0.866025, 0.866025 * 2.0, 2.0, 1.0);
  quiz_assert_hyperbola("x^2+2*x*y-4y^2+2x+y-16", 1.13334, 1.13334 * 3.76999,
                        3.76999, 2.01069, -0.9, -0.1);  // 0.946213
  quiz_assert_ellipse("3x^2-2x*y+4y^2+x-y-7", 0.695847, 0.695847 * 1.72814,
                      1.72814, 1.24113, -0.136364, 0.0909091);  // 1.28098
  quiz_assert_parabola("x^2+2*x*y+y^2+2*x+y-1", 0.176777, 1.1875, -1.9375);
  quiz_assert_parabola("4x^2-4x*y+y^2-x-2*y+2", 0.223607, 0.4, 0.8);
  // With negative coefficients
  quiz_assert_circle("-x^2-y^2+x-y", 0.707107, 0.5, -0.5);  // 1/sqrt(2)
  quiz_assert_circle("x^2+y^2+10x+1", 4.89898, -5, 0.0);    // sqrt(77)/2
  quiz_assert_hyperbola("-x^2+10*x*y-y^2-1", 1.29099, 1.29099 * 0.5, 0.5,
                        0.408248, 0.0, 0.0);  // sqrt(5/3), 1/sqrt(6)
  quiz_assert_parabola("-x^2-2*x*y-y^2-2*x-y+1", 0.176777, 1.1875, -1.9375);
  // Try different amplitudes of coefficients
  quiz_assert_hyperbola("y^2+2*x*y+3", 1.17557, 1.17557 * 2.2032, 2.2032,
                        1.36165, 0.0, 0.0);
  quiz_assert_ellipse("23*x^2+y^2-12", 0.978019, 0.978019 * 3.4641, 3.4641,
                      0.722315, 0.0, 0.0);
  quiz_assert_ellipse("(23x^2+y^2-12)*10^9", 0.978019, 0.978019 * 3.4641,
                      3.4641, 0.722315, 0.0, 0.0);
  quiz_assert_ellipse("(23x^2+y^2-12)*10^(-9)", 0.978019, 0.978019 * 3.4641,
                      3.4641, 0.722315, 0.0, 0.0);
  quiz_assert_hyperbola("x*y-1", 1.41421, 1.41421 * 1.41421, 1.41421, 1.41421,
                      0.0, 0.0);  // sqrt(2)
}

void quiz_assert_same_circle(const char * canonicForm, const char * offCenteredForm, double cx, double cy) {
  CartesianConic conic = getConicFromExpression(canonicForm);
  quiz_assert(conic.conicType().shape == Conic::Shape::Circle);
  quiz_assert_circle(offCenteredForm, conic.getRadius(), cx, cy);
}

void quiz_assert_same_ellipse(const char * canonicForm, const char * offCenteredForm, double cx, double cy) {
  CartesianConic conic = getConicFromExpression(canonicForm);
  quiz_assert(conic.conicType().shape == Conic::Shape::Ellipse);
  quiz_assert_ellipse(offCenteredForm, conic.getEccentricity(), conic.getLinearEccentricity(), conic.getSemiMajorAxis(), conic.getSemiMinorAxis(), cx, cy);
}

void quiz_assert_same_hyperbola(const char * canonicForm, const char * offCenteredForm, double cx, double cy) {
  CartesianConic conic = getConicFromExpression(canonicForm);
  quiz_assert(conic.conicType().shape == Conic::Shape::Hyperbola);
  quiz_assert_hyperbola(offCenteredForm, conic.getEccentricity(), conic.getLinearEccentricity(), conic.getSemiMajorAxis(), conic.getSemiMinorAxis(), cx, cy);
}

void quiz_assert_same_parabola(const char * canonicForm, const char * offCenteredForm, double cx, double cy) {
  CartesianConic conic = getConicFromExpression(canonicForm);
  quiz_assert(conic.conicType().shape == Conic::Shape::Parabola);
  quiz_assert_parabola(offCenteredForm, conic.getParameter(), cx, cy);
}

QUIZ_CASE(poincare_conics_reduction) {
  /* Check that off-centered (and rotated) conics have the same properties as
   * their canonic counter-parts. */
  const char * canonicForm;
  const char * offCenteredForm;
  canonicForm = "3*x^2 + 3*y^2 - 1";
  offCenteredForm = "(3*(cos(10)*(x-1)+sin(10)*(y+1))^2 + 3*(-sin(10)*(x-1)+cos(10)*(y+1))^2 - 1)*π";
  quiz_assert_same_circle(canonicForm, offCenteredForm, 1.0, -1.0);
  canonicForm = "3*x^2 + 5*y^2 - 1";
  offCenteredForm = "(3*(cos(20)*(x-2)+sin(20)*(y+2))^2 + 5*(-sin(20)*(x-2)+cos(20)*(y+2))^2 - 1)*100";
  quiz_assert_same_ellipse(canonicForm, offCenteredForm, 2.0, -2.0);
  canonicForm = "2*x^2 - 7*y^2 - 1";
  offCenteredForm = "(2*(cos(30)*(x-3)+sin(30)*(y+3))^2 - 7*(-sin(30)*(x-3)+cos(30)*(y+3))^2 - 1)*0.1";
  quiz_assert_same_hyperbola(canonicForm, offCenteredForm, 3.0, -3.0);
  canonicForm = "x^2 - π*y";
  offCenteredForm = "((cos(40)*(x-4)+sin(40)*(y+4))^2 - π*(-sin(40)*(x-4)+cos(40)*(y+4)))*root(2,3)";
  quiz_assert_same_parabola(canonicForm, offCenteredForm, 4.0, -4.0);
}
