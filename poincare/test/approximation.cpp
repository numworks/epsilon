#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

template<typename T>
void assert_expression_approximates_to_scalar(const char * expression, T approximation, Preferences::AngleUnit angleUnit = Degree, Preferences::ComplexFormat complexFormat = Cartesian) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  T result = e.approximateToScalar<T>(&globalContext, complexFormat, angleUnit);
  quiz_assert_print_if_failure((std::isnan(result) && std::isnan(approximation)) || (std::isinf(result) && std::isinf(approximation) && result*approximation >= 0) || (std::fabs(result - approximation) <= Poincare::Expression::Epsilon<T>()), expression);
}

QUIZ_CASE(poincare_approximation_decimal) {
  assert_expression_approximates_to<float>("-0", "0");
  assert_expression_approximates_to<float>("-0.1", "-0.1");
  assert_expression_approximates_to<float>("-1.", "-1");
  assert_expression_approximates_to<float>("-.1", "-0.1");
  assert_expression_approximates_to<float>("-0ᴇ2", "0");
  assert_expression_approximates_to<float>("-0.1ᴇ2", "-10");
  assert_expression_approximates_to<float>("-1.ᴇ2", "-100");
  assert_expression_approximates_to<float>("-.1ᴇ2", "-10");
  assert_expression_approximates_to<float>("-0ᴇ-2", "0");
  assert_expression_approximates_to<float>("-0.1ᴇ-2", "-0.001");
  assert_expression_approximates_to<float>("-1.ᴇ-2", "-0.01");
  assert_expression_approximates_to<float>("-.1ᴇ-2", "-0.001");
  assert_expression_approximates_to<float>("-.003", "-0.003");
  assert_expression_approximates_to<float>("1.2343ᴇ-2", "0.012343");
  assert_expression_approximates_to<double>("-567.2ᴇ2", "-56720");

  assert_expression_approximates_to_scalar<float>("-0", 0.0f);
  assert_expression_approximates_to_scalar<float>("-1.ᴇ-2", -0.01f);
  assert_expression_approximates_to_scalar<double>("-.003", -0.003);
  assert_expression_approximates_to_scalar<float>("1.2343ᴇ-2", 0.012343f);
  assert_expression_approximates_to_scalar<double>("-567.2ᴇ2", -56720.0);
}

QUIZ_CASE(poincare_approximation_based_integer) {
  assert_expression_approximates_to<float>("1232", "1232");
  assert_expression_approximates_to<double>("0b110101", "53");
  assert_expression_approximates_to<double>("0xabc1234", "180097588");
}

QUIZ_CASE(poincare_approximation_rational) {
  assert_expression_approximates_to<float>("1/3", "0.3333333");
  assert_expression_approximates_to<double>("123456/1234567", "9.9999432999586ᴇ-2");

  assert_expression_approximates_to_scalar<float>("1/3", 0.3333333f);
  assert_expression_approximates_to_scalar<double>("123456/1234567", 9.9999432999586E-2);
}

template<typename T>
void assert_float_approximates_to(Float<T> f, const char * result) {
  Shared::GlobalContext globalContext;
  int numberOfDigits = sizeof(T) == sizeof(double) ? PrintFloat::k_numberOfStoredSignificantDigits : PrintFloat::k_numberOfPrintedSignificantDigits;
  char buffer[500];
  f.template approximate<T>(&globalContext, Cartesian, Radian).serialize(buffer, sizeof(buffer), DecimalMode, numberOfDigits);
  quiz_assert_print_if_failure(strcmp(buffer, result) == 0, result);
}

QUIZ_CASE(poincare_approximation_float) {
  assert_float_approximates_to<double>(Float<double>::Builder(-1.23456789E30), "-1.23456789ᴇ30");
  assert_float_approximates_to<double>(Float<double>::Builder(1.23456789E30), "1.23456789ᴇ30");
  assert_float_approximates_to<double>(Float<double>::Builder(-1.23456789E-30), "-1.23456789ᴇ-30");
  assert_float_approximates_to<double>(Float<double>::Builder(-1.2345E-3), "-0.0012345");
  assert_float_approximates_to<double>(Float<double>::Builder(1.2345E-3), "0.0012345");
  assert_float_approximates_to<double>(Float<double>::Builder(1.2345E3), "1234.5");
  assert_float_approximates_to<double>(Float<double>::Builder(-1.2345E3), "-1234.5");
  assert_float_approximates_to<double>(Float<double>::Builder(0.99999999999995), "9.9999999999995ᴇ-1");
  assert_float_approximates_to<double>(Float<double>::Builder(0.00000099999999999995), "9.9999999999995ᴇ-7");
  assert_float_approximates_to<double>(Float<double>::Builder(0.0000009999999999901200121020102010201201201021099995), "9.9999999999012ᴇ-7");
  assert_float_approximates_to<float>(Float<float>::Builder(1.2345E-1), "0.12345");
  assert_float_approximates_to<float>(Float<float>::Builder(1), "1");
  assert_float_approximates_to<float>(Float<float>::Builder(0.9999999999999995), "1");
  assert_float_approximates_to<float>(Float<float>::Builder(1.2345E6), "1234500");
  assert_float_approximates_to<float>(Float<float>::Builder(-1.2345E6), "-1234500");
  assert_float_approximates_to<float>(Float<float>::Builder(0.0000009999999999999995), "0.000001");
  assert_float_approximates_to<float>(Float<float>::Builder(-1.2345E-1), "-0.12345");

  assert_float_approximates_to<double>(Float<double>::Builder(INFINITY), Infinity::Name());
  assert_float_approximates_to<float>(Float<float>::Builder(0.0f), "0");
  assert_float_approximates_to<float>(Float<float>::Builder(NAN), Undefined::Name());
}

QUIZ_CASE(poincare_approximation_infinity) {
  assert_expression_approximates_to<double>("10^1000", "∞");
  assert_expression_approximates_to_scalar<double>("10^1000", INFINITY);
}

QUIZ_CASE(poincare_approximation_addition) {
  assert_expression_approximates_to<float>("1+2", "3");
  assert_expression_approximates_to<float>("𝐢+𝐢", "2×𝐢");
  assert_expression_approximates_to<double>("2+𝐢+4+𝐢", "6+2×𝐢");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]+3", "undef");
  assert_expression_approximates_to<double>("[[1,2+𝐢][3,4][5,6]]+3+𝐢", "undef");
  assert_expression_approximates_to<float>("3+[[1,2][3,4][5,6]]", "undef");
  assert_expression_approximates_to<double>("3+𝐢+[[1,2+𝐢][3,4][5,6]]", "undef");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]", "[[2,4][6,8][10,12]]");
  assert_expression_approximates_to<double>("[[1,2+𝐢][3,4][5,6]]+[[1,2+𝐢][3,4][5,6]]", "[[2,4+2×𝐢][6,8][10,12]]");

  assert_expression_approximates_to_scalar<float>("1+2", 3.0f);
  assert_expression_approximates_to_scalar<double>("𝐢+𝐢", NAN);
  assert_expression_approximates_to_scalar<float>("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]", NAN);
}

QUIZ_CASE(poincare_approximation_multiplication) {
  assert_expression_approximates_to<float>("1×2", "2");
  assert_expression_approximates_to<double>("(3+𝐢)×(4+𝐢)", "11+7×𝐢");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]×2", "[[2,4][6,8][10,12]]");
  assert_expression_approximates_to<double>("[[1,2+𝐢][3,4][5,6]]×(3+𝐢)", "[[3+𝐢,5+5×𝐢][9+3×𝐢,12+4×𝐢][15+5×𝐢,18+6×𝐢]]");
  assert_expression_approximates_to<float>("2×[[1,2][3,4][5,6]]", "[[2,4][6,8][10,12]]");
  assert_expression_approximates_to<double>("(3+𝐢)×[[1,2+𝐢][3,4][5,6]]", "[[3+𝐢,5+5×𝐢][9+3×𝐢,12+4×𝐢][15+5×𝐢,18+6×𝐢]]");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]×[[1,2,3,4][5,6,7,8]]", "[[11,14,17,20][23,30,37,44][35,46,57,68]]");
  assert_expression_approximates_to<double>("[[1,2+𝐢][3,4][5,6]]×[[1,2+𝐢,3,4][5,6+𝐢,7,8]]", "[[11+5×𝐢,13+9×𝐢,17+7×𝐢,20+8×𝐢][23,30+7×𝐢,37,44][35,46+11×𝐢,57,68]]");

  assert_expression_approximates_to_scalar<float>("1×2", 2.0f);
  assert_expression_approximates_to_scalar<double>("(3+𝐢)×(4+𝐢)", NAN);
  assert_expression_approximates_to_scalar<float>("[[1,2][3,4][5,6]]×2", NAN);
}

