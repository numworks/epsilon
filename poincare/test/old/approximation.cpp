#include <apps/shared/global_context.h>
#include <poincare/print.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/test/helper.h>

#include "helper.h"

using namespace Poincare;

template <typename T>
void assert_float_approximates_to(Expression f, const char *result) {
#if 0  // TODO_PCJ
  Shared::GlobalContext globalContext;
  int numberOfDigits = PrintFloat::SignificantDecimalDigits<T>();
  char buffer[500];
  f.approximate<T>(ApproximationContext(&globalContext, Cartesian, Radian))
      .serialize(buffer, sizeof(buffer), DecimalMode, numberOfDigits);
  quiz_assert_print_if_failure(strcmp(buffer, result) == 0, result);
#endif
}

QUIZ_CASE(poincare_approximation_float) {
  assert_float_approximates_to<double>(
      Expression::Builder<double>(-1.23456789E30), "-1.23456789ᴇ30");
  assert_float_approximates_to<double>(
      Expression::Builder<double>(1.23456789E30), "1.23456789ᴇ30");
  assert_float_approximates_to<double>(
      Expression::Builder<double>(-1.23456789E-30), "-1.23456789ᴇ-30");
  assert_float_approximates_to<double>(Expression::Builder<double>(-1.2345E-3),
                                       "-0.0012345");
  assert_float_approximates_to<double>(Expression::Builder<double>(1.2345E-3),
                                       "0.0012345");
  assert_float_approximates_to<double>(Expression::Builder<double>(1.2345E3),
                                       "1234.5");
  assert_float_approximates_to<double>(Expression::Builder<double>(-1.2345E3),
                                       "-1234.5");
  assert_float_approximates_to<double>(
      Expression::Builder<double>(0.99999999999995), "0.99999999999995");
  assert_float_approximates_to<double>(
      Expression::Builder<double>(0.00000099999999999995),
      "9.9999999999995ᴇ-7");
  assert_float_approximates_to<double>(
      Expression::Builder<double>(
          0.0000009999999999901200121020102010201201201021099995),
      "9.9999999999012ᴇ-7");
  assert_float_approximates_to<float>(Expression::Builder<float>(1.2345E-1),
                                      "0.12345");
  assert_float_approximates_to<float>(Expression::Builder<float>(1), "1");
  assert_float_approximates_to<float>(
      Expression::Builder<float>(0.9999999999999995), "1");
  assert_float_approximates_to<float>(Expression::Builder<float>(1.2345E6),
                                      "1234500");
  assert_float_approximates_to<float>(Expression::Builder<float>(-1.2345E6),
                                      "-1234500");
  assert_float_approximates_to<float>(
      Expression::Builder<float>(0.0000009999999999999995), "1ᴇ-6");
  assert_float_approximates_to<float>(Expression::Builder<float>(-1.2345E-1),
                                      "-0.12345");

  assert_float_approximates_to<double>(Expression::Builder<double>(INFINITY),
                                       "∞");
  assert_float_approximates_to<float>(Expression::Builder<float>(0.0f), "0");
  assert_float_approximates_to<float>(Expression::Builder<float>(NAN), "undef");
}

template <typename T>
void assert_expression_approximation_is_bounded(const char *expression,
                                                T lowBound, T upBound,
                                                bool upBoundIncluded = false) {
  // TODO_PCJ
#if 0
  Shared::GlobalContext globalContext;
  Internal::Tree *e = parse_expression(expression, &globalContext, true);
  T result = Internal::Approximation::To<T>(
      e, Internal::Approximation::Parameters{.isRootAndCanHaveRandom = true, .projectLocalVariables=true},
      Internal::Approximation::Context(Radian, Cartesian, &globalContext));
  quiz_assert_print_if_failure(result >= lowBound, expression);
  quiz_assert_print_if_failure(
      result < upBound || (result == upBound && upBoundIncluded), expression);
  e->removeTree();
#endif
}

QUIZ_CASE(poincare_approximation_function) {
  assert_expression_approximates_to<float>("abs(-2.3ᴇ-39)", "2.3ᴇ-39", Degree,
                                           MetricUnitFormat, Cartesian, 5);

  // inverse is not precise enough to display 7 significative digits
  assert_expression_approximates_to<float>(
      "inverse([[1,2,3][4,5,-6][7,8,9]])",
      "[[-1.2917,-0.083333,0.375][1.0833,0.16667,-0.25][0.041667,-0.083333,0."
      "041667]]",
      Degree, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<double>(
      "inverse([[1,2,3][4,5,-6][7,8,9]])",
      "[[-1.2916666666667,-0.083333333333333,0.375][1.0833333333333,0."
      "16666666666667,-0.25][0.041666666666667,-0.083333333333333,0."
      "041666666666667]]");
  // inverse is not precise enough to display 7 significative digits
  assert_expression_approximates_to<float>(
      "inverse([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])",
      "[[-0.0118-0.0455×i,-0.5-0.727×i,0.318+0.489×i][0.0409+0.00364×i,0.04-0."
      "0218×i,-0.0255+9.1ᴇ-4×i][0.00334-0.00182×i,0.361+0.535×i,-0.13-0.358×i]"
      "]",
      Degree, MetricUnitFormat, Cartesian, 3);
  // FIXME: inverse is not precise enough to display 14 significative digits
  assert_expression_approximates_to<double>(
      "inverse([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])",
      "[[-0.0118289353958-0.0454959053685×i,-0.500454959054-0.727024567789×i,0."
      "31847133758+0.488626023658×i][0.0409463148317+0.00363967242948×i,0."
      "0400363967243-0.0218380345769×i,-0.0254777070064+9.0991810737ᴇ-4×i][0."
      "00333636639369-0.00181983621474×i,0.36093418259+0.534728541098×i,-0."
      "130118289354-0.357597816197×i]]",
      Degree, MetricUnitFormat, Cartesian, 12);

  assert_expression_approximates_to<float>(
      "root(5^((-i)3^9),i)", "3.504", Degree, MetricUnitFormat, Cartesian, 4);
  assert_expression_approximates_to<double>("root(5^((-i)3^9),i)",
                                            "3.5039410843", Degree,
                                            MetricUnitFormat, Cartesian, 11);

  assert_expression_approximation_is_bounded("random()", 0.0f, 1.0f);
  assert_expression_approximation_is_bounded("random()", 0.0, 1.0);

  assert_expression_approximation_is_bounded("randint(4,45)", 4.0f, 45.0f,
                                             true);
  assert_expression_approximation_is_bounded("randint(4,45)", 4.0, 45.0, true);
  assert_expression_approximation_is_bounded("randint(1, 10)", 1.0, 10.0, true);
  assert_expression_approximation_is_bounded(
      "randint(0, 2^31 - 1)", 0.0, static_cast<double>(0x7FFFFFFF), true);
  assert_expression_approximation_is_bounded(
      "randint(123456789876543, 123456789876543+10)", 123456789876543.0,
      123456789876553.0, true);
  assert_expression_approximation_is_bounded(
      "randint(-123456789876543, 123456789876543)", -123456789876543.0,
      123456789876543.0, true);
  assert_expression_approximates_to<double>("randint({1,1})", "{1,1}");
  assert_expression_approximates_to<double>("randint({1,2},{1,2})", "{1,2}");
  assert_expression_approximates_to<double>("1/randint(2,2)+1/2", "1");
  assert_expression_approximates_to<double>("randint(45,4)", "undef");
  assert_expression_approximates_to<double>("randint(1, inf)", "undef");
  assert_expression_approximates_to<double>("randint(-inf, 3)", "undef");
  assert_expression_approximates_to<double>("randint(4, 3)", "undef");
#if 0  // TODO_PCJ:
  assert_expression_approximates_to<double>("randint(2, 23345678909876545678)",
                                            "undef");
#endif
}

template <typename T>
void assert_no_duplicates_in_list(const char *expression) {
  Shared::GlobalContext globalContext;
  Internal::Tree *e = parse_expression(expression, &globalContext);
  // Sort list
  e->cloneNodeAtNode(Poincare::Internal::KListSort);
  e->moveTreeOverTree(Internal::Approximation::ToTree<T>(
      e,
      Internal::Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                          .projectLocalVariables = true},
      Internal::Approximation::Context(Radian, Cartesian)));
  // Find duplicates
  assert(e->isList());
  int n = e->numberOfChildren();
  bool bad = false;
  for (int i = 1; i < n; i++) {
    if (e->child(i)->treeIsIdenticalTo(e->child(i - 1))) {
      bad = true;
      break;
    }
  }
#if POINCARE_STRICT_TESTS
  quiz_assert_print_if_failure(!bad, expression);
#else
  quiz_tolerate_print_if_failure(!bad, expression, "no duplicates",
                                 "duplicates");
#endif
  e->removeTree();
}

