#include <quiz.h>
#include <poincare.h>
#include <cmath>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

template<typename T>
void assert_exp_is_bounded(Expression * exp, T lowBound, T upBound, bool upBoundIncluded = false) {
  GlobalContext globalContext;
  T result = exp->approximateToScalar<T>(globalContext, Radian);
  assert(result >= lowBound);
  assert(result < upBound || (result == upBound && upBoundIncluded));
}

QUIZ_CASE(poincare_parse_function) {
  assert_parsed_expression_type("abs(-1)", Expression::Type::AbsoluteValue);
  assert_parsed_expression_type("arg(2+I)", Expression::Type::ComplexArgument);
  assert_parsed_expression_type("binomial(10, 4)", Expression::Type::BinomialCoefficient);
  assert_parsed_expression_type("ceil(0.2)", Expression::Type::Ceiling);
  assert_parsed_expression_type("diff(2*x, 2)", Expression::Type::Derivative);
#if MATRICES_ARE_DEFINED
  assert_parsed_expression_type("dim([[2]])", Expression::Type::MatrixDimension);
  assert_parsed_expression_type("det([[1,2,3][4,5,6][7,8,9]])", Expression::Type::Determinant);
#endif
  assert_parsed_expression_type("confidence(0.1, 100)", Expression::Type::ConfidenceInterval);
  assert_parsed_expression_type("conj(2)", Expression::Type::Conjugate);
  assert_parsed_expression_type("factor(23/42)", Expression::Type::Factor);
  assert_parsed_expression_type("floor(2.3)", Expression::Type::Floor);
  assert_parsed_expression_type("frac(2.3)", Expression::Type::FracPart);
  assert_parsed_expression_type("gcd(2,3)", Expression::Type::GreatCommonDivisor);
  assert_parsed_expression_type("im(2+I)", Expression::Type::ImaginaryPart);
  assert_parsed_expression_type("int(x, 2, 3)", Expression::Type::Integral);
#if MATRICES_ARE_DEFINED
  assert_parsed_expression_type("inverse([[1,2,3][4,5,6][7,8,9]])", Expression::Type::MatrixInverse);
#endif
  assert_parsed_expression_type("lcm(2,3)", Expression::Type::LeastCommonMultiple);
  assert_parsed_expression_type("ln(2)", Expression::Type::NaperianLogarithm);
  assert_parsed_expression_type("log(2)", Expression::Type::Logarithm);
  assert_parsed_expression_type("permute(10, 4)", Expression::Type::PermuteCoefficient);
  assert_parsed_expression_type("prediction(0.1, 100)", Expression::Type::ConfidenceInterval);
  assert_parsed_expression_type("prediction95(0.1, 100)", Expression::Type::PredictionInterval);
  assert_parsed_expression_type("product(n, 4, 10)", Expression::Type::Product);
  assert_parsed_expression_type("quo(29, 10)", Expression::Type::DivisionQuotient);
  assert_parsed_expression_type("random()", Expression::Type::Random);
  assert_parsed_expression_type("randint(1, 2)", Expression::Type::Randint);
  assert_parsed_expression_type("re(2+I)", Expression::Type::RealPart);
  assert_parsed_expression_type("rem(29, 10)", Expression::Type::DivisionRemainder);
  assert_parsed_expression_type("root(2,3)", Expression::Type::NthRoot);
  assert_parsed_expression_type("R(2)", Expression::Type::SquareRoot);
  assert_parsed_expression_type("round(2,3)", Expression::Type::Round);
  assert_parsed_expression_type("sum(n, 4, 10)", Expression::Type::Sum);
#if MATRICES_ARE_DEFINED
  assert_parsed_expression_type("trace([[1,2,3][4,5,6][7,8,9]])", Expression::Type::MatrixTrace);
  assert_parsed_expression_type("transpose([[1,2,3][4,5,6][7,8,9]])", Expression::Type::MatrixTranspose);
#endif
  assert_parsed_expression_type("6!", Expression::Type::Factorial);
}