QUIZ_CASE(poincare_approximation_power) {
  assert_expression_approximates_to<float>("2^3", "8");
  assert_expression_approximates_to<double>("(3+𝐢)^4", "28+96×𝐢");
  assert_expression_approximates_to<float>("4^(3+𝐢)", "11.74125+62.91378×𝐢");
  assert_expression_approximates_to<double>("(3+𝐢)^(3+𝐢)", "-11.898191759852+19.592921596609×𝐢");

  assert_expression_approximates_to<double>("0^0", Undefined::Name());
  assert_expression_approximates_to<double>("0^2", "0");
  assert_expression_approximates_to<double>("0^(-2)", Undefined::Name());

  assert_expression_approximates_to<double>("(-2)^4.2", "14.8690638497+10.8030072384×𝐢", Radian, Metric, Cartesian, 12);
  assert_expression_approximates_to<double>("(-0.1)^4", "0.0001", Radian, Metric, Cartesian, 12);

  assert_expression_approximates_to<float>("0^2", "0");
  assert_expression_approximates_to<double>("𝐢^𝐢", "2.0787957635076ᴇ-1");
  assert_expression_approximates_to<float>("1.0066666666667^60", "1.48985", Radian, Metric, Cartesian, 6);
  assert_expression_approximates_to<double>("1.0066666666667^60", "1.489845708305", Radian, Metric, Cartesian, 13);
  assert_expression_approximates_to<float>("ℯ^(𝐢×π)", "-1");
  assert_expression_approximates_to<double>("ℯ^(𝐢×π)", "-1");
  assert_expression_approximates_to<float>("ℯ^(𝐢×π+2)", "-7.38906", Radian, Metric, Cartesian, 6);
  assert_expression_approximates_to<double>("ℯ^(𝐢×π+2)", "-7.3890560989307");
  assert_expression_approximates_to<float>("(-1)^(1/3)", "0.5+0.8660254×𝐢");
  assert_expression_approximates_to<double>("(-1)^(1/3)", "0.5+8.6602540378444ᴇ-1×𝐢");
  assert_expression_approximates_to<float>("ℯ^(𝐢×π/3)", "0.5+0.866025×𝐢", Radian, Metric, Cartesian, 6);
  assert_expression_approximates_to<double>("ℯ^(𝐢×π/3)", "0.5+8.6602540378444ᴇ-1×𝐢");
  assert_expression_approximates_to<float>("𝐢^(2/3)", "0.5+0.8660254×𝐢");
  assert_expression_approximates_to<double>("𝐢^(2/3)", "0.5+8.6602540378444ᴇ-1×𝐢");

  assert_expression_approximates_to_scalar<float>("2^3", 8.0f);
  assert_expression_approximates_to_scalar<double>("(3+𝐢)^(4+𝐢)", NAN);
  assert_expression_approximates_to_scalar<float>("[[1,2][3,4]]^2", NAN);


  assert_expression_approximates_to<float>("(-10)^0.00000001", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<float>("(-10)^0.00000001", "1+3.141593ᴇ-8×𝐢", Radian, Metric, Cartesian);
  assert_expression_simplifies_approximates_to<float>("3.5^2.0000001", "12.25");
  assert_expression_simplifies_approximates_to<float>("3.7^2.0000001", "13.69");
}

QUIZ_CASE(poincare_approximation_subtraction) {
  assert_expression_approximates_to<float>("1-2", "-1");
  assert_expression_approximates_to<double>("3+𝐢-(4+𝐢)", "-1");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]-3", "undef");
  assert_expression_approximates_to<double>("[[1,2+𝐢][3,4][5,6]]-(4+𝐢)", "undef");
  assert_expression_approximates_to<float>("3-[[1,2][3,4][5,6]]", "undef");
  assert_expression_approximates_to<double>("3+𝐢-[[1,2+𝐢][3,4][5,6]]", "undef");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]-[[6,5][4,3][2,1]]", "[[-5,-3][-1,1][3,5]]");
  assert_expression_approximates_to<double>("[[1,2+𝐢][3,4][5,6]]-[[1,2+𝐢][3,4][5,6]]", "[[0,0][0,0][0,0]]");

  assert_expression_approximates_to_scalar<float>("1-2", -1.0f);
  assert_expression_approximates_to_scalar<double>("(1)-(4+𝐢)", NAN);
  assert_expression_approximates_to_scalar<float>("[[1,2][3,4][5,6]]-[[3,2][3,4][5,6]]", NAN);
}

QUIZ_CASE(poincare_approximation_constant) {
  assert_expression_approximates_to<float>("𝐢", "𝐢");
  assert_expression_approximates_to<double>("π", "3.1415926535898");
  assert_expression_approximates_to<float>("ℯ", "2.718282");

  assert_expression_approximates_to_scalar<float>("𝐢", NAN);
  assert_expression_approximates_to_scalar<double>("π", 3.141592653589793238);
  assert_expression_approximates_to_scalar<double>("ℯ", 2.718281828459045235);
}

QUIZ_CASE(poincare_approximation_division) {
  assert_expression_approximates_to<float>("1/2", "0.5");
  assert_expression_approximates_to<double>("(3+𝐢)/(4+𝐢)", "7.6470588235294ᴇ-1+5.8823529411765ᴇ-2×𝐢");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  assert_expression_approximates_to<double>("[[1,2+𝐢][3,4][5,6]]/(1+𝐢)", "[[0.5-0.5×𝐢,1.5-0.5×𝐢][1.5-1.5×𝐢,2-2×𝐢][2.5-2.5×𝐢,3-3×𝐢]]");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  assert_expression_approximates_to<double>("[[1,2][3,4]]/[[3,4][6,9]]", "[[-1,6.6666666666667ᴇ-1][1,0]]");
  assert_expression_approximates_to<double>("3/[[3,4][5,6]]", "[[-9,6][7.5,-4.5]]");
  assert_expression_approximates_to<double>("(3+4𝐢)/[[1,𝐢][3,4]]", "[[4×𝐢,1][-3×𝐢,𝐢]]");
  // assert_expression_approximates_to<double>("(3+4𝐢)/[[3,4][1,𝐢]]", "[[1,4×𝐢][𝐢,-3×𝐢]]");
  /* TODO: this tests fails because of neglectable real or imaginary parts.
   * It currently approximates to
   * [[1+5.5511151231258ᴇ-17×𝐢,-2.2204460492503ᴇ-16+4×𝐢][𝐢,-3×𝐢]] or
   * [[1-1.1102230246252ᴇ-16×𝐢,2.2204460492503ᴇ-16+4×𝐢]
   *  [-1.1102230246252ᴇ-16+𝐢,-2.2204460492503ᴇ-16-3×𝐢]] on Linux */
  assert_expression_approximates_to<float>("1ᴇ20/(1ᴇ20+1ᴇ20𝐢)", "0.5-0.5×𝐢");
  assert_expression_approximates_to<double>("1ᴇ155/(1ᴇ155+1ᴇ155𝐢)", "0.5-0.5×𝐢");

  assert_expression_approximates_to_scalar<float>("1/2", 0.5f);
  assert_expression_approximates_to_scalar<float>("(3+𝐢)/(4+𝐢)", NAN);
  assert_expression_approximates_to_scalar<float>("[[1,2][3,4][5,6]]/2", NAN);
}

QUIZ_CASE(poincare_approximation_logarithm) {
  assert_expression_approximates_to<float>("log(2,64)", "0.1666667");
  assert_expression_approximates_to<double>("log(6,7)", "0.9207822211616");
  assert_expression_approximates_to<float>("log(5)", "0.69897");
  assert_expression_approximates_to<double>("ln(5)", "1.6094379124341");
  assert_expression_approximates_to<float>("log(2+5×𝐢,64)", "0.4048317+0.2862042×𝐢");
  assert_expression_approximates_to<double>("log(6,7+4×𝐢)", "8.0843880717528ᴇ-1-2.0108238082167ᴇ-1×𝐢");
  assert_expression_approximates_to<float>("log(5+2×𝐢)", "0.731199+0.1652518×𝐢");
  assert_expression_approximates_to<double>("ln(5+2×𝐢)", "1.6836479149932+3.8050637711236ᴇ-1×𝐢");
  assert_expression_approximates_to<double>("log(0,0)", Undefined::Name());
  assert_expression_approximates_to<double>("log(0)", "-∞");
  assert_expression_approximates_to<double>("log(2,0)", "0");

  // WARNING: evaluate on branch cut can be multivalued
  assert_expression_approximates_to<double>("ln(-4)", "1.3862943611199+3.1415926535898×𝐢");
}

template<typename T>
void assert_expression_approximation_is_bounded(const char * expression, T lowBound, T upBound, bool upBoundIncluded = false) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, true);
  T result = e.approximateToScalar<T>(&globalContext, Cartesian, Radian);
  quiz_assert_print_if_failure(result >= lowBound, expression);
  quiz_assert_print_if_failure(result < upBound || (result == upBound && upBoundIncluded), expression);
}