QUIZ_CASE(poincare_approximation_unique_random) {
  assert_expression_approximates_to<double>("randintnorep(10,1,3)", "undef");
  assert_expression_approximates_to<double>("randintnorep(1,10,100)", "undef");
  assert_expression_approximates_to<double>("randintnorep(1,10,-1)", "undef");
  assert_expression_approximates_to<double>("randintnorep(1,10,0)", "{}");
  assert_expression_approximates_to<double>(
      "randintnorep(536427840,-2145711360,4)", "undef");

  assert_no_duplicates_in_list<float>("randintnorep(-100,99,200)");
  assert_no_duplicates_in_list<float>("randintnorep(1234,5678,20)");

  // Random lists can be sorted
  assert_expression_approximates_to<double>("sort(randintnorep(5,8,4))",
                                            "{5,6,7,8}");

  /* The simplification process should understand that the expression is not a
   * scalar if it encounters a randintnorep. */
  assert_expression_simplifies_approximates_to<double>(
      "rem(randintnorep(1,10,5),1)", "{0,0,0,0,0}");
}

QUIZ_CASE(poincare_approximation_integral) {
  assert_expression_approximates_to<float>("int(x,x, 1, 2)", "1.5");
  assert_expression_approximates_to<double>("int(x,x, 1, 2)", "1.5");
  assert_expression_approximates_to<float>("int(1/x,x,0,1)", "undef");

  // Ensure this does not take up too much time
  assert_expression_approximates_to<double>("int(e^(x^3),x,0,6)",
                                            "5.9639380918976ᴇ91");

  assert_expression_approximates_to<float>("int(1+cos(a),a, 0, 180)", "180");
  assert_expression_approximates_to<double>("int(1+cos(a),a, 0, 180)", "180");

  // former precision issues
  // #1912
  assert_expression_approximates_to<double>("int(abs(2*sin(e^(x/4))+1),x,0,6)",
                                            "12.573260585347", Radian);
  // #1912
  assert_expression_approximates_to<double>("int(4*√(1-x^2),x,0,1)",
                                            "3.1415926535898");
  // Runge function
  assert_expression_approximates_to<double>("int(1/(1+25*x^2),x,0,1)",
                                            "0.274680153389");
  // #1378
  assert_expression_approximates_to<double>("int(2*√(9-(x-3)^2),x,0,6)",
                                            "28.274333882309");

  // far bounds
  assert_expression_approximates_to<double>("int(e^(-x^2),x,0,100000)",
                                            "0.88622692545276");
  assert_expression_approximates_to<double>("int(1/(x^2+1),x,0,100000)",
                                            "1.5707863267949");
  assert_expression_approximates_to<double>("int(1/(x^2+1),x,0,inf)",
                                            "1.5707963267949");
  assert_expression_approximates_to<double>("int(1/x,x,1,1000000)",
                                            "13.81551056", Radian,
                                            MetricUnitFormat, Cartesian, 10);
  assert_expression_approximates_to<double>("int(e^(-x)/√(x),x,0,inf)",
                                            "1.7724", Radian, MetricUnitFormat,
                                            Cartesian, 5);
  assert_expression_approximates_to<double>("int(1,x,inf,0)", "undef");
  assert_expression_approximates_to<double>("int(e^(-x),x,inf,0)", "-1");

  // singularities
  assert_expression_approximates_to<double>("int(ln(x)*√(x),x,0,1)",
                                            "-0.444444444444", Radian,
                                            MetricUnitFormat, Cartesian, 12);
  assert_expression_approximates_to<double>("int(1/√(x),x,0,1)", "2", Radian,
                                            MetricUnitFormat, Cartesian, 12);
  assert_expression_approximates_to<double>("int(1/√(1-x),x,0,1)", "2", Radian,
                                            MetricUnitFormat, Cartesian, 12);
  assert_expression_approximates_to<double>("int(1/√(x)+1/√(1-x),x,0,1)", "4",
                                            Radian, MetricUnitFormat, Cartesian,
                                            12);
  assert_expression_approximates_to<double>("int(ln(x)^2,x,0,1)", "2", Radian,
                                            MetricUnitFormat, Cartesian, 12);
  // #596
  assert_expression_approximates_to<double>("int(1/√(x-1),x,1,2)", "2", Radian,
                                            MetricUnitFormat, Cartesian, 12);
  // #1780
  assert_expression_approximates_to<double>("int(2/√(1-x^2),x,0,1)",
                                            "3.1415926535898");
  // #1780
  assert_expression_approximates_to<double>("int(4x/√(1-x^4),x,0,1)",
                                            "3.1415926535898");
  // convergence is slow with 1/x^k k≈1, therefore precision is poor
  assert_expression_approximates_to<float>("int(1/x^0.9,x,0,1)", "10", Radian,
                                           MetricUnitFormat, Cartesian, 3);

  // double integration
  assert_expression_approximates_to<float>("int(int(x×x,x,0,x),x,0,4)",
                                           "21.33333");
  assert_expression_approximates_to<double>("int(int(x×x,x,0,x),x,0,4)",
                                            "21.333333333333");
  assert_expression_approximates_to<double>("int(sum(sin(x)^k,k,0,100),x,0,π)",
                                            "48.3828", Radian, MetricUnitFormat,
                                            Cartesian, 6);
  assert_expression_approximates_to<double>(
      "int(int(1/(1-x)^k,x,0,1),k,0.5,0.25)", "-0.405465", Radian,
      MetricUnitFormat, Cartesian, 6);
  // this integral on R2 takes about one minute to compute on N110
  // assert_expression_approximates_to<double>("int(int(e^(-(x^2+y^2)),x,-inf,inf),y,-inf,inf)",
  // "3.141592654", Radian, MetricUnitFormat, Cartesian, 10);
  // assert_expression_approximates_to<double>("int(int(e^(-x*t)/t^2,t,1,inf),x,0,1)",
  // "0.3903080328", Radian, MetricUnitFormat, Cartesian, 10);

  // oscillator
  // poor precision
  assert_expression_approximates_to<double>("int((sin(x)/x)^2,x,0,inf)",
                                            "1.5708", Radian, MetricUnitFormat,
                                            Cartesian, 5);
  assert_expression_approximates_to<double>("int(x*sin(1/x)*√(abs(1-x)),x,0,3)",
                                            "1.9819412", Radian,
                                            MetricUnitFormat, Cartesian, 8);

  /* The integral approximation escapes before finding a result, because it
   * would take too much time. */
  assert_expression_approximates_to<float>("int(sin((10^7)*x),x,0,1)", "undef");
}

