#include <quiz.h>
#include <apps/shared/global_context.h>
#include <poincare/expression.h>
#include <cmath>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

template<typename T>
void assert_exp_is_bounded(Expression exp, T lowBound, T upBound, bool upBoundIncluded = false) {
  Shared::GlobalContext globalContext;
  T result = exp.approximateToScalar<T>(globalContext, Cartesian, Radian);
  quiz_assert(result >= lowBound);
  quiz_assert(result < upBound || (result == upBound && upBoundIncluded));
}

QUIZ_CASE(poincare_parse_function) {
  assert_parsed_expression_type("abs(-1)", ExpressionNode::Type::AbsoluteValue);
  assert_parsed_expression_type("binomial(10, 4)", ExpressionNode::Type::BinomialCoefficient);
  assert_parsed_expression_type("ceil(0.2)", ExpressionNode::Type::Ceiling);
  assert_parsed_expression_type("arg(2+𝐢)", ExpressionNode::Type::ComplexArgument);
  assert_parsed_expression_type("det([[1,2,3][4,5,6][7,8,9]])", ExpressionNode::Type::Determinant);
  assert_parsed_expression_type("diff(2×x, x, 2)", ExpressionNode::Type::Derivative);
  assert_parsed_expression_type("dim([[2]])", ExpressionNode::Type::MatrixDimension);
  assert_parsed_expression_type("confidence(0.1, 100)", ExpressionNode::Type::ConfidenceInterval);
  assert_parsed_expression_type("conj(2)", ExpressionNode::Type::Conjugate);
  assert_parsed_expression_type("factor(23/42)", ExpressionNode::Type::Factor);
  assert_parsed_expression_type("floor(2.3)", ExpressionNode::Type::Floor);
  assert_parsed_expression_type("frac(2.3)", ExpressionNode::Type::FracPart);
  assert_parsed_expression_type("gcd(2,3)", ExpressionNode::Type::GreatCommonDivisor);
  assert_parsed_expression_type("im(2+𝐢)", ExpressionNode::Type::ImaginaryPart);
  assert_parsed_expression_type("lcm(2,3)", ExpressionNode::Type::LeastCommonMultiple);
  assert_parsed_expression_type("int(x, x, 2, 3)", ExpressionNode::Type::Integral);
  assert_parsed_expression_type("inverse([[1,2,3][4,5,6][7,8,9]])", ExpressionNode::Type::MatrixInverse);
  assert_parsed_expression_type("ln(2)", ExpressionNode::Type::NaperianLogarithm);
  assert_parsed_expression_type("log(2)", ExpressionNode::Type::Logarithm);
  assert_parsed_expression_type("permute(10, 4)", ExpressionNode::Type::PermuteCoefficient);
  assert_parsed_expression_type("prediction(0.1, 100)", ExpressionNode::Type::ConfidenceInterval);
  assert_parsed_expression_type("prediction95(0.1, 100)", ExpressionNode::Type::PredictionInterval);
  assert_parsed_expression_type("product(y,y, 4, 10)", ExpressionNode::Type::Product);
  assert_parsed_expression_type("quo(29, 10)", ExpressionNode::Type::DivisionQuotient);

  assert_parsed_expression_type("random()", ExpressionNode::Type::Random);
  assert_parsed_expression_type("randint(1, 2)", ExpressionNode::Type::Randint);

  assert_parsed_expression_type("re(2+𝐢)", ExpressionNode::Type::RealPart);
  assert_parsed_expression_type("rem(29, 10)", ExpressionNode::Type::DivisionRemainder);
  assert_parsed_expression_type("root(2,3)", ExpressionNode::Type::NthRoot);
  assert_parsed_expression_type("√(2)", ExpressionNode::Type::SquareRoot);
  assert_parsed_expression_type("round(2,3)", ExpressionNode::Type::Round);
  assert_parsed_expression_type("sign(3)", ExpressionNode::Type::SignFunction);
  assert_parsed_expression_type("sum(n,n, 4, 10)", ExpressionNode::Type::Sum);
  assert_parsed_expression_type("trace([[1,2,3][4,5,6][7,8,9]])", ExpressionNode::Type::MatrixTrace);
  assert_parsed_expression_type("transpose([[1,2,3][4,5,6][7,8,9]])", ExpressionNode::Type::MatrixTranspose);
  assert_parsed_expression_type("6!", ExpressionNode::Type::Factorial);
}