QUIZ_CASE(poincare_approximation_function) {
  assert_expression_approximates_to<float>("abs(-1)", "1");
  assert_expression_approximates_to<double>("abs(-1)", "1");

  assert_expression_approximates_to<float>("abs(-2.3ᴇ-39)", "2.3ᴇ-39", Degree, Metric, Cartesian, 5);
  assert_expression_approximates_to<double>("abs(-2.3ᴇ-39)", "2.3ᴇ-39");

  assert_expression_approximates_to<float>("abs(3+2𝐢)", "3.605551");
  assert_expression_approximates_to<double>("abs(3+2𝐢)", "3.605551275464");

  assert_expression_approximates_to<float>("abs([[1,-2][3,-4]])", "[[1,2][3,4]]");
  assert_expression_approximates_to<double>("abs([[1,-2][3,-4]])", "[[1,2][3,4]]");

  assert_expression_approximates_to<float>("abs([[3+2𝐢,3+4𝐢][5+2𝐢,3+2𝐢]])", "[[3.605551,5][5.385165,3.605551]]");
  assert_expression_approximates_to<double>("abs([[3+2𝐢,3+4𝐢][5+2𝐢,3+2𝐢]])", "[[3.605551275464,5][5.3851648071345,3.605551275464]]");

  assert_expression_approximates_to<float>("binomcdf(5.3, 9, 0.7)", "0.270341", Degree, Metric, Cartesian, 6); // FIXME: precision problem
  assert_expression_approximates_to<double>("binomcdf(5.3, 9, 0.7)", "0.270340902", Degree, Metric, Cartesian, 10); //FIXME precision problem

  assert_expression_approximates_to<float>("binomial(10, 4)", "210");
  assert_expression_approximates_to<double>("binomial(10, 4)", "210");
  assert_expression_approximates_to<float>("binomial(12, 3)", "220");
  assert_expression_approximates_to<double>("binomial(12, 3)", "220");
  assert_expression_approximates_to<float>("binomial(-4.6, 3)", "-28.336");
  assert_expression_approximates_to<double>("binomial(-4.6, 3)", "-28.336");
  assert_expression_approximates_to<float>("binomial(π, 3)", "1.280108");
  assert_expression_approximates_to<double>("binomial(π, 3)", "1.2801081307019");
  assert_expression_approximates_to<float>("binomial(7, 9)", "0");
  assert_expression_approximates_to<double>("binomial(7, 9)", "0");
  assert_expression_approximates_to<float>("binomial(-7, 9)", "-5005");
  assert_expression_approximates_to<double>("binomial(-7, 9)", "-5005");

  assert_expression_approximates_to<float>("binompdf(4.4, 9, 0.7)", "0.0735138", Degree, Metric, Cartesian, 6); // FIXME: precision problem
  assert_expression_approximates_to<double>("binompdf(4.4, 9, 0.7)", "0.073513818");

  assert_expression_approximates_to<float>("ceil(0.2)", "1");
  assert_expression_approximates_to<double>("ceil(0.2)", "1");

  assert_expression_approximates_to<float>("det([[1,23,3][4,5,6][7,8,9]])", "126", Degree, Metric, Cartesian, 6); // FIXME: the determinant computation is not precised enough to be displayed with 7 significant digits
  assert_expression_approximates_to<double>("det([[1,23,3][4,5,6][7,8,9]])", "126");

  assert_expression_approximates_to<float>("det([[𝐢,23-2𝐢,3×𝐢][4+𝐢,5×𝐢,6][7,8×𝐢+2,9]])", "126-231×𝐢", Degree, Metric, Cartesian, 6); // FIXME: the determinant computation is not precised enough to be displayed with 7 significant digits
  assert_expression_approximates_to<double>("det([[𝐢,23-2𝐢,3×𝐢][4+𝐢,5×𝐢,6][7,8×𝐢+2,9]])", "126-231×𝐢");

  assert_expression_approximates_to<float>("diff(2×x, x, 2)", "2");
  assert_expression_approximates_to<double>("diff(2×x, x, 2)", "2");

  assert_expression_approximates_to<float>("diff(2×TO^2, TO, 7)", "28");
  assert_expression_approximates_to<double>("diff(2×TO^2, TO, 7)", "28");

  //assert_expression_approximates_to<float>("diff(-1/3×x^3+6x^2-11x-50,x,11)", "0"); // FIXME error too big
  assert_expression_approximates_to<double>("diff(-1/3×x^3+6x^2-11x-50,x,11)", "0");

  assert_expression_approximates_to<float>("floor(2.3)", "2");
  assert_expression_approximates_to<double>("floor(2.3)", "2");

  assert_expression_approximates_to<float>("frac(2.3)", "0.3");
  assert_expression_approximates_to<double>("frac(2.3)", "0.3");

  assert_expression_approximates_to<float>("gcd(234,394)", "2");
  assert_expression_approximates_to<double>("gcd(234,394)", "2");
  assert_expression_approximates_to<float>("gcd(-234,394)", "2");
  assert_expression_approximates_to<double>("gcd(234,-394)", "2");
  assert_expression_approximates_to<float>("gcd(-234,-394)", "2");
  assert_expression_approximates_to<float>("gcd(-234,-394, -16)", "2");
  assert_expression_approximates_to<double>("gcd(-234,-394, -16)", "2");
  assert_expression_approximates_to<float>("gcd(6,15,10)", "1");
  assert_expression_approximates_to<double>("gcd(6,15,10)", "1");
  assert_expression_approximates_to<float>("gcd(30,105,70,42)", "1");
  assert_expression_approximates_to<double>("gcd(30,105,70,42)", "1");

  assert_expression_approximates_to<float>("im(2+3𝐢)", "3");
  assert_expression_approximates_to<double>("im(2+3𝐢)", "3");

  assert_expression_approximates_to<float>("lcm(234,394)", "46098");
  assert_expression_approximates_to<double>("lcm(234,394)", "46098");
  assert_expression_approximates_to<float>("lcm(-234,394)", "46098");
  assert_expression_approximates_to<double>("lcm(234,-394)", "46098");
  assert_expression_approximates_to<float>("lcm(-234,-394)", "46098");
  assert_expression_approximates_to<float>("lcm(-234,-394, -16)", "368784");
  assert_expression_approximates_to<double>("lcm(-234,-394, -16)", "368784");
  assert_expression_approximates_to<float>("lcm(6,15,10)", "30");
  assert_expression_approximates_to<double>("lcm(6,15,10)", "30");
  assert_expression_approximates_to<float>("lcm(30,105,70,42)", "210");
  assert_expression_approximates_to<double>("lcm(30,105,70,42)", "210");

  assert_expression_approximates_to<float>("int(x,x, 1, 2)", "1.5");
  assert_expression_approximates_to<double>("int(x,x, 1, 2)", "1.5");

  assert_expression_approximates_to<float>("invbinom(0.9647324002, 15, 0.7)", "13");
  assert_expression_approximates_to<double>("invbinom(0.9647324002, 15, 0.7)", "13");

  assert_expression_approximates_to<float>("invnorm(0.56, 1.3, 2.4)", "1.662326");
  //assert_expression_approximates_to<double>("invnorm(0.56, 1.3, 2.4)", "1.6623258450088"); FIXME precision error

  assert_expression_approximates_to<float>("ln(2)", "0.6931472");
  assert_expression_approximates_to<double>("ln(2)", "6.9314718055995ᴇ-1");

  assert_expression_approximates_to<float>("log(2)", "0.30103");
  assert_expression_approximates_to<double>("log(2)", "3.0102999566398ᴇ-1");

  assert_expression_approximates_to<double>("normcdf(5, 7, 0.3162)", "1.265256ᴇ-10", Radian, Metric, Cartesian, 7);

  assert_expression_approximates_to<float>("normcdf(1.2, 3.4, 5.6)", "0.3472125");
  assert_expression_approximates_to<double>("normcdf(1.2, 3.4, 5.6)", "3.4721249841587ᴇ-1");
  assert_expression_approximates_to<float>("normcdf(-1ᴇ99,3.4,5.6)", "0");
  assert_expression_approximates_to<float>("normcdf(1ᴇ99,3.4,5.6)", "1");
  assert_expression_approximates_to<float>("normcdf(-6,0,1)", "0");
  assert_expression_approximates_to<float>("normcdf(6,0,1)", "1");

  assert_expression_approximates_to<float>("normcdf2(0.5, 3.6, 1.3, 3.4)", "0.3436388");
  assert_expression_approximates_to<double>("normcdf2(0.5, 3.6, 1.3, 3.4)", "3.4363881299147ᴇ-1");

  assert_expression_approximates_to<float>("normpdf(1.2, 3.4, 5.6)", "0.06594901");

  assert_expression_approximates_to<float>("permute(10, 4)", "5040");
  assert_expression_approximates_to<double>("permute(10, 4)", "5040");

  assert_expression_approximates_to<float>("product(n,n, 4, 10)", "604800");
  assert_expression_approximates_to<double>("product(n,n, 4, 10)", "604800");

  assert_expression_approximates_to<float>("quo(29, 10)", "2");
  assert_expression_approximates_to<double>("quo(29, 10)", "2");

  assert_expression_approximates_to<float>("re(2+𝐢)", "2");
  assert_expression_approximates_to<double>("re(2+𝐢)", "2");

  assert_expression_approximates_to<float>("rem(29, 10)", "9");
  assert_expression_approximates_to<double>("rem(29, 10)", "9");
  assert_expression_approximates_to<float>("root(2,3)", "1.259921");
  assert_expression_approximates_to<double>("root(2,3)", "1.2599210498949");

  assert_expression_approximates_to<float>("√(2)", "1.414214");
  assert_expression_approximates_to<double>("√(2)", "1.4142135623731");

  assert_expression_approximates_to<float>("√(-1)", "𝐢");
  assert_expression_approximates_to<double>("√(-1)", "𝐢");

  assert_expression_approximates_to<float>("sum(r,r, 4, 10)", "49");
  assert_expression_approximates_to<double>("sum(k,k, 4, 10)", "49");

  assert_expression_approximates_to<float>("trace([[1,2,3][4,5,6][7,8,9]])", "15");
  assert_expression_approximates_to<double>("trace([[1,2,3][4,5,6][7,8,9]])", "15");

  assert_expression_approximates_to<float>("confidence(0.1, 100)", "[[0,0.2]]");
  assert_expression_approximates_to<double>("confidence(0.1, 100)", "[[0,0.2]]");

  assert_expression_approximates_to<float>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");
  assert_expression_approximates_to<double>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");

  assert_expression_approximates_to<float>("conj(3+2×𝐢)", "3-2×𝐢");
  assert_expression_approximates_to<double>("conj(3+2×𝐢)", "3-2×𝐢");

  assert_expression_approximates_to<float>("factor(-23/4)", "-5.75");
  assert_expression_approximates_to<double>("factor(-123/24)", "-5.125");
  assert_expression_approximates_to<float>("factor(𝐢)", "undef");

  assert_expression_approximates_to<float>("inverse([[1,2,3][4,5,-6][7,8,9]])", "[[-1.2917,-0.083333,0.375][1.0833,0.16667,-0.25][0.041667,-0.083333,0.041667]]", Degree, Metric, Cartesian, 5); // inverse is not precise enough to display 7 significative digits
  assert_expression_approximates_to<double>("inverse([[1,2,3][4,5,-6][7,8,9]])", "[[-1.2916666666667,-8.3333333333333ᴇ-2,0.375][1.0833333333333,1.6666666666667ᴇ-1,-0.25][4.1666666666667ᴇ-2,-8.3333333333333ᴇ-2,4.1666666666667ᴇ-2]]");
  assert_expression_approximates_to<float>("inverse([[𝐢,23-2𝐢,3×𝐢][4+𝐢,5×𝐢,6][7,8×𝐢+2,9]])", "[[-0.0118-0.0455×𝐢,-0.5-0.727×𝐢,0.318+0.489×𝐢][0.0409+0.00364×𝐢,0.04-0.0218×𝐢,-0.0255+0.00091×𝐢][0.00334-0.00182×𝐢,0.361+0.535×𝐢,-0.13-0.358×𝐢]]", Degree, Metric, Cartesian, 3); // inverse is not precise enough to display 7 significative digits
  assert_expression_approximates_to<double>("inverse([[𝐢,23-2𝐢,3×𝐢][4+𝐢,5×𝐢,6][7,8×𝐢+2,9]])", "[[-0.0118289353958-0.0454959053685×𝐢,-0.500454959054-0.727024567789×𝐢,0.31847133758+0.488626023658×𝐢][0.0409463148317+3.63967242948ᴇ-3×𝐢,0.0400363967243-0.0218380345769×𝐢,-0.0254777070064+9.0991810737ᴇ-4×𝐢][3.33636639369ᴇ-3-1.81983621474ᴇ-3×𝐢,0.36093418259+0.534728541098×𝐢,-0.130118289354-0.357597816197×𝐢]]", Degree, Metric, Cartesian, 12); // FIXME: inverse is not precise enough to display 14 significative digits

  assert_expression_approximates_to<float>("prediction(0.1, 100)", "[[0,0.2]]");
  assert_expression_approximates_to<double>("prediction(0.1, 100)", "[[0,0.2]]");

  assert_expression_approximates_to<float>("prediction95(0.1, 100)", "[[0.0412,0.1588]]");
  assert_expression_approximates_to<double>("prediction95(0.1, 100)", "[[0.0412,0.1588]]");

  assert_expression_approximates_to<float>("product(2+k×𝐢,k, 1, 5)", "-100-540×𝐢");
  assert_expression_approximates_to<double>("product(2+o×𝐢,o, 1, 5)", "-100-540×𝐢");

  assert_expression_approximates_to<float>("root(3+𝐢, 3)", "1.459366+0.1571201×𝐢");
  assert_expression_approximates_to<double>("root(3+𝐢, 3)", "1.4593656008684+1.5712012294394ᴇ-1×𝐢");

  assert_expression_approximates_to<float>("root(3, 3+𝐢)", "1.382007-0.1524428×𝐢");
  assert_expression_approximates_to<double>("root(3, 3+𝐢)", "1.3820069623326-0.1524427794159×𝐢");

  assert_expression_approximates_to<float>("root(5^((-𝐢)3^9),𝐢)", "3.504", Degree, Metric, Cartesian, 4);
  assert_expression_approximates_to<double>("root(5^((-𝐢)3^9),𝐢)", "3.5039410843", Degree, Metric, Cartesian, 11);

  assert_expression_approximates_to<float>("√(3+𝐢)", "1.755317+0.2848488×𝐢");
  assert_expression_approximates_to<double>("√(3+𝐢)", "1.7553173018244+2.8484878459314ᴇ-1×𝐢");

  assert_expression_approximates_to<float>("sign(-23+1)", "-1");
  assert_expression_approximates_to<float>("sign(inf)", "1");
  assert_expression_approximates_to<float>("sign(-inf)", "-1");
  assert_expression_approximates_to<float>("sign(0)", "0");
  assert_expression_approximates_to<float>("sign(-0)", "0");
  assert_expression_approximates_to<float>("sign(x)", "undef");
  assert_expression_approximates_to<double>("sign(2+𝐢)", "undef");
  assert_expression_approximates_to<double>("sign(undef)", "undef");

  assert_expression_approximates_to<double>("sum(2+n×𝐢,n,1,5)", "10+15×𝐢");
  assert_expression_approximates_to<double>("sum(2+n×𝐢,n,1,5)", "10+15×𝐢");

  assert_expression_approximates_to<float>("transpose([[1,2,3][4,5,-6][7,8,9]])", "[[1,4,7][2,5,8][3,-6,9]]");
  assert_expression_approximates_to<float>("transpose([[1,7,5][4,2,8]])", "[[1,4][7,2][5,8]]");
  assert_expression_approximates_to<float>("transpose([[1,2][4,5][7,8]])", "[[1,4,7][2,5,8]]");
  assert_expression_approximates_to<double>("transpose([[1,2,3][4,5,-6][7,8,9]])", "[[1,4,7][2,5,8][3,-6,9]]");
  assert_expression_approximates_to<double>("transpose([[1,7,5][4,2,8]])", "[[1,4][7,2][5,8]]");
  assert_expression_approximates_to<double>("transpose([[1,2][4,5][7,8]])", "[[1,4,7][2,5,8]]");

  assert_expression_approximates_to<double>("ref([[0,2,-1][5,6,7][10,11,10]])", "[[1,1.1,1][0,1,-0.5][0,0,1]]");
  assert_expression_approximates_to<double>("rref([[0,2,-1][5,6,7][10,11,10]])", "[[1,0,0][0,1,0][0,0,1]]");
  assert_expression_approximates_to<float>("ref([[0,2,-1][5,6,7][10,11,10]])", "[[1,1.1,1][0,1,-0.5][0,0,1]]");
  assert_expression_approximates_to<float>("rref([[0,2,-1][5,6,7][10,11,10]])", "[[1,0,0][0,1,0][0,0,1]]");

  assert_expression_approximates_to<float>("cross([[1][2][3]],[[4][7][8]])", "[[-5][4][-1]]");
  assert_expression_approximates_to<double>("cross([[1][2][3]],[[4][7][8]])", "[[-5][4][-1]]");
  assert_expression_approximates_to<float>("cross([[1,2,3]],[[4,7,8]])", "[[-5,4,-1]]");
  assert_expression_approximates_to<double>("cross([[1,2,3]],[[4,7,8]])", "[[-5,4,-1]]");

  assert_expression_approximates_to<float>("dot([[1][2][3]],[[4][7][8]])", "42");
  assert_expression_approximates_to<double>("dot([[1][2][3]],[[4][7][8]])", "42");
  assert_expression_approximates_to<float>("dot([[1,2,3]],[[4,7,8]])", "42");
  assert_expression_approximates_to<double>("dot([[1,2,3]],[[4,7,8]])", "42");

  assert_expression_approximates_to<float>("norm([[-5][4][-1]])", "6.480741");
  assert_expression_approximates_to<double>("norm([[-5][4][-1]])", "6.4807406984079");
  assert_expression_approximates_to<float>("norm([[-5,4,-1]])", "6.480741");
  assert_expression_approximates_to<double>("norm([[-5,4,-1]])", "6.4807406984079");

  assert_expression_approximates_to<float>("round(2.3246,3)", "2.325");
  assert_expression_approximates_to<double>("round(2.3245,3)", "2.325");

  assert_expression_approximates_to<float>("6!", "720");
  assert_expression_approximates_to<double>("6!", "720");

  assert_expression_approximates_to<float>("√(-1)", "𝐢");
  assert_expression_approximates_to<double>("√(-1)", "𝐢");

  assert_expression_approximates_to<float>("root(-1,3)", "0.5+0.8660254×𝐢");
  assert_expression_approximates_to<double>("root(-1,3)", "0.5+8.6602540378444ᴇ-1×𝐢");

  assert_expression_approximates_to<float>("int(int(x×x,x,0,x),x,0,4)", "21.33333");
  assert_expression_approximates_to<double>("int(int(x×x,x,0,x),x,0,4)", "21.333333333333");

  assert_expression_approximates_to<float>("int(1+cos(e),e, 0, 180)", "180");
  assert_expression_approximates_to<double>("int(1+cos(e),e, 0, 180)", "180");

  assert_expression_approximation_is_bounded("random()", 0.0f, 1.0f);
  assert_expression_approximation_is_bounded("random()", 0.0, 1.0);

  assert_expression_approximation_is_bounded("randint(4,45)", 4.0f, 45.0f, true);
  assert_expression_approximation_is_bounded("randint(4,45)", 4.0, 45.0, true);
}

