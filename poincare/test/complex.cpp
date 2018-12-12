#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

void assert_expression_has_complex_cartesian_parts(const char * expression, const char * real, const char * imag, Preferences::AngleUnit angleUnit = Degree) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression).reduce(context, angleUnit);
  ComplexCartesian cartesian = e.complexCartesian(context, angleUnit);
  if (cartesian.isUninitialized()) {
    assert(real == nullptr);
    assert(imag == nullptr);
  } else {
    cartesian.real().simplify(context, angleUnit);
    cartesian.imag().simplify(context, angleUnit);
    assert_parsed_expression_serialize_to(cartesian.real(), real);
    assert_parsed_expression_serialize_to(cartesian.imag(), imag);
  }
}

void assert_expression_has_complex_polar_parts(const char * expression, const char * norm, const char * arg, Preferences::AngleUnit angleUnit = Degree) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression).simplify(context, angleUnit);
  ComplexPolar polar = e.complexPolar(context, angleUnit);
  if (polar.isUninitialized()) {
    assert(norm == nullptr);
    assert(arg == nullptr);
  } else {
    polar.norm().simplify(context, angleUnit);
    polar.arg().simplify(context, angleUnit);
    assert_parsed_expression_serialize_to(polar.norm(), norm);
    assert_parsed_expression_serialize_to(polar.arg(), arg);
  }
}