QUIZ_CASE(poincare_function_evaluate) {
  assert_parsed_expression_evaluates_to<float>("abs(-1)", "1");
  assert_parsed_expression_evaluates_to<double>("abs(-1)", "1");

  assert_parsed_expression_evaluates_to<float>("abs(3+2I)", "3.605551");
  assert_parsed_expression_evaluates_to<double>("abs(3+2I)", "3.605551275464");

  assert_parsed_expression_evaluates_to<float>("abs([[1,-2][3,-4]])", "[[1,2][3,4]]");
  assert_parsed_expression_evaluates_to<double>("abs([[1,-2][3,-4]])", "[[1,2][3,4]]");

  assert_parsed_expression_evaluates_to<float>("abs([[3+2I,3+4I][5+2I,3+2I]])", "[[3.605551,5][5.385165,3.605551]]");
  assert_parsed_expression_evaluates_to<double>("abs([[3+2I,3+4I][5+2I,3+2I]])", "[[3.605551275464,5][5.3851648071345,3.605551275464]]");

  assert_parsed_expression_evaluates_to<float>("binomial(10, 4)", "210");
  assert_parsed_expression_evaluates_to<double>("binomial(10, 4)", "210");

  assert_parsed_expression_evaluates_to<float>("ceil(0.2)", "1");
  assert_parsed_expression_evaluates_to<double>("ceil(0.2)", "1");

  assert_parsed_expression_evaluates_to<float>("diff(2*x, 2)", "2");
  assert_parsed_expression_evaluates_to<double>("diff(2*x, 2)", "2");

#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("det([[1,23,3][4,5,6][7,8,9]])", "126", Degree, Cartesian, 6); // FIXME: the determinant computation is not precised enough to be displayed with 7 significant digits
  assert_parsed_expression_evaluates_to<double>("det([[1,23,3][4,5,6][7,8,9]])", "126");
  assert_parsed_expression_evaluates_to<float>("det([[I,23-2I,3*I][4+I,5*I,6][7,8*I+2,9]])", "126-231*I", Degree, Cartesian, 6); // FIXME: the determinant computation is not precised enough to be displayed with 7 significant digits
  assert_parsed_expression_evaluates_to<double>("det([[I,23-2I,3*I][4+I,5*I,6][7,8*I+2,9]])", "126-231*I");
#endif

  assert_parsed_expression_evaluates_to<float>("floor(2.3)", "2");
  assert_parsed_expression_evaluates_to<double>("floor(2.3)", "2");

  assert_parsed_expression_evaluates_to<float>("frac(2.3)", "0.3");
  assert_parsed_expression_evaluates_to<double>("frac(2.3)", "0.3");

  assert_parsed_expression_evaluates_to<float>("gcd(234,394)", "2");
  assert_parsed_expression_evaluates_to<double>("gcd(234,394)", "2");

  assert_parsed_expression_evaluates_to<float>("im(2+3I)", "3");
  assert_parsed_expression_evaluates_to<double>("im(2+3I)", "3");

  assert_parsed_expression_evaluates_to<float>("int(x, 1, 2)", "1.5");
  assert_parsed_expression_evaluates_to<double>("int(x, 1, 2)", "1.5");

  assert_parsed_expression_evaluates_to<float>("lcm(234,394)", "46098");
  assert_parsed_expression_evaluates_to<double>("lcm(234,394)", "46098");

  assert_parsed_expression_evaluates_to<float>("ln(2)", "0.6931472");
  assert_parsed_expression_evaluates_to<double>("ln(2)", "6.9314718055995E-1");

  assert_parsed_expression_evaluates_to<float>("log(2)", "0.30103");
  assert_parsed_expression_evaluates_to<double>("log(2)", "3.0102999566398E-1");

  assert_parsed_expression_evaluates_to<float>("permute(10, 4)", "5040");
  assert_parsed_expression_evaluates_to<double>("permute(10, 4)", "5040");

  assert_parsed_expression_evaluates_to<float>("product(n, 4, 10)", "604800");
  assert_parsed_expression_evaluates_to<double>("product(n, 4, 10)", "604800");

  assert_parsed_expression_evaluates_to<float>("re(2+I)", "2");
  assert_parsed_expression_evaluates_to<double>("re(2+I)", "2");

  assert_parsed_expression_evaluates_to<float>("rem(29, 10)", "9");
  assert_parsed_expression_evaluates_to<double>("rem(29, 10)", "9");

  assert_parsed_expression_evaluates_to<float>("root(2,3)", "1.259921");
  assert_parsed_expression_evaluates_to<double>("root(2,3)", "1.2599210498949");

  assert_parsed_expression_evaluates_to<float>("R(2)", "1.414214");
  assert_parsed_expression_evaluates_to<double>("R(2)", "1.4142135623731");

  assert_parsed_expression_evaluates_to<float>("R(-1)", "I");
  assert_parsed_expression_evaluates_to<double>("R(-1)", "I");

  assert_parsed_expression_evaluates_to<float>("sum(n, 4, 10)", "49");
  assert_parsed_expression_evaluates_to<double>("sum(n, 4, 10)", "49");

#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("trace([[1,2,3][4,5,6][7,8,9]])", "15");
  assert_parsed_expression_evaluates_to<double>("trace([[1,2,3][4,5,6][7,8,9]])", "15");
#endif

  assert_parsed_expression_evaluates_to<float>("confidence(0.1, 100)", "[[0,0.2]]");
  assert_parsed_expression_evaluates_to<double>("confidence(0.1, 100)", "[[0,0.2]]");

#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");
  assert_parsed_expression_evaluates_to<double>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");
#endif

  assert_parsed_expression_evaluates_to<float>("conj(3+2*I)", "3-2*I");
  assert_parsed_expression_evaluates_to<double>("conj(3+2*I)", "3-2*I");

#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("inverse([[1,2,3][4,5,-6][7,8,9]])", "[[-1.2917,-0.083333,0.375][1.0833,0.16667,-0.25][0.041667,-0.083333,0.041667]]", Degree, Cartesian, 5); // inverse is not precise enough to display 7 significative digits
  assert_parsed_expression_evaluates_to<double>("inverse([[1,2,3][4,5,-6][7,8,9]])", "[[-1.2916666666667,-8.3333333333333E-2,0.375][1.0833333333333,1.6666666666667E-1,-0.25][4.1666666666667E-2,-8.3333333333333E-2,4.1666666666667E-2]]");
  assert_parsed_expression_evaluates_to<float>("inverse([[I,23-2I,3*I][4+I,5*I,6][7,8*I+2,9]])", "[[(-0.011829)-0.045496*I,(-0.50045)-0.72702*I,0.31847+0.48863*I][0.040946+0.0036397*I,0.040036-0.021838*I,(-0.025478)+0.00090992*I][0.0033364-0.0018198*I,0.36093+0.53473*I,(-0.13012)-0.3576*I]]", Degree, Cartesian, 5); // inverse is not precise enough to display 7 significative digits
  assert_parsed_expression_evaluates_to<double>("inverse([[I,23-2I,3*I][4+I,5*I,6][7,8*I+2,9]])", "[[(-1.182893539581E-2)-4.549590536852E-2*I,(-0.5004549590537)-0.7270245677889*I,0.3184713375796+0.4886260236579*I][4.094631483167E-2+3.639672429481E-3*I,4.003639672429E-2-2.183803457689E-2*I,(-2.547770700637E-2)+9.099181073703E-4*I][3.336366393691E-3-1.819836214741E-3*I,0.3609341825902+0.534728541098*I,(-0.130118289354)-0.3575978161965*I]]", Degree, Cartesian, 13); // FIXME: inverse is not precise enough to display 14 significative digits

#endif

  assert_parsed_expression_evaluates_to<float>("prediction(0.1, 100)", "[[0,0.2]]");
  assert_parsed_expression_evaluates_to<double>("prediction(0.1, 100)", "[[0,0.2]]");

  assert_parsed_expression_evaluates_to<float>("prediction95(0.1, 100)", "[[0.0412,0.1588]]");
  assert_parsed_expression_evaluates_to<double>("prediction95(0.1, 100)", "[[0.0412,0.1588]]");

  assert_parsed_expression_evaluates_to<float>("product(2+n*I, 1, 5)", "(-100)-540*I");
  assert_parsed_expression_evaluates_to<double>("product(2+n*I, 1, 5)", "(-100)-540*I");

  assert_parsed_expression_evaluates_to<float>("root(3+I, 3)", "1.459366+0.1571201*I");
  assert_parsed_expression_evaluates_to<double>("root(3+I, 3)", "1.4593656008684+1.5712012294394E-1*I");

  assert_parsed_expression_evaluates_to<float>("root(3, 3+I)", "1.382007-0.1524428*I");
  assert_parsed_expression_evaluates_to<double>("root(3, 3+I)", "1.3820069623326-0.1524427794159*I");

  assert_parsed_expression_evaluates_to<float>("root(5^(-I)3^9,I)", "3.504", Degree, Cartesian, 4);
  assert_parsed_expression_evaluates_to<double>("root(5^(-I)3^9,I)", "3.5039410843", Degree, Cartesian, 11);

  assert_parsed_expression_evaluates_to<float>("R(3+I)", "1.755317+0.2848488*I");
  assert_parsed_expression_evaluates_to<double>("R(3+I)", "1.7553173018244+2.8484878459314E-1*I");

  assert_parsed_expression_evaluates_to<double>("sum(2+n*I,1,5)", "10+15*I");
  assert_parsed_expression_evaluates_to<double>("sum(2+n*I,1,5)", "10+15*I");
#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("transpose([[1,2,3][4,5,-6][7,8,9]])", "[[1,4,7][2,5,8][3,-6,9]]");
  assert_parsed_expression_evaluates_to<float>("transpose([[1,7,5][4,2,8]])", "[[1,4][7,2][5,8]]");
  assert_parsed_expression_evaluates_to<float>("transpose([[1,2][4,5][7,8]])", "[[1,4,7][2,5,8]]");
  assert_parsed_expression_evaluates_to<double>("transpose([[1,2,3][4,5,-6][7,8,9]])", "[[1,4,7][2,5,8][3,-6,9]]");
  assert_parsed_expression_evaluates_to<double>("transpose([[1,7,5][4,2,8]])", "[[1,4][7,2][5,8]]");
  assert_parsed_expression_evaluates_to<double>("transpose([[1,2][4,5][7,8]])", "[[1,4,7][2,5,8]]");
#endif

  assert_parsed_expression_evaluates_to<float>("round(2.3246,3)", "2.325");
  assert_parsed_expression_evaluates_to<double>("round(2.3245,3)", "2.325");

  assert_parsed_expression_evaluates_to<float>("6!", "720");
  assert_parsed_expression_evaluates_to<double>("6!", "720");

  assert_parsed_expression_evaluates_to<float>("R(-1)", "I");
  assert_parsed_expression_evaluates_to<double>("R(-1)", "I");

  assert_parsed_expression_evaluates_to<float>("root(-1,3)", "0.5+0.8660254*I");
  assert_parsed_expression_evaluates_to<double>("root(-1,3)", "0.5+8.6602540378444E-1*I");

  assert_parsed_expression_evaluates_to<float>("factor(-23/4)", "-5.75");
  assert_parsed_expression_evaluates_to<double>("factor(-123/24)", "-5.125");

  Expression * exp = parse_expression("random()");
  assert_exp_is_bounded(exp, 0.0f, 1.0f);
  assert_exp_is_bounded(exp, 0.0, 1.0);
  delete exp;

  exp = parse_expression("randint(4,45)");
  assert_exp_is_bounded(exp, 4.0f, 45.0f, true);
  assert_exp_is_bounded(exp, 4.0, 45.0, true);
  delete exp;
}