QUIZ_CASE(poincare_approximation_trigonometry_functions) {
  /* cos: R  ->  R (oscillator)
   *      Ri ->  R (even)
   */
  // On R
  assert_expression_approximates_to<double>("cos(2)", "-4.1614683654714ᴇ-1", Radian);
  assert_expression_approximates_to<double>("cos(2)", "0.9993908270191", Degree);
  assert_expression_approximates_to<double>("cos(2)", "9.9950656036573ᴇ-1", Gradian);
  // Oscillator
  assert_expression_approximates_to<float>("cos(π/2)", "0", Radian);
  assert_expression_approximates_to<float>("cos(100)", "0", Gradian);
  assert_expression_approximates_to<double>("cos(3×π/2)", "0", Radian);
  assert_expression_approximates_to<float>("cos(300)", "0", Gradian);
  assert_expression_approximates_to<float>("cos(3×π)", "-1", Radian);
  assert_expression_approximates_to<float>("cos(-540)", "-1", Degree);
  assert_expression_approximates_to<float>("cos(-600)", "-1", Gradian);
  // On R×i
  assert_expression_approximates_to<double>("cos(-2×𝐢)", "3.7621956910836", Radian);
  assert_expression_approximates_to<double>("cos(-2×𝐢)", "1.0006092967033", Degree);
  assert_expression_approximates_to<double>("cos(-2×𝐢)", "1.0004935208085", Gradian);
  // Symmetry: even
  assert_expression_approximates_to<double>("cos(2×𝐢)", "3.7621956910836", Radian);
  assert_expression_approximates_to<double>("cos(2×𝐢)", "1.0006092967033", Degree);
  assert_expression_approximates_to<double>("cos(2×𝐢)", "1.0004935208085", Gradian);
  // On C
  assert_expression_approximates_to<float>("cos(𝐢-4)", "-1.008625-0.8893952×𝐢", Radian);
  assert_expression_approximates_to<float>("cos(𝐢-4)", "0.997716+0.00121754×𝐢", Degree, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("cos(𝐢-4)", "0.99815+0.000986352×𝐢", Gradian, Metric, Cartesian, 6);

  /* sin: R  ->  R (oscillator)
   *      Ri ->  Ri (odd)
   */
  // On R
  assert_expression_approximates_to<double>("sin(2)", "9.0929742682568ᴇ-1", Radian);
  assert_expression_approximates_to<double>("sin(2)", "3.4899496702501ᴇ-2", Degree);
  assert_expression_approximates_to<double>("sin(2)", "3.1410759078128ᴇ-2", Gradian);
  // Oscillator
  assert_expression_approximates_to<float>("sin(π/2)", "1", Radian);
  assert_expression_approximates_to<double>("sin(3×π/2)", "-1", Radian);
  assert_expression_approximates_to<float>("sin(3×π)", "0", Radian);
  assert_expression_approximates_to<float>("sin(-540)", "0", Degree);
  assert_expression_approximates_to<float>("sin(-600)", "0", Gradian);
  assert_expression_approximates_to<float>("sin(300)", "-1", Gradian);
  assert_expression_approximates_to<float>("sin(100)", "1", Gradian);
  // On R×i
  assert_expression_approximates_to<double>("sin(3×𝐢)", "10.01787492741×𝐢", Radian);
  assert_expression_approximates_to<float>("sin(3×𝐢)", "0.05238381×𝐢", Degree);
  assert_expression_approximates_to<double>("sin(3×𝐢)", "4.7141332771113ᴇ-2×𝐢", Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("sin(-3×𝐢)", "-10.01787492741×𝐢", Radian);
  assert_expression_approximates_to<float>("sin(-3×𝐢)", "-0.05238381×𝐢", Degree);
  assert_expression_approximates_to<double>("sin(-3×𝐢)", "-4.7141332771113ᴇ-2×𝐢", Gradian);
  // On: C
  assert_expression_approximates_to<float>("sin(𝐢-4)", "1.16781-0.768163×𝐢", Radian, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("sin(𝐢-4)", "-0.0697671+0.0174117×𝐢", Degree, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("sin(𝐢-4)", "-0.0627983+0.0156776×𝐢", Gradian, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("sin(1.234567890123456ᴇ-15)", "1.23457ᴇ-15", Radian, Metric, Cartesian, 6);

  /* tan: R  ->  R (tangent-style)
   *      Ri ->  Ri (odd)
   */
  // On R
  assert_expression_approximates_to<double>("tan(2)", "-2.1850398632615", Radian);
  assert_expression_approximates_to<double>("tan(2)", "3.4920769491748ᴇ-2", Degree);
  assert_expression_approximates_to<double>("tan(2)", "3.1426266043351ᴇ-2", Gradian);
  // Tangent-style
  assert_expression_approximates_to<float>("tan(3×π)", "0", Radian);
  assert_expression_approximates_to<float>("tan(-540)", "0", Degree);
  assert_expression_approximates_to<float>("tan(-600)", "0", Gradian);
  // On R×i
  assert_expression_approximates_to<double>("tan(-2×𝐢)", "-9.6402758007582ᴇ-1×𝐢", Radian);
  assert_expression_approximates_to<float>("tan(-2×𝐢)", "-0.03489241×𝐢", Degree);
  assert_expression_approximates_to<float>("tan(-2×𝐢)", "-0.0314056×𝐢", Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("tan(2×𝐢)", "9.6402758007582ᴇ-1×𝐢", Radian);
  assert_expression_approximates_to<float>("tan(2×𝐢)", "0.03489241×𝐢", Degree);
  assert_expression_approximates_to<float>("tan(2×𝐢)", "0.0314056×𝐢", Gradian);
  // On C
  assert_expression_approximates_to<float>("tan(𝐢-4)", "-0.273553+1.00281×𝐢", Radian, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("tan(𝐢-4)", "-0.0699054+0.0175368×𝐢", Degree, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("tan(𝐢-4)", "-0.0628991+0.0157688×𝐢", Gradian, Metric, Cartesian, 6);

  /* acos: [-1,1]    -> R
   *       ]-inf,-1[ -> π+R×i (odd imaginary)
   *       ]1, inf[  -> R×i (odd imaginary)
   *       R×i       -> π/2+R×i (odd imaginary)
   */
  // On [-1, 1]
  assert_expression_approximates_to<double>("acos(0.5)", "1.0471975511966", Radian);
  assert_expression_approximates_to<double>("acos(0.03)", "1.5407918249714", Radian);
  assert_expression_approximates_to<double>("acos(0.5)", "60", Degree);
  assert_expression_approximates_to<double>("acos(0.5)", "66.666666666667", Gradian);
  // On [1, inf[
  assert_expression_approximates_to<double>("acos(2)", "1.3169578969248×𝐢", Radian);
  assert_expression_approximates_to<double>("acos(2)", "75.456129290217×𝐢", Degree);
  assert_expression_approximates_to<double>("acos(2)", "83.84×𝐢", Gradian, Metric, Cartesian, 4);
  // Symmetry: odd on imaginary
  assert_expression_approximates_to<double>("acos(-2)", "3.1415926535898-1.3169578969248×𝐢", Radian);
  assert_expression_approximates_to<double>("acos(-2)", "180-75.456129290217×𝐢", Degree);
  assert_expression_approximates_to<double>("acos(-2)", "200-83.84×𝐢", Gradian, Metric, Cartesian, 4);
  // On ]-inf, -1[
  assert_expression_approximates_to<double>("acos(-32)", "3.14159265359-4.158638853279×𝐢", Radian, Metric, Cartesian, 13);
  assert_expression_approximates_to<float>("acos(-32)", "180-238.3×𝐢", Degree, Metric, Cartesian, 4);
  assert_expression_approximates_to<float>("acos(-32)", "200-264.7×𝐢", Gradian, Metric, Cartesian, 4);
  // On R×i
  assert_expression_approximates_to<float>("acos(3×𝐢)", "1.5708-1.8184×𝐢", Radian, Metric, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(3×𝐢)", "90-104.19×𝐢", Degree, Metric, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(3×𝐢)", "100-115.8×𝐢", Gradian, Metric, Cartesian, 4);
  // Symmetry: odd on imaginary
  assert_expression_approximates_to<float>("acos(-3×𝐢)", "1.5708+1.8184×𝐢", Radian, Metric, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(-3×𝐢)", "90+104.19×𝐢", Degree, Metric, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(-3×𝐢)", "100+115.8×𝐢", Gradian, Metric, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("acos(𝐢-4)", "2.8894-2.0966×𝐢", Radian, Metric, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(𝐢-4)", "165.551-120.126×𝐢", Degree, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("acos(𝐢-4)", "183.9-133.5×𝐢", Gradian, Metric, Cartesian, 4);
  // Key values
  assert_expression_approximates_to<double>("acos(0)", "90", Degree);
  assert_expression_approximates_to<float>("acos(-1)", "180", Degree);
  assert_expression_approximates_to<double>("acos(1)", "0", Degree);
  assert_expression_approximates_to<double>("acos(0)", "100", Gradian);
  assert_expression_approximates_to<float>("acos(-1)", "200", Gradian);
  assert_expression_approximates_to<double>("acos(1)", "0", Gradian);

  /* asin: [-1,1]    -> R
   *       ]-inf,-1[ -> -π/2+R×i (odd)
   *       ]1, inf[  -> π/2+R×i (odd)
   *       R×i       -> R×i (odd)
   */
  // On [-1, 1]
  assert_expression_approximates_to<double>("asin(0.5)", "0.5235987755983", Radian);
  assert_expression_approximates_to<double>("asin(0.03)", "3.0004501823477ᴇ-2", Radian);
  assert_expression_approximates_to<double>("asin(0.5)", "30", Degree);
  assert_expression_approximates_to<double>("asin(0.5)", "33.3333", Gradian, Metric, Cartesian, 6);
  // On [1, inf[
  assert_expression_approximates_to<double>("asin(2)", "1.5707963267949-1.3169578969248×𝐢", Radian);
  assert_expression_approximates_to<double>("asin(2)", "90-75.456129290217×𝐢", Degree);
  assert_expression_approximates_to<double>("asin(2)", "100-83.84×𝐢", Gradian, Metric, Cartesian, 4);
  // Symmetry: odd
  assert_expression_approximates_to<double>("asin(-2)", "-1.5707963267949+1.3169578969248×𝐢", Radian);
  assert_expression_approximates_to<double>("asin(-2)", "-90+75.456129290217×𝐢", Degree);
  assert_expression_approximates_to<double>("asin(-2)", "-100+83.84×𝐢", Gradian, Metric, Cartesian, 4);
  // On ]-inf, -1[
  assert_expression_approximates_to<float>("asin(-32)", "-1.571+4.159×𝐢", Radian, Metric, Cartesian, 4);
  assert_expression_approximates_to<float>("asin(-32)", "-90+238×𝐢", Degree, Metric, Cartesian, 3);
  assert_expression_approximates_to<float>("asin(-32)", "-100+265×𝐢", Gradian, Metric, Cartesian, 3);
  // On R×i
  assert_expression_approximates_to<double>("asin(3×𝐢)", "1.8184464592321×𝐢", Radian);
  assert_expression_approximates_to<double>("asin(3×𝐢)", "115.8×𝐢", Gradian, Metric, Cartesian, 4);
  // Symmetry: odd
  assert_expression_approximates_to<double>("asin(-3×𝐢)", "-1.8184464592321×𝐢", Radian);
  assert_expression_approximates_to<double>("asin(-3×𝐢)", "-115.8×𝐢", Gradian, Metric, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("asin(𝐢-4)", "-1.3186+2.0966×𝐢", Radian, Metric, Cartesian, 5);
  assert_expression_approximates_to<float>("asin(𝐢-4)", "-75.551+120.13×𝐢", Degree, Metric, Cartesian, 5);
  assert_expression_approximates_to<float>("asin(𝐢-4)", "-83.95+133.5×𝐢", Gradian, Metric, Cartesian, 4);
  // Key values
  assert_expression_approximates_to<double>("asin(0)", "0", Degree);
  assert_expression_approximates_to<double>("asin(0)", "0", Gradian);
  assert_expression_approximates_to<float>("asin(-1)", "-90", Degree, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("asin(-1)", "-100", Gradian, Metric, Cartesian, 6);
  assert_expression_approximates_to<double>("asin(1)", "90", Degree);
  assert_expression_approximates_to<double>("asin(1)", "100", Gradian, Metric, Cartesian);

  /* atan: R         ->  R (odd)
   *       [-𝐢,𝐢]    ->  R×𝐢 (odd)
   *       ]-inf×𝐢,-𝐢[ -> -π/2+R×𝐢 (odd)
   *       ]𝐢, inf×𝐢[  -> π/2+R×𝐢 (odd)
   */
  // On R
  assert_expression_approximates_to<double>("atan(2)", "1.1071487177941", Radian);
  assert_expression_approximates_to<double>("atan(0.01)", "9.9996666866652ᴇ-3", Radian);
  assert_expression_approximates_to<double>("atan(2)", "63.434948822922", Degree);
  assert_expression_approximates_to<double>("atan(2)", "70.48", Gradian, Metric, Cartesian, 4);
  assert_expression_approximates_to<float>("atan(0.5)", "0.4636476", Radian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("atan(-2)", "-1.1071487177941", Radian);
  assert_expression_approximates_to<double>("atan(-2)", "-63.434948822922", Degree);
  // On [-𝐢, 𝐢]
  assert_expression_approximates_to<float>("atan(0.2×𝐢)", "0.202733×𝐢", Radian, Metric, Cartesian, 6);
  // Symmetry: odd
  assert_expression_approximates_to<float>("atan(-0.2×𝐢)", "-0.202733×𝐢", Radian, Metric, Cartesian, 6);
  // On [𝐢, inf×𝐢[
  assert_expression_approximates_to<double>("atan(26×𝐢)", "1.5707963267949+3.8480520568064ᴇ-2×𝐢", Radian);
  assert_expression_approximates_to<double>("atan(26×𝐢)", "90+2.2047714220164×𝐢", Degree);
  assert_expression_approximates_to<double>("atan(26×𝐢)", "100+2.45×𝐢", Gradian, Metric, Cartesian, 3);
  // Symmetry: odd
  assert_expression_approximates_to<double>("atan(-26×𝐢)", "-1.5707963267949-3.8480520568064ᴇ-2×𝐢", Radian);
  assert_expression_approximates_to<double>("atan(-26×𝐢)", "-90-2.2047714220164×𝐢", Degree);
  assert_expression_approximates_to<double>("atan(-26×𝐢)", "-100-2.45×𝐢", Gradian, Metric, Cartesian, 3);
 // On ]-inf×𝐢, -𝐢[
  assert_expression_approximates_to<float>("atan(-3.4×𝐢)", "-1.570796-0.3030679×𝐢", Radian);
  assert_expression_approximates_to<float>("atan(-3.4×𝐢)", "-90-17.3645×𝐢", Degree, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("atan(-3.4×𝐢)", "-100-19.29×𝐢", Gradian, Metric, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("atan(𝐢-4)", "-1.338973+0.05578589×𝐢", Radian);
  assert_expression_approximates_to<float>("atan(𝐢-4)", "-76.7175+3.1963×𝐢", Degree, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("atan(𝐢-4)", "-85.24+3.551×𝐢", Gradian, Metric, Cartesian, 4);
  // Key values
  assert_expression_approximates_to<float>("atan(0)", "0", Degree);
  assert_expression_approximates_to<float>("atan(0)", "0", Gradian);
  assert_expression_approximates_to<double>("atan(-𝐢)", "-∞×𝐢", Radian);
  assert_expression_approximates_to<double>("atan(𝐢)", "∞×𝐢", Radian);

  /* cosh: R         -> R (even)
   *       R×𝐢       -> R (oscillator)
   */
  // On R
  assert_expression_approximates_to<double>("cosh(2)", "3.7621956910836", Radian);
  assert_expression_approximates_to<double>("cosh(2)", "3.7621956910836", Degree);
  assert_expression_approximates_to<double>("cosh(2)", "3.7621956910836", Gradian);
  // Symmetry: even
  assert_expression_approximates_to<double>("cosh(-2)", "3.7621956910836", Radian);
  assert_expression_approximates_to<double>("cosh(-2)", "3.7621956910836", Degree);
  assert_expression_approximates_to<double>("cosh(-2)", "3.7621956910836", Gradian);
  // On R×𝐢
  assert_expression_approximates_to<double>("cosh(43×𝐢)", "5.5511330152063ᴇ-1", Radian);
  // Oscillator
  assert_expression_approximates_to<float>("cosh(π×𝐢/2)", "0", Radian);
  assert_expression_approximates_to<float>("cosh(5×π×𝐢/2)", "0", Radian);
  assert_expression_approximates_to<float>("cosh(8×π×𝐢/2)", "1", Radian);
  assert_expression_approximates_to<float>("cosh(9×π×𝐢/2)", "0", Radian);
  // On C
  assert_expression_approximates_to<float>("cosh(𝐢-4)", "14.7547-22.9637×𝐢", Radian, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("cosh(𝐢-4)", "14.7547-22.9637×𝐢", Degree, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("cosh(𝐢-4)", "14.7547-22.9637×𝐢", Gradian, Metric, Cartesian, 6);

  /* sinh: R         -> R (odd)
   *       R×𝐢       -> R×𝐢 (oscillator)
   */
  // On R
  assert_expression_approximates_to<double>("sinh(2)", "3.626860407847", Radian);
  assert_expression_approximates_to<double>("sinh(2)", "3.626860407847", Degree);
  assert_expression_approximates_to<double>("sinh(2)", "3.626860407847", Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("sinh(-2)", "-3.626860407847", Radian);
  // On R×𝐢
  assert_expression_approximates_to<double>("sinh(43×𝐢)", "-0.8317747426286×𝐢", Radian);
  // Oscillator
  assert_expression_approximates_to<float>("sinh(π×𝐢/2)", "𝐢", Radian);
  assert_expression_approximates_to<float>("sinh(5×π×𝐢/2)", "𝐢", Radian);
  assert_expression_approximates_to<float>("sinh(7×π×𝐢/2)", "-𝐢", Radian);
  assert_expression_approximates_to<float>("sinh(8×π×𝐢/2)", "0", Radian);
  assert_expression_approximates_to<float>("sinh(9×π×𝐢/2)", "𝐢", Radian);
  // On C
  assert_expression_approximates_to<float>("sinh(𝐢-4)", "-14.7448+22.9791×𝐢", Radian, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("sinh(𝐢-4)", "-14.7448+22.9791×𝐢", Degree, Metric, Cartesian, 6);

  /* tanh: R         -> R (odd)
   *       R×𝐢       -> R×𝐢 (tangent-style)
   */
  // On R
  assert_expression_approximates_to<double>("tanh(2)", "9.6402758007582ᴇ-1", Radian);
  assert_expression_approximates_to<double>("tanh(2)", "9.6402758007582ᴇ-1", Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("tanh(-2)", "-9.6402758007582ᴇ-1", Degree);
  // On R×i
  assert_expression_approximates_to<double>("tanh(43×𝐢)", "-1.4983873388552×𝐢", Radian);
  // Tangent-style
  // FIXME: this depends on the libm implementation and does not work on travis/appveyor servers
  /*assert_expression_approximates_to<float>("tanh(π×𝐢/2)", Undefined::Name(), Radian);
  assert_expression_approximates_to<float>("tanh(5×π×𝐢/2)", Undefined::Name(), Radian);
  assert_expression_approximates_to<float>("tanh(7×π×𝐢/2)", Undefined::Name(), Radian);
  assert_expression_approximates_to<float>("tanh(8×π×𝐢/2)", "0", Radian);
  assert_expression_approximates_to<float>("tanh(9×π×𝐢/2)", Undefined::Name(), Radian);*/
  // On C
  assert_expression_approximates_to<float>("tanh(𝐢-4)", "-1.00028+0.000610241×𝐢", Radian, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("tanh(𝐢-4)", "-1.00028+0.000610241×𝐢", Degree, Metric, Cartesian, 6);

  /* acosh: [-1,1]       -> R×𝐢
   *        ]-inf,-1[    -> π×𝐢+R (even on real)
   *        ]1, inf[     -> R (even on real)
   *        ]-inf×𝐢, 0[  -> -π/2×𝐢+R (even on real)
   *        ]0, inf*𝐢[   -> π/2×𝐢+R (even on real)
   */
  // On [-1,1]
  assert_expression_approximates_to<double>("acosh(2)", "1.3169578969248", Radian);
  assert_expression_approximates_to<double>("acosh(2)", "1.3169578969248", Degree);
  assert_expression_approximates_to<double>("acosh(2)", "1.3169578969248", Gradian);
  // On ]-inf, -1[
  assert_expression_approximates_to<double>("acosh(-4)", "2.0634370688956+3.1415926535898×𝐢", Radian);
  assert_expression_approximates_to<float>("acosh(-4)", "2.06344+3.14159×𝐢", Radian, Metric, Cartesian, 6);
  // On ]1,inf[: Symmetry: even on real
  assert_expression_approximates_to<double>("acosh(4)", "2.0634370688956", Radian);
  assert_expression_approximates_to<float>("acosh(4)", "2.063437", Radian);
  // On ]-inf×𝐢, 0[
  assert_expression_approximates_to<double>("acosh(-42×𝐢)", "4.4309584920805-1.5707963267949×𝐢", Radian);
  assert_expression_approximates_to<float>("acosh(-42×𝐢)", "4.431-1.571×𝐢", Radian, Metric, Cartesian, 4);
  // On ]0, 𝐢×inf[: Symmetry: even on real
  assert_expression_approximates_to<double>("acosh(42×𝐢)", "4.4309584920805+1.5707963267949×𝐢", Radian);
  assert_expression_approximates_to<float>("acosh(42×𝐢)", "4.431+1.571×𝐢", Radian, Metric, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("acosh(𝐢-4)", "2.0966+2.8894×𝐢", Radian, Metric, Cartesian, 5);
  assert_expression_approximates_to<float>("acosh(𝐢-4)", "2.0966+2.8894×𝐢", Degree, Metric, Cartesian, 5);
  // Key values
  //assert_expression_approximates_to<double>("acosh(-1)", "3.1415926535898×𝐢", Radian);
  assert_expression_approximates_to<double>("acosh(1)", "0", Radian);
  assert_expression_approximates_to<float>("acosh(0)", "1.570796×𝐢", Radian);

  /* asinh: R            -> R (odd)
   *        [-𝐢,𝐢]       -> R*𝐢 (odd)
   *        ]-inf×𝐢,-𝐢[  -> -π/2×𝐢+R (odd)
   *        ]𝐢, inf×𝐢[   -> π/2×𝐢+R (odd)
   */
  // On R
  assert_expression_approximates_to<double>("asinh(2)", "1.4436354751788", Radian);
  assert_expression_approximates_to<double>("asinh(2)", "1.4436354751788", Degree);
  assert_expression_approximates_to<double>("asinh(2)", "1.4436354751788", Gradian);
 // Symmetry: odd
  assert_expression_approximates_to<double>("asinh(-2)", "-1.4436354751788", Radian);
  assert_expression_approximates_to<double>("asinh(-2)", "-1.4436354751788", Degree);
  // On [-𝐢,𝐢]
  assert_expression_approximates_to<double>("asinh(0.2×𝐢)", "2.0135792079033ᴇ-1×𝐢", Radian);
  // asinh(0.2*i) has a too low precision in float on the web platform
  assert_expression_approximates_to<float>("asinh(0.3×𝐢)", "0.3046927×𝐢", Degree);
  // Symmetry: odd
  assert_expression_approximates_to<double>("asinh(-0.2×𝐢)", "-2.0135792079033ᴇ-1×𝐢", Radian);
  // asinh(-0.2*i) has a too low precision in float on the web platform
  assert_expression_approximates_to<float>("asinh(-0.3×𝐢)", "-0.3046927×𝐢", Degree);
  // On ]-inf×𝐢, -𝐢[
  assert_expression_approximates_to<double>("asinh(-22×𝐢)", "-3.7836727043295-1.5707963267949×𝐢", Radian);
  assert_expression_approximates_to<float>("asinh(-22×𝐢)", "-3.784-1.571×𝐢", Degree, Metric, Cartesian, 4);
  // On ]𝐢, inf×𝐢[, Symmetry: odd
  assert_expression_approximates_to<double>("asinh(22×𝐢)", "3.7836727043295+1.5707963267949×𝐢", Radian);
  assert_expression_approximates_to<float>("asinh(22×𝐢)", "3.784+1.571×𝐢", Degree, Metric, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("asinh(𝐢-4)", "-2.123+0.2383×𝐢", Radian, Metric, Cartesian, 4);
  assert_expression_approximates_to<float>("asinh(𝐢-4)", "-2.123+0.2383×𝐢", Degree, Metric, Cartesian, 4);

  /* atanh: [-1,1]       -> R (odd)
   *        ]-inf,-1[    -> π/2*𝐢+R (odd)
   *        ]1, inf[     -> -π/2×𝐢+R (odd)
   *        R×𝐢          -> R×𝐢 (odd)
   */
  // On [-1,1]
  assert_expression_approximates_to<double>("atanh(0.4)", "0.4236489301936", Radian);
  assert_expression_approximates_to<double>("atanh(0.4)", "0.4236489301936", Degree);
  assert_expression_approximates_to<double>("atanh(0.4)", "0.4236489301936", Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("atanh(-0.4)", "-0.4236489301936", Radian);
  assert_expression_approximates_to<double>("atanh(-0.4)", "-0.4236489301936", Degree);
  // On ]1, inf[
  assert_expression_approximates_to<double>("atanh(4)", "0.255412811883-1.5707963267949×𝐢", Radian);
  assert_expression_approximates_to<float>("atanh(4)", "0.2554128-1.570796×𝐢", Degree);
  // On ]-inf,-1[, Symmetry: odd
  assert_expression_approximates_to<double>("atanh(-4)", "-0.255412811883+1.5707963267949×𝐢", Radian);
  assert_expression_approximates_to<float>("atanh(-4)", "-0.2554128+1.570796×𝐢", Degree);
  // On R×𝐢
  assert_expression_approximates_to<double>("atanh(4×𝐢)", "1.325817663668×𝐢", Radian);
  assert_expression_approximates_to<float>("atanh(4×𝐢)", "1.325818×𝐢", Radian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("atanh(-4×𝐢)", "-1.325817663668×𝐢", Radian);
  assert_expression_approximates_to<float>("atanh(-4×𝐢)", "-1.325818×𝐢", Radian);
  // On C
  assert_expression_approximates_to<float>("atanh(𝐢-4)", "-0.238878+1.50862×𝐢", Radian, Metric, Cartesian, 6);
  assert_expression_approximates_to<float>("atanh(𝐢-4)", "-0.238878+1.50862×𝐢", Degree, Metric, Cartesian, 6);

  // Check that the complex part is not neglected
  assert_expression_approximates_to<double>("atanh(0.99999999999+1.0ᴇ-26×𝐢)", "13+5ᴇ-16×𝐢", Radian, Metric, Cartesian, 3);
  assert_expression_approximates_to<double>("atanh(0.99999999999+1.0ᴇ-60×𝐢)", "13+5ᴇ-50×𝐢", Radian, Metric, Cartesian, 3);
  assert_expression_approximates_to<double>("atanh(0.99999999999+1.0ᴇ-150×𝐢)", "13+5ᴇ-140×𝐢", Radian, Metric, Cartesian, 3);
  assert_expression_approximates_to<double>("atanh(0.99999999999+1.0ᴇ-250×𝐢)", "13+5ᴇ-240×𝐢", Radian, Metric, Cartesian, 3);
  assert_expression_approximates_to<double>("atanh(0.99999999999+1.0ᴇ-300×𝐢)", "13+5ᴇ-290×𝐢", Radian, Metric, Cartesian, 3);

  // WARNING: evaluate on branch cut can be multivalued
  assert_expression_approximates_to<double>("acos(2)", "1.3169578969248×𝐢", Radian);
  assert_expression_approximates_to<double>("acos(2)", "75.456129290217×𝐢", Degree);
  assert_expression_approximates_to<double>("asin(2)", "1.5707963267949-1.3169578969248×𝐢", Radian);
  assert_expression_approximates_to<double>("asin(2)", "90-75.456129290217×𝐢", Degree);
  assert_expression_approximates_to<double>("atanh(2)", "5.4930614433405ᴇ-1-1.5707963267949×𝐢", Radian);
  assert_expression_approximates_to<double>("atan(2𝐢)", "1.5707963267949+5.4930614433405ᴇ-1×𝐢", Radian);
  assert_expression_approximates_to<double>("atan(2𝐢)", "90+31.472923730945×𝐢", Degree);
  assert_expression_approximates_to<double>("asinh(2𝐢)", "1.3169578969248+1.5707963267949×𝐢", Radian);
  assert_expression_approximates_to<double>("acosh(-2)", "1.3169578969248+3.1415926535898×𝐢", Radian);
}

QUIZ_CASE(poincare_approximation_matrix) {
  assert_expression_approximates_to<float>("[[1,2,3][4,5,6]]", "[[1,2,3][4,5,6]]");
  assert_expression_approximates_to<double>("[[1,2,3][4,5,6]]", "[[1,2,3][4,5,6]]");
}

QUIZ_CASE(poincare_approximation_store) {
  assert_expression_approximates_to<float>("1+42→A", "43");
  assert_expression_approximates_to<double>("0.123+𝐢→B", "0.123+𝐢");

  assert_expression_approximates_to_scalar<float>("1+42→A", 43.0f);
  assert_expression_approximates_to_scalar<double>("0.123+𝐢→B", NAN);

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("A.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("B.exp").destroy();
}

QUIZ_CASE(poincare_approximation_store_matrix) {
  assert_expression_approximates_to<double>("[[7]]→a", "[[7]]");

  assert_expression_approximates_to_scalar<float>("[[7]]→a", NAN);

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
}

QUIZ_CASE(poincare_approximation_complex_format) {
  // Real
  assert_expression_approximates_to<float>("0", "0", Radian, Metric, Real);
  assert_expression_approximates_to<double>("0", "0", Radian, Metric, Real);
  assert_expression_approximates_to<float>("10", "10", Radian, Metric, Real);
  assert_expression_approximates_to<double>("-10", "-10", Radian, Metric, Real);
  assert_expression_approximates_to<float>("100", "100", Radian, Metric, Real);
  assert_expression_approximates_to<double>("0.1", "0.1", Radian, Metric, Real);
  assert_expression_approximates_to<float>("0.1234567", "0.1234567", Radian, Metric, Real);
  assert_expression_approximates_to<double>("0.123456789012345", "1.2345678901235ᴇ-1", Radian, Metric, Real);
  assert_expression_approximates_to<float>("1+2×𝐢", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<double>("1+𝐢-𝐢", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<float>("1+𝐢-1", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<double>("1+𝐢", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<float>("3+𝐢", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<double>("3-𝐢", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<float>("3-𝐢-3", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<float>("𝐢", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<double>("√(-1)", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<double>("√(-1)×√(-1)", "unreal", Radian, Metric, Real);
  assert_expression_approximates_to<double>("ln(-2)", "unreal", Radian, Metric, Real);
  // Power/Root approximates to the first REAL root in Real mode
  assert_expression_simplifies_approximates_to<double>("(-8)^(1/3)", "-2", Radian, Metric, Real); // Power have to be simplified first in order to spot the right form c^(p/q) with p, q integers
  assert_expression_approximates_to<double>("root(-8,3)", "-2", Radian, Metric, Real); // Root approximates to the first REAL root in Real mode
  assert_expression_approximates_to<double>("8^(1/3)", "2", Radian, Metric, Real);
  assert_expression_simplifies_approximates_to<float>("(-8)^(2/3)", "4", Radian, Metric, Real); // Power have to be simplified first (cf previous comment)
  assert_expression_approximates_to<float>("root(-8, 3)^2", "4", Radian, Metric, Real);
  assert_expression_approximates_to<double>("root(-8,3)", "-2", Radian, Metric, Real);

  // Cartesian
  assert_expression_approximates_to<float>("0", "0", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("0", "0", Radian, Metric, Cartesian);
  assert_expression_approximates_to<float>("10", "10", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("-10", "-10", Radian, Metric, Cartesian);
  assert_expression_approximates_to<float>("100", "100", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("0.1", "0.1", Radian, Metric, Cartesian);
  assert_expression_approximates_to<float>("0.1234567", "0.1234567", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("0.123456789012345", "1.2345678901235ᴇ-1", Radian, Metric, Cartesian);
  assert_expression_approximates_to<float>("1+2×𝐢", "1+2×𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("1+𝐢-𝐢", "1", Radian, Metric, Cartesian);
  assert_expression_approximates_to<float>("1+𝐢-1", "𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("1+𝐢", "1+𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<float>("3+𝐢", "3+𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("3-𝐢", "3-𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<float>("3-𝐢-3", "-𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<float>("𝐢", "𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("√(-1)", "𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("√(-1)×√(-1)", "-1", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("ln(-2)", "6.9314718055995ᴇ-1+3.1415926535898×𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("(-8)^(1/3)", "1+1.7320508075689×𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<float>("(-8)^(2/3)", "-2+3.4641×𝐢", Radian, Metric, Cartesian, 6);
  assert_expression_approximates_to<double>("root(-8,3)", "1+1.7320508075689×𝐢", Radian, Metric, Cartesian);

  // Polar
  assert_expression_approximates_to<float>("0", "0", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("0", "0", Radian, Metric, Polar);
  assert_expression_approximates_to<float>("10", "10", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("-10", "10×ℯ^\u00123.1415926535898×𝐢\u0013", Radian, Metric, Polar);

  assert_expression_approximates_to<float>("100", "100", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("0.1", "0.1", Radian, Metric, Polar);
  assert_expression_approximates_to<float>("0.1234567", "0.1234567", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("0.12345678", "0.12345678", Radian, Metric, Polar);

  assert_expression_approximates_to<float>("1+2×𝐢", "2.236068×ℯ^\u00121.107149×𝐢\u0013", Radian, Metric, Polar);
  assert_expression_approximates_to<float>("1+𝐢-𝐢", "1", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("1+𝐢-1", "ℯ^\u00121.57079632679×𝐢\u0013", Radian, Metric, Polar, 12);
  assert_expression_approximates_to<float>("1+𝐢", "1.414214×ℯ^\u00120.7853982×𝐢\u0013", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("3+𝐢", "3.16227766017×ℯ^\u00120.321750554397×𝐢\u0013", Radian, Metric, Polar,12);
  assert_expression_approximates_to<float>("3-𝐢", "3.162278×ℯ^\u0012-0.3217506×𝐢\u0013", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("3-𝐢-3", "ℯ^\u0012-1.57079632679×𝐢\u0013", Radian, Metric, Polar,12);
   // 2ℯ^(𝐢) has a too low precision in float on the web platform
  assert_expression_approximates_to<float>("3ℯ^(2*𝐢)", "3×ℯ^\u00122×𝐢\u0013", Radian, Metric, Polar, 4);
  assert_expression_approximates_to<double>("2ℯ^(-𝐢)", "2×ℯ^\u0012-𝐢\u0013", Radian, Metric, Polar, 9);

  assert_expression_approximates_to<float>("𝐢", "ℯ^\u00121.570796×𝐢\u0013", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("√(-1)", "ℯ^\u00121.5707963267949×𝐢\u0013", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("√(-1)×√(-1)", "ℯ^\u00123.1415926535898×𝐢\u0013", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("(-8)^(1/3)", "2×ℯ^\u00121.0471975511966×𝐢\u0013", Radian, Metric, Polar);
  assert_expression_approximates_to<float>("(-8)^(2/3)", "4×ℯ^\u00122.094395×𝐢\u0013", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("root(-8,3)", "2×ℯ^\u00121.0471975511966×𝐢\u0013", Radian, Metric, Polar);

  // Cartesian to Polar and vice versa
  assert_expression_approximates_to<double>("2+3×𝐢", "3.60555127546×ℯ^\u00120.982793723247×𝐢\u0013", Radian, Metric, Polar, 12);
  assert_expression_approximates_to<double>("3.60555127546×ℯ^(0.982793723247×𝐢)", "2+3×𝐢", Radian, Metric, Cartesian, 12);
  assert_expression_approximates_to<float>("12.04159457879229548012824103×ℯ^(1.4876550949×𝐢)", "1+12×𝐢", Radian, Metric, Cartesian, 5);

  // Overflow
  assert_expression_approximates_to<float>("-2ᴇ20+2ᴇ20×𝐢", "-2ᴇ20+2ᴇ20×𝐢", Radian, Metric, Cartesian);
  /* TODO: this test fails on the device because libm hypotf (which is called
   * eventually by std::abs) is not accurate enough. We might change the
   * embedded libm? */
  //assert_expression_approximates_to<float>("-2ᴇ20+2ᴇ20×𝐢", "2.828427ᴇ20×ℯ^\u00122.356194×𝐢\u0013", Radian, Metric, Polar);
  assert_expression_approximates_to<float>("-2ᴇ10+2ᴇ10×𝐢", "2.828427ᴇ10×ℯ^\u00122.356194×𝐢\u0013", Radian, Metric, Polar);
  assert_expression_approximates_to<double>("1ᴇ155-1ᴇ155×𝐢", "1ᴇ155-1ᴇ155×𝐢", Radian, Metric, Cartesian);
  assert_expression_approximates_to<double>("1ᴇ155-1ᴇ155×𝐢", "1.41421356237ᴇ155×ℯ^\u0012-0.785398163397×𝐢\u0013", Radian, Metric, Polar,12);
  assert_expression_approximates_to<float>("-2ᴇ100+2ᴇ100×𝐢", Undefined::Name());
  assert_expression_approximates_to<double>("-2ᴇ360+2ᴇ360×𝐢", Undefined::Name());
  assert_expression_approximates_to<float>("-2ᴇ100+2ᴇ10×𝐢", "-∞+2ᴇ10×𝐢");
  assert_expression_approximates_to<double>("-2ᴇ360+2×𝐢", "-∞+2×𝐢");
  assert_expression_approximates_to<float>("undef+2ᴇ100×𝐢", Undefined::Name());
  assert_expression_approximates_to<double>("-2ᴇ360+undef×𝐢", Undefined::Name());
}

QUIZ_CASE(poincare_approximation_mix) {
  assert_expression_approximates_to<float>("-2-3", "-5");
  assert_expression_approximates_to<float>("1.2×ℯ^(1)", "3.261938");
  assert_expression_approximates_to<float>("2ℯ^(3)", "40.1711", Radian, Metric, Cartesian, 6); // WARNING: the 7th significant digit is wrong on blackbox simulator
  assert_expression_approximates_to<float>("ℯ^2×ℯ^(1)", "20.0855", Radian, Metric, Cartesian, 6); // WARNING: the 7th significant digit is wrong on simulator
  assert_expression_approximates_to<double>("ℯ^2×ℯ^(1)", "20.085536923188");
  assert_expression_approximates_to<double>("2×3^4+2", "164");
  assert_expression_approximates_to<float>("-2×3^4+2", "-160");
  assert_expression_approximates_to<double>("-sin(3)×2-3", "-3.2822400161197", Radian);
  assert_expression_approximates_to<float>("5-2/3", "4.333333");
  assert_expression_approximates_to<double>("2/3-5", "-4.3333333333333");
  assert_expression_approximates_to<float>("-2/3-5", "-5.666667");
  assert_expression_approximates_to<double>("sin(3)2(4+2)", "1.6934400967184", Radian);
  assert_expression_approximates_to<float>("4/2×(2+3)", "10");
  assert_expression_approximates_to<double>("4/2×(2+3)", "10");

  assert_expression_simplifies_and_approximates_to("1.0092^(20)", "1.2010050593402");
  assert_expression_simplifies_and_approximates_to("1.0092^(50)×ln(3/2)", "0.6409373488899", Degree, Metric, Cartesian, 13);
  assert_expression_simplifies_and_approximates_to("1.0092^(50)×ln(1.0092)", "1.447637354655ᴇ-2", Degree, Metric, Cartesian, 13);
  assert_expression_approximates_to<double>("1.0092^(20)", "1.2010050593402");
  assert_expression_approximates_to<double>("1.0092^(50)×ln(3/2)", "0.6409373488899", Degree, Metric, Cartesian, 13);
  assert_expression_approximates_to<double>("1.0092^(50)×ln(1.0092)", "1.447637354655ᴇ-2", Degree, Metric, Cartesian, 13);
  assert_expression_simplifies_approximates_to<double>("1.0092^(20)", "1.2010050593402");
  assert_expression_simplifies_approximates_to<double>("1.0092^(50)×ln(3/2)", "0.6409373488899", Degree, Metric, Cartesian, 13);
  //assert_expression_approximates_to<float>("1.0092^(20)", "1.201005"); TODO does not work
  assert_expression_approximates_to<float>("1.0092^(50)×ln(3/2)", "0.6409366");
  //assert_expression_simplifies_approximates_to<float>("1.0092^(20)", "1.2010050593402"); TODO does not work
  //assert_expression_simplifies_approximates_to<float>("1.0092^(50)×ln(3/2)", "6.4093734888993ᴇ-1"); TODO does not work
}


template void assert_expression_approximates_to_scalar(const char * expression, float approximation, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat);
template void assert_expression_approximates_to_scalar(const char * expression, double approximation, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat);

