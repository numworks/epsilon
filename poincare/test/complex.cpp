#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_complex_parts) {
  assert_parsed_expression_simplify_to("-2.3E3", "-2300", Radian, Cartesian);
  assert_parsed_expression_simplify_to("3", "3", Radian, Cartesian);
  assert_parsed_expression_simplify_to("inf", "inf", Radian, Cartesian);
  assert_parsed_expression_simplify_to("1+2+I", "3+I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("-(5+2*I)", "-5-2*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("(5+2*I)", "5+2*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("I+I", "2*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("-2+2*I", "-2+2*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+I)-(2+4*I)", "1-3*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("(2+3*I)*(4-2*I)", "14+8*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+I)/2", "3/2+1/2*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+I)/(2+I)", "7/5-1/5*I", Radian, Cartesian);
  // The simplification of (3+I)^(2+I) in a Cartesian complex form generates to many nodes
  //assert_parsed_expression_simplify_to("(3+I)^(2+I)", "10*cos((-4*atan(3)+ln(2)+ln(5)+2*P)/2)*X^((2*atan(3)-P)/2)+10*sin((-4*atan(3)+ln(2)+ln(5)+2*P)/2)*X^((2*atan(3)-P)/2)*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+I)^(2+I)", "(3+I)^(2+I)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("R(1+6I)", "R(2+2*R(37))/2+R(-2+2*R(37))/2*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("(1+I)^2", "2*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("2*I", "2*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("I!", "I!", Radian, Cartesian);
  assert_parsed_expression_simplify_to("3!", "6", Radian, Cartesian);
  assert_parsed_expression_simplify_to("x!", "x!", Radian, Cartesian);
  assert_parsed_expression_simplify_to("X", "X", Radian, Cartesian);
  assert_parsed_expression_simplify_to("P", "P", Radian, Cartesian);
  assert_parsed_expression_simplify_to("I", "I", Radian, Cartesian);

  assert_parsed_expression_simplify_to("abs(-3)", "3", Radian, Cartesian);
  assert_parsed_expression_simplify_to("abs(-3+I)", "R(10)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("atan(2)", "atan(2)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("atan(2+I)", "atan(2+I)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("binomial(10, 4)", "210", Radian, Cartesian);
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1", Radian, Cartesian);
  assert_parsed_expression_simplify_to("arg(-2)", "P", Radian, Cartesian);
  // TODO: confidence is not simplified yet
  //assert_parsed_expression_simplify_to("confidence(-2,-3)", "confidence(-2)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("conj(-2)", "-2", Radian, Cartesian);
  assert_parsed_expression_simplify_to("conj(-2+2*I+I)", "-2-3*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("cos(12)", "cos(12)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("cos(12+I)", "cos(12+I)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("diff(3*x, x, 3)", "diff(3*x,x,3)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("quo(34,x)", "quo(34,x)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("rem(5,3)", "2", Radian, Cartesian);
  assert_parsed_expression_simplify_to("floor(x)", "floor(x)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("frac(x)", "frac(x)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("gcd(x,y)", "gcd(x,y)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("im(1+I)", "1", Radian, Cartesian);
  assert_parsed_expression_simplify_to("int(x^2, x, 1, 2)", "int(x^2,x,1,2)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("lcm(x,y)", "lcm(x,y)", Radian, Cartesian);
  // TODO: dim is not simplified yet
  //assert_parsed_expression_simplify_to("dim(x)", "dim(x)", Radian, Cartesian);

  assert_parsed_expression_simplify_to("root(2,I)", "cos(ln(2))-sin(ln(2))*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("root(2,I+1)", "R(2)*cos((90*ln(2))/P)-R(2)*sin((90*ln(2))/P)*I", Degree, Cartesian);
  assert_parsed_expression_simplify_to("root(2,I+1)", "R(2)*cos(ln(2)/2)-R(2)*sin(ln(2)/2)*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("permute(10, 4)", "5040", Radian, Cartesian);
  // TODO: prediction is not simplified yet
  //assert_parsed_expression_simplify_to("prediction(-2,-3)", "prediction(-2)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("randint(2,4)", "randint(2,4)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("random()", "random()", Radian, Cartesian);
  assert_parsed_expression_simplify_to("re(x)", "x", Radian, Cartesian);
  assert_parsed_expression_simplify_to("round(x,y)", "round(x,y)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("sign(x)", "sign(x)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("sin(23)", "sin(23)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("sin(23+I)", "sin(23+I)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("R(1-I)", "R(2+2*R(2))/2-R(-2+2*R(2))/2*I", Radian, Cartesian);
  assert_parsed_expression_simplify_to("tan(23)", "tan(23)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("tan(23+I)", "tan(23+I)", Radian, Cartesian);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "a", Radian, Cartesian);
  // a = 2+i
  assert_simplify("2+I>a");
  assert_parsed_expression_simplify_to("a", "2+I", Radian, Cartesian);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_parsed_expression_simplify_to("f(3)", "f(3)", Radian, Cartesian);
  // f: x -> x+1
  assert_simplify("x+1+I>f(x)");
  assert_parsed_expression_simplify_to("f(3)", "4+I", Radian, Cartesian);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();

  assert_parsed_expression_simplify_to("-2.3E3", "2300*X^(P*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("3", "3", Radian, Polar);
  assert_parsed_expression_simplify_to("inf", "inf", Radian, Polar);
  assert_parsed_expression_simplify_to("1+2+I", "R(10)*X^((-2*atan(3)+P)/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("1+2+I", "R(10)*X^((90*P-atan(3)*P)/180*I)", Degree, Polar);
  assert_parsed_expression_simplify_to("-(5+2*I)", "R(29)*X^((-2*atan(5/2)-P)/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("(5+2*I)", "R(29)*X^((-2*atan(5/2)+P)/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("I+I", "2*X^(P/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("I+I", "2*X^(P/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("-2+2*I", "2*R(2)*X^((3*P)/4*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("(3+I)-(2+4*I)", "R(10)*X^((2*atan(1/3)-P)/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("(2+3*I)*(4-2*I)", "2*R(65)*X^((-2*atan(7/4)+P)/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("(3+I)/2", "R(10)/2*X^((-2*atan(3)+P)/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("(3+I)/(2+I)", "R(2)*X^((2*atan(7)-P)/2*I)", Radian, Polar);
  // TODO: simplify atan(tan(x)) = x±k*pi?
  //assert_parsed_expression_simplify_to("(3+I)^(2+I)", "10*X^((2*atan(3)-P)/2)*X^((-4*atan(3)+ln(2)+ln(5)+2*P)/2*I)", Radian, Polar);
  // The simplification of (3+I)^(2+I) in a Polar complex form generates to many nodes
  //assert_parsed_expression_simplify_to("(3+I)^(2+I)", "10*X^((2*atan(3)-P)/2)*X^((atan(tan((-4*atan(3)+ln(2)+ln(5)+2*P)/2))+P)*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("(3+I)^(2+I)", "(3+I)^(2+I)", Radian, Polar);
  assert_parsed_expression_simplify_to("(1+I)^2", "2*X^(P/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("2*I", "2*X^(P/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("3!", "6", Radian, Polar);
  assert_parsed_expression_simplify_to("x!", "x!", Radian, Polar);
  assert_parsed_expression_simplify_to("X", "X", Radian, Polar);
  assert_parsed_expression_simplify_to("P", "P", Radian, Polar);
  assert_parsed_expression_simplify_to("I", "X^(P/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("abs(-3)", "3", Radian, Polar);
  assert_parsed_expression_simplify_to("abs(-3+I)", "R(10)", Radian, Polar);
  assert_parsed_expression_simplify_to("conj(2*X^(I*P/2))", "2*X^(-P/2*I)", Radian, Polar);
  assert_parsed_expression_simplify_to("-2*X^(I*P/2)", "2*X^(-P/2*I)", Radian, Polar);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "R(a^2)*X^((P-sign(a)*P)/2*I)", Radian, Polar);
  // a = 2+i
  assert_simplify("2+I>a");
  assert_parsed_expression_simplify_to("a", "R(5)*X^((-2*atan(2)+P)/2*I)", Radian, Polar);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_parsed_expression_simplify_to("f(3)", "R(f(3)^2)*X^((P-sign(f(3))*P)/2*I)", Radian, Polar);
  // f: x -> x+1
  assert_simplify("x+1+I>f(x)");
  assert_parsed_expression_simplify_to("f(3)", "R(17)*X^((-2*atan(4)+P)/2*I)", Radian, Polar);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();

}
