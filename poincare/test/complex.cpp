#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_complex_evaluate) {
  // Real
  assert_parsed_expression_evaluates_to<float>("I", "unreal", System, Radian, Real);
  assert_parsed_expression_evaluates_to<double>("R(-1)", "unreal", System, Radian, Real);
  assert_parsed_expression_evaluates_to<double>("R(-1)*R(-1)", "unreal", System, Radian, Real);
  assert_parsed_expression_evaluates_to<double>("ln(-2)", "unreal", System, Radian, Real);
  assert_parsed_expression_evaluates_to<double>("(-8)^(1/3)", "-2", System, Radian, Real);
  assert_parsed_expression_evaluates_to<double>("8^(1/3)", "2", System, Radian, Real);
  assert_parsed_expression_evaluates_to<float>("(-8)^(2/3)", "4", System, Radian, Real);
  assert_parsed_expression_evaluates_without_simplifying_to<double>("root(-8,3)", "-2", Radian, Real);

  // Cartesian
  assert_parsed_expression_evaluates_to<float>("I", "I", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_to<double>("R(-1)", "I", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_to<double>("R(-1)*R(-1)", "-1", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_to<double>("ln(-2)", "6.9314718055995E-1+3.1415926535898*I", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_to<double>("(-8)^(1/3)", "1+1.7320508075689*I", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_to<float>("(-8)^(2/3)", "-2+3.464102*I", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_without_simplifying_to<double>("root(-8,3)", "1+1.7320508075689*I", Radian, Cartesian);

  // Polar
  assert_parsed_expression_evaluates_to<float>("I", "X^(1.570796*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("R(-1)", "X^(1.5707963267949*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("R(-1)*R(-1)", "X^(3.1415926535898*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("(-8)^(1/3)", "2*X^(1.0471975511966*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("(-8)^(2/3)", "4*X^(2.094395*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_without_simplifying_to<double>("root(-8,3)", "2*X^(1.0471975511966*I)", Radian, Polar);
}

QUIZ_CASE(poincare_complex_simplify) {
  // Real
  assert_parsed_expression_simplify_to("I", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("R(-1)", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("R(-1)*R(-1)", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("ln(-2)", "ln(-2)", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(2/3)", "4", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(2/5)", "2*root(2,5)", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/5)", "-root(8,5)", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/4)", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/3)", "-2", User, Radian, Real);

  // Cartesian
  assert_parsed_expression_simplify_to("-2.3E3", "-2300", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("3", "3", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("inf", "inf", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("1+2+I", "3+I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("-(5+2*I)", "-5-2*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(5+2*I)", "5+2*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("I+I", "2*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("-2+2*I", "-2+2*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+I)-(2+4*I)", "1-3*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(2+3*I)*(4-2*I)", "14+8*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+I)/2", "3/2+1/2*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+I)/(2+I)", "7/5-1/5*I", User, Radian, Cartesian);
  // The simplification of (3+I)^(2+I) in a Cartesian complex form generates to many nodes
  //assert_parsed_expression_simplify_to("(3+I)^(2+I)", "10*cos((-4*atan(3)+ln(2)+ln(5)+2*P)/2)*X^((2*atan(3)-P)/2)+10*sin((-4*atan(3)+ln(2)+ln(5)+2*P)/2)*X^((2*atan(3)-P)/2)*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+I)^(2+I)", "(I+3)^(I+2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("R(1+6I)", "R(2*R(37)+2)/2+R(2*R(37)-2)/2*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(1+I)^2", "2*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("2*I", "2*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("I!", "I!", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("3!", "6", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("x!", "x!", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("X", "X", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("P", "P", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("I", "I", User, Radian, Cartesian);

  assert_parsed_expression_simplify_to("abs(-3)", "3", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("abs(-3+I)", "R(10)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("atan(2)", "atan(2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("atan(2+I)", "atan(2+I)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("binomial(10, 4)", "210", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("arg(-2)", "P", User, Radian, Cartesian);
  // TODO: confidence is not simplified yet
  //assert_parsed_expression_simplify_to("confidence(-2,-3)", "confidence(-2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("conj(-2)", "-2", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("conj(-2+2*I+I)", "-2-3*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("cos(12)", "cos(12)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("cos(12+I)", "cos(12+I)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("diff(3*x, x, 3)", "diff(3*x,x,3)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("quo(34,x)", "quo(34,x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("rem(5,3)", "2", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("floor(x)", "floor(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("frac(x)", "frac(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("gcd(x,y)", "gcd(x,y)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("im(1+I)", "1", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("int(x^2, x, 1, 2)", "int(x^2,x,1,2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("lcm(x,y)", "lcm(x,y)", User, Radian, Cartesian);
  // TODO: dim is not simplified yet
  //assert_parsed_expression_simplify_to("dim(x)", "dim(x)", User, Radian, Cartesian);

  assert_parsed_expression_simplify_to("root(2,I)", "cos(ln(2))-sin(ln(2))*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("root(2,I+1)", "R(2)*cos((90*ln(2))/P)-R(2)*sin((90*ln(2))/P)*I", User, Degree, Cartesian);
  assert_parsed_expression_simplify_to("root(2,I+1)", "R(2)*cos(ln(2)/2)-R(2)*sin(ln(2)/2)*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("permute(10, 4)", "5040", User, Radian, Cartesian);
  // TODO: prediction is not simplified yet
  //assert_parsed_expression_simplify_to("prediction(-2,-3)", "prediction(-2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("randint(2,4)", "randint(2,4)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("random()", "random()", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("re(x)", "x", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("round(x,y)", "round(x,y)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("sign(x)", "sign(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("sin(23)", "sin(23)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("sin(23+I)", "sin(23+I)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("R(1-I)", "R(2*R(2)+2)/2-R(2*R(2)-2)/2*I", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("tan(23)", "tan(23)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("tan(23+I)", "tan(23+I)", User, Radian, Cartesian);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "a", User, Radian, Cartesian);
  // a = 2+i
  assert_simplify("2+I>a");
  assert_parsed_expression_simplify_to("a", "2+I", User, Radian, Cartesian);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_parsed_expression_simplify_to("f(3)", "f(3)", User, Radian, Cartesian);
  // f: x -> x+1
  assert_simplify("x+1+I>f(x)");
  assert_parsed_expression_simplify_to("f(3)", "4+I", User, Radian, Cartesian);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();

  // Polar
  assert_parsed_expression_simplify_to("-2.3E3", "2300*X^(P*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("3", "3", User, Radian, Polar);
  assert_parsed_expression_simplify_to("inf", "inf", User, Radian, Polar);
  assert_parsed_expression_simplify_to("1+2+I", "R(10)*X^((-2*atan(3)+P)/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("1+2+I", "R(10)*X^((-P*atan(3)+90*P)/180*I)", User, Degree, Polar);
  assert_parsed_expression_simplify_to("-(5+2*I)", "R(29)*X^((-2*atan(5/2)-P)/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(5+2*I)", "R(29)*X^((-2*atan(5/2)+P)/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("I+I", "2*X^(P/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("I+I", "2*X^(P/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("-2+2*I", "2*R(2)*X^((3*P)/4*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+I)-(2+4*I)", "R(10)*X^((2*atan(1/3)-P)/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(2+3*I)*(4-2*I)", "2*R(65)*X^((-2*atan(7/4)+P)/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+I)/2", "R(10)/2*X^((-2*atan(3)+P)/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+I)/(2+I)", "R(2)*X^((2*atan(7)-P)/2*I)", User, Radian, Polar);
  // TODO: simplify atan(tan(x)) = x±k*pi?
  //assert_parsed_expression_simplify_to("(3+I)^(2+I)", "10*X^((2*atan(3)-P)/2)*X^((-4*atan(3)+ln(2)+ln(5)+2*P)/2*I)", User, Radian, Polar);
  // The simplification of (3+I)^(2+I) in a Polar complex form generates to many nodes
  //assert_parsed_expression_simplify_to("(3+I)^(2+I)", "10*X^((2*atan(3)-P)/2)*X^((atan(tan((-4*atan(3)+ln(2)+ln(5)+2*P)/2))+P)*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+I)^(2+I)", "(I+3)^(I+2)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(1+I)^2", "2*X^(P/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("2*I", "2*X^(P/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("3!", "6", User, Radian, Polar);
  assert_parsed_expression_simplify_to("x!", "x!", User, Radian, Polar);
  assert_parsed_expression_simplify_to("X", "X", User, Radian, Polar);
  assert_parsed_expression_simplify_to("P", "P", User, Radian, Polar);
  assert_parsed_expression_simplify_to("I", "X^(P/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("abs(-3)", "3", User, Radian, Polar);
  assert_parsed_expression_simplify_to("abs(-3+I)", "R(10)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("conj(2*X^(I*P/2))", "2*X^(-P/2*I)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("-2*X^(I*P/2)", "2*X^(-P/2*I)", User, Radian, Polar);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "R(a^2)*X^((-P*sign(a)+P)/2*I)", User, Radian, Polar);
  // a = 2+i
  assert_simplify("2+I>a");
  assert_parsed_expression_simplify_to("a", "R(5)*X^((-2*atan(2)+P)/2*I)", User, Radian, Polar);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_parsed_expression_simplify_to("f(3)", "R(f(3)^2)*X^((-P*sign(f(3))+P)/2*I)", User, Radian, Polar);
  // f: x -> x+1
  assert_simplify("x+1+I>f(x)");
  assert_parsed_expression_simplify_to("f(3)", "R(17)*X^((-2*atan(4)+P)/2*I)", User, Radian, Polar);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
}
