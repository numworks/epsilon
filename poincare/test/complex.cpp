#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_complex_evaluate) {
  // Real
  assert_parsed_expression_evaluates_to<float>("𝐢", "unreal", System, Radian, Real);
  assert_parsed_expression_evaluates_to<double>("√(-1)", "unreal", System, Radian, Real);
  assert_parsed_expression_evaluates_to<double>("√(-1)×√(-1)", "unreal", System, Radian, Real);
  assert_parsed_expression_evaluates_to<double>("ln(-2)", "unreal", System, Radian, Real);
  assert_parsed_expression_evaluates_to<double>("(-8)^(1/3)", "-2", System, Radian, Real);
  assert_parsed_expression_evaluates_to<double>("8^(1/3)", "2", System, Radian, Real);
  assert_parsed_expression_evaluates_to<float>("(-8)^(2/3)", "4", System, Radian, Real);
  assert_parsed_expression_evaluates_without_simplifying_to<double>("root(-8,3)", "-2", Radian, Real);

  // Cartesian
  assert_parsed_expression_evaluates_to<float>("𝐢", "𝐢", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_to<double>("√(-1)", "𝐢", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_to<double>("√(-1)×√(-1)", "-1", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_to<double>("ln(-2)", "6.9314718055995ᴇ-1+3.1415926535898×𝐢", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_to<double>("(-8)^(1/3)", "1+1.7320508075689×𝐢", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_to<float>("(-8)^(2/3)", "-2+3.464102×𝐢", System, Radian, Cartesian);
  assert_parsed_expression_evaluates_without_simplifying_to<double>("root(-8,3)", "1+1.7320508075689×𝐢", Radian, Cartesian);

  // Polar
  assert_parsed_expression_evaluates_to<float>("𝐢", "ℯ^(1.570796×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("√(-1)", "ℯ^(1.5707963267949×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("√(-1)×√(-1)", "ℯ^(3.1415926535898×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("(-8)^(1/3)", "2×ℯ^(1.0471975511966×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("(-8)^(2/3)", "4×ℯ^(2.094395×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_without_simplifying_to<double>("root(-8,3)", "2×ℯ^(1.0471975511966×𝐢)", Radian, Polar);
}

QUIZ_CASE(poincare_complex_simplify) {
  // Real
  assert_parsed_expression_simplify_to("𝐢", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("√(-1)", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("√(-1)×√(-1)", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("ln(-2)", "ln(-2)", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(2/3)", "4", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(2/5)", "2×root(2,5)", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/5)", "-root(8,5)", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/4)", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/3)", "-2", User, Radian, Real);

  // Cartesian
  assert_parsed_expression_simplify_to("-2.3ᴇ3", "-2300", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("3", "3", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("inf", "inf", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("1+2+𝐢", "3+𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("-(5+2×𝐢)", "-5-2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(5+2×𝐢)", "5+2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("𝐢+𝐢", "2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("-2+2×𝐢", "-2+2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+𝐢)-(2+4×𝐢)", "1-3×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(2+3×𝐢)×(4-2×𝐢)", "14+8×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+𝐢)/2", "3/2+1/2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+𝐢)/(2+𝐢)", "7/5-1/5×𝐢", User, Radian, Cartesian);
  // The simplification of (3+𝐢)^(2+𝐢) in a Cartesian complex form generates to many nodes
  //assert_parsed_expression_simplify_to("(3+𝐢)^(2+𝐢)", "10×cos((-4×atan(3)+ln(2)+ln(5)+2×π)/2)×ℯ^((2×atan(3)-π)/2)+10×sin((-4×atan(3)+ln(2)+ln(5)+2×π)/2)×ℯ^((2×atan(3)-π)/2)×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+𝐢)^(2+𝐢)", "(𝐢+3)^(𝐢+2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("√(1+6𝐢)", "√(2×√(37)+2)/2+√(2×√(37)-2)/2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(1+𝐢)^2", "2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("2×𝐢", "2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("𝐢!", "𝐢!", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("3!", "6", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("x!", "x!", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("ℯ", "ℯ", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("π", "π", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("𝐢", "𝐢", User, Radian, Cartesian);

  assert_parsed_expression_simplify_to("abs(-3)", "3", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("abs(-3+𝐢)", "√(10)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("atan(2)", "atan(2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("atan(2+𝐢)", "atan(2+𝐢)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("binomial(10, 4)", "210", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("arg(-2)", "π", User, Radian, Cartesian);
  // TODO: confidence is not simplified yet
  //assert_parsed_expression_simplify_to("confidence(-2,-3)", "confidence(-2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("conj(-2)", "-2", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("conj(-2+2×𝐢+𝐢)", "-2-3×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("cos(12)", "cos(12)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("cos(12+𝐢)", "cos(12+𝐢)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("diff(3×x, x, 3)", "diff(3×x,x,3)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("quo(34,x)", "quo(34,x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("rem(5,3)", "2", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("floor(x)", "floor(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("frac(x)", "frac(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("gcd(x,y)", "gcd(x,y)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("im(1+𝐢)", "1", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("int(x^2, x, 1, 2)", "int(x^2,x,1,2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("lcm(x,y)", "lcm(x,y)", User, Radian, Cartesian);
  // TODO: dim is not simplified yet
  //assert_parsed_expression_simplify_to("dim(x)", "dim(x)", User, Radian, Cartesian);

  assert_parsed_expression_simplify_to("root(2,𝐢)", "cos(ln(2))-sin(ln(2))×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("root(2,𝐢+1)", "√(2)×cos((90×ln(2))/π)-√(2)×sin((90×ln(2))/π)×𝐢", User, Degree, Cartesian);
  assert_parsed_expression_simplify_to("root(2,𝐢+1)", "√(2)×cos(ln(2)/2)-√(2)×sin(ln(2)/2)×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("permute(10, 4)", "5040", User, Radian, Cartesian);
  // TODO: prediction is not simplified yet
  //assert_parsed_expression_simplify_to("prediction(-2,-3)", "prediction(-2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("randint(2,2)", "2", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("random()", "random()", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("re(x)", "re(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("round(x,y)", "round(x,y)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("sign(x)", "sign(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("sin(23)", "sin(23)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("sin(23+𝐢)", "sin(23+𝐢)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("√(1-𝐢)", "√(2×√(2)+2)/2-√(2×√(2)-2)/2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("tan(23)", "tan(23)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("tan(23+𝐢)", "tan(23+𝐢)", User, Radian, Cartesian);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "a", User, Radian, Cartesian);
  // a = 2+i
  assert_simplify("2+𝐢→a");
  assert_parsed_expression_simplify_to("a", "2+𝐢", User, Radian, Cartesian);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_parsed_expression_simplify_to("f(3)", "f(3)", User, Radian, Cartesian);
  // f : x → x+1
  assert_simplify("x+1+𝐢→f(x)");
  assert_parsed_expression_simplify_to("f(3)", "4+𝐢", User, Radian, Cartesian);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();

  // Polar
  assert_parsed_expression_simplify_to("-2.3ᴇ3", "2300×ℯ^(π×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("3", "3", User, Radian, Polar);
  assert_parsed_expression_simplify_to("inf", "inf", User, Radian, Polar);
  assert_parsed_expression_simplify_to("1+2+𝐢", "√(10)×ℯ^((-2×atan(3)+π)/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("1+2+𝐢", "√(10)×ℯ^((-π×atan(3)+90×π)/180×𝐢)", User, Degree, Polar);
  assert_parsed_expression_simplify_to("-(5+2×𝐢)", "√(29)×ℯ^((-2×atan(5/2)-π)/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(5+2×𝐢)", "√(29)×ℯ^((-2×atan(5/2)+π)/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("𝐢+𝐢", "2×ℯ^(π/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("𝐢+𝐢", "2×ℯ^(π/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("-2+2×𝐢", "2×√(2)×ℯ^((3×π)/4×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+𝐢)-(2+4×𝐢)", "√(10)×ℯ^((2×atan(1/3)-π)/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(2+3×𝐢)×(4-2×𝐢)", "2×√(65)×ℯ^((-2×atan(7/4)+π)/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+𝐢)/2", "√(10)/2×ℯ^((-2×atan(3)+π)/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+𝐢)/(2+𝐢)", "√(2)×ℯ^((2×atan(7)-π)/2×𝐢)", User, Radian, Polar);
  // TODO: simplify atan(tan(x)) = x±k×pi?
  //assert_parsed_expression_simplify_to("(3+𝐢)^(2+𝐢)", "10×ℯ^((2×atan(3)-π)/2)×ℯ^((-4×atan(3)+ln(2)+ln(5)+2×π)/2×𝐢)", User, Radian, Polar);
  // The simplification of (3+𝐢)^(2+𝐢) in a Polar complex form generates to many nodes
  //assert_parsed_expression_simplify_to("(3+𝐢)^(2+𝐢)", "10×ℯ^((2×atan(3)-π)/2)×ℯ^((atan(tan((-4×atan(3)+ln(2)+ln(5)+2×π)/2))+π)×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+𝐢)^(2+𝐢)", "(𝐢+3)^(𝐢+2)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(1+𝐢)^2", "2×ℯ^(π/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("2×𝐢", "2×ℯ^(π/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("3!", "6", User, Radian, Polar);
  assert_parsed_expression_simplify_to("x!", "x!", User, Radian, Polar);
  assert_parsed_expression_simplify_to("ℯ", "ℯ", User, Radian, Polar);
  assert_parsed_expression_simplify_to("π", "π", User, Radian, Polar);
  assert_parsed_expression_simplify_to("𝐢", "ℯ^(π/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("abs(-3)", "3", User, Radian, Polar);
  assert_parsed_expression_simplify_to("abs(-3+𝐢)", "√(10)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("conj(2×ℯ^(𝐢×π/2))", "2×ℯ^(-π/2×𝐢)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("-2×ℯ^(𝐢×π/2)", "2×ℯ^(-π/2×𝐢)", User, Radian, Polar);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "a", User, Radian, Polar);
  // a = 2 + 𝐢
  assert_simplify("2+𝐢→a");
  assert_parsed_expression_simplify_to("a", "√(5)×ℯ^((-2×atan(2)+π)/2×𝐢)", User, Radian, Polar);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_parsed_expression_simplify_to("f(3)", "f(3)", User, Radian, Polar);
  // f: x → x+1
  assert_simplify("x+1+𝐢→f(x)");
  assert_parsed_expression_simplify_to("f(3)", "√(17)×ℯ^((-2×atan(4)+π)/2×𝐢)", User, Radian, Polar);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
}