QUIZ_CASE(poincare_complex_parts) {
  assert_expression_has_complex_cartesian_parts("-2.3E3", "-2300", "0");
  assert_expression_has_complex_cartesian_parts("3", "3", "0");
  assert_expression_has_complex_cartesian_parts("inf", "inf", "0");
  assert_expression_has_complex_cartesian_parts("1+2+I", "3", "1");
  assert_expression_has_complex_cartesian_parts("-(5+2*I)", "-5", "-2");
  assert_expression_has_complex_cartesian_parts("(5+2*I)", "5", "2");
  assert_expression_has_complex_cartesian_parts("I+I", "0", "2");
  assert_expression_has_complex_cartesian_parts("-2+2*I", "-2", "2");
  assert_expression_has_complex_cartesian_parts("(3+I)-(2+4*I)", "1", "-3");
  assert_expression_has_complex_cartesian_parts("(2+3*I)*(4-2*I)", "14", "8");
  assert_expression_has_complex_cartesian_parts("(3+I)/2", "3/2", "1/2");
  assert_expression_has_complex_cartesian_parts("(3+I)/(2+I)", "7/5", "-1/5");
  assert_expression_has_complex_cartesian_parts("(3+I)^(2+I)", "10*cos((-4*atan(3)+ln(2)+ln(5)+2*P)/2)*X^((2*atan(3)-P)/2)", "10*sin((-4*atan(3)+ln(2)+ln(5)+2*P)/2)*X^((2*atan(3)-P)/2)", Radian);
  assert_expression_has_complex_cartesian_parts("R(1+6I)", "R(2+2*R(37))/2", "R(-2+2*R(37))/2");
  assert_expression_has_complex_cartesian_parts("(1+I)^2", "0", "2");
  assert_expression_has_complex_cartesian_parts("2*I", "0", "2");
  assert_expression_has_complex_cartesian_parts("I!", "undef", "0");
  assert_expression_has_complex_cartesian_parts("3!", "6", "0");
  assert_expression_has_complex_cartesian_parts("x!", "x!", "0");
  assert_expression_has_complex_cartesian_parts("X", "X", "0");
  assert_expression_has_complex_cartesian_parts("P", "P", "0");
  assert_expression_has_complex_cartesian_parts("I", "0", "1");

  assert_expression_has_complex_cartesian_parts("abs(-3)", "3", "0");
  assert_expression_has_complex_cartesian_parts("abs(-3+I)", "R(10)", "0");
  assert_expression_has_complex_cartesian_parts("atan(2)", "atan(2)", "0");
  assert_expression_has_complex_cartesian_parts("atan(2+I)", nullptr, nullptr);
  assert_expression_has_complex_cartesian_parts("binomial(10, 4)", "210", "0");
  assert_expression_has_complex_cartesian_parts("ceil(-1.3)", "-1", "0");
  assert_expression_has_complex_cartesian_parts("arg(-2)", "P", "0");
  // TODO: confidence is not simplified yet
  //assert_expression_has_complex_cartesian_parts("confidence(-2,-3)", "confidence(-2)", "0");
  assert_expression_has_complex_cartesian_parts("conj(-2)", "-2", "0");
  assert_expression_has_complex_cartesian_parts("conj(-2+2*I+I)", "-2", "-3");
  assert_expression_has_complex_cartesian_parts("cos(12)", "cos(12)", "0");
  assert_expression_has_complex_cartesian_parts("cos(12+I)", nullptr, nullptr);
  assert_expression_has_complex_cartesian_parts("diff(3*x, x, 3)", "diff(3*x,x,3)", "0");
  assert_expression_has_complex_cartesian_parts("quo(34,x)", "quo(34,x)", "0");
  assert_expression_has_complex_cartesian_parts("rem(5,3)", "2", "0");
  assert_expression_has_complex_cartesian_parts("floor(x)", "floor(x)", "0");
  assert_expression_has_complex_cartesian_parts("frac(x)", "frac(x)", "0");
  assert_expression_has_complex_cartesian_parts("gcd(x,y)", "gcd(x,y)", "0");
  assert_expression_has_complex_cartesian_parts("im(1+I)", "1", "0");
  assert_expression_has_complex_cartesian_parts("int(x^2, x, 1, 2)", "int(x^2,x,1,2)", "0");
  assert_expression_has_complex_cartesian_parts("lcm(x,y)", "lcm(x,y)", "0");
  // TODO: dim is not simplified yet
  //assert_expression_has_complex_cartesian_parts("dim(x)", "dim(x)", "0");

  assert_expression_has_complex_cartesian_parts("root(2,I)", "cos(ln(2))", "-sin(ln(2))", Radian);
  assert_expression_has_complex_cartesian_parts("root(2,I+1)", "R(2)*cos((90*ln(2))/P)", "-R(2)*sin((90*ln(2))/P)");
  assert_expression_has_complex_cartesian_parts("root(2,I+1)", "R(2)*cos(ln(2)/2)", "-R(2)*sin(ln(2)/2)", Radian);
  assert_expression_has_complex_cartesian_parts("permute(10, 4)", "5040", "0");
  // TODO: prediction is not simplified yet
  //assert_expression_has_complex_cartesian_parts("prediction(-2,-3)", "prediction(-2)", "0");
  assert_expression_has_complex_cartesian_parts("randint(2,4)", "randint(2,4)", "0");
  assert_expression_has_complex_cartesian_parts("random()", "random()", "0");
  assert_expression_has_complex_cartesian_parts("re(x)", "x", "0");
  assert_expression_has_complex_cartesian_parts("round(x,y)", "round(x,y)", "0");
  assert_expression_has_complex_cartesian_parts("sign(x)", "sign(x)", "0");
  assert_expression_has_complex_cartesian_parts("sin(23)", "sin(23)", "0");
  assert_expression_has_complex_cartesian_parts("sin(23+I)", nullptr, nullptr);
  assert_expression_has_complex_cartesian_parts("R(1-I)", "R(2+2*R(2))/2", "-R(-2+2*R(2))/2");
  assert_expression_has_complex_cartesian_parts("tan(23)", "tan(23)", "0");
  assert_expression_has_complex_cartesian_parts("tan(23+I)", nullptr, nullptr);

  // User defined variable
  assert_expression_has_complex_cartesian_parts("a", "a", "0");
  // a = 2+i
  assert_simplify("2+I>a");
  assert_expression_has_complex_cartesian_parts("a", "2", "1");
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_expression_has_complex_cartesian_parts("f(3)", "f(3)", "0");
  // f: x -> x+1
  assert_simplify("x+1+I>f(x)");
  assert_expression_has_complex_cartesian_parts("f(3)", "4", "1");
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();

  assert_expression_has_complex_polar_parts("-2.3E3", "2300", "P");
  assert_expression_has_complex_polar_parts("3", "3", "0");
  assert_expression_has_complex_polar_parts("inf", "inf", "0");
  assert_expression_has_complex_polar_parts("1+2+I", "R(10)", "(-2*atan(3)+P)/2", Radian);
  assert_expression_has_complex_polar_parts("1+2+I", "R(10)", "(90*P-atan(3)*P)/180");
  assert_expression_has_complex_polar_parts("-(5+2*I)", "R(29)", "(-2*atan(5/2)-P)/2", Radian);
  assert_expression_has_complex_polar_parts("(5+2*I)", "R(29)", "(-2*atan(5/2)+P)/2", Radian);
  assert_expression_has_complex_polar_parts("I+I", "2", "P/2", Radian);
  assert_expression_has_complex_polar_parts("I+I", "2", "P/2");
  assert_expression_has_complex_polar_parts("-2+2*I", "2*R(2)", "(3*P)/4", Radian);
  assert_expression_has_complex_polar_parts("(3+I)-(2+4*I)", "R(10)", "(2*atan(1/3)-P)/2", Radian);
  assert_expression_has_complex_polar_parts("(2+3*I)*(4-2*I)", "2*R(65)", "(-2*atan(7/4)+P)/2", Radian);
  assert_expression_has_complex_polar_parts("(3+I)/2", "R(10)/2", "(-2*atan(3)+P)/2", Radian);
  assert_expression_has_complex_polar_parts("(3+I)/(2+I)", "R(2)", "(2*atan(7)-P)/2", Radian);
  assert_expression_has_complex_polar_parts("(3+I)^(2+I)", "10*X^((2*atan(3)-P)/2)", "(-4*atan(3)+ln(2)+ln(5)+2*P)/2", Radian);
  assert_expression_has_complex_polar_parts("(1+I)^2", "2", "P/2");
  assert_expression_has_complex_polar_parts("2*I", "2", "P/2");
  assert_expression_has_complex_polar_parts("3!", "6", "0");
  assert_expression_has_complex_polar_parts("x!", "x!", "0");
  assert_expression_has_complex_polar_parts("X", "X", "0");
  assert_expression_has_complex_polar_parts("P", "P", "0");
  assert_expression_has_complex_polar_parts("I", "1", "P/2");
  assert_expression_has_complex_polar_parts("abs(-3)", "3", "0");
  assert_expression_has_complex_polar_parts("abs(-3+I)", "R(10)", "0");
  assert_expression_has_complex_polar_parts("conj(2*X^(I*cos(2)))", "2", "-cos(2)");
  assert_expression_has_complex_polar_parts("-2*X^(I*cos(2))", "2", "cos(2)+P");

  // User defined variable
  assert_expression_has_complex_polar_parts("a", "R(a^2)", "(P-sign(a)*P)/2");
  // a = 2+i
  assert_simplify("2+I>a");
  assert_expression_has_complex_polar_parts("a", "R(5", "(-2*atan(2)+P)/2");
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_expression_has_complex_polar_parts("f(3)", "R(f(3)^2)", "(P-sign(f(3))*P)/2");
  // f: x -> x+1
  assert_simplify("x+1+I>f(x)");
  assert_expression_has_complex_polar_parts("f(3)", "R(17)", "(-2*atan(4)+P)/2");
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();

}