QUIZ_CASE(poincare_function_evaluate) {
  assert_parsed_expression_evaluates_to<float>("abs(-1)", "1");
  assert_parsed_expression_evaluates_to<double>("abs(-1)", "1");

  assert_parsed_expression_evaluates_to<float>("abs(3+2𝐢)", "3.605551");
  assert_parsed_expression_evaluates_to<double>("abs(3+2𝐢)", "3.605551275464");

  assert_parsed_expression_evaluates_to<float>("abs([[1,-2][3,-4]])", "[[1,2][3,4]]");
  assert_parsed_expression_evaluates_to<double>("abs([[1,-2][3,-4]])", "[[1,2][3,4]]");

  assert_parsed_expression_evaluates_to<float>("abs([[3+2𝐢,3+4𝐢][5+2𝐢,3+2𝐢]])", "[[3.605551,5][5.385165,3.605551]]");
  assert_parsed_expression_evaluates_to<double>("abs([[3+2𝐢,3+4𝐢][5+2𝐢,3+2𝐢]])", "[[3.605551275464,5][5.3851648071345,3.605551275464]]");

  assert_parsed_expression_evaluates_to<float>("binomial(10, 4)", "210");
  assert_parsed_expression_evaluates_to<double>("binomial(10, 4)", "210");

  assert_parsed_expression_evaluates_to<float>("ceil(0.2)", "1");
  assert_parsed_expression_evaluates_to<double>("ceil(0.2)", "1");

  assert_parsed_expression_evaluates_to<float>("det([[1,23,3][4,5,6][7,8,9]])", "126", System, Degree, Cartesian, 6); // FIXME: the determinant computation is not precised enough to be displayed with 7 significant digits
  assert_parsed_expression_evaluates_to<double>("det([[1,23,3][4,5,6][7,8,9]])", "126");

  assert_parsed_expression_evaluates_to<float>("det([[𝐢,23-2𝐢,3×𝐢][4+𝐢,5×𝐢,6][7,8×𝐢+2,9]])", "126-231×𝐢", System, Degree, Cartesian, 6); // FIXME: the determinant computation is not precised enough to be displayed with 7 significant digits
  assert_parsed_expression_evaluates_to<double>("det([[𝐢,23-2𝐢,3×𝐢][4+𝐢,5×𝐢,6][7,8×𝐢+2,9]])", "126-231×𝐢");

  assert_parsed_expression_evaluates_to<float>("diff(2×x, x, 2)", "2");
  assert_parsed_expression_evaluates_to<double>("diff(2×x, x, 2)", "2");

  assert_parsed_expression_evaluates_to<float>("diff(2×TO^2, TO, 7)", "28");
  assert_parsed_expression_evaluates_to<double>("diff(2×TO^2, TO, 7)", "28");

  assert_parsed_expression_evaluates_to<float>("floor(2.3)", "2");
  assert_parsed_expression_evaluates_to<double>("floor(2.3)", "2");

  assert_parsed_expression_evaluates_to<float>("frac(2.3)", "0.3");
  assert_parsed_expression_evaluates_to<double>("frac(2.3)", "0.3");

  assert_parsed_expression_evaluates_to<float>("gcd(234,394)", "2");
  assert_parsed_expression_evaluates_to<double>("gcd(234,394)", "2");

  assert_parsed_expression_evaluates_to<float>("im(2+3𝐢)", "3");
  assert_parsed_expression_evaluates_to<double>("im(2+3𝐢)", "3");

  assert_parsed_expression_evaluates_to<float>("lcm(234,394)", "46098");
  assert_parsed_expression_evaluates_to<double>("lcm(234,394)", "46098");

  assert_parsed_expression_evaluates_to<float>("int(x,x, 1, 2)", "1.5");
  assert_parsed_expression_evaluates_to<double>("int(x,x, 1, 2)", "1.5");

  assert_parsed_expression_evaluates_to<float>("ln(2)", "0.6931472");
  assert_parsed_expression_evaluates_to<double>("ln(2)", "6.9314718055995ᴇ-1");

  assert_parsed_expression_evaluates_to<float>("log(2)", "0.30103");
  assert_parsed_expression_evaluates_to<double>("log(2)", "3.0102999566398ᴇ-1");

  assert_parsed_expression_evaluates_to<float>("permute(10, 4)", "5040");
  assert_parsed_expression_evaluates_to<double>("permute(10, 4)", "5040");

  assert_parsed_expression_evaluates_to<float>("product(n,n, 4, 10)", "604800");
  assert_parsed_expression_evaluates_to<double>("product(n,n, 4, 10)", "604800");

  assert_parsed_expression_evaluates_to<float>("quo(29, 10)", "2");
  assert_parsed_expression_evaluates_to<double>("quo(29, 10)", "2");

  assert_parsed_expression_evaluates_to<float>("re(2+𝐢)", "2");
  assert_parsed_expression_evaluates_to<double>("re(2+𝐢)", "2");

  assert_parsed_expression_evaluates_to<float>("rem(29, 10)", "9");
  assert_parsed_expression_evaluates_to<double>("rem(29, 10)", "9");
  assert_parsed_expression_evaluates_to<float>("root(2,3)", "1.259921");
  assert_parsed_expression_evaluates_to<double>("root(2,3)", "1.2599210498949");

  assert_parsed_expression_evaluates_to<float>("√(2)", "1.414214");
  assert_parsed_expression_evaluates_to<double>("√(2)", "1.4142135623731");

  assert_parsed_expression_evaluates_to<float>("√(-1)", "𝐢");
  assert_parsed_expression_evaluates_to<double>("√(-1)", "𝐢");

  assert_parsed_expression_evaluates_to<float>("sum(r,r, 4, 10)", "49");
  assert_parsed_expression_evaluates_to<double>("sum(k,k, 4, 10)", "49");

  assert_parsed_expression_evaluates_to<float>("trace([[1,2,3][4,5,6][7,8,9]])", "15");
  assert_parsed_expression_evaluates_to<double>("trace([[1,2,3][4,5,6][7,8,9]])", "15");

  assert_parsed_expression_evaluates_to<float>("confidence(0.1, 100)", "[[0,0.2]]");
  assert_parsed_expression_evaluates_to<double>("confidence(0.1, 100)", "[[0,0.2]]");

  assert_parsed_expression_evaluates_to<float>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");
  assert_parsed_expression_evaluates_to<double>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");

  assert_parsed_expression_evaluates_to<float>("conj(3+2×𝐢)", "3-2×𝐢");
  assert_parsed_expression_evaluates_to<double>("conj(3+2×𝐢)", "3-2×𝐢");

  assert_parsed_expression_evaluates_to<float>("factor(-23/4)", "-5.75");
  assert_parsed_expression_evaluates_to<double>("factor(-123/24)", "-5.125");

  assert_parsed_expression_evaluates_to<float>("inverse([[1,2,3][4,5,-6][7,8,9]])", "[[-1.2917,-0.083333,0.375][1.0833,0.16667,-0.25][0.041667,-0.083333,0.041667]]", System, Degree, Cartesian, 5); // inverse is not precise enough to display 7 significative digits
  assert_parsed_expression_evaluates_to<double>("inverse([[1,2,3][4,5,-6][7,8,9]])", "[[-1.2916666666667,-8.3333333333333ᴇ-2,0.375][1.0833333333333,1.6666666666667ᴇ-1,-0.25][4.1666666666667ᴇ-2,-8.3333333333333ᴇ-2,4.1666666666667ᴇ-2]]");
  assert_parsed_expression_evaluates_to<float>("inverse([[𝐢,23-2𝐢,3×𝐢][4+𝐢,5×𝐢,6][7,8×𝐢+2,9]])", "[[-0.0118-0.0455×𝐢,-0.5-0.727×𝐢,0.318+0.489×𝐢][0.0409+0.00364×𝐢,0.04-0.0218×𝐢,-0.0255+0.00091×𝐢][0.00334-0.00182×𝐢,0.361+0.535×𝐢,-0.13-0.358×𝐢]]", System, Degree, Cartesian, 3); // inverse is not precise enough to display 7 significative digits
  assert_parsed_expression_evaluates_to<double>("inverse([[𝐢,23-2𝐢,3×𝐢][4+𝐢,5×𝐢,6][7,8×𝐢+2,9]])", "[[-0.0118289353958-0.0454959053685×𝐢,-0.500454959054-0.727024567789×𝐢,0.31847133758+0.488626023658×𝐢][0.0409463148317+3.63967242948ᴇ-3×𝐢,0.0400363967243-0.0218380345769×𝐢,-0.0254777070064+9.0991810737ᴇ-4×𝐢][3.33636639369ᴇ-3-1.81983621474ᴇ-3×𝐢,0.36093418259+0.534728541098×𝐢,-0.130118289354-0.357597816197×𝐢]]", System, Degree, Cartesian, 12); // FIXME: inverse is not precise enough to display 14 significative digits

  assert_parsed_expression_evaluates_to<float>("prediction(0.1, 100)", "[[0,0.2]]");
  assert_parsed_expression_evaluates_to<double>("prediction(0.1, 100)", "[[0,0.2]]");

  assert_parsed_expression_evaluates_to<float>("prediction95(0.1, 100)", "[[0.0412,0.1588]]");
  assert_parsed_expression_evaluates_to<double>("prediction95(0.1, 100)", "[[0.0412,0.1588]]");

  assert_parsed_expression_evaluates_to<float>("product(2+k×𝐢,k, 1, 5)", "-100-540×𝐢");
  assert_parsed_expression_evaluates_to<double>("product(2+o×𝐢,o, 1, 5)", "-100-540×𝐢");

  assert_parsed_expression_evaluates_to<float>("root(3+𝐢, 3)", "1.459366+0.1571201×𝐢");
  assert_parsed_expression_evaluates_to<double>("root(3+𝐢, 3)", "1.4593656008684+1.5712012294394ᴇ-1×𝐢");

  assert_parsed_expression_evaluates_to<float>("root(3, 3+𝐢)", "1.382007-0.1524428×𝐢");
  assert_parsed_expression_evaluates_to<double>("root(3, 3+𝐢)", "1.3820069623326-0.1524427794159×𝐢");

  assert_parsed_expression_evaluates_to<float>("root(5^((-𝐢)3^9),𝐢)", "3.504", System, Degree, Cartesian, 4);
  assert_parsed_expression_evaluates_to<double>("root(5^((-𝐢)3^9),𝐢)", "3.5039410843", System, Degree, Cartesian, 11);

  assert_parsed_expression_evaluates_to<float>("√(3+𝐢)", "1.755317+0.2848488×𝐢");
  assert_parsed_expression_evaluates_to<double>("√(3+𝐢)", "1.7553173018244+2.8484878459314ᴇ-1×𝐢");

  assert_parsed_expression_evaluates_to<float>("sign(-23+1)", "-1");
  assert_parsed_expression_evaluates_to<float>("sign(inf)", "1");
  assert_parsed_expression_evaluates_to<float>("sign(-inf)", "-1");
  assert_parsed_expression_evaluates_to<float>("sign(0)", "0");
  assert_parsed_expression_evaluates_to<float>("sign(-0)", "0");
  assert_parsed_expression_evaluates_to<float>("sign(x)", "undef");
  assert_parsed_expression_evaluates_to<double>("sign(2+𝐢)", "undef");
  assert_parsed_expression_evaluates_to<double>("sign(undef)", "undef");

  assert_parsed_expression_evaluates_to<double>("sum(2+n×𝐢,n,1,5)", "10+15×𝐢");
  assert_parsed_expression_evaluates_to<double>("sum(2+n×𝐢,n,1,5)", "10+15×𝐢");

  assert_parsed_expression_evaluates_to<float>("transpose([[1,2,3][4,5,-6][7,8,9]])", "[[1,4,7][2,5,8][3,-6,9]]");
  assert_parsed_expression_evaluates_to<float>("transpose([[1,7,5][4,2,8]])", "[[1,4][7,2][5,8]]");
  assert_parsed_expression_evaluates_to<float>("transpose([[1,2][4,5][7,8]])", "[[1,4,7][2,5,8]]");
  assert_parsed_expression_evaluates_to<double>("transpose([[1,2,3][4,5,-6][7,8,9]])", "[[1,4,7][2,5,8][3,-6,9]]");
  assert_parsed_expression_evaluates_to<double>("transpose([[1,7,5][4,2,8]])", "[[1,4][7,2][5,8]]");
  assert_parsed_expression_evaluates_to<double>("transpose([[1,2][4,5][7,8]])", "[[1,4,7][2,5,8]]");

  assert_parsed_expression_evaluates_to<float>("round(2.3246,3)", "2.325");
  assert_parsed_expression_evaluates_to<double>("round(2.3245,3)", "2.325");

  assert_parsed_expression_evaluates_to<float>("6!", "720");
  assert_parsed_expression_evaluates_to<double>("6!", "720");

  assert_parsed_expression_evaluates_to<float>("√(-1)", "𝐢");
  assert_parsed_expression_evaluates_to<double>("√(-1)", "𝐢");

  assert_parsed_expression_evaluates_to<float>("root(-1,3)", "0.5+0.8660254×𝐢");
  assert_parsed_expression_evaluates_to<double>("root(-1,3)", "0.5+8.6602540378444ᴇ-1×𝐢");

  assert_parsed_expression_evaluates_to<float>("int(int(x×x,x,0,x),x,0,4)", "21.33333");
  assert_parsed_expression_evaluates_to<double>("int(int(x×x,x,0,x),x,0,4)", "21.333333333333");

  assert_parsed_expression_evaluates_to<float>("int(1+cos(e),e, 0, 180)", "180");
  assert_parsed_expression_evaluates_to<double>("int(1+cos(e),e, 0, 180)", "180");

  Expression exp = parse_expression("random()");
  assert_exp_is_bounded(exp, 0.0f, 1.0f);
  assert_exp_is_bounded(exp, 0.0, 1.0);

  exp = parse_expression("randint(4,45)");
  assert_exp_is_bounded(exp, 4.0f, 45.0f, true);
  assert_exp_is_bounded(exp, 4.0, 45.0, true);
}

