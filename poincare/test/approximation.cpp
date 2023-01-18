#include <apps/shared/global_context.h>
#include <poincare/constant.h>
#include <poincare/infinity.h>
#include <poincare/list_sort.h>
#include <poincare/undefined.h>
#include "helper.h"

using namespace Poincare;

template<typename T>
void assert_expression_approximates_to_scalar(const char * expression, T approximation, Preferences::AngleUnit angleUnit = Degree, Preferences::ComplexFormat complexFormat = Cartesian, Preferences::MixedFractions mixedFractionsParameter = Preferences::MixedFractions::Enabled) {
  Shared::GlobalContext globalContext;
  Preferences::sharedPreferences()->enableMixedFractions(mixedFractionsParameter);
  Expression e = parse_expression(expression, &globalContext, false);
  T result = e.approximateToScalar<T>(&globalContext, complexFormat, angleUnit);
  quiz_assert_print_if_failure(roughly_equal(result, approximation, Poincare::Float<T>::EpsilonLax(), true), expression);
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
  assert_expression_approximates_to<double>("123456/1234567", "0.099999432999586");

  assert_expression_approximates_to_scalar<float>("1/3", 0.3333333f);
  assert_expression_approximates_to_scalar<double>("123456/1234567", 9.9999432999586E-2);
}

template<typename T>
void assert_float_approximates_to(Float<T> f, const char * result) {
  Shared::GlobalContext globalContext;
  int numberOfDigits = PrintFloat::SignificantDecimalDigits<T>();
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
  assert_float_approximates_to<double>(Float<double>::Builder(0.99999999999995), "0.99999999999995");
  assert_float_approximates_to<double>(Float<double>::Builder(0.00000099999999999995), "9.9999999999995ᴇ-7");
  assert_float_approximates_to<double>(Float<double>::Builder(0.0000009999999999901200121020102010201201201021099995), "9.9999999999012ᴇ-7");
  assert_float_approximates_to<float>(Float<float>::Builder(1.2345E-1), "0.12345");
  assert_float_approximates_to<float>(Float<float>::Builder(1), "1");
  assert_float_approximates_to<float>(Float<float>::Builder(0.9999999999999995), "1");
  assert_float_approximates_to<float>(Float<float>::Builder(1.2345E6), "1234500");
  assert_float_approximates_to<float>(Float<float>::Builder(-1.2345E6), "-1234500");
  assert_float_approximates_to<float>(Float<float>::Builder(0.0000009999999999999995), "1ᴇ-6");
  assert_float_approximates_to<float>(Float<float>::Builder(-1.2345E-1), "-0.12345");

  assert_float_approximates_to<double>(Float<double>::Builder(INFINITY), Infinity::Name());
  assert_float_approximates_to<float>(Float<float>::Builder(0.0f), "0");
  assert_float_approximates_to<float>(Float<float>::Builder(NAN), Undefined::Name());
}

QUIZ_CASE(poincare_approximation_infinity) {
  assert_expression_approximates_to<double>("10^1000", Infinity::Name());
  assert_expression_approximates_to<double>("2*10^1000", Infinity::Name());
  assert_expression_approximates_to<double>("(10^1000)/2", Infinity::Name());
  assert_expression_approximates_to_scalar<double>("10^1000", INFINITY);
  assert_expression_approximates_to<double>("(∞)×(i)", "∞×i");
  assert_expression_approximates_to<double>("(inf×i)×(i)", Infinity::Name(true));
  assert_expression_approximates_to<double>("(inf×i)×(2)", "∞×i");
  // (inf+i)×(2) = inf * 2 - 1 * 0 + i * (inf * 0 + 1 * 2), inf * 0 return NAN
  assert_expression_approximates_to<double>("(inf+i)×(2)", Undefined::Name());
}

QUIZ_CASE(poincare_approximation_addition) {
  assert_expression_approximates_to<float>("1+2", "3");
  assert_expression_approximates_to<float>("i+i", "2×i");
  assert_expression_approximates_to<double>("2+i+4+i", "6+2×i");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]+3", Undefined::Name());
  assert_expression_approximates_to<double>("[[1,2+i][3,4][5,6]]+3+i", Undefined::Name());
  assert_expression_approximates_to<float>("3+[[1,2][3,4][5,6]]", Undefined::Name());
  assert_expression_approximates_to<double>("3+i+[[1,2+i][3,4][5,6]]", Undefined::Name());
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]", "[[2,4][6,8][10,12]]");
  assert_expression_approximates_to<double>("[[1,2+i][3,4][5,6]]+[[1,2+i][3,4][5,6]]", "[[2,4+2×i][6,8][10,12]]");

  assert_expression_approximates_to<float>("{1,2,3}+10", "{11,12,13}");
  assert_expression_approximates_to<float>("10+{1,2,3}", "{11,12,13}");
  assert_expression_approximates_to<float>("{1,2,3}+{4,5,6}", "{5,7,9}");
  assert_expression_approximates_to<float>("{1,2,3}+{4,5}", Undefined::Name());
  assert_expression_approximates_to<float>("{1,2,3}+[[4,5,6]]", Undefined::Name());
  assert_expression_approximates_to<double>("{1,2,3}+10", "{11,12,13}");
  assert_expression_approximates_to<double>("10+{1,2,3}", "{11,12,13}");
  assert_expression_approximates_to<double>("{1,2,3}+{4,5,6}", "{5,7,9}");
  assert_expression_approximates_to<double>("{1,2,3}+{4,5}", Undefined::Name());
  assert_expression_approximates_to<double>("{1,2,3}+[[4,5,6]]", Undefined::Name());

  assert_expression_approximates_to_scalar<float>("1+2", 3.0f);
  assert_expression_approximates_to_scalar<double>("i+i", NAN);
  assert_expression_approximates_to_scalar<float>("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]", NAN);
}

QUIZ_CASE(poincare_approximation_multiplication) {
  assert_expression_approximates_to<float>("1×2", "2");
  assert_expression_approximates_to<double>("(3+i)×(4+i)", "11+7×i");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]×2", "[[2,4][6,8][10,12]]");
  assert_expression_approximates_to<double>("[[1,2+i][3,4][5,6]]×(3+i)", "[[3+i,5+5×i][9+3×i,12+4×i][15+5×i,18+6×i]]");
  assert_expression_approximates_to<float>("2×[[1,2][3,4][5,6]]", "[[2,4][6,8][10,12]]");
  assert_expression_approximates_to<double>("(3+i)×[[1,2+i][3,4][5,6]]", "[[3+i,5+5×i][9+3×i,12+4×i][15+5×i,18+6×i]]");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]×[[1,2,3,4][5,6,7,8]]", "[[11,14,17,20][23,30,37,44][35,46,57,68]]");
  assert_expression_approximates_to<double>("[[1,2+i][3,4][5,6]]×[[1,2+i,3,4][5,6+i,7,8]]", "[[11+5×i,13+9×i,17+7×i,20+8×i][23,30+7×i,37,44][35,46+11×i,57,68]]");

  assert_expression_approximates_to<float>("{1,2,3}×10", "{10,20,30}");
  assert_expression_approximates_to<float>("10×{1,2,3}", "{10,20,30}");
  assert_expression_approximates_to<float>("{1,2,3}×{4,5,6}", "{4,10,18}");
  assert_expression_approximates_to<float>("{1,2,3}×{4,5}", Undefined::Name());
  assert_expression_approximates_to<float>("{1,2,3}×[[4,5,6]]", Undefined::Name());
  assert_expression_approximates_to<double>("{1,2,3}×10", "{10,20,30}");
  assert_expression_approximates_to<double>("10×{1,2,3}", "{10,20,30}");
  assert_expression_approximates_to<double>("{1,2,3}×{4,5,6}", "{4,10,18}");
  assert_expression_approximates_to<double>("{1,2,3}×{4,5}", Undefined::Name());
  assert_expression_approximates_to<double>("{1,2,3}×[[4,5,6]]", Undefined::Name());

  assert_expression_approximates_to_scalar<float>("1×2", 2.0f);
  assert_expression_approximates_to_scalar<double>("(3+i)×(4+i)", NAN);
  assert_expression_approximates_to_scalar<float>("[[1,2][3,4][5,6]]×2", NAN);
}