QUIZ_CASE(poincare_approximation_trigonometry_functions) {
  // On C
  assert_expression_approximates_to<float>("cos(i-4)", "-1.008625-0.8893952×i",
                                           Radian);
  assert_expression_approximates_to<float>("cos(i-4)", "0.997716+0.00121754×i",
                                           Degree, MetricUnitFormat, Cartesian,
                                           6);
  assert_expression_approximates_to<float>("cos(i-4)", "0.99815+9.86352ᴇ-4×i",
                                           Gradian, MetricUnitFormat, Cartesian,
                                           6);

  // Advanced function : sec
  assert_expression_approximates_to<float>("sec(i-4)", "-0.5577604+0.4918275×i",
                                           Radian, MetricUnitFormat, Cartesian,
                                           7);

  // On: C
  assert_expression_approximates_to<float>(
      "sin(i-4)", "1.16781-0.768163×i", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("sin(i-4)", "-0.0697671+0.0174117×i",
                                           Degree, MetricUnitFormat, Cartesian,
                                           6);
  assert_expression_approximates_to<float>("sin(i-4)", "-0.0627983+0.0156776×i",
                                           Gradian, MetricUnitFormat, Cartesian,
                                           6);
  assert_expression_approximates_to<float>("sin(1.234567890123456ᴇ-15)",
                                           "1.23457ᴇ-15", Radian,
                                           MetricUnitFormat, Cartesian, 6);

  // Advanced function : csc
  assert_expression_approximates_to<float>("csc(i-4)", "0.597696+0.393154×i",
                                           Radian, MetricUnitFormat, Cartesian,
                                           6);

  /* tan: R  ->  R (tangent-style)
   *      Ri ->  Ri (odd)
   */
  // On R
  assert_expression_approximates_to<double>("tan(2)", "-2.1850398632615",
                                            Radian);
  assert_expression_approximates_to<double>("tan(2)", "0.034920769491748",
                                            Degree);
  assert_expression_approximates_to<double>("tan(2)", "0.031426266043351",
                                            Gradian);
  // Tangent-style
  assert_expression_approximates_to<float>("tan(3×π)", "0", Radian);
  assert_expression_approximates_to<float>("tan(-540)", "0", Degree);
  assert_expression_approximates_to<float>("tan(-600)", "0", Gradian);
  // On R×i
  assert_expression_approximates_to<double>("tan(-2×i)", "-0.96402758007582×i",
                                            Radian);
  assert_expression_approximates_to<float>("tan(-2×i)", "-0.03489241×i",
                                           Degree);
  assert_expression_approximates_to<float>("tan(-3×i)", "-0.04708904×i",
                                           Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("tan(2×i)", "0.96402758007582×i",
                                            Radian);
  assert_expression_approximates_to<float>("tan(2×i)", "0.03489241×i", Degree);
  assert_expression_approximates_to<float>("tan(3×i)", "0.04708904×i", Gradian);
  // On C
  assert_expression_approximates_to<float>("tan(i-4)", "-0.273553+1.00281×i",
                                           Radian, MetricUnitFormat, Cartesian,
                                           6);
  assert_expression_approximates_to<float>("tan(i-4)", "-0.0699054+0.0175368×i",
                                           Degree, MetricUnitFormat, Cartesian,
                                           6);
  assert_expression_approximates_to<float>("tan(i-4)", "-0.0628991+0.0157688×i",
                                           Gradian, MetricUnitFormat, Cartesian,
                                           6);
  // tan(π/2)
  assert_expression_approximates_to<double>("tan(π/2)", "undef", Radian);
  assert_expression_approximates_to<double>("sin(π/2)/cos(π/2)", "undef",
                                            Radian);
  assert_expression_approximates_to<double>("1/cot(π/2)", "undef", Radian);
  // tan(almost π/2)
  assert_expression_approximates_to<double>("tan(1.57079632)",
                                            "147169276.57047", Radian);
  assert_expression_approximates_to<double>("sin(1.57079632)/cos(1.57079632)",
                                            "147169276.57047", Radian);
  assert_expression_approximates_to<double>("1/cot(1.57079632)",
                                            "147169276.57047", Radian);
  // Advanced function : cot
  assert_expression_approximates_to<double>("cot(2)", "-0.45765755436029",
                                            Radian);
  assert_expression_approximates_to<float>("cot(100)", "0", Gradian);
  assert_expression_approximates_to<float>("cot(3×i)", "-19.11604×i", Degree);
  assert_expression_approximates_to<float>("cot(-3×i)", "19.11604×i", Degree);
  assert_expression_approximates_to<float>("cot(i-4)", "-0.253182-0.928133×i",
                                           Radian, MetricUnitFormat, Cartesian,
                                           6);
  // cot(π/2)
  assert_expression_approximates_to<double>("1/tan(π/2)", "undef", Radian);
  assert_expression_approximates_to<double>("cos(π/2)/sin(π/2)", "0", Radian);
  assert_expression_approximates_to<double>("cot(π/2)", "0", Radian);
  // cot(almost π/2)
  assert_expression_approximates_to<double>("1/tan(1.57079632)",
                                            "6.7948964845335ᴇ-9", Radian);
  assert_expression_approximates_to<double>("cos(1.57079632)/sin(1.57079632)",
                                            "6.7948964845335ᴇ-9", Radian);
  assert_expression_approximates_to<double>("cot(1.57079632)",
                                            "6.7948964845335ᴇ-9", Radian);

  /* acos: [-1,1]    -> R
   *       ]-inf,-1[ -> π+R×i (odd imaginary)
   *       ]1, inf[  -> R×i (odd imaginary)
   *       R×i       -> π/2+R×i (odd imaginary)
   */
  // On [-1, 1]
  assert_expression_approximates_to<double>("acos(0.5)", "1.0471975511966",
                                            Radian);
  assert_expression_approximates_to<double>("acos(0.03)", "1.5407918249714",
                                            Radian);
  assert_expression_approximates_to<double>("acos(0.5)", "60", Degree);
  assert_expression_approximates_to<double>("acos(0.5)", "66.666666666667",
                                            Gradian);
  // On [1, inf[
  assert_expression_approximates_to<double>("acos(2)", "nonreal", Radian,
                                            MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("acos(2)", "1.3169578969248×i",
                                            Radian);
  assert_expression_approximates_to<double>("acos(2)", "75.456129290217×i",
                                            Degree);
  assert_expression_approximates_to<double>("acos(2)", "83.84×i", Gradian,
                                            MetricUnitFormat, Cartesian, 4);
  // Symmetry: odd on imaginary
  assert_expression_approximates_to<double>(
      "acos(-2)", "3.1415926535898-1.3169578969248×i", Radian);
  assert_expression_approximates_to<double>("acos(-2)", "180-75.456129290217×i",
                                            Degree);
  assert_expression_approximates_to<double>("acos(-2)", "200-83.84×i", Gradian,
                                            MetricUnitFormat, Cartesian, 4);
  // On ]-inf, -1[
  assert_expression_approximates_to<double>(
      "acos(-32)", "3.14159265359-4.158638853279×i", Radian, MetricUnitFormat,
      Cartesian, 13);
  assert_expression_approximates_to<float>("acos(-32)", "180-238.3×i", Degree,
                                           MetricUnitFormat, Cartesian, 4);
  assert_expression_approximates_to<float>("acos(-32)", "200-264.7×i", Gradian,
                                           MetricUnitFormat, Cartesian, 4);
  // On R×i
  assert_expression_approximates_to<float>(
      "acos(3×i)", "1.5708-1.8184×i", Radian, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(3×i)", "90-104.19×i", Degree,
                                           MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(3×i)", "100-115.8×i", Gradian,
                                           MetricUnitFormat, Cartesian, 4);
  // Symmetry: odd on imaginary
  assert_expression_approximates_to<float>(
      "acos(-3×i)", "1.5708+1.8184×i", Radian, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(-3×i)", "90+104.19×i", Degree,
                                           MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>("acos(-3×i)", "100+115.8×i", Gradian,
                                           MetricUnitFormat, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>(
      "acos(i-4)", "2.8894-2.0966×i", Radian, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>(
      "acos(i-4)", "165.551-120.126×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>(
      "acos(i-4)", "183.9-133.5×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // Key values
  assert_expression_approximates_to<double>("acos(0)", "90", Degree);
  assert_expression_approximates_to<float>("acos(-1)", "180", Degree);
  assert_expression_approximates_to<double>("acos(1)", "0", Degree);
  assert_expression_approximates_to<double>("acos(0)", "100", Gradian);
  assert_expression_approximates_to<float>("acos(-1)", "200", Gradian);
  assert_expression_approximates_to<double>("acos(1)", "0", Gradian);
  // Advanced function : asec
  assert_expression_approximates_to<double>("arcsec(-2.4029979617224)", "2",
                                            Radian);
  assert_expression_approximates_to<float>("arcsec(-1)", "200", Gradian);
  assert_expression_approximates_to<float>("arcsec(0.9986307857)", "2.9999×i",
                                           Degree, MetricUnitFormat, Cartesian,
                                           5);
  assert_expression_approximates_to<float>("arcsec(-0.55776+0.491828×i)",
                                           "2.28318+1×i", Radian,
                                           MetricUnitFormat, Cartesian, 6);

  /* asin: [-1,1]    -> R
   *       ]-inf,-1[ -> -π/2+R×i (odd)
   *       ]1, inf[  -> π/2+R×i (odd)
   *       R×i       -> R×i (odd)
   */
  // On [-1, 1]
  assert_expression_approximates_to<double>("asin(0.5)", "0.5235987755983",
                                            Radian);
  assert_expression_approximates_to<double>("asin(0.03)", "0.030004501823477",
                                            Radian);
  assert_expression_approximates_to<double>("asin(0.5)", "30", Degree);
  assert_expression_approximates_to<double>("asin(0.5)", "33.3333", Gradian,
                                            MetricUnitFormat, Cartesian, 6);
  // On [1, inf[
  assert_expression_approximates_to<double>("asin(2)", "nonreal", Radian,
                                            MetricUnitFormat, Real);
  assert_expression_approximates_to<double>(
      "asin(2)", "1.5707963267949-1.3169578969248×i", Radian);
  assert_expression_approximates_to<double>("asin(2)", "90-75.456129290217×i",
                                            Degree);
  assert_expression_approximates_to<double>("asin(2)", "100-83.84×i", Gradian,
                                            MetricUnitFormat, Cartesian, 4);
  // Symmetry: odd
  assert_expression_approximates_to<double>(
      "asin(-2)", "-1.5707963267949+1.3169578969248×i", Radian);
  assert_expression_approximates_to<double>("asin(-2)", "-90+75.456129290217×i",
                                            Degree);
  assert_expression_approximates_to<double>("asin(-2)", "-100+83.84×i", Gradian,
                                            MetricUnitFormat, Cartesian, 4);
  // On ]-inf, -1[
  assert_expression_approximates_to<float>(
      "asin(-32)", "-1.571+4.159×i", Radian, MetricUnitFormat, Cartesian, 4);
  assert_expression_approximates_to<float>("asin(-32)", "-90+238×i", Degree,
                                           MetricUnitFormat, Cartesian, 3);
  assert_expression_approximates_to<float>("asin(-32)", "-100+265×i", Gradian,
                                           MetricUnitFormat, Cartesian, 3);
  // On R×i
  assert_expression_approximates_to<double>("asin(3×i)", "1.8184464592321×i",
                                            Radian);
  assert_expression_approximates_to<double>("asin(3×i)", "115.8×i", Gradian,
                                            MetricUnitFormat, Cartesian, 4);
  // Symmetry: odd
  assert_expression_approximates_to<double>("asin(-3×i)", "-1.8184464592321×i",
                                            Radian);
  assert_expression_approximates_to<double>("asin(-3×i)", "-115.8×i", Gradian,
                                            MetricUnitFormat, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>(
      "asin(i-4)", "-1.3186+2.0966×i", Radian, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>(
      "asin(i-4)", "-75.551+120.13×i", Degree, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>(
      "asin(i-4)", "-83.95+133.5×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // Key values
  assert_expression_approximates_to<double>("asin(0)", "0", Degree);
  assert_expression_approximates_to<double>("asin(0)", "0", Gradian);
  assert_expression_approximates_to<float>("asin(-1)", "-90", Degree,
                                           MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("asin(-1)", "-100", Gradian,
                                           MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("asin(1)", "90", Degree);
  assert_expression_approximates_to<double>("asin(1)", "100", Gradian,
                                            MetricUnitFormat, Cartesian);
  // Advanced function : acsc
  assert_expression_approximates_to<double>("arccsc(1.0997501702946)",
                                            "1.1415926535898", Radian);
  assert_expression_approximates_to<double>("arccsc(1)", "100", Gradian,
                                            MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>(
      "arccsc(-19.08987×i)", "3×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>(
      "arccsc(19.08987×i)", "-3×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("arccsc(0.5+0.4×i)",
                                            "0.792676-1.13208×i", Radian,
                                            MetricUnitFormat, Cartesian, 6);

  /* atan: R         ->  R (odd)
   *       [-i,i]    ->  R×i (odd)
   *       ]-inf×i,-i[ -> -π/2+R×i (odd)
   *       ]i, inf×i[  -> π/2+R×i (odd)
   */
  // On R
  assert_expression_approximates_to<double>("atan(2)", "1.1071487177941",
                                            Radian);
  assert_expression_approximates_to<double>("atan(0.01)", "0.0099996666866652",
                                            Radian);
  assert_expression_approximates_to<double>("atan(2)", "63.434948822922",
                                            Degree);
  assert_expression_approximates_to<double>("atan(2)", "70.48", Gradian,
                                            MetricUnitFormat, Cartesian, 4);
  assert_expression_approximates_to<float>("atan(0.5)", "0.4636476", Radian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("atan(-2)", "-1.1071487177941",
                                            Radian);
  assert_expression_approximates_to<double>("atan(-2)", "-63.434948822922",
                                            Degree);
  // On [-i, i]
  assert_expression_approximates_to<float>("atan(0.2×i)", "0.202733×i", Radian,
                                           MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("atan(i)", "undef");
  // Symmetry: odd
  assert_expression_approximates_to<float>(
      "atan(-0.2×i)", "-0.202733×i", Radian, MetricUnitFormat, Cartesian, 6);
  // On [i, inf×i[
  assert_expression_approximates_to<double>(
      "atan(26×i)", "1.5707963267949+0.038480520568064×i", Radian);
  assert_expression_approximates_to<double>("atan(26×i)",
                                            "90+2.2047714220164×i", Degree);
  assert_expression_approximates_to<double>("atan(26×i)", "100+2.45×i", Gradian,
                                            MetricUnitFormat, Cartesian, 3);
  // Symmetry: odd
  assert_expression_approximates_to<double>(
      "atan(-26×i)", "-1.5707963267949-0.038480520568064×i", Radian);
  assert_expression_approximates_to<double>("atan(-26×i)",
                                            "-90-2.2047714220164×i", Degree);
  assert_expression_approximates_to<double>(
      "atan(-26×i)", "-100-2.45×i", Gradian, MetricUnitFormat, Cartesian, 3);
  // On ]-inf×i, -i[
  assert_expression_approximates_to<float>("atan(-3.4×i)",
                                           "-1.570796-0.3030679×i", Radian);
  assert_expression_approximates_to<float>(
      "atan(-3.4×i)", "-90-17.3645×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>(
      "atan(-3.4×i)", "-100-19.29×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>("atan(i-4)",
                                           "-1.338973+0.05578589×i", Radian);
  assert_expression_approximates_to<float>(
      "atan(i-4)", "-76.7175+3.1963×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>(
      "atan(i-4)", "-85.24+3.551×i", Gradian, MetricUnitFormat, Cartesian, 4);
  // Key values
  assert_expression_approximates_to<float>("atan(0)", "0", Degree);
  assert_expression_approximates_to<float>("atan(0)", "0", Gradian);
  assert_expression_approximates_to<double>("atan(-i)", "undef");
  assert_expression_approximates_to<double>("atan(i)", "undef");
  // Advanced function : acot
  assert_expression_approximates_to<double>("arccot(-0.45765755436029)", "2",
                                            Radian);
  assert_expression_approximates_to<double>("arccot(0)", "90", Degree);
  assert_expression_approximates_to<float>("arccot(0)", "1.570796", Radian);
  assert_expression_approximates_to<float>(
      "arccot(-19.11604×i)", "180+3×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("arccot(19.11604×i)", "-3×i", Degree,
                                           MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("arccot(-0.253182-0.928133×i)",
                                           "2.28319+1×i", Radian,
                                           MetricUnitFormat, Cartesian, 6);

  /* cosh: R         -> R (even)
   *       R×i       -> R (oscillator)
   */
  // On R
  assert_expression_approximates_to<double>("cosh(2)", "3.7621956910836",
                                            Radian);
  assert_expression_approximates_to<double>("cosh(2)", "3.7621956910836",
                                            Degree);
  assert_expression_approximates_to<double>("cosh(2)", "3.7621956910836",
                                            Gradian);
  // Symmetry: even
  assert_expression_approximates_to<double>("cosh(-2)", "3.7621956910836",
                                            Radian);
  assert_expression_approximates_to<double>("cosh(-2)", "3.7621956910836",
                                            Degree);
  assert_expression_approximates_to<double>("cosh(-2)", "3.7621956910836",
                                            Gradian);
  // On R×i
  assert_expression_approximates_to<double>("cosh(43×i)", "0.55511330152063",
                                            Radian);
  // Oscillator
  assert_expression_approximates_to<float>("cosh(π×i/2)", "0", Radian);
  assert_expression_approximates_to<float>("cosh(5×π×i/2)", "0", Radian);
  assert_expression_approximates_to<float>("cosh(8×π×i/2)", "1", Radian);
  assert_expression_approximates_to<float>("cosh(9×π×i/2)", "0", Radian);
  // On C
  assert_expression_approximates_to<float>(
      "cosh(i-4)", "14.7547-22.9637×i", Radian, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>(
      "cosh(i-4)", "14.7547-22.9637×i", Degree, MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<float>("cosh(i-4)", "14.7547-22.9637×i",
                                           Gradian, MetricUnitFormat, Cartesian,
                                           6);

  /* sinh: R         -> R (odd)
   *       R×i       -> R×i (oscillator)
   */
  // On R
  assert_expression_approximates_to<double>("sinh(2)", "3.626860407847",
                                            Radian);
  assert_expression_approximates_to<double>("sinh(2)", "3.626860407847",
                                            Degree);
  assert_expression_approximates_to<double>("sinh(2)", "3.626860407847",
                                            Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("sinh(-2)", "-3.626860407847",
                                            Radian);
  // On R×i
  assert_expression_approximates_to<double>("sinh(43×i)", "-0.8317747426286×i",
                                            Radian);
  // Oscillator
  assert_expression_approximates_to<float>("sinh(π×i/2)", "i", Radian);
  assert_expression_approximates_to<float>("sinh(5×π×i/2)", "i", Radian);
  assert_expression_approximates_to<float>("sinh(7×π×i/2)", "-i", Radian);
  assert_expression_approximates_to<float>("sinh(8×π×i/2)", "0", Radian);
  assert_expression_approximates_to<float>("sinh(9×π×i/2)", "i", Radian);
  // On C
  assert_expression_approximates_to<float>("sinh(i-4)", "-14.7448+22.9791×i",
                                           Radian, MetricUnitFormat, Cartesian,
                                           6);
  assert_expression_approximates_to<float>("sinh(i-4)", "-14.7448+22.9791×i",
                                           Degree, MetricUnitFormat, Cartesian,
                                           6);

  /* tanh: R         -> R (odd)
   *       R×i       -> R×i (tangent-style)
   */
  // On R
  assert_expression_approximates_to<double>("tanh(2)", "0.96402758007582",
                                            Radian);
  assert_expression_approximates_to<double>("tanh(2)", "0.96402758007582",
                                            Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("tanh(-2)", "-0.96402758007582",
                                            Degree);
  // On R×i
  assert_expression_approximates_to<double>("tanh(43×i)", "-1.4983873388552×i",
                                            Radian);
  // Tangent-style
  // FIXME: this depends on the libm implementation and does not work on
  // travis/appveyor servers
  /*assert_expression_approximates_to<float>("tanh(π×i/2)", "undef",
  Radian); assert_expression_approximates_to<float>("tanh(5×π×i/2)",
  "undef", Radian);
  assert_expression_approximates_to<float>("tanh(7×π×i/2)", "undef",
  Radian); assert_expression_approximates_to<float>("tanh(8×π×i/2)", "0",
  Radian); assert_expression_approximates_to<float>("tanh(9×π×i/2)",
  "undef", Radian);*/
  // On C
  assert_expression_approximates_to<float>("tanh(i-4)", "-1.00028+6.10241ᴇ-4×i",
                                           Radian, MetricUnitFormat, Cartesian,
                                           6);
  assert_expression_approximates_to<float>("tanh(i-4)", "-1.00028+6.10241ᴇ-4×i",
                                           Degree, MetricUnitFormat, Cartesian,
                                           6);

  /* acosh: [-1,1]       -> R×i
   *        ]-inf,-1[    -> π×i+R (even on real)
   *        ]1, inf[     -> R (even on real)
   *        ]-inf×i, 0[  -> -π/2×i+R (even on real)
   *        ]0, inf*i[   -> π/2×i+R (even on real)
   */
  // On [-1,1]
  assert_expression_approximates_to<double>("arcosh(2)", "1.3169578969248",
                                            Radian);
  assert_expression_approximates_to<double>("arcosh(2)", "1.3169578969248",
                                            Degree);
  assert_expression_approximates_to<double>("arcosh(2)", "1.3169578969248",
                                            Gradian);
  // On ]-inf, -1[
  assert_expression_approximates_to<double>(
      "arcosh(-4)", "2.0634370688956+3.1415926535898×i", Radian);
  assert_expression_approximates_to<float>("arcosh(-4)", "2.06344+3.14159×i",
                                           Radian, MetricUnitFormat, Cartesian,
                                           6);
  // On ]1,inf[: Symmetry: even on real
  assert_expression_approximates_to<double>("arcosh(4)", "2.0634370688956",
                                            Radian);
  assert_expression_approximates_to<float>("arcosh(4)", "2.063437", Radian);
  // On ]-inf×i, 0[
  assert_expression_approximates_to<double>(
      "arcosh(-42×i)", "4.4309584920805-1.5707963267947×i", Radian);
  assert_expression_approximates_to<float>(
      "arcosh(-42×i)", "4.431-1.571×i", Radian, MetricUnitFormat, Cartesian, 4);
  // On ]0, i×inf[: Symmetry: even on real
  assert_expression_approximates_to<double>(
      "arcosh(42×i)", "4.4309584920805+1.5707963267949×i", Radian);
  assert_expression_approximates_to<float>(
      "arcosh(42×i)", "4.431+1.571×i", Radian, MetricUnitFormat, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>(
      "arcosh(i-4)", "2.0966+2.8894×i", Radian, MetricUnitFormat, Cartesian, 5);
  assert_expression_approximates_to<float>(
      "arcosh(i-4)", "2.0966+2.8894×i", Degree, MetricUnitFormat, Cartesian, 5);
  // Key values
  // assert_expression_approximates_to<double>("arcosh(-1)",
  // "3.1415926535898×i", Radian);
  assert_expression_approximates_to<double>("arcosh(1)", "0", Radian);
  assert_expression_approximates_to<float>("arcosh(0)", "1.570796×i", Radian);

  /* asinh: R            -> R (odd)
   *        [-i,i]       -> R*i (odd)
   *        ]-inf×i,-i[  -> -π/2×i+R (odd)
   *        ]i, inf×i[   -> π/2×i+R (odd)
   */
  // On R
  assert_expression_approximates_to<double>("arsinh(2)", "1.4436354751788",
                                            Radian);
  assert_expression_approximates_to<double>("arsinh(2)", "1.4436354751788",
                                            Degree);
  assert_expression_approximates_to<double>("arsinh(2)", "1.4436354751788",
                                            Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("arsinh(-2)", "-1.4436354751788",
                                            Radian);
  assert_expression_approximates_to<double>("arsinh(-2)", "-1.4436354751788",
                                            Degree);
  // On [-i,i]
  assert_expression_approximates_to<double>("arsinh(0.2×i)",
                                            "0.20135792079033×i", Radian);
  // arsinh(0.2*i) has a too low precision in float on the web platform
  assert_expression_approximates_to<float>("arsinh(0.3×i)", "0.3046927×i",
                                           Degree);
  // Symmetry: odd
  assert_expression_approximates_to<double>("arsinh(-0.2×i)",
                                            "-0.20135792079033×i", Radian);
  // arsinh(-0.2*i) has a too low precision in float on the web platform
  assert_expression_approximates_to<float>("arsinh(-0.3×i)", "-0.3046927×i",
                                           Degree);
  // On ]-inf×i, -i[
  assert_expression_approximates_to<double>(
      "arsinh(-22×i)", "-3.7836727043295-1.5707963267948×i", Radian);
  assert_expression_approximates_to<float>("arsinh(-22×i)", "-3.784-1.571×i",
                                           Degree, MetricUnitFormat, Cartesian,
                                           4);
  // On ]i, inf×i[, Symmetry: odd
  assert_expression_approximates_to<double>(
      "arsinh(22×i)", "3.7836727043295+1.5707963267949×i", Radian);
  assert_expression_approximates_to<float>(
      "arsinh(22×i)", "3.784+1.571×i", Degree, MetricUnitFormat, Cartesian, 4);
  // On C
  assert_expression_approximates_to<float>(
      "arsinh(i-4)", "-2.123+0.2383×i", Radian, MetricUnitFormat, Cartesian, 4);
  assert_expression_approximates_to<float>(
      "arsinh(i-4)", "-2.123+0.2383×i", Degree, MetricUnitFormat, Cartesian, 4);

  /* atanh: [-1,1]       -> R (odd)
   *        ]-inf,-1[    -> π/2*i+R (odd)
   *        ]1, inf[     -> -π/2×i+R (odd)
   *        R×i          -> R×i (odd)
   */
  // On [-1,1]
  assert_expression_approximates_to<double>("artanh(0.4)", "0.4236489301936",
                                            Radian);
  assert_expression_approximates_to<double>("artanh(0.4)", "0.4236489301936",
                                            Degree);
  assert_expression_approximates_to<double>("artanh(0.4)", "0.4236489301936",
                                            Gradian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("artanh(-0.4)", "-0.4236489301936",
                                            Radian);
  assert_expression_approximates_to<double>("artanh(-0.4)", "-0.4236489301936",
                                            Degree);
  // On ]1, inf[
  assert_expression_approximates_to<double>(
      "artanh(4)", "0.255412811883-1.5707963267949×i", Radian);
  assert_expression_approximates_to<float>("artanh(4)", "0.2554128-1.570796×i",
                                           Degree);
  // On ]-inf,-1[, Symmetry: odd
  assert_expression_approximates_to<double>(
      "artanh(-4)", "-0.255412811883+1.5707963267949×i", Radian);
  assert_expression_approximates_to<float>("artanh(-4)",
                                           "-0.2554128+1.570796×i", Degree);
  // On R×i
  assert_expression_approximates_to<double>("artanh(4×i)", "1.325817663668×i",
                                            Radian);
  assert_expression_approximates_to<float>("artanh(4×i)", "1.325818×i", Radian);
  // Symmetry: odd
  assert_expression_approximates_to<double>("artanh(-4×i)", "-1.325817663668×i",
                                            Radian);
  assert_expression_approximates_to<float>("artanh(-4×i)", "-1.325818×i",
                                           Radian);
  // On C
  assert_expression_approximates_to<float>("artanh(i-4)", "-0.238878+1.50862×i",
                                           Radian, MetricUnitFormat, Cartesian,
                                           6);
  assert_expression_approximates_to<float>("artanh(i-4)", "-0.238878+1.50862×i",
                                           Degree, MetricUnitFormat, Cartesian,
                                           6);

  // Check that the complex part is not neglected
  assert_expression_approximates_to<double>("artanh(0.99999999999+1.0ᴇ-26×i)",
                                            "13+5ᴇ-16×i", Radian,
                                            MetricUnitFormat, Cartesian, 3);
  assert_expression_approximates_to<double>("artanh(0.99999999999+1.0ᴇ-60×i)",
                                            "13+5ᴇ-50×i", Radian,
                                            MetricUnitFormat, Cartesian, 3);
  assert_expression_approximates_to<double>("artanh(0.99999999999+1.0ᴇ-150×i)",
                                            "13+5ᴇ-140×i", Radian,
                                            MetricUnitFormat, Cartesian, 3);
  assert_expression_approximates_to<double>("artanh(0.99999999999+1.0ᴇ-250×i)",
                                            "13+5ᴇ-240×i", Radian,
                                            MetricUnitFormat, Cartesian, 3);
  assert_expression_approximates_to<double>("artanh(0.99999999999+1.0ᴇ-300×i)",
                                            "13+5ᴇ-290×i", Radian,
                                            MetricUnitFormat, Cartesian, 3);

  // WARNING: evaluate on branch cut can be multi-valued
  assert_expression_approximates_to<double>("acos(2)", "1.3169578969248×i",
                                            Radian);
  assert_expression_approximates_to<double>("acos(2)", "75.456129290217×i",
                                            Degree);
  assert_expression_approximates_to<double>(
      "asin(2)", "1.5707963267949-1.3169578969248×i", Radian);
  assert_expression_approximates_to<double>("asin(2)", "90-75.456129290217×i",
                                            Degree);
  assert_expression_approximates_to<double>(
      "artanh(2)", "0.54930614433405-1.5707963267949×i", Radian);
  assert_expression_approximates_to<double>(
      "atan(2i)", "1.5707963267949+0.54930614433405×i", Radian);
  assert_expression_approximates_to<double>("atan(2i)", "90+31.472923730945×i",
                                            Degree);
  assert_expression_approximates_to<double>(
      "arsinh(2i)", "1.3169578969248+1.5707963267949×i", Radian);
  assert_expression_approximates_to<double>(
      "arcosh(-2)", "1.3169578969248+3.1415926535898×i", Radian);
}

QUIZ_CASE(poincare_approximation_matrix) {
  assert_expression_approximates_to<float>("[[1,2,3][4,5,6]]",
                                           "[[1,2,3][4,5,6]]");
  assert_expression_approximates_to<double>("[[1,2,3][4,5,6]]",
                                            "[[1,2,3][4,5,6]]");
}

QUIZ_CASE(poincare_approximation_list) {
  assert_expression_approximates_to<float>("{1,2,3,4,5,6}", "{1,2,3,4,5,6}");
  assert_expression_approximates_to<double>("{1,2,3,4,5,6}", "{1,2,3,4,5,6}");
}

QUIZ_CASE(poincare_approximation_list_sequence) {
  assert_expression_approximates_to<float>("sequence(k^2,k,4)", "{1,4,9,16}");
  assert_expression_approximates_to<double>("sequence(k/2,k,7)",
                                            "{0.5,1,1.5,2,2.5,3,3.5}");
}

QUIZ_CASE(poincare_approximation_map_on_list) {
  // We do not map on matrices anymore
  assert_expression_approximates_to<float>("abs([[1,-2][3,-4]])", "undef");
  assert_expression_approximates_to<double>("abs([[1,-2][3,-4]])", "undef");

  // TODO: Implement more tests on lists, with every functions
  assert_expression_approximates_to<float>("abs({1,-1,2,-3})", "{1,1,2,3}");
  assert_expression_approximates_to<float>("ceil({0.3,180})", "{1,180}");
  assert_expression_approximates_to<float>("cos({0,π})", "{1,-1}", Radian);
  assert_expression_approximates_to<float>("{1,3}!", "{1,6}");
  assert_expression_approximates_to<float>("{1,2,3,4}!", "{1,2,6,24}");
  assert_expression_approximates_to<float>("floor({1/√(2),1/2,1,-1.3})",
                                           "{0,0,1,-2}");
  assert_expression_approximates_to<float>("floor({0.3,180})", "{0,180}");
  assert_expression_approximates_to<float>("frac({0.3,180})", "{0.3,0}");
  assert_expression_approximates_to<float>("im({1/√(2),1/2,1,-1})",
                                           "{0,0,0,0}");
  assert_expression_approximates_to<float>("im({1,1+i})", "{0,1}");
  assert_expression_approximates_to<float>("re({1,i})", "{1,0}");
  assert_expression_approximates_to<float>("round({2.12,3.42}, 1)",
                                           "{2.1,3.4}");
  assert_expression_approximates_to<float>("round(1.23456, {2,3})",
                                           "{1.23,1.235}");
  assert_expression_approximates_to<float>("sin({0,π})", "{0,0}", Radian);
  assert_expression_approximates_to<float>("{2,3.4}-{0.1,3.1}", "{1.9,0.3}");
  assert_expression_approximates_to<float>("tan({0,π/4})", "{0,1}", Radian);
  assert_expression_approximates_to<float>("abs(sum({0}×k,k,0,0))", "{0}");
}

QUIZ_CASE(poincare_approximation_probability) {
  // FIXME: precision problem
  assert_expression_approximates_to<float>("binomcdf(5.3, 9, 0.7)", "0.270341",
                                           Degree, MetricUnitFormat, Cartesian,
                                           6);
  // FIXME precision problem
  assert_expression_approximates_to<double>("binomcdf(5.3, 9, 0.7)",
                                            "0.270340902", Degree,
                                            MetricUnitFormat, Cartesian, 10);

  // FIXME: precision problem
  assert_expression_approximates_to<float>("binompdf(4.4, 9, 0.7)", "0.0735138",
                                           Degree, MetricUnitFormat, Cartesian,
                                           6);
  assert_expression_approximates_to<double>("binompdf(4.4, 9, 0.7)",
                                            "0.073513818");

  assert_expression_approximates_to<float>("invbinom(0.9647324002, 15, 0.7)",
                                           "13");
  assert_expression_approximates_to<double>("invbinom(0.9647324002, 15, 0.7)",
                                            "13");
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
  assert_expression_approximates_to<double>("hgeomcdfrange(13,15,40,20,30)",
                                            "0.60937014162821");
  assert_expression_approximates_to<double>("invhgeom(.5,4,2,3)", "1");
  assert_expression_approximates_to<double>("invhgeom(.6,40,20,30)", "15");
  assert_expression_approximates_to<double>("invhgeom(-1,4,2,3)", "undef");
  assert_expression_approximates_to<double>("invhgeom(0,4,2,2)", "undef");
  assert_expression_approximates_to<double>("invhgeom(0,4,2,3)", "0");
  assert_expression_approximates_to<double>("invhgeom(1,4,2,3)", "2");

  assert_expression_approximates_to<double>("normcdf(5, 7, 0.3162)",
                                            "1.265256ᴇ-10", Radian,
                                            MetricUnitFormat, Cartesian, 7);

  assert_expression_approximates_to<float>("normcdf(1.2, 3.4, 5.6)",
                                           "0.3472125");
  assert_expression_approximates_to<double>("normcdf(1.2, 3.4, 5.6)",
                                            "0.34721249841587");
  assert_expression_approximates_to<float>("normcdf(-1ᴇ99,3.4,5.6)", "0");
  assert_expression_approximates_to<float>("normcdf(1ᴇ99,3.4,5.6)", "1");
  assert_expression_approximates_to<float>("normcdf(-6,0,1)", "0");
  assert_expression_approximates_to<float>("normcdf(6,0,1)", "1");

  assert_expression_approximates_to<float>("normcdfrange(0.5, 3.6, 1.3, 3.4)",
                                           "0.3436388");
  assert_expression_approximates_to<double>("normcdfrange(0.5, 3.6, 1.3, 3.4)",
                                            "0.34363881299147");

  assert_expression_approximates_to<float>("normpdf(1.2, 3.4, 5.6)",
                                           "0.06594901");
  assert_expression_approximates_to<float>("invnorm(0.56, 1.3, 2.4)",
                                           "1.662326");
  // assert_expression_approximates_to<double>("invnorm(0.56, 1.3, 2.4)",
  // "1.6623258450088"); FIXME precision error

  assert_expression_approximates_to<float>("poissonpdf(2,1)", "0.1839397");
  assert_expression_approximates_to<double>("poissonpdf(2,1)",
                                            "0.18393972058572");
  assert_expression_approximates_to<float>("poissonpdf(2,2)", "0.2706706");
  assert_expression_approximates_to<float>("poissoncdf(2,2)", "0.6766764");
  assert_expression_approximates_to<double>("poissoncdf(2,2)",
                                            "0.67667641618306");

  assert_expression_approximates_to<float>("tpdf(1.2, 3.4)", "0.1706051");
  assert_expression_approximates_to<double>("tpdf(1.2, 3.4)",
                                            "0.17060506917323");
  assert_expression_approximates_to<float>("tcdf(0.5, 2)", "0.6666667");
  assert_expression_approximates_to<float>("tcdf(1.2, 3.4)", "0.8464878");
  // FIXME: precision problem
  assert_expression_approximates_to<double>("tcdf(1.2, 3.4)", "0.8464877995",
                                            Radian, MetricUnitFormat, Cartesian,
                                            10);
  assert_expression_approximates_to<float>("invt(0.8464878,3.4)", "1.2");
  assert_expression_approximates_to<double>("invt(0.84648779949601043,3.4)",
                                            "1.2", Radian, MetricUnitFormat,
                                            Cartesian, 10);
}

QUIZ_CASE(poincare_approximation_complex_format) {
  // Real
  assert_expression_approximates_to<float>("0", "0", Radian, MetricUnitFormat,
                                           Real);
  assert_expression_approximates_to<double>("0", "0", Radian, MetricUnitFormat,
                                            Real);
  assert_expression_approximates_to<float>("10", "10", Radian, MetricUnitFormat,
                                           Real);
  assert_expression_approximates_to<double>("-10", "-10", Radian,
                                            MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("100", "100", Radian,
                                           MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("0.1", "0.1", Radian,
                                            MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("0.1234567", "0.1234567", Radian,
                                           MetricUnitFormat, Real);
  assert_expression_approximates_to<double>(
      "0.123456789012345", "0.12345678901235", Radian, MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("1+2×i", "nonreal", Radian,
                                           MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("1+i-i", "nonreal", Radian,
                                            MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("1+i-1", "nonreal", Radian,
                                           MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("1+i", "nonreal", Radian,
                                            MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("3+i", "nonreal", Radian,
                                           MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("3-i", "nonreal", Radian,
                                            MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("3-i-3", "nonreal", Radian,
                                           MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("i", "nonreal", Radian,
                                           MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("√(-1)", "nonreal", Radian,
                                            MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("√(-1)×√(-1)", "nonreal", Radian,
                                            MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("ln(-2)", "nonreal", Radian,
                                            MetricUnitFormat, Real);
  // Power/Root approximates to the first REAL root in Real mode
  // Power have to be simplified first in order to spot the right form c^(p/q)
  // with p, q integers
  assert_expression_simplifies_approximates_to<double>(
      "(-8)^(1/3)", "-2", Radian, MetricUnitFormat, Real);
  // Root approximates to the first REAL root in Real mode
  assert_expression_approximates_to<double>("root(-8,3)", "-2", Radian,
                                            MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("8^(1/3)", "2", Radian,
                                            MetricUnitFormat, Real);
  // Power have to be simplified first (cf previous comment)
  assert_expression_simplifies_approximates_to<float>("(-8)^(2/3)", "4", Radian,
                                                      MetricUnitFormat, Real);
  assert_expression_approximates_to<float>("root(-8, 3)^2", "4", Radian,
                                           MetricUnitFormat, Real);
  assert_expression_approximates_to<double>("root(-8,3)", "-2", Radian,
                                            MetricUnitFormat, Real);

  // Cartesian
  assert_expression_approximates_to<float>("0", "0", Radian, MetricUnitFormat,
                                           Cartesian);
  assert_expression_approximates_to<double>("0", "0", Radian, MetricUnitFormat,
                                            Cartesian);
  assert_expression_approximates_to<float>("10", "10", Radian, MetricUnitFormat,
                                           Cartesian);
  assert_expression_approximates_to<double>("-10", "-10", Radian,
                                            MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("100", "100", Radian,
                                           MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("0.1", "0.1", Radian,
                                            MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("0.1234567", "0.1234567", Radian,
                                           MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("0.123456789012345",
                                            "0.12345678901235", Radian,
                                            MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("1+2×i", "1+2×i", Radian,
                                           MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("1+i-i", "1", Radian,
                                            MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("1+i-1", "i", Radian,
                                           MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("1+i", "1+i", Radian,
                                            MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("3+i", "3+i", Radian,
                                           MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("3-i", "3-i", Radian,
                                            MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("3-i-3", "-i", Radian,
                                           MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("i", "i", Radian, MetricUnitFormat,
                                           Cartesian);
  assert_expression_approximates_to<double>("√(-1)", "i", Radian,
                                            MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>("√(-1)×√(-1)", "-1", Radian,
                                            MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>(
      "ln(-2)", "0.69314718055995+3.1415926535898×i", Radian, MetricUnitFormat,
      Cartesian);
  assert_expression_approximates_to<double>(
      "(-8)^(1/3)", "1+1.7320508075689×i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<float>("(-8)^(2/3)", "-2+3.4641×i", Radian,
                                           MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>(
      "root(-8,3)", "1+1.7320508075689×i", Radian, MetricUnitFormat, Cartesian);

  // Polar
  assert_expression_approximates_to<float>("0", "0", Radian, MetricUnitFormat,
                                           Polar);
  assert_expression_approximates_to<double>("0", "0", Radian, MetricUnitFormat,
                                            Polar);
  assert_expression_approximates_to<float>("10", "10", Radian, MetricUnitFormat,
                                           Polar);
  assert_expression_approximates_to<double>("-10", "10×e^(3.1415926535898×i)",
                                            Radian, MetricUnitFormat, Polar);

  assert_expression_approximates_to<float>("100", "100", Radian,
                                           MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("0.1", "0.1", Radian,
                                            MetricUnitFormat, Polar);
  assert_expression_approximates_to<float>("0.1234567", "0.1234567", Radian,
                                           MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("0.12345678", "0.12345678", Radian,
                                            MetricUnitFormat, Polar);

  assert_expression_approximates_to<float>("1+2×i", "2.236068×e^(1.107149×i)",
                                           Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<float>("1+i-i", "1", Radian,
                                           MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>(
      "1+i-1", "e^(1.57079632679×i)", Radian, MetricUnitFormat, Polar, 12);
  assert_expression_approximates_to<float>("1+i", "1.414214×e^(0.7853982×i)",
                                           Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>(
      "3+i", "3.16227766017×e^(0.321750554397×i)", Radian, MetricUnitFormat,
      Polar, 12);
  assert_expression_approximates_to<float>("3-i", "3.162278×e^(-0.3217506×i)",
                                           Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>(
      "3-i-3", "e^(-1.57079632679×i)", Radian, MetricUnitFormat, Polar, 12);
  // 2e^(i) has a too low precision in float on the web platform
  assert_expression_approximates_to<float>("3e^(2*i)", "3×e^(2×i)", Radian,
                                           MetricUnitFormat, Polar, 4);
  assert_expression_approximates_to<double>("2e^(-i)", "2×e^(-i)", Radian,
                                            MetricUnitFormat, Polar, 9);

  assert_expression_approximates_to<float>("i", "e^(1.570796×i)", Radian,
                                           MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>("√(-1)", "e^(1.5707963267949×i)",
                                            Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>(
      "√(-1)×√(-1)", "e^(3.1415926535898×i)", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>(
      "(-8)^(1/3)", "2×e^(1.0471975511966×i)", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<float>("(-8)^(2/3)", "4×e^(2.094395×i)",
                                           Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>(
      "root(-8,3)", "2×e^(1.0471975511966×i)", Radian, MetricUnitFormat, Polar);

  // Cartesian to Polar and vice versa
  assert_expression_approximates_to<double>(
      "2+3×i", "3.60555127546×e^(0.982793723247×i)", Radian, MetricUnitFormat,
      Polar, 12);
  assert_expression_approximates_to<double>(
      "3.60555127546×e^(0.982793723247×i)", "2+3×i", Radian, MetricUnitFormat,
      Cartesian, 12);
  assert_expression_approximates_to<float>(
      "12.04159457879229548012824103×e^(1.4876550949×i)", "1+12×i", Radian,
      MetricUnitFormat, Cartesian, 5);

  // Overflow
  assert_expression_approximates_to<float>("-2ᴇ20+2ᴇ20×i", "-2ᴇ20+2ᴇ20×i",
                                           Radian, MetricUnitFormat, Cartesian);
  /* TODO: this test fails on the device because libm hypotf (which is called
   * eventually by std::abs) is not accurate enough. We might change the
   * embedded libm? */
  // assert_expression_approximates_to<float>("-2ᴇ20+2ᴇ20×i",
  // "2.828427ᴇ20×e^(2.356194×i)", Radian, MetricUnitFormat, Polar);
  assert_expression_approximates_to<float>("-2ᴇ10+2ᴇ10×i",
                                           "2.828427ᴇ10×e^(2.356194×i)", Radian,
                                           MetricUnitFormat, Polar);
  assert_expression_approximates_to<double>(
      "1ᴇ155-1ᴇ155×i", "1ᴇ155-1ᴇ155×i", Radian, MetricUnitFormat, Cartesian);
  assert_expression_approximates_to<double>(
      "1ᴇ155-1ᴇ155×i", "1.41421356237ᴇ155×e^(-0.785398163397×i)", Radian,
      MetricUnitFormat, Polar, 12);
  assert_expression_approximates_to<float>("-2ᴇ100+2ᴇ100×i", "-∞+∞×i");
  assert_expression_approximates_to<double>("-2ᴇ360+2ᴇ360×i", "-∞+∞×i");
  assert_expression_approximates_to<float>("-2ᴇ100+2ᴇ10×i", "-∞+2ᴇ10×i");
  assert_expression_approximates_to<double>("-2ᴇ360+2×i", "-∞+2×i");
  assert_expression_approximates_to<float>("undef+2ᴇ100×i", "undef");
  assert_expression_approximates_to<double>("-2ᴇ360+undef×i", "undef");
}

QUIZ_CASE(poincare_approximation_mix) {
  assert_expression_approximates_to<float>("-2-3", "-5");
  assert_expression_approximates_to<float>("1.2×e^(1)", "3.261938");
  // WARNING: the 7th significant digit is wrong on blackbos simulator
  assert_expression_approximates_to<float>("2e^(3)", "40.1711", Radian,
                                           MetricUnitFormat, Cartesian, 6);
  // WARNING: the 7th significant digit is wrong on simulator
  assert_expression_approximates_to<float>("e^2×e^(1)", "20.0855", Radian,
                                           MetricUnitFormat, Cartesian, 6);
  assert_expression_approximates_to<double>("e^2×e^(1)", "20.085536923188");
  assert_expression_approximates_to<double>("2×3^4+2", "164");
  assert_expression_approximates_to<float>("-2×3^4+2", "-160");
  assert_expression_approximates_to<double>("-sin(3)×2-3", "-3.2822400161197",
                                            Radian);
  assert_expression_approximates_to<float>("5-2/3", "4.333333");
  assert_expression_approximates_to<double>("2/3-5", "-4.3333333333333");
  assert_expression_approximates_to<float>("-2/3-5", "-5.666667");
  assert_expression_approximates_to<double>("sin(3)2(4+2)", "1.6934400967184",
                                            Radian);
  assert_expression_approximates_to<float>("4/2×(2+3)", "10");
  assert_expression_approximates_to<double>("4/2×(2+3)", "10");

  assert_expression_simplifies_approximates_to<double>("1.0092^(20)",
                                                       "1.2010050593402");
  assert_expression_simplifies_approximates_to<double>(
      "1.0092^(50)×ln(3/2)", "0.6409373488899", Degree, MetricUnitFormat,
      Cartesian, 13);
  assert_expression_simplifies_approximates_to<double>(
      "1.0092^(50)×ln(1.0092)", "0.01447637354655", Degree, MetricUnitFormat,
      Cartesian, 13);
  assert_expression_approximates_to<double>("1.0092^(20)", "1.2010050593402");
  assert_expression_approximates_to<double>("1.0092^(50)×ln(3/2)",
                                            "0.6409373488899", Degree,
                                            MetricUnitFormat, Cartesian, 13);
  assert_expression_approximates_to<double>("1.0092^(50)×ln(1.0092)",
                                            "0.01447637354655", Degree,
                                            MetricUnitFormat, Cartesian, 13);
  assert_expression_simplifies_approximates_to<double>("1.0092^(20)",
                                                       "1.2010050593402");
  assert_expression_simplifies_approximates_to<double>(
      "1.0092^(50)×ln(3/2)", "0.6409373488899", Degree, MetricUnitFormat,
      Cartesian, 13);
  // assert_expression_approximates_to<float>("1.0092^(20)", "1.201005"); TODO
  // does not work
  assert_expression_approximates_to<float>("1.0092^(50)×ln(3/2)", "0.6409366");
  // assert_expression_simplifies_approximates_to<float>("1.0092^(20)",
  // "1.2010050593402"); TODO does not work
  // assert_expression_simplifies_approximates_to<float>("1.0092^(50)×ln(3/2)",
  // "6.4093734888993ᴇ-1"); TODO does not work
}

QUIZ_CASE(poincare_approximation_lists_functions) {
  // Sort a list of complexes
  assert_expression_approximates_to<double>("sort({})", "{}");
  assert_expression_approximates_to<double>("sort({4})", "{4}");
  assert_expression_approximates_to<double>("sort({undef})", "{undef}");
  assert_expression_approximates_to<double>("sort({i})", "{i}");
  assert_expression_approximates_to<double>("sort({-1,5,2+6,-0})",
                                            "{-1,0,5,8}");
  assert_expression_approximates_to<double>("sort({-1,-2,-inf,inf})",
                                            "{-∞,-2,-1,∞}");
  assert_expression_approximates_to<double>("sort({-1,undef,-2,-inf,inf})",
                                            "{-1,undef,-2,-∞,∞}");
  assert_expression_approximates_to<double>("sort({-1,i,8,-0})", "{-1,i,8,0}");
  assert_expression_approximates_to<double>("sort({-1,undef,1})",
                                            "{-1,undef,1}");
  // Sort list of points
  assert_expression_approximates_to<double>(
      "sort({(8,1),(5,0),(5,-3),(1,0),(5,9)})",
      "{(1,0),(5,-3),(5,0),(5,9),(8,1)}");
  assert_expression_approximates_to<double>("sort({(8,1),(5,i),(5,-3)})",
                                            "{(8,1),(5,undef),(5,-3)}");
  assert_expression_approximates_to<double>("sort({(undef,1),(6,1),(5,-3)})",
                                            "{(undef,1),(6,1),(5,-3)}");
  assert_expression_approximates_to<double>(
      "sort({(inf,1),(6,1),(5,-3),(-inf,9),(-inf,1)})",
      "{(-∞,1),(-∞,9),(5,-3),(6,1),(∞,1)}");
  // Mean
  assert_expression_approximates_to<double>("mean({5,8,7,4,12},{2})", "undef");
  assert_expression_approximates_to<double>("mean({5,8,7,4,12},{-1,1,1,1,1})",
                                            "undef");
  assert_expression_approximates_to<double>("mean({5,8,7,4,12},{0,0,0,0,0})",
                                            "undef");
  assert_expression_approximates_to<double>("med({1,undef,6,3,5,undef,2})",
                                            "undef");
  assert_expression_approximates_to<double>("var({1,2,3,undef,4,5,6})",
                                            "undef");
  assert_expression_approximates_to<double>(
      "var({1,2,3,3,4,5,6},{-2,2,2,2,2,2,2})", "undef");
  assert_expression_approximates_to<double>("var({1,2,3,4,5,6},{0,0,0,0,0,0})",
                                            "undef");
  assert_expression_approximates_to<double>("samplestddev({1})", "undef");
}

template <typename T>
void assert_expression_approximates_with_value_for_symbol(
    const char *expression, T approximation, const char *symbol, T symbolValue,
    Preferences::AngleUnit angleUnit = Degree,
    Preferences::ComplexFormat complexFormat = Cartesian) {
  Shared::GlobalContext globalContext;
#if 0
  Internal::Tree *e = parse_expression(expression, &globalContext);
  ApproximationContext approximationContext(&globalContext, complexFormat,
                                            angleUnit);
  T result = e.approximateToRealScalarWithValueForSymbol<T>(symbol, symbolValue,
                                                        approximationContext);
  quiz_assert_print_if_failure(
      roughly_equal(result, approximation, OMG::Float::EpsilonLax<T>(),
                    true),
      expression);
#endif
}

QUIZ_CASE(poincare_approximation_floor_ceil_integer) {
  constexpr double upperBound = 1000.;
  for (double d = 0.; d < upperBound; d += 1.) {
    assert_expression_approximates_with_value_for_symbol(
        "floor(x * (x+1)^(-1) + x^2 * (x+1)^(-1))", d, "x", d);
    assert_expression_approximates_with_value_for_symbol(
        "ceil(x * (x+1)^(-1) + x^2 * (x+1)^(-1))", d, "x", d);
  }
}

QUIZ_CASE(poincare_approximation_point) {
  assert_expression_approximates_to<float>("(1,2)", "(1,2)");
  assert_expression_approximates_to<float>("(1/0,2)", "(undef,2)");
  assert_expression_approximates_to<float>("(1,2)+3", "undef");
  assert_expression_approximates_to<float>("abs((1.23,4.56))", "undef");
  assert_expression_approximates_to<float>("{(1+2,3+4),(5+6,7+8)}",
                                           "{(3,7),(11,15)}");

  assert_expression_approximates_to<double>("(1,2)", "(1,2)");
  assert_expression_approximates_to<double>("(1/0,2)", "(undef,2)");
  assert_expression_approximates_to<double>("(1,2)+3", "undef");
  assert_expression_approximates_to<double>("abs((1.23,4.56))", "undef");
  assert_expression_approximates_to<double>("{(1+2,3+4),(5+6,7+8)}",
                                            "{(3,7),(11,15)}");

  assert_expression_approximates_to<double>("(undef,i)", "(undef,undef)");
  assert_expression_approximates_to<double>("(undef,i)", "(undef,undef)",
                                            Degree, MetricUnitFormat, Real);
}

QUIZ_CASE(poincare_approximation_keeping_symbols) {
  assert_expression_approximates_keeping_symbols_to("ln(10)+cos(10)+3x",
                                                    "3×x+3.287392846");
  assert_expression_approximates_keeping_symbols_to(
      "cos(4/3+ln(x-1/2))", "cos(ln(2×x-1)+0.6401861528)");
  assert_expression_approximates_keeping_symbols_to(
      "ln(ln(ln(10+10)))+ln(ln(ln(x+10)))", "ln(ln(ln(x+10)))+0.09275118142");
  assert_expression_approximates_keeping_symbols_to("int(x,x,0,2)+int(x,x,0,x)",
                                                    "int(x,x,0,x)+2");
  assert_expression_approximates_keeping_symbols_to(
      "[[x,cos(10)][1/2+x,cos(4/3+x)]]",
      "[[x,0.984807753][(2×x+1)/2,cos((3×x+4)/3)]]");
  assert_expression_approximates_keeping_symbols_to(
      "{x,undef,cos(10)+x,cos(10)}", "{x,undef,x+0.984807753,0.984807753}");
  assert_expression_approximates_keeping_symbols_to("cos(10)→x",
                                                    "0.984807753→x");
  assert_expression_approximates_keeping_symbols_to("4×kg×s^(-3)", "undef");
  // TODO_PCJ investigate why this one crashes on the CI only
  // assert_expression_approximates_keeping_symbols_to("piecewise(T×x<0)",
  // "undef");
  // Check that it still reduces
  assert_expression_approximates_keeping_symbols_to("x^2+x×x", "2×x^2");
}