QUIZ_CASE(poincare_function_simplify) {
  assert_parsed_expression_simplify_to("abs(P)", "P");
  assert_parsed_expression_simplify_to("abs(-P)", "P");
  assert_parsed_expression_simplify_to("binomial(20,3)", "1140");
  assert_parsed_expression_simplify_to("binomial(20,10)", "184756");
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1");
  assert_parsed_expression_simplify_to("conj(1/2)", "1/2");
  assert_parsed_expression_simplify_to("quo(19,3)", "6");
  assert_parsed_expression_simplify_to("quo(19,0)", "undef");
  assert_parsed_expression_simplify_to("quo(-19,3)", "-7");
  assert_parsed_expression_simplify_to("rem(19,3)", "1");
  assert_parsed_expression_simplify_to("rem(-19,3)", "2");
  assert_parsed_expression_simplify_to("rem(19,0)", "undef");
  assert_parsed_expression_simplify_to("99!", "933262154439441526816992388562667004907159682643816214685929638952175999932299156089414639761565182862536979208272237582511852109168640000000000000000000000");
  assert_parsed_expression_simplify_to("factor(-10008/6895)", "-(2^3*3^2*139)/(5*7*197)");
  assert_parsed_expression_simplify_to("factor(1008/6895)", "(2^4*3^2)/(5*197)");
  assert_parsed_expression_simplify_to("factor(10007)", "10007");
  assert_parsed_expression_simplify_to("factor(10007^2)", "undef");
  assert_parsed_expression_simplify_to("floor(-1.3)", "-2");
  assert_parsed_expression_simplify_to("frac(-1.3)", "7/10");
  assert_parsed_expression_simplify_to("gcd(123,278)", "1");
  assert_parsed_expression_simplify_to("gcd(11,121)", "11");
  assert_parsed_expression_simplify_to("lcm(123,278)", "34194");
  assert_parsed_expression_simplify_to("lcm(11,121)", "121");
  assert_parsed_expression_simplify_to("R(4)", "2");
  assert_parsed_expression_simplify_to("root(4,3)", "root(4,3)");
  assert_parsed_expression_simplify_to("root(4,P)", "4^(1/P)");
  assert_parsed_expression_simplify_to("root(27,3)", "3");
  assert_parsed_expression_simplify_to("round(4.235,2)", "106/25");
  assert_parsed_expression_simplify_to("round(4.23,0)", "4");
  assert_parsed_expression_simplify_to("round(4.9,0)", "5");
  assert_parsed_expression_simplify_to("round(12.9,-1)", "10");
  assert_parsed_expression_simplify_to("round(12.9,-2)", "0");
  assert_parsed_expression_simplify_to("permute(99,4)", "90345024");
  assert_parsed_expression_simplify_to("permute(20,-10)", "undef");
  assert_parsed_expression_simplify_to("re(1/2)", "1/2");
}