QUIZ_CASE(poincare_approximation_power) {
  assert_expression_approximates_to<float>("2^3", "8");
  assert_expression_approximates_to<double>("(3+i)^4", "28+96×i");
  assert_expression_approximates_to<float>("4^(3+i)", "11.74125+62.91378×i");
  assert_expression_approximates_to<double>("(3+i)^(3+i)", "-11.898191759852+19.592921596609×i");

  assert_expression_approximates_to<double>("0^0", Undefined::Name());
  assert_expression_approximates_to<double>("0^2", "0");
  assert_expression_approximates_to<double>("0^(-2)", Undefined::Name());

  assert_expression_approximates_to<double>("(-2)^4.2", "14.8690638497+10.8030072384×i", Radian, MetricUnitFormat, Cartesian, 12);
  assert_expression_approximates_to<double>("(-0.1)^4", "1ᴇ-4", Radian, MetricUnitFormat, Cartesian, 12);

  assert_expression_approximates_to<float>("0^2", "0");
  assert_expression_approximates_to<double>("i^i", "0.20787957635076");
  assert_expression_approximates_to<float>("1.0066666666667^60", "1.48985", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("1.0066666666667^60", "1.489845708305", Radian, MetricUnitFormat, Cartesian, 13);
  assert_expression_approximates_to<double>("1.0092^50", "1.5807460027336");
  assert_expression_approximates_to<float>("1.0092^50", "1.580744");
  assert_expression_approximates_to<float>("e^(i×π)", "-1");
  assert_expression_approximates_to<double>("e^(i×π)", "-1");
  assert_expression_approximates_to<float>("e^(i×π+2)", "-7.38906", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("e^(i×π+2)", "-7.3890560989307");
  assert_expression_approximates_to<float>("(-1)^(1/3)", "0.5+0.8660254×i");
  assert_expression_approximates_to<double>("(-1)^(1/3)", "0.5+0.86602540378444×i");
  assert_expression_approximates_to<float>("e^(i×π/3)", "0.5+0.866025×i", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("e^(i×π/3)", "0.5+0.86602540378444×i");
  assert_expression_approximates_to<float>("i^(2/3)", "0.5+0.8660254×i");
  assert_expression_approximates_to<double>("i^(2/3)", "0.5+0.86602540378444×i");

  assert_expression_approximates_to<float>("{1,2,3}^2", "{1,4,9}");
  assert_expression_approximates_to<float>("2^{1,2,3}", "{2,4,8}");
  assert_expression_approximates_to<float>("{1,2,3}^{1,2,3}", "{1,4,27}");
  assert_expression_approximates_to<float>("{1,2,3}^{4,5}", Undefined::Name());
  assert_expression_approximates_to<float>("{1,2,3}^[[4,5,6]]", Undefined::Name());
  assert_expression_approximates_to<double>("{1,2,3}^2", "{1,4,9}");
  assert_expression_approximates_to<double>("2^{1,2,3}", "{2,4,8}");
  assert_expression_approximates_to<double>("{1,2,3}^{1,2,3}", "{1,4,27}");
  assert_expression_approximates_to<double>("{1,2,3}^{4,5}", Undefined::Name());
  assert_expression_approximates_to<double>("{1,2,3}^[[4,5,6]]", Undefined::Name());

  assert_expression_approximates_to_scalar<float>("2^3", 8.0f);
  assert_expression_approximates_to_scalar<double>("(3+i)^(4+i)", NAN);
  assert_expression_approximates_to_scalar<float>("[[1,2][3,4]]^2", NAN);


  assert_expression_approximates_to<float>("(-10)^0.00000001", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("(-10)^0.00000001", "1+3.141593ᴇ-8×i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_simplifies_approximates_to<float>("3.5^2.0000001", "12.25");
  assert_expression_simplifies_approximates_to<float>("3.7^2.0000001", "13.69");
}

QUIZ_CASE(poincare_approximation_subtraction) {
  assert_expression_approximates_to<float>("1-2", "-1");
  assert_expression_approximates_to<double>("3+i-(4+i)", "-1");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]-3", Undefined::Name());
  assert_expression_approximates_to<double>("[[1,2+i][3,4][5,6]]-(4+i)", Undefined::Name());
  assert_expression_approximates_to<float>("3-[[1,2][3,4][5,6]]", Undefined::Name());
  assert_expression_approximates_to<double>("3+i-[[1,2+i][3,4][5,6]]", Undefined::Name());
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]-[[6,5][4,3][2,1]]", "[[-5,-3][-1,1][3,5]]");
  assert_expression_approximates_to<double>("[[1,2+i][3,4][5,6]]-[[1,2+i][3,4][5,6]]", "[[0,0][0,0][0,0]]");

  assert_expression_approximates_to<float>("{1,2,3}-10", "{-9,-8,-7}");
  assert_expression_approximates_to<float>("10-{1,2,3}", "{9,8,7}");
  assert_expression_approximates_to<float>("{1,2,3}-{4,5,6}", "{-3,-3,-3}");
  assert_expression_approximates_to<float>("{1,2,3}-{4,5}", Undefined::Name());
  assert_expression_approximates_to<float>("{1,2,3}-[[4,5,6]]", Undefined::Name());
  assert_expression_approximates_to<double>("{1,2,3}-10", "{-9,-8,-7}");
  assert_expression_approximates_to<double>("10-{1,2,3}", "{9,8,7}");
  assert_expression_approximates_to<double>("{1,2,3}-{4,5,6}", "{-3,-3,-3}");
  assert_expression_approximates_to<double>("{1,2,3}-{4,5}", Undefined::Name());
  assert_expression_approximates_to<double>("{1,2,3}-[[4,5,6]]", Undefined::Name());

  assert_expression_approximates_to_scalar<float>("1-2", -1.0f);
  assert_expression_approximates_to_scalar<double>("(1)-(4+i)", NAN);
  assert_expression_approximates_to_scalar<float>("[[1,2][3,4][5,6]]-[[3,2][3,4][5,6]]", NAN);
}

QUIZ_CASE(poincare_approximation_constant) {
  assert_expression_approximates_to<double>("π", "3.1415926535898");
  assert_expression_approximates_to<float>("e", "2.718282");
  for (ConstantNode::ConstantInfo info : Constant::k_constants) {
    for (const char * constantNameAlias : info.aliasesList()) {
      if (strcmp(constantNameAlias, "i") == 0) {
        assert_expression_approximates_to<float>("i", "i");
        assert_expression_approximates_to<double>("i", "i");
        assert_expression_approximates_to_scalar<float>("i", NAN);
        assert_expression_approximates_to_scalar<double>("i", NAN);
      } else if (info.unit() == nullptr) {
        constexpr int k_bufferSize = PrintFloat::charSizeForFloatsWithPrecision(PrintFloat::SignificantDecimalDigits<double>());
        char buffer[k_bufferSize];
        PrintFloat::ConvertFloatToText<double>(info.value(), buffer, k_bufferSize, PrintFloat::k_maxFloatGlyphLength, PrintFloat::SignificantDecimalDigits<double>(), DecimalMode);
        assert_expression_approximates_to<double>(constantNameAlias, buffer);
        assert_expression_approximates_to_scalar<float>(constantNameAlias, info.value());
      } else {
        assert_expression_approximates_to<double>(constantNameAlias, Undefined::Name());
        assert_expression_approximates_to_scalar<float>(constantNameAlias, NAN);
      }
    }
  }
}

QUIZ_CASE(poincare_approximation_division) {
  assert_expression_approximates_to<float>("1/2", "0.5");
  assert_expression_approximates_to<double>("(3+i)/(4+i)", "0.76470588235294+0.058823529411765×i");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  assert_expression_approximates_to<double>("[[1,2+i][3,4][5,6]]/(1+i)", "[[0.5-0.5×i,1.5-0.5×i][1.5-1.5×i,2-2×i][2.5-2.5×i,3-3×i]]");
  assert_expression_approximates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  assert_expression_approximates_to<double>("[[1,2][3,4]]/[[3,4][6,9]]", Undefined::Name());
  assert_expression_approximates_to<double>("3/[[3,4][5,6]]", Undefined::Name());
  // assert_expression_approximates_to<double>("(3+4i)/[[3,4][1,i]]", "[[1,4×i][i,-3×i]]");
  /* TODO: this tests fails because of neglectable real or imaginary parts.
   * It currently approximates to
   * [[1+5.5511151231258ᴇ-17×i,-2.2204460492503ᴇ-16+4×i][i,-3×i]] or
   * [[1-1.1102230246252ᴇ-16×i,2.2204460492503ᴇ-16+4×i]
   *  [-1.1102230246252ᴇ-16+i,-2.2204460492503ᴇ-16-3×i]] on Linux */
  assert_expression_approximates_to<float>("1ᴇ20/(1ᴇ20+1ᴇ20i)", "0.5-0.5×i");
  assert_expression_approximates_to<double>("1ᴇ155/(1ᴇ155+1ᴇ155i)", "0.5-0.5×i");

  assert_expression_approximates_to<float>("{1,2,3}/10", "{0.1,0.2,0.3}");
  assert_expression_approximates_to<float>("10/{1,2,4}", "{10,5,2.5}");
  assert_expression_approximates_to<float>("{12,100,1}/{4,2,1}", "{3,50,1}");
  assert_expression_approximates_to<float>("{1,2,3}/{4,5}", Undefined::Name());
  assert_expression_approximates_to<float>("{1,2,3}/[[4,5,6]]", Undefined::Name());
  assert_expression_approximates_to<double>("{1,2,3}/10", "{0.1,0.2,0.3}");
  assert_expression_approximates_to<double>("10/{1,2,4}", "{10,5,2.5}");
  assert_expression_approximates_to<double>("{12,100,1}/{4,2,1}", "{3,50,1}");
  assert_expression_approximates_to<double>("{1,2,3}/{4,5}", Undefined::Name());
  assert_expression_approximates_to<double>("{1,2,3}/[[4,5,6]]", Undefined::Name());

  assert_expression_approximates_to_scalar<float>("1/2", 0.5f);
  assert_expression_approximates_to_scalar<float>("(3+i)/(4+i)", NAN);
  assert_expression_approximates_to_scalar<float>("[[1,2][3,4][5,6]]/2", NAN);

  assert_expression_approximates_to_scalar<float>("quo(23,12)", 1);
  assert_expression_approximates_to_scalar<float>("rem(23,12)", 11);
  assert_expression_approximates_to_scalar<float>("quo(-23,12)", -2);
  assert_expression_approximates_to_scalar<float>("rem(-23,12)", 1);
  assert_expression_approximates_to_scalar<float>("quo(23,-12)", -1);
  assert_expression_approximates_to_scalar<float>("rem(23,-12)", 11);
  assert_expression_approximates_to_scalar<float>("quo(-23,-12)", 2);
  assert_expression_approximates_to_scalar<float>("rem(-23,-12)", 1);
}

QUIZ_CASE(poincare_approximation_logarithm) {
  assert_expression_approximates_to<float>("log(2,64)", "0.1666667");
  assert_expression_approximates_to<double>("log(6,7)", "0.9207822211616");
  assert_expression_approximates_to<float>("log(5)", "0.69897");
  assert_expression_approximates_to<double>("ln(5)", "1.6094379124341");
  assert_expression_approximates_to<float>("log(2+5×i,64)", "0.4048317+0.2862042×i");
  assert_expression_approximates_to<double>("log(6,7+4×i)", "0.80843880717528-0.20108238082167×i");
  assert_expression_approximates_to<float>("log(5+2×i)", "0.731199+0.1652518×i");
  assert_expression_approximates_to<double>("ln(5+2×i)", "1.6836479149932+0.38050637711236×i");
  assert_expression_approximates_to<double>("log(0,0)", Undefined::Name());
  assert_expression_approximates_to<double>("log(0)", Undefined::Name());
  assert_expression_approximates_to<double>("log(2,0)", Undefined::Name());

  // WARNING: evaluate on branch cut can be multivalued
  assert_expression_approximates_to<double>("ln(-4)", "1.3862943611199+3.1415926535898×i");
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

  assert_expression_approximates_to<float>("abs(-2.3ᴇ-39)", "2.3ᴇ-39", Degree, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<double>("abs(-2.3ᴇ-39)", "2.3ᴇ-39");

  assert_expression_approximates_to<float>("abs(3+2i)", "3.605551");
  assert_expression_approximates_to<double>("abs(3+2i)", "3.605551275464");

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
  assert_expression_approximates_to<float>("binomial(13, 0)", "1");
  assert_expression_approximates_to<double>("binomial(13, 0)", "1");
  assert_expression_approximates_to<float>("binomial(10, -1)", "0");
  assert_expression_approximates_to<double>("binomial(10, -1)", "0");
  assert_expression_approximates_to<float>("binomial(-5, -10)", "0");
  assert_expression_approximates_to<double>("binomial(-5, -10)", "0");
  assert_expression_approximates_to<float>("binomial(10, 2.1)", Undefined::Name());
  assert_expression_approximates_to<double>("binomial(10, 2.1)", Undefined::Name());

  assert_expression_approximates_to<float>("ceil(0.2)", "1");
  assert_expression_approximates_to<double>("ceil(0.2)", "1");

  assert_expression_approximates_to<float>("det([[1,23,3][4,5,6][7,8,9]])", "126", Degree, MetricUnitFormat, Cartesian, 6); // FIXME: the determinant computation is not precised enough to be displayed with 7 significant digits
  assert_expression_approximates_to<double>("det([[1,23,3][4,5,6][7,8,9]])", "126");

  assert_expression_approximates_to<float>("det([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])", "126-231×i", Degree, MetricUnitFormat, Cartesian, 6); // FIXME: the determinant computation is not precised enough to be displayed with 7 significant digits
  assert_expression_approximates_to<double>("det([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])", "126-231×i");

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

  assert_expression_approximates_to<float>("im(2+3i)", "3");
  assert_expression_approximates_to<double>("im(2+3i)", "3");

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
  /* Testing LCM and GCD integer limits :
   * undef result is expected when manipulating overflowing/inaccurate integers
   * For context :
   * - INT_MAX =            2,147,483,647
   * - UINT32_MAX =         4,294,967,295
   * - Maximal representable integer without loss of precision in :
   *     - float :             16,777,216
   *     - double : 9,007,199,254,740,992
   */
  // Integers that can't be accurately represented as float
  assert_expression_approximates_to<float>("gcd(16777219,13)", Undefined::Name()); // 1
  assert_expression_approximates_to<double>("gcd(16777219,13)", "1");
  assert_expression_approximates_to<float>("lcm(1549, 10831)", Undefined::Name()); // 16777219
  assert_expression_approximates_to<double>("lcm(1549, 10831)", "16777219");
  // Integers overflowing int, but not uint32_t
  assert_expression_approximates_to<float>("gcd(2147483650,13)", Undefined::Name()); // 13
  assert_expression_approximates_to<double>("gcd(2147483650,13)", "13");
  assert_expression_approximates_to<float>("lcm(2,25,13,41,61,1321)", Undefined::Name()); // 2147483650
  assert_expression_approximates_to<double>("lcm(2,25,13,41,61,1321)", "2147483650");
  // Integers overflowing uint32_t
  assert_expression_approximates_to<float>("gcd(4294967300,13)", Undefined::Name()); // 13
  assert_expression_approximates_to<double>("gcd(4294967300,13)", Undefined::Name()); // 13
  assert_expression_approximates_to<float>("lcm(4,25,13,41,61,1321)", Undefined::Name()); // 4294967300
  assert_expression_approximates_to<double>("lcm(4,25,13,41,61,1321)", Undefined::Name()); // 4294967300
  // Integers that can't be accurately represented as double
  assert_expression_approximates_to<float>("gcd(1ᴇ16,10)", Undefined::Name());
  assert_expression_approximates_to<double>("gcd(1ᴇ16,10)", Undefined::Name());

  assert_expression_approximates_to<float>("invbinom(0.9647324002, 15, 0.7)", "13");
  assert_expression_approximates_to<double>("invbinom(0.9647324002, 15, 0.7)", "13");
  assert_expression_approximates_to<float>("invbinom(0.95,100,0.42)", "50");
  assert_expression_approximates_to<double>("invbinom(0.95,100,0.42)", "50");
  assert_expression_approximates_to<float>("invbinom(0.01,150,0.9)", "126");
  assert_expression_approximates_to<double>("invbinom(0.01,150,0.9)", "126");

  assert_expression_approximates_to<float>("invnorm(0.56, 1.3, 2.4)", "1.662326");
  //assert_expression_approximates_to<double>("invnorm(0.56, 1.3, 2.4)", "1.6623258450088"); FIXME precision error

  assert_expression_approximates_to<float>("ln(2)", "0.6931472");
  assert_expression_approximates_to<double>("ln(2)", "0.69314718055995");

  assert_expression_approximates_to<float>("log(2)", "0.30103");
  assert_expression_approximates_to<double>("log(2)", "0.30102999566398");

  assert_expression_approximates_to<float>("permute(10, 4)", "5040");
  assert_expression_approximates_to<double>("permute(10, 4)", "5040");

  assert_expression_approximates_to<float>("product(n,n, 4, 10)", "604800");
  assert_expression_approximates_to<double>("product(n,n, 4, 10)", "604800");

  assert_expression_approximates_to<float>("quo(29, 10)", "2");
  assert_expression_approximates_to<double>("quo(29, 10)", "2");

  assert_expression_approximates_to<float>("re(2+i)", "2");
  assert_expression_approximates_to<double>("re(2+i)", "2");

  assert_expression_approximates_to<float>("rem(29, 10)", "9");
  assert_expression_approximates_to<double>("rem(29, 10)", "9");
  assert_expression_approximates_to<float>("root(2,3)", "1.259921");
  assert_expression_approximates_to<double>("root(2,3)", "1.2599210498949");

  assert_expression_approximates_to<float>("√(2)", "1.414214");
  assert_expression_approximates_to<double>("√(2)", "1.4142135623731");

  assert_expression_approximates_to<float>("√(-1)", "i");
  assert_expression_approximates_to<double>("√(-1)", "i");

  assert_expression_approximates_to<float>("sum(r,r, 4, 10)", "49");
  assert_expression_approximates_to<double>("sum(k,k, 4, 10)", "49");

  assert_expression_approximates_to<float>("trace([[1,2,3][4,5,6][7,8,9]])", "15");
  assert_expression_approximates_to<double>("trace([[1,2,3][4,5,6][7,8,9]])", "15");

  assert_expression_approximates_to<float>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");
  assert_expression_approximates_to<double>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");

  assert_expression_approximates_to<float>("conj(3+2×i)", "3-2×i");
  assert_expression_approximates_to<double>("conj(3+2×i)", "3-2×i");

  assert_expression_approximates_to<float>("factor(-23/4)", "-5.75");
  assert_expression_approximates_to<double>("factor(-123/24)", "-5.125");
  assert_expression_approximates_to<float>("factor(i)", Undefined::Name());

  assert_expression_approximates_to<float>("inverse([[1,2,3][4,5,-6][7,8,9]])", "[[-1.2917,-0.083333,0.375][1.0833,0.16667,-0.25][0.041667,-0.083333,0.041667]]", Degree, MetricUnitFormat, Cartesian, 5); // inverse is not precise enough to display 7 significative digits
  assert_expression_approximates_to<double>("inverse([[1,2,3][4,5,-6][7,8,9]])", "[[-1.2916666666667,-0.083333333333333,0.375][1.0833333333333,0.16666666666667,-0.25][0.041666666666667,-0.083333333333333,0.041666666666667]]");
  assert_expression_approximates_to<float>("inverse([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])", "[[-0.0118-0.0455×i,-0.5-0.727×i,0.318+0.489×i][0.0409+0.00364×i,0.04-0.0218×i,-0.0255+9.1ᴇ-4×i][0.00334-0.00182×i,0.361+0.535×i,-0.13-0.358×i]]", Degree, MetricUnitFormat, Cartesian, 3); // inverse is not precise enough to display 7 significative digits
  assert_expression_approximates_to<double>("inverse([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])", "[[-0.0118289353958-0.0454959053685×i,-0.500454959054-0.727024567789×i,0.31847133758+0.488626023658×i][0.0409463148317+0.00363967242948×i,0.0400363967243-0.0218380345769×i,-0.0254777070064+9.0991810737ᴇ-4×i][0.00333636639369-0.00181983621474×i,0.36093418259+0.534728541098×i,-0.130118289354-0.357597816197×i]]", Degree, MetricUnitFormat, Cartesian, 12); // FIXME: inverse is not precise enough to display 14 significative digits

  assert_expression_approximates_to<float>("product(2+k×i,k, 1, 5)", "-100-540×i");
  assert_expression_approximates_to<double>("product(2+o×i,o, 1, 5)", "-100-540×i");

  assert_expression_approximates_to<float>("root(3+i, 3)", "1.459366+0.1571201×i");
  assert_expression_approximates_to<double>("root(3+i, 3)", "1.4593656008684+0.15712012294394×i");

  assert_expression_approximates_to<float>("root(3, 3+i)", "1.382007-0.1524428×i");
  assert_expression_approximates_to<double>("root(3, 3+i)", "1.3820069623326-0.1524427794159×i");

  assert_expression_approximates_to<float>("root(5^((-i)3^9),i)", "3.504", Degree, MetricUnitFormat, Cartesian, 4);
  assert_expression_approximates_to<double>("root(5^((-i)3^9),i)", "3.5039410843", Degree, MetricUnitFormat, Cartesian, 11);

  assert_expression_approximates_to<float>("√(3+i)", "1.755317+0.2848488×i");
  assert_expression_approximates_to<double>("√(3+i)", "1.7553173018244+0.28484878459314×i");

  assert_expression_approximates_to<float>("sign(-23+1)", "-1");
  assert_expression_approximates_to<float>("sign(inf)", "1");
  assert_expression_approximates_to<float>("sign(-inf)", "-1");
  assert_expression_approximates_to<float>("sign(0)", "0");
  assert_expression_approximates_to<float>("sign(-0)", "0");
  assert_expression_approximates_to<float>("sign(x)", Undefined::Name());
  assert_expression_approximates_to<double>("sign(2+i)", Undefined::Name());
  assert_expression_approximates_to<double>("sign(undef)", Undefined::Name());

  assert_expression_approximates_to<double>("sum(2+n×i,n,1,5)", "10+15×i");
  assert_expression_approximates_to<double>("sum(2+n×i,n,1,5)", "10+15×i");

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
  assert_expression_approximates_to<float>("round(2.3245)", "2");
  assert_expression_approximates_to<double>("round(2.3245)", "2");

  assert_expression_approximates_to<float>("6!", "720");
  assert_expression_approximates_to<double>("6!", "720");

  assert_expression_approximates_to<float>("20%", "0.2");
  assert_expression_approximates_to<float>("20%%", "0.002");
  assert_expression_approximates_to<float>("80*20%", "16");
  assert_expression_approximates_to<float>("80/(20%)", "400");
  assert_expression_approximates_to<float>("80+20%", "96");
  assert_expression_approximates_to<float>("20%+80+20%", "96.24");
  assert_expression_approximates_to<float>("80+20%+20%", "115.2");
  assert_expression_approximates_to<float>("80-20%", "64");
  assert_expression_approximates_to<float>("80+20-20%", "80");
  assert_expression_approximates_to<float>("10+2*3%", "10.06");
  assert_expression_approximates_to<float>("10+3%3", "10.09");
  assert_expression_approximates_to<float>("10+3%%", "10.0003");
  assert_expression_approximates_to<float>("10+3%%2", "10.0006");
  assert_expression_approximates_to<float>("10+3%-1", "9.3");
  assert_expression_approximates_to<float>("{10+3%,5+4%}", "{10.3,5.2}");
  assert_expression_approximates_to<float>("10+98%^2", "10.9604");

  assert_expression_approximates_to<float>("√(-1)", "i");
  assert_expression_approximates_to<double>("√(-1)", "i");

  assert_expression_approximates_to<float>("root(-1,3)", "0.5+0.8660254×i");
  assert_expression_approximates_to<double>("root(-1,3)", "0.5+0.86602540378444×i");

  assert_expression_approximation_is_bounded("random()", 0.0f, 1.0f);
  assert_expression_approximation_is_bounded("random()", 0.0, 1.0);

  assert_expression_approximation_is_bounded("randint(4,45)", 4.0f, 45.0f, true);
  assert_expression_approximation_is_bounded("randint(4,45)", 4.0, 45.0, true);
  assert_expression_approximation_is_bounded("randint(10)", 1.0, 10.0, true);
  assert_expression_approximates_to<double>("randint({1,1})", "{1,1}");
  assert_expression_approximates_to<double>("randint({1,2},{1,2})", "{1,2}");
  assert_expression_approximates_to<double>("1/randint(2,2)+1/2", "1");
  assert_expression_approximates_to<double>("randint(45,4)", Undefined::Name());
  assert_expression_approximates_to<double>("randint(1, inf)",Undefined::Name());
  assert_expression_approximates_to<double>("randint(-inf, 3)", Undefined::Name());
  assert_expression_approximates_to<double>("randint(4, 3)", Undefined::Name());
  assert_expression_approximates_to<double>("randint(2, 23345678909876545678)", Undefined::Name());
  assert_expression_approximates_to<double>("randint(123456789876543, 123456789876543+10)", Undefined::Name());
}

template<typename T>
void assert_no_duplicates_in_list(const char * expression) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, true);
  e = ListSort::Builder(e);
  Expression result = e.approximate<T>(&globalContext, Cartesian, Radian);
  assert(result.type() == ExpressionNode::Type::List);
  List list = static_cast<List &>(result);
  int n = list.numberOfChildren();
  for (int i = 1; i < n; i++) {
    quiz_assert_print_if_failure(!list.childAtIndex(i).isIdenticalTo(list.childAtIndex(i - 1)), expression);
  }
}

QUIZ_CASE(poincare_approximation_unique_random) {
  assert_expression_approximates_to<double>("randintnorep(10,1,3)", Undefined::Name());
  assert_expression_approximates_to<double>("randintnorep(1,10,100)", Undefined::Name());
  assert_expression_approximates_to<double>("randintnorep(1,10,-1)", Undefined::Name());
  assert_expression_approximates_to<double>("randintnorep(1,10,0)", "{}");

  assert_no_duplicates_in_list<float>("randintnorep(-100,99,200)");
  assert_no_duplicates_in_list<float>("randintnorep(1234,5678,20)");

  // Random lists can be sorted
  assert_expression_approximates_to<double>("sort(randintnorep(5,8,4))", "{5,6,7,8}");

  /* The simplification process should understand that the expression is not a
   * scalar if it encounters a randintnorep. */
  assert_expression_simplifies_and_approximates_to("rem(randintnorep(1,10,5),1)", "{0,0,0,0,0}");
}

QUIZ_CASE(poincare_approximation_integral) {
  assert_expression_approximates_to<float>("int(x,x, 1, 2)", "1.5");
  assert_expression_approximates_to<double>("int(x,x, 1, 2)", "1.5");
  assert_expression_approximates_to<float>("int(1/x,x,0,1)", "undef");

  assert_expression_approximates_to<float>("int(1+cos(a),a, 0, 180)", "180");
  assert_expression_approximates_to<double>("int(1+cos(a),a, 0, 180)", "180");

  // former precision issues
  assert_expression_approximates_to<double>("int(abs(2*sin(e^(x/4))+1),x,0,6)", "12.573260585347", Radian); // #1912
  assert_expression_approximates_to<double>("int(4*√(1-x^2),x,0,1)", "3.1415926535899"); // #1912
  assert_expression_approximates_to<double>("int(1/(1+25*x^2),x,0,1)", "0.274680153389"); // Runge function
  assert_expression_approximates_to<double>("int(2*√(9-(x-3)^2),x,0,6)", "28.27433388231"); // #1378

  // far bounds
  assert_expression_approximates_to<double>("int(e^(-x^2),x,0,100000)", "0.88622692545276"); // ≈sqrt(pi)/2 #258
  assert_expression_approximates_to<double>("int(1/(x^2+1),x,0,100000)", "1.5707863267949"); // atan(100000) #1104
  assert_expression_approximates_to<double>("int(1/(x^2+1),x,0,inf)", "1.5707963267949"); // pi/2 #1104
  assert_expression_approximates_to<double>("int(1/x,x,1,1000000)", "13.81551056", Radian, MetricUnitFormat, Cartesian, 10); // #1104
  assert_expression_approximates_to<double>("int(e^(-x)/√(x),x,0,inf)", "1.7724", Radian, MetricUnitFormat, Cartesian, 5); // poor precision
  assert_expression_approximates_to<double>("int(1,x,inf,0)", "undef"); // had performance issues
  assert_expression_approximates_to<double>("int(e^(-x),x,inf,0)", "-1");

  // singularities
  assert_expression_approximates_to<double>("int(ln(x)*√(x),x,0,1)", "-0.444444444444", Radian, MetricUnitFormat, Cartesian, 12);
  assert_expression_approximates_to<double>("int(1/√(x),x,0,1)", "2", Radian, MetricUnitFormat, Cartesian, 12);
  assert_expression_approximates_to<double>("int(1/√(1-x),x,0,1)", "2", Radian, MetricUnitFormat, Cartesian, 12);
  assert_expression_approximates_to<double>("int(1/√(x)+1/√(1-x),x,0,1)", "4", Radian, MetricUnitFormat, Cartesian, 12);
  assert_expression_approximates_to<double>("int(ln(x)^2,x,0,1)", "2", Radian, MetricUnitFormat, Cartesian, 12);
  assert_expression_approximates_to<double>("int(1/√(x-1),x,1,2)", "2", Radian, MetricUnitFormat, Cartesian, 12); // #596
  assert_expression_approximates_to<double>("int(2/√(1-x^2),x,0,1)", "3.1415926535898"); // #1780
  assert_expression_approximates_to<double>("int(4x/√(1-x^4),x,0,1)", "3.1415926535898"); // #1780
  // convergence is slow with 1/x^k k≈1, therefore precision is poor
  assert_expression_approximates_to<float>("int(1/x^0.9,x,0,1)", "10", Radian, MetricUnitFormat, Cartesian, 3);

  // double integration
  assert_expression_approximates_to<float>("int(int(x×x,x,0,x),x,0,4)", "21.33333");
  assert_expression_approximates_to<double>("int(int(x×x,x,0,x),x,0,4)", "21.333333333333");
  assert_expression_approximates_to<double>("int(sum(sin(x)^k,k,0,100),x,0,π)", "48.3828", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("int(int(1/(1-x)^k,x,0,1),k,0.5,0.25)", "-0.405465", Radian, MetricUnitFormat, Cartesian, 6);
  // this integral on R2 takes about one minute to compute on N110
  // assert_expression_approximates_to<double>("int(int(e^(-(x^2+y^2)),x,-inf,inf),y,-inf,inf)", "3.141592654", Radian, MetricUnitFormat, Cartesian, 10);
  // assert_expression_approximates_to<double>("int(int(e^(-x*t)/t^2,t,1,inf),x,0,1)", "0.3903080328", Radian, MetricUnitFormat, Cartesian, 10);

  // oscillator
  assert_expression_approximates_to<double>("int((sin(x)/x)^2,x,0,inf)", "1.5708", Radian, MetricUnitFormat, Cartesian, 5); // poor precision
  assert_expression_approximates_to<double>("int(x*sin(1/x)*√(abs(1-x)),x,0,3)", "1.9819412", Radian, MetricUnitFormat, Cartesian, 8);
}

QUIZ_CASE(poincare_approximation_trigonometry_functions) {
  /* cos: R  ->  R (oscillator)
   *      Ri ->  R (even)
   */
  // On R
  assert_expression_approximates_to<double>("cos(2)", "-0.41614683654714", Radian);
  assert_expression_approximates_to<double>("cos(2)", "0.9993908270191", Degree);
  assert_expression_approximates_to<double>("cos(2)", "0.99950656036573", Gradian);
  // Oscillator
  assert_expression_approximates_to<float>("cos(π/2)", "0", Radian);
  assert_expression_approximates_to<float>("cos(100)", "0", Gradian);
  assert_expression_approximates_to<double>("cos(3×π/2)", "0", Radian);
  assert_expression_approximates_to<float>("cos(300)", "0", Gradian);
  assert_expression_approximates_to<float>("cos(3×π)", "-1", Radian);
  assert_expression_approximates_to<float>("cos(-540)", "-1", Degree);
  assert_expression_approximates_to<float>("cos(-600)", "-1", Gradian);
  // On R×i
  assert_expression_approximates_to<double>("cos(-2×i)", "3.7621956910836", Radian);
  assert_expression_approximates_to<double>("cos(-2×i)", "1.0006092967033", Degree);
  assert_expression_approximates_to<double>("cos(-2×i)", "1.0004935208085", Gradian);
  // Symmetry: even
  assert_expression_approximates_to<double>("cos(2×i)", "3.7621956910836", Radian);
  assert_expression_approximates_to<double>("cos(2×i)", "1.0006092967033", Degree);
  assert_expression_approximates_to<double>("cos(2×i)", "1.0004935208085", Gradian);
  // On C
  assert_expression_approximates_to<float>("cos(i-4)", "-1.008625-0.8893952×i", Radian);
  assert_expression_approximates_to<float>("cos(i-4)", "0.997716+0.00121754×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("cos(i-4)", "0.99815+9.86352ᴇ-4×i", Gradian, MetricUnitFormat, Cartesian, 6);
  // Advanced function : sec
  assert_expression_approximates_to<double>("sec(2)", "-2.4029979617224", Radian);
  assert_expression_approximates_to<float>("sec(200)", "-1", Gradian);
  assert_expression_approximates_to<float>("sec(3×i)", "0.9986307", Degree);
  assert_expression_approximates_to<float>("sec(-3×i)", "0.9986307", Degree);
  assert_expression_approximates_to<float>("sec(i-4)", "-0.5577604+0.4918275×i", Radian, MetricUnitFormat, Cartesian, 7);

  /* sin: R  ->  R (oscillator)
   *      Ri ->  Ri (odd)
   */
  // On R
  assert_expression_approximates_to<double>("sin(2)", "0.90929742682568", Radian);
  assert_expression_approximates_to<double>("sin(2)", "0.034899496702501", Degree);
  assert_expression_approximates_to<double>("sin(2)", "0.031410759078128", Gradian);
  // Oscillator
  assert_expression_approximates_to<float>("sin(π/2)", "1", Radian);
  assert_expression_approximates_to<double>("sin(3×π/2)", "-1", Radian);
  assert_expression_approximates_to<float>("sin(3×π)", "0", Radian);
  assert_expression_approximates_to<float>("sin(-540)", "0", Degree);
  assert_expression_approximates_to<float>("sin(-600)", "0", Gradian);
  assert_expression_approximates_to<float>("sin(300)", "-1", Gradian);
  assert_expression_approximates_to<float>("sin(100)", "1", Gradian);
  // On R×i
  assert_expression_approximates_to<double>("sin(3×i)", "10.01787492741×i", Radian);
  assert_expression_approximates_to<float>("sin(3×i)", "0.05238381×i", Degree);
  assert_expression_approximates_to<double>("sin(3×i)", "0.047141332771113×i", Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("sin(-3×i)", "-10.01787492741×i", Radian);
  assert_expression_approximates_to<float>("sin(-3×i)", "-0.05238381×i", Degree);
  assert_expression_approximates_to<double>("sin(-3×i)", "-0.047141332771113×i", Gradian);
  // On: C
  assert_expression_approximates_to<float>("sin(i-4)", "1.16781-0.768163×i", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("sin(i-4)", "-0.0697671+0.0174117×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("sin(i-4)", "-0.0627983+0.0156776×i", Gradian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("sin(1.234567890123456ᴇ-15)", "1.23457ᴇ-15", Radian, MetricUnitFormat, Cartesian, 6);
  // Advanced function : csc
  assert_expression_approximates_to<double>("csc(2)", "1.0997501702946", Radian);
  assert_expression_approximates_to<float>("csc(100)", "1", Gradian);
  assert_expression_approximates_to<float>("csc(3×i)", "-19.08987×i", Degree);
  assert_expression_approximates_to<float>("csc(-3×i)", "19.08987×i", Degree);
  assert_expression_approximates_to<float>("csc(i-4)", "0.597696+0.393154×i", Radian, MetricUnitFormat, Cartesian, 6);

  /* tan: R  ->  R (tangent-style)
   *      Ri ->  Ri (odd)
   */
  // On R
  assert_expression_approximates_to<double>("tan(2)", "-2.1850398632615", Radian);
  assert_expression_approximates_to<double>("tan(2)", "0.034920769491748", Degree);
  assert_expression_approximates_to<double>("tan(2)", "0.031426266043351", Gradian);
  // Tangent-style
  assert_expression_approximates_to<float>("tan(3×π)", "0", Radian);
  assert_expression_approximates_to<float>("tan(-540)", "0", Degree);
  assert_expression_approximates_to<float>("tan(-600)", "0", Gradian);
  // On R×i
  assert_expression_approximates_to<double>("tan(-2×i)", "-0.96402758007582×i", Radian);
  assert_expression_approximates_to<float>("tan(-2×i)", "-0.03489241×i", Degree);
  assert_expression_approximates_to<float>("tan(-2×i)", "-0.0314056×i", Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("tan(2×i)", "0.96402758007582×i", Radian);
  assert_expression_approximates_to<float>("tan(2×i)", "0.03489241×i", Degree);
  assert_expression_approximates_to<float>("tan(2×i)", "0.0314056×i", Gradian);
  // On C
  assert_expression_approximates_to<float>("tan(i-4)", "-0.273553+1.00281×i", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("tan(i-4)", "-0.0699054+0.0175368×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("tan(i-4)", "-0.0628991+0.0157688×i", Gradian, MetricUnitFormat, Cartesian, 6);
  // Advanced function : cot
  assert_expression_approximates_to<double>("cot(2)", "-0.45765755436029", Radian);
  assert_expression_approximates_to<float>("cot(100)", "0", Gradian);
  assert_expression_approximates_to<float>("cot(3×i)", "-19.11604×i", Degree);
  assert_expression_approximates_to<float>("cot(-3×i)", "19.11604×i", Degree);
  assert_expression_approximates_to<float>("cot(i-4)", "-0.253182-0.928133×i", Radian, MetricUnitFormat, Cartesian, 6);
  // Other
  assert_expression_approximates_to<double>("tan(π/2)", Undefined::Name(), Radian);
  assert_expression_approximates_to<double>("1/tan(π/2)", Undefined::Name(), Radian);

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
  assert_expression_approximates_to<double>("acos(2)", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("acos(2)", "1.3169578969248×i", Radian);
  assert_expression_approximates_to<double>("acos(2)", "75.456129290217×i", Degree);
  assert_expression_approximates_to<double>("acos(2)", "83.84×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // Symmetry: odd on imaginary
  assert_expression_approximates_to<double>("acos(-2)", "3.1415926535898-1.3169578969248×i", Radian);
  assert_expression_approximates_to<double>("acos(-2)", "180-75.456129290217×i", Degree);
  assert_expression_approximates_to<double>("acos(-2)", "200-83.84×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // On ]-inf, -1[
  assert_expression_approximates_to<double>("acos(-32)", "3.14159265359-4.158638853279×i", Radian, MetricUnitFormat, Cartesian, 13);
  assert_expression_approximates_to<float>("acos(-32)", "180-238.3×i", Degree, MetricUnitFormat, Cartesian, 4);
  assert_expression_approximates_to<float>("acos(-32)", "200-264.7×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // On R×i
  assert_expression_approximates_to<float>("acos(3×i)", "1.5708-1.8184×i", Radian, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(3×i)", "90-104.19×i", Degree, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(3×i)", "100-115.8×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // Symmetry: odd on imaginary
  assert_expression_approximates_to<float>("acos(-3×i)", "1.5708+1.8184×i", Radian, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(-3×i)", "90+104.19×i", Degree, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(-3×i)", "100+115.8×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("acos(i-4)", "2.8894-2.0966×i", Radian, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(i-4)", "165.551-120.126×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("acos(i-4)", "183.9-133.5×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // Key values
  assert_expression_approximates_to<double>("acos(0)", "90", Degree);
  assert_expression_approximates_to<float>("acos(-1)", "180", Degree);
  assert_expression_approximates_to<double>("acos(1)", "0", Degree);
  assert_expression_approximates_to<double>("acos(0)", "100", Gradian);
  assert_expression_approximates_to<float>("acos(-1)", "200", Gradian);
  assert_expression_approximates_to<double>("acos(1)", "0", Gradian);
  // Advanced function : asec
  assert_expression_approximates_to<double>("arcsec(-2.4029979617224)", "2", Radian);
  assert_expression_approximates_to<float>("arcsec(-1)", "200", Gradian);
  assert_expression_approximates_to<float>("arcsec(0.9986307857)", "2.9999×i", Degree, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("arcsec(-0.55776+0.491828×i)", "2.28318+1×i", Radian, MetricUnitFormat, Cartesian, 6);

  /* asin: [-1,1]    -> R
   *       ]-inf,-1[ -> -π/2+R×i (odd)
   *       ]1, inf[  -> π/2+R×i (odd)
   *       R×i       -> R×i (odd)
   */
  // On [-1, 1]
  assert_expression_approximates_to<double>("asin(0.5)", "0.5235987755983", Radian);
  assert_expression_approximates_to<double>("asin(0.03)", "0.030004501823477", Radian);
  assert_expression_approximates_to<double>("asin(0.5)", "30", Degree);
  assert_expression_approximates_to<double>("asin(0.5)", "33.3333", Gradian, MetricUnitFormat, Cartesian, 6);
  // On [1, inf[
  assert_expression_approximates_to<double>("asin(2)", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("asin(2)", "1.5707963267949-1.3169578969248×i", Radian);
  assert_expression_approximates_to<double>("asin(2)", "90-75.456129290217×i", Degree);
  assert_expression_approximates_to<double>("asin(2)", "100-83.84×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // Symmetry: odd
  assert_expression_approximates_to<double>("asin(-2)", "-1.5707963267949+1.3169578969248×i", Radian);
  assert_expression_approximates_to<double>("asin(-2)", "-90+75.456129290217×i", Degree);
  assert_expression_approximates_to<double>("asin(-2)", "-100+83.84×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // On ]-inf, -1[
  assert_expression_approximates_to<float>("asin(-32)", "-1.571+4.159×i", Radian, MetricUnitFormat, Cartesian, 4);
  assert_expression_approximates_to<float>("asin(-32)", "-90+238×i", Degree, MetricUnitFormat, Cartesian, 3);
  assert_expression_approximates_to<float>("asin(-32)", "-100+265×i", Gradian, MetricUnitFormat, Cartesian, 3);
  // On R×i
  assert_expression_approximates_to<double>("asin(3×i)", "1.8184464592321×i", Radian);
  assert_expression_approximates_to<double>("asin(3×i)", "115.8×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // Symmetry: odd
  assert_expression_approximates_to<double>("asin(-3×i)", "-1.8184464592321×i", Radian);
  assert_expression_approximates_to<double>("asin(-3×i)", "-115.8×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("asin(i-4)", "-1.3186+2.0966×i", Radian, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("asin(i-4)", "-75.551+120.13×i", Degree, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("asin(i-4)", "-83.95+133.5×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // Key values
  assert_expression_approximates_to<double>("asin(0)", "0", Degree);
  assert_expression_approximates_to<double>("asin(0)", "0", Gradian);
  assert_expression_approximates_to<float>("asin(-1)", "-90", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("asin(-1)", "-100", Gradian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("asin(1)", "90", Degree);
  assert_expression_approximates_to<double>("asin(1)", "100", Gradian, MetricUnitFormat, Cartesian);
  // Advanced function : acsc
  assert_expression_approximates_to<double>("arccsc(1.0997501702946)", "1.1415926535898", Radian);
  assert_expression_approximates_to<double>("arccsc(1)", "100", Gradian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("arccsc(-19.08987×i)", "3×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("arccsc(19.08987×i)", "-3×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("arccsc(0.5+0.4×i)", "0.792676-1.13208×i", Radian, MetricUnitFormat, Cartesian, 6);

  /* atan: R         ->  R (odd)
   *       [-i,i]    ->  R×i (odd)
   *       ]-inf×i,-i[ -> -π/2+R×i (odd)
   *       ]i, inf×i[  -> π/2+R×i (odd)
   */
  // On R
  assert_expression_approximates_to<double>("atan(2)", "1.1071487177941", Radian);
  assert_expression_approximates_to<double>("atan(0.01)", "0.0099996666866652", Radian);
  assert_expression_approximates_to<double>("atan(2)", "63.434948822922", Degree);
  assert_expression_approximates_to<double>("atan(2)", "70.48", Gradian, MetricUnitFormat, Cartesian, 4);
  assert_expression_approximates_to<float>("atan(0.5)", "0.4636476", Radian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("atan(-2)", "-1.1071487177941", Radian);
  assert_expression_approximates_to<double>("atan(-2)", "-63.434948822922", Degree);
  // On [-i, i]
  assert_expression_approximates_to<float>("atan(0.2×i)", "0.202733×i", Radian, MetricUnitFormat, Cartesian, 6);
  // Symmetry: odd
  assert_expression_approximates_to<float>("atan(-0.2×i)", "-0.202733×i", Radian, MetricUnitFormat, Cartesian, 6);
  // On [i, inf×i[
  assert_expression_approximates_to<double>("atan(26×i)", "1.5707963267949+0.038480520568064×i", Radian);
  assert_expression_approximates_to<double>("atan(26×i)", "90+2.2047714220164×i", Degree);
  assert_expression_approximates_to<double>("atan(26×i)", "100+2.45×i", Gradian, MetricUnitFormat, Cartesian, 3);
  // Symmetry: odd
  assert_expression_approximates_to<double>("atan(-26×i)", "-1.5707963267949-0.038480520568064×i", Radian);
  assert_expression_approximates_to<double>("atan(-26×i)", "-90-2.2047714220164×i", Degree);
  assert_expression_approximates_to<double>("atan(-26×i)", "-100-2.45×i", Gradian, MetricUnitFormat, Cartesian, 3);
 // On ]-inf×i, -i[
  assert_expression_approximates_to<float>("atan(-3.4×i)", "-1.570796-0.3030679×i", Radian);
  assert_expression_approximates_to<float>("atan(-3.4×i)", "-90-17.3645×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("atan(-3.4×i)", "-100-19.29×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("atan(i-4)", "-1.338973+0.05578589×i", Radian);
  assert_expression_approximates_to<float>("atan(i-4)", "-76.7175+3.1963×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("atan(i-4)", "-85.24+3.551×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // Key values
  assert_expression_approximates_to<float>("atan(0)", "0", Degree);
  assert_expression_approximates_to<float>("atan(0)", "0", Gradian);
  assert_expression_approximates_to<double>("atan(-i)", "-∞×i", Radian);
  assert_expression_approximates_to<double>("atan(i)", "∞×i", Radian);
  // Advanced function : acot
  assert_expression_approximates_to<double>("arccot(-0.45765755436029)", "-1.1415926535898", Radian);
  assert_expression_approximates_to<double>("arccot(0)", "90", Degree);
  assert_expression_approximates_to<float>("arccot(0)", "1.570796", Radian);
  assert_expression_approximates_to<float>("arccot(-19.11604×i)", "3×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("arccot(19.11604×i)", "-3×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("arccot(-0.253182-0.928133×i)", "-0.858407+1×i", Radian, MetricUnitFormat, Cartesian, 6);

  /* cosh: R         -> R (even)
   *       R×i       -> R (oscillator)
   */
  // On R
  assert_expression_approximates_to<double>("cosh(2)", "3.7621956910836", Radian);
  assert_expression_approximates_to<double>("cosh(2)", "3.7621956910836", Degree);
  assert_expression_approximates_to<double>("cosh(2)", "3.7621956910836", Gradian);
  // Symmetry: even
  assert_expression_approximates_to<double>("cosh(-2)", "3.7621956910836", Radian);
  assert_expression_approximates_to<double>("cosh(-2)", "3.7621956910836", Degree);
  assert_expression_approximates_to<double>("cosh(-2)", "3.7621956910836", Gradian);
  // On R×i
  assert_expression_approximates_to<double>("cosh(43×i)", "0.55511330152063", Radian);
  // Oscillator
  assert_expression_approximates_to<float>("cosh(π×i/2)", "0", Radian);
  assert_expression_approximates_to<float>("cosh(5×π×i/2)", "0", Radian);
  assert_expression_approximates_to<float>("cosh(8×π×i/2)", "1", Radian);
  assert_expression_approximates_to<float>("cosh(9×π×i/2)", "0", Radian);
  // On C
  assert_expression_approximates_to<float>("cosh(i-4)", "14.7547-22.9637×i", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("cosh(i-4)", "14.7547-22.9637×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("cosh(i-4)", "14.7547-22.9637×i", Gradian, MetricUnitFormat, Cartesian, 6);

  /* sinh: R         -> R (odd)
   *       R×i       -> R×i (oscillator)
   */
  // On R
  assert_expression_approximates_to<double>("sinh(2)", "3.626860407847", Radian);
  assert_expression_approximates_to<double>("sinh(2)", "3.626860407847", Degree);
  assert_expression_approximates_to<double>("sinh(2)", "3.626860407847", Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("sinh(-2)", "-3.626860407847", Radian);
  // On R×i
  assert_expression_approximates_to<double>("sinh(43×i)", "-0.8317747426286×i", Radian);
  // Oscillator
  assert_expression_approximates_to<float>("sinh(π×i/2)", "i", Radian);
  assert_expression_approximates_to<float>("sinh(5×π×i/2)", "i", Radian);
  assert_expression_approximates_to<float>("sinh(7×π×i/2)", "-i", Radian);
  assert_expression_approximates_to<float>("sinh(8×π×i/2)", "0", Radian);
  assert_expression_approximates_to<float>("sinh(9×π×i/2)", "i", Radian);
  // On C
  assert_expression_approximates_to<float>("sinh(i-4)", "-14.7448+22.9791×i", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("sinh(i-4)", "-14.7448+22.9791×i", Degree, MetricUnitFormat, Cartesian, 6);

  /* tanh: R         -> R (odd)
   *       R×i       -> R×i (tangent-style)
   */
  // On R
  assert_expression_approximates_to<double>("tanh(2)", "0.96402758007582", Radian);
  assert_expression_approximates_to<double>("tanh(2)", "0.96402758007582", Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("tanh(-2)", "-0.96402758007582", Degree);
  // On R×i
  assert_expression_approximates_to<double>("tanh(43×i)", "-1.4983873388552×i", Radian);
  // Tangent-style
  // FIXME: this depends on the libm implementation and does not work on travis/appveyor servers
  /*assert_expression_approximates_to<float>("tanh(π×i/2)", Undefined::Name(), Radian);
  assert_expression_approximates_to<float>("tanh(5×π×i/2)", Undefined::Name(), Radian);
  assert_expression_approximates_to<float>("tanh(7×π×i/2)", Undefined::Name(), Radian);
  assert_expression_approximates_to<float>("tanh(8×π×i/2)", "0", Radian);
  assert_expression_approximates_to<float>("tanh(9×π×i/2)", Undefined::Name(), Radian);*/
  // On C
  assert_expression_approximates_to<float>("tanh(i-4)", "-1.00028+6.10241ᴇ-4×i", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("tanh(i-4)", "-1.00028+6.10241ᴇ-4×i", Degree, MetricUnitFormat, Cartesian, 6);

  /* acosh: [-1,1]       -> R×i
   *        ]-inf,-1[    -> π×i+R (even on real)
   *        ]1, inf[     -> R (even on real)
   *        ]-inf×i, 0[  -> -π/2×i+R (even on real)
   *        ]0, inf*i[   -> π/2×i+R (even on real)
   */
  // On [-1,1]
  assert_expression_approximates_to<double>("arcosh(2)", "1.3169578969248", Radian);
  assert_expression_approximates_to<double>("arcosh(2)", "1.3169578969248", Degree);
  assert_expression_approximates_to<double>("arcosh(2)", "1.3169578969248", Gradian);
  // On ]-inf, -1[
  assert_expression_approximates_to<double>("arcosh(-4)", "2.0634370688956+3.1415926535898×i", Radian);
  assert_expression_approximates_to<float>("arcosh(-4)", "2.06344+3.14159×i", Radian, MetricUnitFormat, Cartesian, 6);
  // On ]1,inf[: Symmetry: even on real
  assert_expression_approximates_to<double>("arcosh(4)", "2.0634370688956", Radian);
  assert_expression_approximates_to<float>("arcosh(4)", "2.063437", Radian);
  // On ]-inf×i, 0[
  assert_expression_approximates_to<double>("arcosh(-42×i)", "4.4309584920805-1.5707963267949×i", Radian);
  assert_expression_approximates_to<float>("arcosh(-42×i)", "4.431-1.571×i", Radian, MetricUnitFormat, Cartesian, 4);
  // On ]0, i×inf[: Symmetry: even on real
  assert_expression_approximates_to<double>("arcosh(42×i)", "4.4309584920805+1.5707963267949×i", Radian);
  assert_expression_approximates_to<float>("arcosh(42×i)", "4.431+1.571×i", Radian, MetricUnitFormat, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("arcosh(i-4)", "2.0966+2.8894×i", Radian, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("arcosh(i-4)", "2.0966+2.8894×i", Degree, MetricUnitFormat, Cartesian, 5);
  // Key values
  //assert_expression_approximates_to<double>("arcosh(-1)", "3.1415926535898×i", Radian);
  assert_expression_approximates_to<double>("arcosh(1)", "0", Radian);
  assert_expression_approximates_to<float>("arcosh(0)", "1.570796×i", Radian);

  /* asinh: R            -> R (odd)
   *        [-i,i]       -> R*i (odd)
   *        ]-inf×i,-i[  -> -π/2×i+R (odd)
   *        ]i, inf×i[   -> π/2×i+R (odd)
   */
  // On R
  assert_expression_approximates_to<double>("arsinh(2)", "1.4436354751788", Radian);
  assert_expression_approximates_to<double>("arsinh(2)", "1.4436354751788", Degree);
  assert_expression_approximates_to<double>("arsinh(2)", "1.4436354751788", Gradian);
 // Symmetry: odd
  assert_expression_approximates_to<double>("arsinh(-2)", "-1.4436354751788", Radian);
  assert_expression_approximates_to<double>("arsinh(-2)", "-1.4436354751788", Degree);
  // On [-i,i]
  assert_expression_approximates_to<double>("arsinh(0.2×i)", "0.20135792079033×i", Radian);
  // arsinh(0.2*i) has a too low precision in float on the web platform
  assert_expression_approximates_to<float>("arsinh(0.3×i)", "0.3046927×i", Degree);
  // Symmetry: odd
  assert_expression_approximates_to<double>("arsinh(-0.2×i)", "-0.20135792079033×i", Radian);
  // arsinh(-0.2*i) has a too low precision in float on the web platform
  assert_expression_approximates_to<float>("arsinh(-0.3×i)", "-0.3046927×i", Degree);
  // On ]-inf×i, -i[
  assert_expression_approximates_to<double>("arsinh(-22×i)", "-3.7836727043295-1.5707963267949×i", Radian);
  assert_expression_approximates_to<float>("arsinh(-22×i)", "-3.784-1.571×i", Degree, MetricUnitFormat, Cartesian, 4);
  // On ]i, inf×i[, Symmetry: odd
  assert_expression_approximates_to<double>("arsinh(22×i)", "3.7836727043295+1.5707963267949×i", Radian);
  assert_expression_approximates_to<float>("arsinh(22×i)", "3.784+1.571×i", Degree, MetricUnitFormat, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("arsinh(i-4)", "-2.123+0.2383×i", Radian, MetricUnitFormat, Cartesian, 4);
  assert_expression_approximates_to<float>("arsinh(i-4)", "-2.123+0.2383×i", Degree, MetricUnitFormat, Cartesian, 4);

  /* atanh: [-1,1]       -> R (odd)
   *        ]-inf,-1[    -> π/2*i+R (odd)
   *        ]1, inf[     -> -π/2×i+R (odd)
   *        R×i          -> R×i (odd)
   */
  // On [-1,1]
  assert_expression_approximates_to<double>("artanh(0.4)", "0.4236489301936", Radian);
  assert_expression_approximates_to<double>("artanh(0.4)", "0.4236489301936", Degree);
  assert_expression_approximates_to<double>("artanh(0.4)", "0.4236489301936", Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("artanh(-0.4)", "-0.4236489301936", Radian);
  assert_expression_approximates_to<double>("artanh(-0.4)", "-0.4236489301936", Degree);
  // On ]1, inf[
  assert_expression_approximates_to<double>("artanh(4)", "0.255412811883-1.5707963267949×i", Radian);
  assert_expression_approximates_to<float>("artanh(4)", "0.2554128-1.570796×i", Degree);
  // On ]-inf,-1[, Symmetry: odd
  assert_expression_approximates_to<double>("artanh(-4)", "-0.255412811883+1.5707963267949×i", Radian);
  assert_expression_approximates_to<float>("artanh(-4)", "-0.2554128+1.570796×i", Degree);
  // On R×i
  assert_expression_approximates_to<double>("artanh(4×i)", "1.325817663668×i", Radian);
  assert_expression_approximates_to<float>("artanh(4×i)", "1.325818×i", Radian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("artanh(-4×i)", "-1.325817663668×i", Radian);
  assert_expression_approximates_to<float>("artanh(-4×i)", "-1.325818×i", Radian);
  // On C
  assert_expression_approximates_to<float>("artanh(i-4)", "-0.238878+1.50862×i", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("artanh(i-4)", "-0.238878+1.50862×i", Degree, MetricUnitFormat, Cartesian, 6);

  // Check that the complex part is not neglected
  assert_expression_approximates_to<double>("artanh(0.99999999999+1.0ᴇ-26×i)", "13+5ᴇ-16×i", Radian, MetricUnitFormat, Cartesian, 3);
  assert_expression_approximates_to<double>("artanh(0.99999999999+1.0ᴇ-60×i)", "13+5ᴇ-50×i", Radian, MetricUnitFormat, Cartesian, 3);
  assert_expression_approximates_to<double>("artanh(0.99999999999+1.0ᴇ-150×i)", "13+5ᴇ-140×i", Radian, MetricUnitFormat, Cartesian, 3);
  assert_expression_approximates_to<double>("artanh(0.99999999999+1.0ᴇ-250×i)", "13+5ᴇ-240×i", Radian, MetricUnitFormat, Cartesian, 3);
  assert_expression_approximates_to<double>("artanh(0.99999999999+1.0ᴇ-300×i)", "13+5ᴇ-290×i", Radian, MetricUnitFormat, Cartesian, 3);

  // WARNING: evaluate on branch cut can be multivalued
  assert_expression_approximates_to<double>("acos(2)", "1.3169578969248×i", Radian);
  assert_expression_approximates_to<double>("acos(2)", "75.456129290217×i", Degree);
  assert_expression_approximates_to<double>("asin(2)", "1.5707963267949-1.3169578969248×i", Radian);
  assert_expression_approximates_to<double>("asin(2)", "90-75.456129290217×i", Degree);
  assert_expression_approximates_to<double>("artanh(2)", "0.54930614433405-1.5707963267949×i", Radian);
  assert_expression_approximates_to<double>("atan(2i)", "1.5707963267949+0.54930614433405×i", Radian);
  assert_expression_approximates_to<double>("atan(2i)", "90+31.472923730945×i", Degree);
  assert_expression_approximates_to<double>("arsinh(2i)", "1.3169578969248+1.5707963267949×i", Radian);
  assert_expression_approximates_to<double>("arcosh(-2)", "1.3169578969248+3.1415926535898×i", Radian);
}

QUIZ_CASE(poincare_approximation_matrix) {
  assert_expression_approximates_to<float>("[[1,2,3][4,5,6]]", "[[1,2,3][4,5,6]]");
  assert_expression_approximates_to<double>("[[1,2,3][4,5,6]]", "[[1,2,3][4,5,6]]");
}

QUIZ_CASE(poincare_approximation_list) {
  assert_expression_approximates_to<float>("{1,2,3,4,5,6}", "{1,2,3,4,5,6}");
  assert_expression_approximates_to<double>("{1,2,3,4,5,6}", "{1,2,3,4,5,6}");
}

QUIZ_CASE(poincare_approximation_list_sequence) {
  assert_expression_approximates_to<float>("sequence(k^2,k,4)", "{1,4,9,16}");
  assert_expression_approximates_to<double>("sequence(k/2,k,7)", "{0.5,1,1.5,2,2.5,3,3.5}");
}

QUIZ_CASE(poincare_approximation_map_on_list) {
  // We do not map on matrices anymore
  assert_expression_approximates_to<float>("abs([[1,-2][3,-4]])", Undefined::Name());
  assert_expression_approximates_to<double>("abs([[1,-2][3,-4]])", Undefined::Name());

  // TODO : Implement more tests on lists, with every functions
  assert_expression_approximates_to<float>("abs({1,-1,2,-3})", "{1,1,2,3}");
  assert_expression_approximates_to<float>("ceil({0.3,180})", "{1,180}");
  assert_expression_approximates_to<float>("cos({0,π})", "{1,-1}", Radian);
  assert_expression_approximates_to<float>("{1,3}!", "{1,6}");
  assert_expression_approximates_to<float>("{1,2,3,4}!", "{1,2,6,24}");
  assert_expression_approximates_to<float>("floor({1/√(2),1/2,1,-1.3})", "{0,0,1,-2}");
  assert_expression_approximates_to<float>("floor({0.3,180})", "{0,180}");
  assert_expression_approximates_to<float>("frac({0.3,180})", "{0.3,0}");
  assert_expression_approximates_to<float>("im({1/√(2),1/2,1,-1})", "{0,0,0,0}");
  assert_expression_approximates_to<float>("im({1,1+i})", "{0,1}");
  assert_expression_approximates_to<float>("re({1,i})", "{1,0}");
  assert_expression_approximates_to<float>("round({2.12,3.42}, 1)", "{2.1,3.4}");
  assert_expression_approximates_to<float>("round(1.23456, {2,3})", "{1.23,1.235}");
  assert_expression_approximates_to<float>("sin({0,π})", "{0,0}", Radian);
  assert_expression_approximates_to<float>("{2,3.4}-{0.1,3.1}", "{1.9,0.3}");
  assert_expression_approximates_to<float>("tan({0,π/4})", "{0,1}", Radian);
}

QUIZ_CASE(poincare_approximation_probability) {
  assert_expression_approximates_to<float>("binomcdf(5.3, 9, 0.7)", "0.270341", Degree, MetricUnitFormat, Cartesian, 6); // FIXME: precision problem
  assert_expression_approximates_to<double>("binomcdf(5.3, 9, 0.7)", "0.270340902", Degree, MetricUnitFormat, Cartesian, 10); //FIXME precision problem

  assert_expression_approximates_to<float>("binompdf(4.4, 9, 0.7)", "0.0735138", Degree, MetricUnitFormat, Cartesian, 6); // FIXME: precision problem
  assert_expression_approximates_to<double>("binompdf(4.4, 9, 0.7)", "0.073513818");

  assert_expression_approximates_to<float>("invbinom(0.9647324002, 15, 0.7)", "13");
  assert_expression_approximates_to<double>("invbinom(0.9647324002, 15, 0.7)", "13");
  assert_expression_approximates_to<float>("invbinom(0.95,100,0.42)", "50");
  assert_expression_approximates_to<double>("invbinom(0.95,100,0.42)", "50");
  assert_expression_approximates_to<float>("invbinom(0.01,150,0.9)", "126");
  assert_expression_approximates_to<double>("invbinom(0.01,150,0.9)", "126");

  assert_expression_approximates_to<double>("geompdf(1,1)", "1");
  assert_expression_approximates_to<double>("geompdf(2,0.5)", "0.25");
  assert_expression_approximates_to<double>("geompdf(2,1)", "0");
  assert_expression_approximates_to<double>("geompdf(1,0)", "undef");
  assert_expression_approximates_to<double>("geomcdf(2,0.5)", "0.75");
  assert_expression_approximates_to<double>("geomcdfrange(2,3,0.5)", "0.375");
  assert_expression_approximates_to<double>("geomcdfrange(2,2,0.5)", "0.25");
  assert_expression_approximates_to<double>("invgeom(1,1)", "1");
  assert_expression_approximates_to<double>("invgeom(0.825,0.5)", "3");

  assert_expression_approximates_to<double>("hgeompdf(-1,2,1,1)", "0");
  assert_expression_approximates_to<double>("hgeompdf(0,2,1,1)", "0.5");
  assert_expression_approximates_to<double>("hgeompdf(1,2,1,1)", "0.5");
  assert_expression_approximates_to<double>("hgeompdf(2,2,1,1)", "0");
  assert_expression_approximates_to<double>("hgeompdf(3,2,1,1)", "0");
  assert_expression_approximates_to<double>("hgeompdf(0,2,1,2)", "0");
  assert_expression_approximates_to<double>("hgeompdf(1,2,1,2)", "1");
  assert_expression_approximates_to<double>("hgeompdf(2,2,1,2)", "0");
  assert_expression_approximates_to<double>("hgeompdf(0,42,0,42)", "1");
  assert_expression_approximates_to<double>("hgeompdf(24,42,24,42)", "1");
  assert_expression_approximates_to<double>("hgeomcdf(1,4,2,3)", "0.5");
  assert_expression_approximates_to<double>("hgeomcdf(24,42,24,34)", "1");
  assert_expression_approximates_to<double>("hgeomcdfrange(2,3,6,3,4)", "0.8");
  assert_expression_approximates_to<double>("hgeomcdfrange(13,15,40,20,30)", "0.60937014162821");
  assert_expression_approximates_to<double>("invhgeom(.5,4,2,3)", "1");
  assert_expression_approximates_to<double>("invhgeom(.6,40,20,30)", "15");
  assert_expression_approximates_to<double>("invhgeom(-1,4,2,3)", "undef");
  assert_expression_approximates_to<double>("invhgeom(0,4,2,2)", "undef");
  assert_expression_approximates_to<double>("invhgeom(0,4,2,3)", "0");
  assert_expression_approximates_to<double>("invhgeom(1,4,2,3)", "2");

  assert_expression_approximates_to<double>("normcdf(5, 7, 0.3162)", "1.265256ᴇ-10", Radian, MetricUnitFormat, Cartesian, 7);

  assert_expression_approximates_to<float>("normcdf(1.2, 3.4, 5.6)", "0.3472125");
  assert_expression_approximates_to<double>("normcdf(1.2, 3.4, 5.6)", "0.34721249841587");
  assert_expression_approximates_to<float>("normcdf(-1ᴇ99,3.4,5.6)", "0");
  assert_expression_approximates_to<float>("normcdf(1ᴇ99,3.4,5.6)", "1");
  assert_expression_approximates_to<float>("normcdf(-6,0,1)", "0");
  assert_expression_approximates_to<float>("normcdf(6,0,1)", "1");

  assert_expression_approximates_to<float>("normcdfrange(0.5, 3.6, 1.3, 3.4)", "0.3436388");
  assert_expression_approximates_to<double>("normcdfrange(0.5, 3.6, 1.3, 3.4)", "0.34363881299147");

  assert_expression_approximates_to<float>("normpdf(1.2, 3.4, 5.6)", "0.06594901");
  assert_expression_approximates_to<float>("invnorm(0.56, 1.3, 2.4)", "1.662326");
  //assert_expression_approximates_to<double>("invnorm(0.56, 1.3, 2.4)", "1.6623258450088"); FIXME precision error

  assert_expression_approximates_to<float>("poissonpdf(2,1)", "0.1839397");
  assert_expression_approximates_to<double>("poissonpdf(2,1)", "0.18393972058572");
  assert_expression_approximates_to<float>("poissonpdf(2,2)", "0.2706706");
  assert_expression_approximates_to<float>("poissoncdf(2,2)", "0.6766764");
  assert_expression_approximates_to<double>("poissoncdf(2,2)", "0.67667641618306");

  assert_expression_approximates_to<float>("tpdf(1.2, 3.4)", "0.1706051");
  assert_expression_approximates_to<double>("tpdf(1.2, 3.4)", "0.17060506917323");
  assert_expression_approximates_to<float>("tcdf(0.5, 2)", "0.6666667");
  assert_expression_approximates_to<float>("tcdf(1.2, 3.4)", "0.8464878");
  assert_expression_approximates_to<double>("tcdf(1.2, 3.4)", "0.8464877995", Radian, MetricUnitFormat, Cartesian, 10); //FIXME: precision problem
  assert_expression_approximates_to<float>("invt(0.8464878,3.4)", "1.2");
  assert_expression_approximates_to<double>("invt(0.84648779949601043,3.4)", "1.2", Radian, MetricUnitFormat, Cartesian, 10);

}

QUIZ_CASE(poincare_approximation_complex_format) {
  // Real
  assert_expression_approximates_to<float>("0", "0", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("0", "0", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("10", "10", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("-10", "-10", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("100", "100", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("0.1", "0.1", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("0.1234567", "0.1234567", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("0.123456789012345", "0.12345678901235", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("1+2×i", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("1+i-i", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("1+i-1", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("1+i", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("3+i", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("3-i", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("3-i-3", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("i", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("√(-1)", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("√(-1)×√(-1)", "nonreal", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("ln(-2)", "nonreal", Radian, MetricUnitFormat, Real);
  // Power/Root approximates to the first REAL root in Real mode
  assert_expression_simplifies_approximates_to<double>("(-8)^(1/3)", "-2", Radian, MetricUnitFormat, Real); // Power have to be simplified first in order to spot the right form c^(p/q) with p, q integers
  assert_expression_approximates_to<double>("root(-8,3)", "-2", Radian, MetricUnitFormat, Real); // Root approximates to the first REAL root in Real mode
  assert_expression_approximates_to<double>("8^(1/3)", "2", Radian, MetricUnitFormat, Real);
  assert_expression_simplifies_approximates_to<float>("(-8)^(2/3)", "4", Radian, MetricUnitFormat, Real); // Power have to be simplified first (cf previous comment)
  assert_expression_approximates_to<float>("root(-8, 3)^2", "4", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("root(-8,3)", "-2", Radian, MetricUnitFormat, Real);

  // Cartesian
  assert_expression_approximates_to<float>("0", "0", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("0", "0", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("10", "10", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("-10", "-10", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("100", "100", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("0.1", "0.1", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("0.1234567", "0.1234567", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("0.123456789012345", "0.12345678901235", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("1+2×i", "1+2×i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("1+i-i", "1", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("1+i-1", "i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("1+i", "1+i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("3+i", "3+i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("3-i", "3-i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("3-i-3", "-i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("i", "i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("√(-1)", "i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("√(-1)×√(-1)", "-1", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("ln(-2)", "0.69314718055995+3.1415926535898×i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("(-8)^(1/3)", "1+1.7320508075689×i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("(-8)^(2/3)", "-2+3.4641×i", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("root(-8,3)", "1+1.7320508075689×i", Radian, MetricUnitFormat, Cartesian);

  // Polar
  assert_expression_approximates_to<float>("0", "0", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("0", "0", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<float>("10", "10", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("-10", "10×e^\u00123.1415926535898×i\u0013", Radian, MetricUnitFormat, Polar);

  assert_expression_approximates_to<float>("100", "100", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("0.1", "0.1", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<float>("0.1234567", "0.1234567", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("0.12345678", "0.12345678", Radian, MetricUnitFormat, Polar);

  assert_expression_approximates_to<float>("1+2×i", "2.236068×e^\u00121.107149×i\u0013", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<float>("1+i-i", "1", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("1+i-1", "e^\u00121.57079632679×i\u0013", Radian, MetricUnitFormat, Polar, 12);
  assert_expression_approximates_to<float>("1+i", "1.414214×e^\u00120.7853982×i\u0013", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("3+i", "3.16227766017×e^\u00120.321750554397×i\u0013", Radian, MetricUnitFormat, Polar,12);
  assert_expression_approximates_to<float>("3-i", "3.162278×e^\u0012-0.3217506×i\u0013", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("3-i-3", "e^\u0012-1.57079632679×i\u0013", Radian, MetricUnitFormat, Polar,12);
   // 2e^(i) has a too low precision in float on the web platform
  assert_expression_approximates_to<float>("3e^(2*i)", "3×e^\u00122×i\u0013", Radian, MetricUnitFormat, Polar, 4);
  assert_expression_approximates_to<double>("2e^(-i)", "2×e^\u0012-i\u0013", Radian, MetricUnitFormat, Polar, 9);

  assert_expression_approximates_to<float>("i", "e^\u00121.570796×i\u0013", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("√(-1)", "e^\u00121.5707963267949×i\u0013", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("√(-1)×√(-1)", "e^\u00123.1415926535898×i\u0013", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("(-8)^(1/3)", "2×e^\u00121.0471975511966×i\u0013", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<float>("(-8)^(2/3)", "4×e^\u00122.094395×i\u0013", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("root(-8,3)", "2×e^\u00121.0471975511966×i\u0013", Radian, MetricUnitFormat, Polar);

  // Cartesian to Polar and vice versa
  assert_expression_approximates_to<double>("2+3×i", "3.60555127546×e^\u00120.982793723247×i\u0013", Radian, MetricUnitFormat, Polar, 12);
  assert_expression_approximates_to<double>("3.60555127546×e^(0.982793723247×i)", "2+3×i", Radian, MetricUnitFormat, Cartesian, 12);
  assert_expression_approximates_to<float>("12.04159457879229548012824103×e^(1.4876550949×i)", "1+12×i", Radian, MetricUnitFormat, Cartesian, 5);

  // Overflow
  assert_expression_approximates_to<float>("-2ᴇ20+2ᴇ20×i", "-2ᴇ20+2ᴇ20×i", Radian, MetricUnitFormat, Cartesian);
  /* TODO: this test fails on the device because libm hypotf (which is called
   * eventually by std::abs) is not accurate enough. We might change the
   * embedded libm? */
  //assert_expression_approximates_to<float>("-2ᴇ20+2ᴇ20×i", "2.828427ᴇ20×e^\u00122.356194×i\u0013", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<float>("-2ᴇ10+2ᴇ10×i", "2.828427ᴇ10×e^\u00122.356194×i\u0013", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("1ᴇ155-1ᴇ155×i", "1ᴇ155-1ᴇ155×i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("1ᴇ155-1ᴇ155×i", "1.41421356237ᴇ155×e^\u0012-0.785398163397×i\u0013", Radian, MetricUnitFormat, Polar,12);
  assert_expression_approximates_to<float>("-2ᴇ100+2ᴇ100×i", "-∞+∞×i");
  assert_expression_approximates_to<double>("-2ᴇ360+2ᴇ360×i", "-∞+∞×i");
  assert_expression_approximates_to<float>("-2ᴇ100+2ᴇ10×i", "-∞+2ᴇ10×i");
  assert_expression_approximates_to<double>("-2ᴇ360+2×i", "-∞+2×i");
  assert_expression_approximates_to<float>("undef+2ᴇ100×i", Undefined::Name());
  assert_expression_approximates_to<double>("-2ᴇ360+undef×i", Undefined::Name());
}

QUIZ_CASE(poincare_approximation_mix) {
  assert_expression_approximates_to<float>("-2-3", "-5");
  assert_expression_approximates_to<float>("1.2×e^(1)", "3.261938");
  assert_expression_approximates_to<float>("2e^(3)", "40.1711", Radian, MetricUnitFormat, Cartesian, 6); // WARNING: the 7th significant digit is wrong on blackbos simulator
  assert_expression_approximates_to<float>("e^2×e^(1)", "20.0855", Radian, MetricUnitFormat, Cartesian, 6); // WARNING: the 7th significant digit is wrong on simulator
  assert_expression_approximates_to<double>("e^2×e^(1)", "20.085536923188");
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
  assert_expression_simplifies_and_approximates_to("1.0092^(50)×ln(3/2)", "0.6409373488899", Degree, MetricUnitFormat, Cartesian, 13);
  assert_expression_simplifies_and_approximates_to("1.0092^(50)×ln(1.0092)", "0.01447637354655", Degree, MetricUnitFormat, Cartesian, 13);
  assert_expression_approximates_to<double>("1.0092^(20)", "1.2010050593402");
  assert_expression_approximates_to<double>("1.0092^(50)×ln(3/2)", "0.6409373488899", Degree, MetricUnitFormat, Cartesian, 13);
  assert_expression_approximates_to<double>("1.0092^(50)×ln(1.0092)", "0.01447637354655", Degree, MetricUnitFormat, Cartesian, 13);
  assert_expression_simplifies_approximates_to<double>("1.0092^(20)", "1.2010050593402");
  assert_expression_simplifies_approximates_to<double>("1.0092^(50)×ln(3/2)", "0.6409373488899", Degree, MetricUnitFormat, Cartesian, 13);
  //assert_expression_approximates_to<float>("1.0092^(20)", "1.201005"); TODO does not work
  assert_expression_approximates_to<float>("1.0092^(50)×ln(3/2)", "0.6409366");
  //assert_expression_simplifies_approximates_to<float>("1.0092^(20)", "1.2010050593402"); TODO does not work
  //assert_expression_simplifies_approximates_to<float>("1.0092^(50)×ln(3/2)", "6.4093734888993ᴇ-1"); TODO does not work
}

QUIZ_CASE(poincare_approximation_lists_access) {
  assert_reduce_and_store("{1,2,3,4,5}→L");

  assert_expression_approximates_to<float>("L(1)", "1");
  assert_expression_approximates_to<float>("L(0)", Undefined::Name());
  assert_expression_approximates_to<float>("L(7)", Undefined::Name());
  assert_expression_approximates_to<double>("L(1)", "1");
  assert_expression_approximates_to<double>("L(0)", Undefined::Name());
  assert_expression_approximates_to<double>("L(7)", Undefined::Name());

  assert_expression_approximates_to<float>("L(1,3)", "{1,2,3}");
  assert_expression_approximates_to<float>("L(1,9)", "{1,2,3,4,5}");
  assert_expression_approximates_to<float>("L(-5,3)", "{1,2,3}");
  assert_expression_approximates_to<float>("L(3,1)","{}");
  assert_expression_approximates_to<float>("L(8,9)","{}");

  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("L.lis").destroy();
}

QUIZ_CASE(poincare_approximation_lists_functions) {
  assert_expression_approximates_to<double>("sort({5,8,7,undef,-inf})", "{-∞,5,7,8,undef}");
  assert_expression_approximates_to_scalar<double>("mean({5,8,7,4,12})", 7.2);
  assert_expression_approximates_to_scalar<double>("mean({5,8,7,4,12},{1,2,3,5,6})", 7.882352941176471);
  assert_expression_approximates_to<double>("mean({5,8,7,4,12},{2})", Undefined::Name());
  assert_expression_approximates_to<double>("mean({5,8,7,4,12},{-1,1,1,1,1})", Undefined::Name());
  assert_expression_approximates_to<double>("mean({5,8,7,4,12},{0,0,0,0,0})", Undefined::Name());
  assert_expression_approximates_to_scalar<double>("stddev({1,2,3,4,5,6})", 1.707825127659933);
  assert_expression_approximates_to_scalar<double>("stddev({1,2,3,4,5,6},{6,2,3,4,5,1})", 1.6700645635000173);
  assert_expression_approximates_to_scalar<double>("stddev({1})", 0.);
  assert_expression_approximates_to_scalar<double>("med({1,6,3,5,2})", 3.);
  assert_expression_approximates_to_scalar<double>("med({1,6,3,4,5,2})", 3.5);
  assert_expression_approximates_to_scalar<double>("med({1,6,3,4,5,2},{2,3,0.1,2.8,3,1})", 5.);
  assert_expression_approximates_to<double>("med({1,undef,6,3,5,undef,2})", Undefined::Name());
  assert_expression_approximates_to_scalar<double>("var({1,2,3,4,5,6})", 2.916666666666666);
  assert_expression_approximates_to<double>("var({1,2,3,undef,4,5,6})", Undefined::Name());
  assert_expression_approximates_to_scalar<double>("var({1,2,3,4,5,6},{7,0.1,2,0,1,10})", 5.2815524368208706);
  assert_expression_approximates_to<double>("var({1,2,3,3,4,5,6},{-2,2,2,2,2,2,2})", Undefined::Name());
  assert_expression_approximates_to<double>("var({1,2,3,4,5,6},{0,0,0,0,0,0})", Undefined::Name());
  assert_expression_approximates_to_scalar<double>("samplestddev({1,2,3,4,5,6})", 1.8708286933869704);
  assert_expression_approximates_to_scalar<double>("samplestddev({1,2,3,4,5,6},{6,2,3,4,5,1})", 1.7113069358158486);
  assert_expression_approximates_to<double>("samplestddev({1})", Undefined::Name());
  assert_expression_approximates_to_scalar<double>("dim({1,2,3})", 3.);
  assert_expression_approximates_to_scalar<double>("min({1,2,3})", 1.);
  // undef is never the min (unless there are only undef in the list)
  assert_expression_approximates_to<double>("min({undef})", Undefined::Name());
  assert_expression_approximates_to_scalar<double>("min({1,undef,3})", 1.);
  assert_expression_approximates_to_scalar<double>("max({1,2,3})", 3.);
  // undef is never the max (unless there are only undef in the list)
  assert_expression_approximates_to<double>("max({undef})", Undefined::Name());
  assert_expression_approximates_to_scalar<double>("max({1,undef,3})", 3.);
  assert_expression_approximates_to_scalar<double>("sum({1,2,3})", 6.);
  assert_expression_approximates_to_scalar<double>("prod({1,4,9})", 36.);
}

QUIZ_CASE(poincare_approximation_mixed_fraction) {
  assert_expression_approximates_to_scalar<double>("1 1/2", 1.5);
  assert_expression_approximates_to_scalar<double>("-1 1/2", -1.5);
  assert_expression_approximates_to_scalar<double>("1\u00121/2\u0013", 1.5);
  assert_expression_approximates_to_scalar<double>("1\u00121/2\u0013", 0.5, Degree, Cartesian, Preferences::MixedFractions::Disabled);
}

template<typename T>
void assert_expression_approximates_with_value_for_symbol(const char * expression, T approximation, const char * symbol, T symbolValue, Preferences::AngleUnit angleUnit = Degree, Preferences::ComplexFormat complexFormat = Cartesian) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  T result = e.approximateWithValueForSymbol<T>(symbol, symbolValue, &globalContext, complexFormat, angleUnit);
  quiz_assert_print_if_failure(roughly_equal(result, approximation, Poincare::Float<T>::EpsilonLax(), true), expression);
}

QUIZ_CASE(poincare_approximation_floor_ceil_integer) {
  constexpr double upperBound = 1000.;
  for (double d = 0.; d < upperBound; d += 1.) {
    assert_expression_approximates_with_value_for_symbol("floor(x * (x+1)^(-1) + x^2 * (x+1)^(-1))", d, "x", d);
    assert_expression_approximates_with_value_for_symbol("ceil(x * (x+1)^(-1) + x^2 * (x+1)^(-1))", d, "x", d);
  }
}

QUIZ_CASE(poincare_approximation_booleans) {
  assert_expression_approximates_to<float>("True and 3<π", "True");
  assert_expression_approximates_to<float>("3>π or False", "False");
  assert_expression_approximates_to<float>("not({3<π, 3>π})", "{False,True}");

  assert_expression_approximates_to<float>("2 and 3", Undefined::Name());
  assert_expression_approximates_to<float>("True + 3", Undefined::Name());
}

QUIZ_CASE(poincare_approximation_piecwise_operator) {
  assert_expression_approximates_to<float>("piecewise(3,1<0,2)", "2");
  assert_expression_approximates_to<float>("piecewise(3,1>0,2)", "3");
  assert_expression_approximates_to<float>("piecewise(3,0>1,4,0>2,5,0<6,2)", "5");
  assert_expression_approximates_to<float>("piecewise(3,0<1,4,0<2,5,0<6,2)", "3");

  assert_expression_approximates_to<float>("piecewise(3,1<0,2,3=4)", Undefined::Name());
}

template void assert_expression_approximates_to_scalar(const char * expression, float approximation, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat, Preferences::MixedFractions mixedFractionsParameter);
template void assert_expression_approximates_to_scalar(const char * expression, double approximation, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat, Preferences::MixedFractions mixedFractionsParameter);