QUIZ_CASE(poincare_function_simplify) {
  assert_parsed_expression_simplify_to("abs(π)", "π");
  assert_parsed_expression_simplify_to("abs(-π)", "π");
  assert_parsed_expression_simplify_to("abs(1+𝐢)", "√(2)");
  assert_parsed_expression_simplify_to("abs(0)", "0");
  assert_parsed_expression_simplify_to("arg(1+𝐢)", "π/4");
  assert_parsed_expression_simplify_to("binomial(20,3)", "1140");
  assert_parsed_expression_simplify_to("binomial(20,10)", "184756");
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1");
  assert_parsed_expression_simplify_to("conj(1/2)", "1/2");
  assert_parsed_expression_simplify_to("quo(19,3)", "6");
  assert_parsed_expression_simplify_to("quo(19,0)", Infinity::Name());
  assert_parsed_expression_simplify_to("quo(-19,3)", "-7");
  assert_parsed_expression_simplify_to("rem(19,3)", "1");
  assert_parsed_expression_simplify_to("rem(-19,3)", "2");
  assert_parsed_expression_simplify_to("rem(19,0)", Infinity::Name());
  assert_parsed_expression_simplify_to("99!", "933262154439441526816992388562667004907159682643816214685929638952175999932299156089414639761565182862536979208272237582511852109168640000000000000000000000");
  assert_parsed_expression_simplify_to("factor(-10008/6895)", "-(2^3×3^2×139)/(5×7×197)");
  assert_parsed_expression_simplify_to("factor(1008/6895)", "(2^4×3^2)/(5×197)");
  assert_parsed_expression_simplify_to("factor(10007)", "10007");
  assert_parsed_expression_simplify_to("factor(10007^2)", Undefined::Name());
  assert_parsed_expression_simplify_to("floor(-1.3)", "-2");
  assert_parsed_expression_simplify_to("frac(-1.3)", "7/10");
  assert_parsed_expression_simplify_to("gcd(123,278)", "1");
  assert_parsed_expression_simplify_to("gcd(11,121)", "11");
  assert_parsed_expression_simplify_to("im(1+5×𝐢)", "5");
  assert_parsed_expression_simplify_to("lcm(123,278)", "34194");
  assert_parsed_expression_simplify_to("lcm(11,121)", "121");
  assert_parsed_expression_simplify_to("√(4)", "2");
  assert_parsed_expression_simplify_to("re(1+5×𝐢)", "1");
  assert_parsed_expression_simplify_to("root(4,3)", "root(4,3)");
  assert_parsed_expression_simplify_to("root(4,π)", "4^(1/π)");
  assert_parsed_expression_simplify_to("root(27,3)", "3");
  assert_parsed_expression_simplify_to("round(4.235,2)", "106/25");
  assert_parsed_expression_simplify_to("round(4.23,0)", "4");
  assert_parsed_expression_simplify_to("round(4.9,0)", "5");
  assert_parsed_expression_simplify_to("round(12.9,-1)", "10");
  assert_parsed_expression_simplify_to("round(12.9,-2)", "0");
  assert_parsed_expression_simplify_to("sign(-23)", "-1");
  assert_parsed_expression_simplify_to("sign(-𝐢)", "sign(-𝐢)");
  assert_parsed_expression_simplify_to("sign(0)", "0");
  assert_parsed_expression_simplify_to("sign(inf)", "1");
  assert_parsed_expression_simplify_to("sign(-inf)", "-1");
  assert_parsed_expression_simplify_to("sign(undef)", "undef");
  assert_parsed_expression_simplify_to("sign(23)", "1");
  assert_parsed_expression_simplify_to("sign(log(18))", "1");
  assert_parsed_expression_simplify_to("sign(-√(2))", "-1");
  assert_parsed_expression_simplify_to("sign(x)", "sign(x)");
  assert_parsed_expression_simplify_to("sign(2+𝐢)", "sign(2+𝐢)");
  assert_parsed_expression_simplify_to("permute(99,4)", "90345024");
  assert_parsed_expression_simplify_to("permute(20,-10)", Undefined::Name());
  assert_parsed_expression_simplify_to("re(1/2)", "1/2");
}
