#include <apps/global_preferences.h>
#include <omg/float.h>
#include <poincare/sign.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/variables.h>
#include <poincare/test/float_helper.h>

#include <cmath>

#include "../helpers/symbol_store.h"
#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(pcj_approximation_float) {
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(-1.23456789E30), "-1.23456789ᴇ30");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(1.23456789E30), "1.23456789ᴇ30");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(-1.23456789E-30), "-1.23456789ᴇ-30");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(-1.2345E-3), "-0.0012345");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(1.2345E-3), "0.0012345");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(1.2345E3), "1234.5");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(-1.2345E3), "-1234.5");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(0.99999999999995), "0.99999999999995");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(0.00000099999999999995),
      "9.9999999999995ᴇ-7");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(
          0.0000009999999999901200121020102010201201201021099995),
      "9.9999999999012ᴇ-7");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(1.2345E-1),
                                      "0.12345");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(1), "1");
  assert_float_approximates_to<float>(
      UserExpression::Builder<float>(0.9999999999999995), "1");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(1.2345E6),
                                      "1234500");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(-1.2345E6),
                                      "-1234500");
  assert_float_approximates_to<float>(
      UserExpression::Builder<float>(0.0000009999999999999995), "1ᴇ-6");
  assert_float_approximates_to<float>(
      UserExpression::Builder<float>(-1.2345E-1), "-0.12345");

  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(INFINITY), "∞");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(0.0f),
                                      "0");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(NAN),
                                      "undef");
}

QUIZ_CASE(pcj_approximation_can_approximate) {
  quiz_assert(Approximation::CanApproximate(KAdd(2_e, 3_e)));
  quiz_assert(!Approximation::CanApproximate(KAdd(2_e, "x"_e)));
  quiz_assert(!Approximation::CanApproximate(KAdd(KVarX, 3_e)));
  quiz_assert(
      Approximation::CanApproximate(KSum("k"_e, 2_e, 8_e, KAdd(KVarK, 3_e))));
  quiz_assert(!Approximation::CanApproximate(
      KSum("k"_e, 2_e, 8_e,
           KAdd(KVarK, KDiscreteVar<Parametric::k_localVariableId + 1>, 3_e))));
}

QUIZ_CASE(pcj_approximation_scalar) {
  approximates_to(123_e, 123.f);
  approximates_to(-123.21_fe, -123.21f);
  approximates_to(π_e, M_PI);
  approximates_to(e_e, M_E);
  approximates_to(KAdd(KMult(2_e, 4_e), KPow(1.5_fe, 3.0_fe)), 11.375f);
  approximates_to(KDiv(KSub(2_e, 4_e), 10.0_fe), -0.2f);
  approximates_to(KTrig(KDiv(π_e, 2_e), 1_e), 1.f);
  approximates_to(KLogBase(KMult(13_e, 13_e), 13_e), 2.f);
  approximates_to(KExp(2_e), M_E * M_E);
  approximates_to(KLog(100_e), 2.f);
  approximates_to(KLn(e_e), 1.f);
  approximates_to(KAbs(100_e), 100.f);
  approximates_to(KAbs(-2.31_fe), 2.31f);
  approximates_to(KCos(π_e), -1.f);
  approximates_to(KSin(π_e), 0.f);
  approximates_to(KTan(0_e), 0.f);
  approximates_to(KPowReal(1_e, KDiv(1_e, 3_e)), 1.f);
  approximates_to(KPowReal(-1_e, KDiv(1_e, 3_e)), -1.f);
  approximates_to(KPowReal(-1_e, 2_e), 1.f);
  approximates_to(KSum("k"_e, 1_e, 3_e, KVarK), 6.f);
  approximates_to("x"_e, NAN);
  approximates_to(KPow(e_e, 3_e / 2_e), 4.481689f);
  approximates_to(KNonNull(KUndef), NAN);
  approximates_to(KRealPos(KUndef), NAN);
  approximates_to(KRealInteger(KUndef), NAN);
  approximates_to(KRealInteger(π_e), NAN);
  approximates_to(KRealInteger(i_e), NAN);

  approximates_to<float>("-0", 0.0f);
  approximates_to<float>("-1.ᴇ-2", -0.01f);
  approximates_to<double>("-.003", -0.003);
  approximates_to<float>("1.2343ᴇ-2", 0.012343f);
  approximates_to<double>("-567.2ᴇ2", -56720.0);
}

QUIZ_CASE(pcj_approximation_rational) {
  approximates_to<float>("1/3", 0.3333333f);
  approximates_to<double>("123456/1234567", 9.9999432999586E-2);

  approximates_to<float>("-2-3", "-5");
  approximates_to<float>("5-2/3", "4.333333");
  approximates_to<double>("2/3-5", "-4.3333333333333");
  approximates_to<float>("-2/3-5", "-5.666667");
  approximates_to<float>("4/2×(2+3)", "10");
  approximates_to<double>("4/2×(2+3)", "10");
}

QUIZ_CASE(pcj_approximation_addition) {
  approximates_to<float>("1+2", 3.0f);
  approximates_to<double>("i+i", NAN);
  approximates_to<float>("1+2", "3", cartesianCtx);
  approximates_to<float>("i+i", "2×i", cartesianCtx);
  approximates_to<double>("2+i+4+i", "6+2×i", cartesianCtx);

  // Matrices
  approximates_to<float>("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]", NAN);
  approximates_to<float>("[[1,2][3,4][5,6]]+3", "undef");
  approximates_to<double>("[[1,2+i][3,4][5,6]]+3+i", "undef", cartesianCtx);
  approximates_to<float>("3+[[1,2][3,4][5,6]]", "undef");
  approximates_to<double>("3+i+[[1,2+i][3,4][5,6]]", "undef", cartesianCtx);
  approximates_to<float>("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]",
                         "[[2,4][6,8][10,12]]");
  approximates_to<double>("[[1,2+i][3,4][5,6]]+[[1,2+i][3,4][5,6]]",
                          "[[2,4+2×i][6,8][10,12]]", cartesianCtx);

  // Lists
  approximates_to<float>("{1,2,3}+10", "{11,12,13}");
  approximates_to<float>("10+{1,2,3}", "{11,12,13}");
  approximates_to<float>("{1,2,3}+{4,5,6}", "{5,7,9}");
  approximates_to<float>("{1,2,3}+{4,5}", "undef");
  approximates_to<float>("{1,2,3}+[[4,5,6]]", "undef");
  approximates_to<double>("{1,2,3}+10", "{11,12,13}");
  approximates_to<double>("10+{1,2,3}", "{11,12,13}");
  approximates_to<double>("{1,2,3}+{4,5,6}", "{5,7,9}");
  approximates_to<double>("{1,2,3}+{4,5}", "undef");
  approximates_to<double>("{1,2,3}+[[4,5,6]]", "undef");
}

QUIZ_CASE(pcj_approximation_multiplication) {
  approximates_to<float>("1×2", 2.0f);
  approximates_to<double>("(3+i)×(4+i)", NAN);
  approximates_to<float>("1×2", "2");
  approximates_to<double>("(3+i)×(4+i)", "11+7×i", cartesianCtx);

  // Matrices
  approximates_to<float>("[[1,2][3,4][5,6]]×2", NAN);
  approximates_to<float>("[[1,2][3,4][5,6]]×2", "[[2,4][6,8][10,12]]");
  approximates_to<double>("[[1,2+i][3,4][5,6]]×(3+i)",
                          "[[3+i,5+5×i][9+3×i,12+4×i][15+5×i,18+6×i]]",
                          cartesianCtx);
  approximates_to<float>("2×[[1,2][3,4][5,6]]", "[[2,4][6,8][10,12]]");
  approximates_to<double>("(3+i)×[[1,2+i][3,4][5,6]]",
                          "[[3+i,5+5×i][9+3×i,12+4×i][15+5×i,18+6×i]]",
                          cartesianCtx);
  approximates_to<float>("[[1,2][3,4][5,6]]×[[1,2,3,4][5,6,7,8]]",
                         "[[11,14,17,20][23,30,37,44][35,46,57,68]]");
  approximates_to<double>(
      "[[1,2+i][3,4][5,6]]×[[1,2+i,3,4][5,6+i,7,8]]",
      "[[11+5×i,13+9×i,17+7×i,20+8×i][23,30+7×i,37,44][35,46+11×i,57,68]]",
      cartesianCtx);

  // Lists
  approximates_to<float>("{1,2,3}×10", "{10,20,30}");
  approximates_to<float>("10×{1,2,3}", "{10,20,30}");
  approximates_to<float>("{1,2,3}×{4,5,6}", "{4,10,18}");
  approximates_to<float>("{1,2,3}×{4,5}", "undef");
  approximates_to<float>("{1,2,3}×[[4,5,6]]", "undef");
  approximates_to<double>("{1,2,3}×10", "{10,20,30}");
  approximates_to<double>("10×{1,2,3}", "{10,20,30}");
  approximates_to<double>("{1,2,3}×{4,5,6}", "{4,10,18}");
  approximates_to<double>("{1,2,3}×{4,5}", "undef");
  approximates_to<double>("{1,2,3}×[[4,5,6]]", "undef");
}

QUIZ_CASE(pcj_approximation_substraction) {
  approximates_to<float>("1-2", -1.0f);
  approximates_to<double>("(1)-(4+i)", NAN);
  approximates_to<float>("[[1,2][3,4][5,6]]-[[3,2][3,4][5,6]]", NAN);
  approximates_to<float>("1-2", "-1");
  approximates_to<double>("3+i-(4+i)", "-1", cartesianCtx);

  // Matrices
  approximates_to<float>("[[1,2][3,4][5,6]]-3", "undef");
  approximates_to<double>("[[1,2+i][3,4][5,6]]-(4+i)", "undef", cartesianCtx);
  approximates_to<float>("3-[[1,2][3,4][5,6]]", "undef");
  approximates_to<double>("3+i-[[1,2+i][3,4][5,6]]", "undef", cartesianCtx);
  approximates_to<float>("[[1,2][3,4][5,6]]-[[6,5][4,3][2,1]]",
                         "[[-5,-3][-1,1][3,5]]");
  approximates_to<double>("[[1,2+i][3,4][5,6]]-[[1,2+i][3,4][5,6]]",
                          "[[0,0][0,0][0,0]]", cartesianCtx);

  // Lists
  approximates_to<float>("{1,2,3}-10", "{-9,-8,-7}");
  approximates_to<float>("10-{1,2,3}", "{9,8,7}");
  approximates_to<float>("{1,2,3}-{4,5,6}", "{-3,-3,-3}");
  approximates_to<float>("{1,2,3}-{4,5}", "undef");
  approximates_to<float>("{1,2,3}-[[4,5,6]]", "undef");
  approximates_to<double>("{1,2,3}-10", "{-9,-8,-7}");
  approximates_to<double>("10-{1,2,3}", "{9,8,7}");
  approximates_to<double>("{1,2,3}-{4,5,6}", "{-3,-3,-3}");
  approximates_to<double>("{1,2,3}-{4,5}", "undef");
  approximates_to<double>("{1,2,3}-[[4,5,6]]", "undef");
}

QUIZ_CASE(pcj_approximation_division) {
  approximates_to<float>("1/2", 0.5f);
  approximates_to<float>("(3+i)/(4+i)", NAN);
  approximates_to<float>("[[1,2][3,4][5,6]]/2", NAN);
  approximates_to<float>("1/2", "0.5");
  approximates_to<double>("(3+i)/(4+i)", "0.76470588235294+0.058823529411765×i",
                          cartesianCtx);
  approximates_to<float>("1ᴇ20/(1ᴇ20+1ᴇ20i)", "0.5-0.5×i", cartesianCtx);
  approximates_to<double>("1ᴇ155/(1ᴇ155+1ᴇ155i)", "0.5-0.5×i", cartesianCtx);

  // Matrices
  approximates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  approximates_to<double>(
      "[[1,2+i][3,4][5,6]]/(1+i)",
      "[[0.5-0.5×i,1.5-0.5×i][1.5-1.5×i,2-2×i][2.5-2.5×i,3-3×i]]",
      cartesianCtx);
  approximates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  approximates_to<double>("[[1,2][3,4]]/[[3,4][6,9]]", "undef");
  approximates_to<double>("3/[[3,4][5,6]]", "undef");
  // approximates_to<double>("(3+4i)/[[3,4][1,i]]",
  // "[[1,4×i][i,-3×i]]");
  /* TODO_PCJ: this test fails because of negligible real or imaginary parts.
   * It currently approximates to
   * [[1+5.5511151231258ᴇ-17×i,-2.2204460492503ᴇ-16+4×i][i,-3×i]] or
   * [[1-1.1102230246252ᴇ-16×i,2.2204460492503ᴇ-16+4×i]
   *  [-1.1102230246252ᴇ-16+i,-2.2204460492503ᴇ-16-3×i]] on Linux */

  // Lists
  approximates_to<float>("{1,2,3}/10", "{0.1,0.2,0.3}");
  approximates_to<float>("10/{1,2,4}", "{10,5,2.5}");
  approximates_to<float>("{12,100,1}/{4,2,1}", "{3,50,1}");
  approximates_to<float>("{1,2,3}/{4,5}", "undef");
  approximates_to<float>("{1,2,3}/[[4,5,6]]", "undef");
  approximates_to<double>("{1,2,3}/10", "{0.1,0.2,0.3}");
  approximates_to<double>("10/{1,2,4}", "{10,5,2.5}");
  approximates_to<double>("{12,100,1}/{4,2,1}", "{3,50,1}");
  approximates_to<double>("{1,2,3}/{4,5}", "undef");
  approximates_to<double>("{1,2,3}/[[4,5,6]]", "undef");

  // Euclidean division
  approximates_to<float>("quo(23,12)", 1);
  approximates_to<float>("rem(23,12)", 11);
  approximates_to<float>("quo(-23,12)", -2);
  approximates_to<float>("rem(-23,12)", 1);
  approximates_to<float>("quo(23,-12)", -1);
  approximates_to<float>("rem(23,-12)", 11);
  approximates_to<float>("quo(-23,-12)", 2);
  approximates_to<float>("rem(-23,-12)", 1);
}

QUIZ_CASE(pcj_approximation_boolean) {
  approximates_to_boolean(KTrue, true);
  approximates_to_boolean(KFalse, false);
  approximates_to_boolean(KDep(KTrue, KDepList(KUndef)),
                          Approximation::BooleanOrUndefined::Undef());

  approximates_to<float>("True and 3<π", "True");
  approximates_to<float>("3>π or False", "False");
  approximates_to<float>("not({3<π, 3>π})", "{False,True}");

  approximates_to<float>("2 and 3", "undef");
  approximates_to<float>("True + 3", "undef");

  approximates_to<float>("4000!4=9", "False");
  approximates_to<float>("4000!4!=9", "True");
}

QUIZ_CASE(pcj_approximation_piecewise) {
  approximates_to<float>("piecewise(3,1<0,2)", "2");
  approximates_to<float>("piecewise(3,1>0,2)", "3");
  approximates_to<float>("piecewise(3,0>1,4,0>2,5,0<6,2)", "5");
  approximates_to<float>("piecewise(3,0<1,4,0<2,5,0<6,2)", "3");
  approximates_to<float>("piecewise(3,1<0,2,3=4)", "undef");
}

QUIZ_CASE(pcj_approximation_replace) {
  TreeRef ref1(KAdd(1_e, 2_e, 10.5_de));
  quiz_assert(Approximation::ApproximateAndReplaceEveryScalar<double>(ref1));
  assert_trees_are_equal(ref1, 13.5_de);

  TreeRef ref2(
      KMult(2.0_de, KDiv("x"_e, KAdd(1_e, 2.0_de)), KAdd(1_e, 2_e, 10.5_de)));
  quiz_assert(Approximation::ApproximateAndReplaceEveryScalar<double>(ref2));
  assert_trees_are_equal(ref2, KMult(KDiv("x"_e, 3.0_de), 27_de));

  TreeRef ref3(
      KMult(2.0_fe, KDiv("x"_e, KAdd(1_e, 2.0_de)), KAdd(1_e, 2_e, 10.5_de)));
  quiz_assert(Approximation::ApproximateAndReplaceEveryScalar<double>(ref3));
  assert_trees_are_equal(ref3, KMult(KDiv("x"_e, 3.0_fe), 27_fe));

  TreeRef ref4(
      KMult(2.0_de, KDiv("x"_e, KAdd(1_e, 2.0_fe)), KAdd(1_e, 2_e, 10.5_fe)));
  quiz_assert(Approximation::ApproximateAndReplaceEveryScalar<float>(ref4));
  assert_trees_are_equal(ref4, KMult(KDiv("x"_e, 3.0_fe), 27_fe));
}

QUIZ_CASE(pcj_approximation_keeping_symbols) {
  approximates_to_keeping_symbols<double>("ln(10)+cos(10)+3x",
                                          "3.287392846+3×x", degreeCtx, 10);
  approximates_to_keeping_symbols<double>(
      "cos(4/3+ln(x-1/2))", "cos(1.333333333+ln(x-0.5))", degreeCtx, 10);
  approximates_to_keeping_symbols<double>("ln(ln(ln(10+10)))+ln(ln(ln(x+10)))",
                                          "0.09275118142+ln(ln(ln(x+10)))",
                                          realCtx, 10);
  approximates_to_keeping_symbols<double>("int(x,x,0,2)+int(x,x,0,x)",
                                          "2+int(x,x,0,x)", realCtx, 10);
  approximates_to_keeping_symbols<double>(
      "[[x,cos(10)][1/2+x,cos(4/3+x)]]",
      "[[x,0.984807753][0.5+x,cos(1.333333333+x)]]", degreeCtx, 10);
  approximates_to_keeping_symbols<double>("{x,undef,cos(10)+x,cos(10)}",
                                          "{x,undef,0.984807753+x,0.984807753}",
                                          degreeCtx, 10);
  approximates_to_keeping_symbols<double>("cos(10)→x", "0.984807753→x",
                                          degreeCtx, 10);
}

QUIZ_CASE(pcj_approximation_power) {
  approximates_to<float>("0^0", "undef");
  approximates_to<float>("0^0.5", "0");
  approximates_to<float>("0^2", "0");
  approximates_to<float>("0^(-0.5)", "undef");
  approximates_to<float>("0^(-2)", "undef");
  approximates_to<float>("0^(-2.5)", "undef");
  approximates_to<float>("0^(-3)", "undef");
  approximates_to<float>("(-1)^(-2024)", "1");
  approximates_to<float>("(-1)^(-2025)", "-1");
  approximates_to<float>("(-1)^(pi)", "nonreal");
  approximates_to<float>("(-2.5)^(-3.14)", "nonreal");
  approximates_to<float>("0^i", "undef", cartesianCtx);
  approximates_to<float>("0^(3+4i)", "0", cartesianCtx);
  approximates_to<float>("0^(3-4i)", "0", cartesianCtx);
  approximates_to<float>("0^(-3+4i)", "undef", cartesianCtx);
  approximates_to<float>("0^(-3-4i)", "undef", cartesianCtx);
  approximates_to<float>("1-e^(-90000000/563)", "1");
  approximates_to<float>("√(888888)", "942.8086");
  approximates_to<float>("888888^(.5)", "942.8086");
  approximates_to<float>("√(-888888)", "942.8086×i", cartesianCtx);
  approximates_to<float>("1/(0.752^-0.245)", 0.9325527);
  approximates_to<float>("1/(0.75^-0.245)", 0.9319444);
#if 0
  // TODO_PCJ: should be a pure imaginary
  approximates_to<float>("(-888888)^(.5)", "7.118005ᴇ-5+942.8084×i",
                         cartesianCtx);
#endif

  approximates_to<float>("2^3", 8.0f);
  approximates_to<double>("(3+i)^(4+i)", NAN, cartesianCtx);
  approximates_to<float>("[[1,2][3,4]]^2", NAN);

  approximates_to<float>("2^3", "8");
  approximates_to<double>("(3+i)^4", "28+96×i", cartesianCtx);
  approximates_to<float>("4^(3+i)", "11.74125+62.91378×i", cartesianCtx);
  approximates_to<double>("(3+i)^(3+i)", "-11.898191759852+19.592921596609×i",
                          cartesianCtx);
  approximates_to<double>("(-2)^4.2", "14.86906384965+10.803007238398×i",
                          cartesianCtx);
  approximates_to<double>("(-0.1)^4", "1ᴇ-4", cartesianCtx);
  approximates_to<double>("i^i", "0.20787957635076", cartesianCtx);
  approximates_to<float>("1.0066666666667^60", "1.489845");
  approximates_to<double>("1.0066666666667^60", "1.4898457083046");
  approximates_to<double>("1.0092^50", "1.5807460027336");
  approximates_to<float>("1.0092^50", "1.580744");
  // TODO_PCJ: approximation of exp(i×π) != approximation of e^(i×π)
  approximates_to<float>("e^(i×π)", "-1", cartesianCtx);
  approximates_to<double>("e^(i×π)", "-1", cartesianCtx);
  approximates_to<float>("e^(i×π+2)", "-7.389055", cartesianCtx);
  approximates_to<double>("e^(i×π+2)", "-7.3890560989307", cartesianCtx);
  approximates_to<double>("(-1)^2", "1");
  approximates_to<double>("(-1)^3", "-1");
  approximates_to<float>("(-1)^(1/3)", "0.5+0.8660254×i", cartesianCtx);
  approximates_to<double>("(-1)^(1/3)", "0.5+0.86602540378444×i", cartesianCtx);
  approximates_to<float>("e^(i×π/3)", "0.5000001+0.8660254×i", cartesianCtx);
  approximates_to<double>("e^(i×π/3)", "0.5+0.86602540378444×i", cartesianCtx);
  approximates_to<float>("i^(2/3)", "0.5+0.8660254×i", cartesianCtx);
  approximates_to<double>("i^(2/3)", "0.5+0.86602540378444×i", cartesianCtx);
  approximates_to<double>("1/cos(-1+50×i)",
                          "2.0842159805955ᴇ-22-3.2459740680286ᴇ-22×i",
                          cartesianCtx);
  approximates_to<float>("(-10)^0.00000001", "nonreal");
  approximates_to<float>("(-10)^0.00000001", "1+3.141593ᴇ-8×i", cartesianCtx);
  projected_approximates_to<float>("3.5^2.0000001", "12.25");
  projected_approximates_to<float>("3.7^2.0000001", "13.69");
  projected_approximates_to<double>("(13619-(185477161)^(1/2))^(-1)", "undef");

  approximates_to<float>("1.2×e^(1)", "3.261938");
  // WARNING: the 7th significant digit is wrong on blackbos simulator
  approximates_to<float>("2e^(3)", "40.1711", realCtx, 6);
  // WARNING: the 7th significant digit is wrong on simulator
  approximates_to<float>("e^2×e^(1)", "20.0855", realCtx, 6);
  approximates_to<double>("e^2×e^(1)", "20.085536923188");
  approximates_to<double>("2×3^4+2", "164");
  approximates_to<float>("-2×3^4+2", "-160");
  simplified_approximates_to<double>("1.0092^(20)", "1.2010050593402");
  simplified_approximates_to<double>("1.0092^(50)×ln(3/2)", "0.6409373488899",
                                     realCtx, 13);
  simplified_approximates_to<double>("1.0092^(50)×ln(1.0092)",
                                     "0.01447637354655", realCtx, 13);
  approximates_to<double>("1.0092^(20)", "1.2010050593402");
  approximates_to<double>("1.0092^(50)×ln(3/2)", "0.6409373488899", realCtx,
                          13);
  approximates_to<double>("1.0092^(50)×ln(1.0092)", "0.01447637354655", realCtx,
                          13);
  simplified_approximates_to<double>("1.0092^(20)", "1.2010050593402");
  simplified_approximates_to<double>("1.0092^(50)×ln(3/2)", "0.6409373488899",
                                     realCtx, 13);
  approximates_to<float>("1.0092^(50)×ln(3/2)", "0.6409366");
  approximates_to<float>("1.0092^(20)", "1.201005");
  // TODO_PCJ: failing power tests
  // simplified_approximates_to<float>("1.0092^(20)", "1.2010050593402");
  // simplified_approximates_to<float>("1.0092^(50)×ln(3/2)",
  //                                   "6.4093734888993ᴇ-1");

  // Lists
  approximates_to<float>("{1,2,3}^2", "{1,4,9}");
  approximates_to<float>("2^{1,2,3}", "{2,4,8}");
  approximates_to<float>("{1,2,3}^{1,2,3}", "{1,4,27}");
  approximates_to<float>("{1,2,3}^{4,5}", "undef");
  approximates_to<float>("{1,2,3}^[[4,5,6]]", "undef");
  approximates_to<double>("{1,2,3}^2", "{1,4,9}");
  approximates_to<double>("2^{1,2,3}", "{2,4,8}");
  approximates_to<double>("{1,2,3}^{1,2,3}", "{1,4,27}");
  approximates_to<double>("{1,2,3}^{4,5}", "undef");
  approximates_to<double>("{1,2,3}^[[4,5,6]]", "undef");
}

QUIZ_CASE(pcj_approximation_logarithm) {
  approximates_to<float>("log(2,64)", "0.1666667");
  approximates_to<double>("log(6,7)", "0.9207822211616");
  approximates_to<float>("log(5)", "0.69897");
  approximates_to<double>("ln(5)", "1.6094379124341");
  approximates_to<float>("log(2+5×i,64)", "0.4048317+0.2862042×i",
                         cartesianCtx);
  approximates_to<double>("log(6,7+4×i)", "0.80843880717528-0.20108238082167×i",
                          cartesianCtx);
  approximates_to<float>("log(5+2×i)", "0.731199+0.1652518×i", cartesianCtx);
  approximates_to<double>("ln(5+2×i)", "1.6836479149932+0.38050637711236×i",
                          cartesianCtx);
  projected_approximates_to<double>("log(0,0)", "undef");
  projected_approximates_to<double>("log(0)", "undef");
  projected_approximates_to<double>("log(2,0)", "undef");

  // WARNING: evaluate on branch cut can be multi-valued
  approximates_to<double>("ln(-4)", "1.3862943611199+3.1415926535898×i",
                          cartesianCtx);
}

QUIZ_CASE(pcj_approximation_list) {
  approximates_to<float>("{1,2,3,4,5,6}", "{1,2,3,4,5,6}");
  approximates_to<double>("{1,2,3,4,5,6}", "{1,2,3,4,5,6}");
  approximates_to<float>("{1,2,3,4}(-5,1)", "undef");
  approximates_to<float>("{1,2,3,4}(0,2)", "{1,2}");

  approximates_to<float>("sequence(k<=1, k, 2)", "{True,False}");
  approximates_to<float>("sequence(1/(k-2)<=3, k, 5)",
                         "{True,undef,True,True,True}");
  approximates_to<float>("sequence(1/(k-2)=3, k, 5)",
                         "{False,undef,False,False,False}");
  approximates_to<float>("sequence(k^2,k,4)", "{1,4,9,16}");
  approximates_to<double>("sequence(k/2,k,7)", "{0.5,1,1.5,2,2.5,3,3.5}");

  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("{1,2,3,4,5}→L", symbolStore);
  approximates_to<float>("L(1)", "1", {.m_context = symbolStore});
  approximates_to<float>("L(0)", "undef", {.m_context = symbolStore});
  approximates_to<float>("L(7)", "undef", {.m_context = symbolStore});
  approximates_to<double>("L(1)", "1", {.m_context = symbolStore});
  approximates_to<double>("L(0)", "undef", {.m_context = symbolStore});
  approximates_to<double>("L(7)", "undef", {.m_context = symbolStore});
  approximates_to<float>("L(1,3)", "{1,2,3}", {.m_context = symbolStore});
  approximates_to<float>("L(1,9)", "{1,2,3,4,5}", {.m_context = symbolStore});
  approximates_to<float>("L(-5,3)", "undef", {.m_context = symbolStore});
  approximates_to<float>("L(3,1)", "{}", {.m_context = symbolStore});
  approximates_to<float>("L(8,9)", "{}", {.m_context = symbolStore});
}

QUIZ_CASE(pcj_approximation_lists_functions) {
  // Mean
  approximates_to<double>("mean({5,8,7,4,12})", 7.2);
  approximates_to<double>("mean({5,8,7,4,12},{1,2,3,5,6})", 7.882352941176471);
  approximates_to<double>("mean({5,8,7,4,12},{2})", "undef");
  approximates_to<double>("mean({5,8,7,4,12},{-1,1,1,1,1})", "undef");
  approximates_to<double>("mean({5,8,7,4,12},{0,0,0,0,0})", "undef");
  // Median
  approximates_to<double>("med({1,6,3,5,2})", 3.);
  approximates_to<double>("med({1,6,3,4,5,2})", 3.5);
  approximates_to<double>("med({1,6,3,4,5,2},{2,3,0.1,2.8,3,1})", 5.);
  approximates_to<double>("med({1,undef,6,3,5,undef,2})", "undef");
  // Standard deviation
  approximates_to<double>("stddev({1,2,3,4,5,6})", 1.707825127659933);
  approximates_to<double>("stddev({1,2,3,4,5,6},{6,2,3,4,5,1})",
                          1.6700645635000173);
  approximates_to<double>("stddev({1})", 0.);
  approximates_to<double>("samplestddev({1,2,3,4,5,6})", 1.8708286933869704);
  approximates_to<double>("samplestddev({1,2,3,4,5,6},{6,2,3,4,5,1})",
                          1.7113069358158486);
  approximates_to<double>("samplestddev({1})", "undef");
  // Variance
  approximates_to<double>("var({1,2,3,4,5,6},{7,0.1,2,0,1,10})",
                          5.2815524368208706);
  approximates_to<double>("var({1,2,3,4,5,6})", 2.916666666666666);
  approximates_to<double>("var({1,2,3,undef,4,5,6})", "undef");
  approximates_to<double>("var({1,2,3,3,4,5,6},{-2,2,2,2,2,2,2})", "undef");
  approximates_to<double>("var({1,2,3,4,5,6},{0,0,0,0,0,0})", "undef");
  // Dimension
  approximates_to<double>("dim({1,2,3})", 3.);
  // Minimum
  approximates_to<double>("min({1,2,3})", 1.);
  approximates_to<double>("min({undef})", "undef");
  approximates_to<double>("min({1,undef,3})", "undef");
  approximates_to<double>("min({1,undef,i})", "undef");
  approximates_to<double>("min({1,7,i})", "undef");
  // Maximum
  approximates_to<double>("max({1,2,3})", 3.);
  approximates_to<double>("max({undef})", "undef");
  approximates_to<double>("max({1,undef,3})", "undef");
  approximates_to<double>("max({1,undef,i})", "undef");
  approximates_to<double>("max({1,7,i})", "undef");
  // Sum
  approximates_to<double>("sum({1,2,3})", 6.);
  // Product
  approximates_to<double>("prod({1,4,9})", 36.);

  // Sort a list of scalars or booleans
  approximates_to<float>("sort({True,False,True})", "{False,True,True}");
  approximates_to<float>("sort({1,3,4,2})", "{1,2,3,4}");
  approximates_to<float>("sort({(3,2),(1,4),(2,0),(1,1)})",
                         "{(1,1),(1,4),(2,0),(3,2)}");
  approximates_to<double>("sort({})", "{}");
  approximates_to<double>("sort({4})", "{4}");
  approximates_to<double>("sort({undef})", "{undef}");
  approximates_to<double>("sort({-1,5,2+6,-0})", "{-1,0,5,8}");
  approximates_to<double>("sort({-1,-2,-inf,inf})", "{-∞,-2,-1,∞}");
  approximates_to<double>("sort({i})", "{i}", cartesianCtx);
  approximates_to<double>("sort({-1,undef,-2,-inf,inf})",
                          "{undef,undef,undef,undef,undef}");
  approximates_to<double>("sort({-1,undef,1})", "{undef,undef,undef}");
  // Sort list of points
  approximates_to<double>("sort({(8,1),(5,0),(5,-3),(1,0),(5,9)})",
                          "{(1,0),(5,-3),(5,0),(5,9),(8,1)}");
  approximates_to<double>("sort({(inf,1),(6,1),(5,-3),(-inf,9),(-inf,1)})",
                          "{(-∞,1),(-∞,9),(5,-3),(6,1),(∞,1)}");

  // Do not sort complexes or undefined expressions
  approximates_to<double>("sort({-1,i,8,-0})", "{undef,undef,undef,undef}",
                          cartesianCtx);
  approximates_to<double>("sort({(8,1),(5,i),(5,-3)})",
                          "{(undef,undef),(undef,undef),(undef,undef)}",
                          cartesianCtx);
  approximates_to<double>("sort({(undef,1),(6,1),(5,-3)})",
                          "{(undef,undef),(undef,undef),(undef,undef)}",
                          cartesianCtx);
}

QUIZ_CASE(pcj_approximation_complex_format) {
  // Real
  approximates_to<float>("0", "0");
  approximates_to<double>("0", "0");
  approximates_to<float>("10", "10");
  approximates_to<double>("-10", "-10");
  approximates_to<float>("100", "100");
  approximates_to<double>("0.1", "0.1");
  approximates_to<float>("0.1234567", "0.1234567");
  approximates_to<double>("0.123456789012345", "0.12345678901235");
#if 0  // TODO_PCJ: nonreal not correctly bubbled up
  projected_approximates_to<float>("1+2×i", "nonreal");
  projected_approximates_to<double>("1+i-i", "nonreal");
  projected_approximates_to<double>("3-i", "nonreal");
  projected_approximates_to<float>("3-i-3", "nonreal");
  projected_approximates_to<float>("i", "nonreal");
#endif
  projected_approximates_to<float>("1+i-1", "nonreal");
  projected_approximates_to<double>("1+i", "nonreal");
  projected_approximates_to<float>("3+i", "nonreal");
  projected_approximates_to<double>("√(-1)", "nonreal");
  projected_approximates_to<double>("√(-1)×√(-1)", "nonreal");
  projected_approximates_to<double>("ln(-2)", "nonreal");
  // Power/Root approximates to the first REAL root in Real mode
  // Power have to be simplified first in order to spot the right form c^(p/q)
  // with p, q integers
  simplified_approximates_to<double>("(-8)^(1/3)", "-2");
  // Root approximates to the first REAL root in Real mode
  approximates_to<double>("root(-8,3)", "-2");
  approximates_to<double>("8^(1/3)", "2");
  // Power have to be simplified first (cf previous comment)
  simplified_approximates_to<float>("(-8)^(2/3)", "4");
  approximates_to<float>("root(-8, 3)^2", "4");
  approximates_to<double>("root(-8,3)", "-2");

  // Cartesian
  approximates_to<float>("0", "0", cartesianCtx);
  approximates_to<double>("0", "0", cartesianCtx);
  approximates_to<float>("10", "10", cartesianCtx);
  approximates_to<double>("-10", "-10", cartesianCtx);
  approximates_to<float>("100", "100", cartesianCtx);
  approximates_to<double>("0.1", "0.1", cartesianCtx);
  approximates_to<float>("0.1234567", "0.1234567", cartesianCtx);
  approximates_to<double>("0.123456789012345", "0.12345678901235",
                          cartesianCtx);
  approximates_to<float>("1+2×i", "1+2×i", cartesianCtx);
  approximates_to<double>("1+i-i", "1", cartesianCtx);
  approximates_to<float>("1+i-1", "i", cartesianCtx);
  approximates_to<double>("1+i", "1+i", cartesianCtx);
  approximates_to<float>("3+i", "3+i", cartesianCtx);
  approximates_to<double>("3-i", "3-i", cartesianCtx);
  approximates_to<float>("3-i-3", "-i", cartesianCtx);
  approximates_to<float>("i", "i", cartesianCtx);
  approximates_to<double>("√(-1)", "i", cartesianCtx);
  approximates_to<double>("√(-1)×√(-1)", "-1", cartesianCtx);
  approximates_to<double>("ln(-2)", "0.69314718055995+3.1415926535898×i",
                          cartesianCtx);
  approximates_to<double>("(-8)^(1/3)", "1+1.7320508075689×i", cartesianCtx);
  approximates_to<float>("(-8)^(2/3)", "-2+3.4641×i", cartesianCtx, 6);
  approximates_to<double>("root(-8,3)", "1+1.7320508075689×i", cartesianCtx);

  // Polar
  approximates_to<float>("0", "0", polarCtx);
  approximates_to<double>("0", "0", polarCtx);
  approximates_to<float>("10", "10", polarCtx);
  approximates_to<double>("-10", "10×e^(3.1415926535898×i)", polarCtx);

  approximates_to<float>("100", "100", polarCtx);
  approximates_to<double>("0.1", "0.1", polarCtx);
  approximates_to<float>("0.1234567", "0.1234567", polarCtx);
  approximates_to<double>("0.12345678", "0.12345678", polarCtx);

  approximates_to<float>("1+2×i", "2.236068×e^(1.107149×i)", polarCtx);
  approximates_to<float>("1+i-i", "1", polarCtx);
  approximates_to<double>("1+i-1", "e^(1.57079632679×i)", polarCtx, 12);
  approximates_to<float>("1+i", "1.414214×e^(0.7853982×i)", polarCtx);
  approximates_to<double>("3+i", "3.16227766017×e^(0.321750554397×i)", polarCtx,
                          12);
  approximates_to<float>("3-i", "3.162278×e^(-0.3217506×i)", polarCtx);
  approximates_to<double>("3-i-3", "e^(-1.57079632679×i)", polarCtx, 12);
  // 2e^(i) has a too low precision in float on the web platform
  approximates_to<float>("3e^(2*i)", "3×e^(2×i)", polarCtx, 4);
#if 0  // TODO_PCJ: approximates to -1*i, fix beautification?
  approximates_to<double>("2e^(-i)", "2×e^(-i)", polarCtx, 9);
#endif

  approximates_to<float>("i", "e^(1.570796×i)", polarCtx);
  approximates_to<double>("√(-1)", "e^(1.5707963267949×i)", polarCtx);
  approximates_to<double>("√(-1)×√(-1)", "e^(3.1415926535898×i)", polarCtx);
  approximates_to<double>("(-8)^(1/3)", "2×e^(1.0471975511966×i)", polarCtx);
  approximates_to<float>("(-8)^(2/3)", "4×e^(2.094395×i)", polarCtx);
  approximates_to<double>("root(-8,3)", "2×e^(1.0471975511966×i)", polarCtx);

  // Cartesian to Polar and vice versa
  approximates_to<double>("2+3×i", "3.60555127546×e^(0.982793723247×i)",
                          polarCtx, 12);
  approximates_to<double>("3.60555127546×e^(0.982793723247×i)", "2+3×i",
                          cartesianCtx, 12);
  approximates_to<float>("12.04159457879229548012824103×e^(1.4876550949×i)",
                         "1+12×i", cartesianCtx, 5);

  // Overflow
  approximates_to<float>("-2ᴇ20+2ᴇ20×i", "-2ᴇ20+2ᴇ20×i", cartesianCtx);
  /* TODO_PCJ: this test fails on the device because libm hypotf (which is
   * called eventually by std::abs) is not accurate enough. We might change the
   * embedded libm? */
  // approximates_to<float>("-2ᴇ20+2ᴇ20×i", "2.828427ᴇ20×e^(2.356194×i)",
  // polarCtx);
  approximates_to<double>("1ᴇ155-1ᴇ155×i", "1ᴇ155-1ᴇ155×i", cartesianCtx);
  approximates_to<double>("1ᴇ155-1ᴇ155×i", "∞×e^(-0.785398163397×i)", polarCtx,
                          12);
#if 0  // TODO_PCJ: nonreal not correctly bubbled up
  approximates_to<float>("-2ᴇ100+2ᴇ100×i", "-∞+∞×i");
  approximates_to<double>("-2ᴇ360+2ᴇ360×i", "-∞+∞×i");
  approximates_to<float>("-2ᴇ100+2ᴇ10×i", "-∞+2ᴇ10×i");
  approximates_to<double>("-2ᴇ360+2×i", "-∞+2×i");
#endif
  approximates_to<float>("undef+2ᴇ100×i", "undef");
  approximates_to<double>("-2ᴇ360+undef×i", "undef");
}

QUIZ_CASE(pcj_approximation_map_on_list) {
  // TODO: Implement more tests on lists, with every functions
  approximates_to<float>("abs({1,-1,2,-3})", "{1,1,2,3}");
  approximates_to<float>("ceil({0.3,180})", "{1,180}");
  approximates_to<float>("cos({0,π})", "{1,-1}");
  approximates_to<float>("{1,3}!", "{1,6}");
  approximates_to<float>("{1,2,3,4}!", "{1,2,6,24}");
  approximates_to<float>("floor({1/√(2),1/2,1,-1.3})", "{0,0,1,-2}");
  approximates_to<float>("floor({0.3,180})", "{0,180}");
  approximates_to<float>("frac({0.3,180})", "{0.3,0}");
  approximates_to<float>("im({1/√(2),1/2,1,-1})", "{0,0,0,0}");
  approximates_to<float>("im({1,1+i})", "{0,1}", cartesianCtx);
  approximates_to<float>("re({1,i})", "{1,0}", cartesianCtx);
  approximates_to<float>("round({2.12,3.42}, 1)", "{2.1,3.4}");
  approximates_to<float>("round(1.23456, {2,3})", "{1.23,1.235}");
  approximates_to<float>("sin({0,π})", "{0,0}");
  approximates_to<float>("{2,3.4}-{0.1,3.1}", "{1.9,0.3}");
  approximates_to<float>("tan({0,π/4})", "{0,1}");
  approximates_to<float>("abs(sum({0}×k,k,0,0))", "{0}");
}

QUIZ_CASE(pcj_approximation_matrix) {
  approximates_to<float>("trace([[1,2][4,3]])", "4");
  approximates_to<float>("identity(3)", "[[1,0,0][0,1,0][0,0,1]]");
  approximates_to<float>("[[1,2][4,3]]+[[1,2][4,3]]*2*[[1,2][4,3]]",
                         "[[19,18][36,37]]");
  approximates_to<float>("([[0]]^(2π))^2×0×[[0]]^(2π)", "undef");
  approximates_to<float>("[[0]]^(2π)", "undef");
  approximates_to<float>("[[1,2]]^2", "undef");
  approximates_to<float>("[[0]]^2", "[[0]]");
  approximates_to<float>("det(inverse([[0]]))", "undef");
  approximates_to<float>("transpose(inverse([[0]]))", "undef");
  approximates_to<float>("dot([[1]], inverse([[0]]))", "undef");

  approximates_to<float>("[[1,2,3][4,5,6]]", "[[1,2,3][4,5,6]]");
  approximates_to<double>("[[1,2,3][4,5,6]]", "[[1,2,3][4,5,6]]");

  approximates_to<float>("trace([[1,2,3][4,5,6][7,8,9]])", "15");
  approximates_to<double>("trace([[1,2,3][4,5,6][7,8,9]])", "15");

  approximates_to<float>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");
  approximates_to<double>("dim([[1,2,3][4,5,-6]])", "[[2,3]]");

  approximates_to<float>("det([[1,23,3][4,5,6][7,8,9]])", "126");
  approximates_to<double>("det([[1,23,3][4,5,6][7,8,9]])", "126");
  approximates_to<double>("det([[1,undef][4,6]])", "undef");
  approximates_to<float>("det([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])",
                         "126-231×i", cartesianCtx);
  approximates_to<double>("det([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])",
                          "126-231×i", cartesianCtx);

  approximates_to<float>("transpose([[1,2,3][4,5,-6][7,8,9]])",
                         "[[1,4,7][2,5,8][3,-6,9]]");
  approximates_to<float>("transpose([[1,7,5][4,2,8]])", "[[1,4][7,2][5,8]]");
  approximates_to<float>("transpose([[1,2][4,5][7,8]])", "[[1,4,7][2,5,8]]");
  approximates_to<double>("transpose([[1,2,3][4,5,-6][7,8,9]])",
                          "[[1,4,7][2,5,8][3,-6,9]]");
  approximates_to<double>("transpose([[1,7,5][4,2,8]])", "[[1,4][7,2][5,8]]");
  approximates_to<double>("transpose([[1,2][4,5][7,8]])", "[[1,4,7][2,5,8]]");

  approximates_to<double>("ref([[0,2,-1][5,6,7][10,11,10]])",
                          "[[1,1.1,1][0,1,-0.5][0,0,1]]");
  approximates_to<double>("rref([[0,2,-1][5,6,7][10,11,10]])",
                          "[[1,0,0][0,1,0][0,0,1]]");
  approximates_to<float>("ref([[0,2,-1][5,6,7][10,11,10]])",
                         "[[1,1.1,1][0,1,-0.5][0,0,1]]");
  approximates_to<float>("rref([[0,2,-1][5,6,7][10,11,10]])",
                         "[[1,0,0][0,1,0][0,0,1]]");
  approximates_to<float>("ref([[0,-1][undef,10]])", "undef");

  approximates_to<float>("cross([[1][2][3]],[[4][7][8]])", "[[-5][4][-1]]");
  approximates_to<double>("cross([[1][2][3]],[[4][7][8]])", "[[-5][4][-1]]");
  approximates_to<float>("cross([[1,2,3]],[[4,7,8]])", "[[-5,4,-1]]");
  approximates_to<double>("cross([[1,2,3]],[[4,7,8]])", "[[-5,4,-1]]");

  approximates_to<float>("dot([[1][2][3]],[[4][7][8]])", "42");
  approximates_to<double>("dot([[1][2][3]],[[4][7][8]])", "42");
  approximates_to<float>("dot([[1,2,3]],[[4,7,8]])", "42");
  approximates_to<double>("dot([[1,2,3]],[[4,7,8]])", "42");

  approximates_to<float>("norm([[-5][4][-1]])", "6.480741");
  approximates_to<double>("norm([[-5][4][-1]])", "6.4807406984079");
  approximates_to<float>("norm([[-5,4,-1]])", "6.480741");
  approximates_to<double>("norm([[-5,4,-1]])", "6.4807406984079");

  // inverse is not precise enough to display 7 significative digits
  approximates_to<float>(
      "inverse([[1,2,3][4,5,-6][7,8,9]])",
      "[[-1.2917,-0.083333,0.375][1.0833,0.16667,-0.25][0.041667,-0.083333,0."
      "041667]]",
      realCtx, 5);
  approximates_to<double>(
      "inverse([[1,2,3][4,5,-6][7,8,9]])",
      "[[-1.2916666666667,-0.083333333333333,0.375][1.0833333333333,0."
      "16666666666667,-0.25][0.041666666666667,-0.083333333333333,0."
      "041666666666667]]");
  // inverse is not precise enough to display 7 significative digits
  approximates_to<float>(
      "inverse([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])",
      "[[-0.0118-0.0455×i,-0.5-0.727×i,0.318+0.489×i][0.0409+0.00364×i,0.04-0."
      "0218×i,-0.0255+9.1ᴇ-4×i][0.00334-0.00182×i,0.361+0.535×i,-0.13-0.358×i]"
      "]",
      cartesianCtx, 3);
  // FIXME: inverse is not precise enough to display 14 significative digits
  approximates_to<double>(
      "inverse([[i,23-2i,3×i][4+i,5×i,6][7,8×i+2,9]])",
      "[[-0.0118289353958-0.0454959053685×i,-0.500454959054-0.727024567789×i,0."
      "31847133758+0.488626023658×i][0.0409463148317+0.00363967242948×i,0."
      "0400363967243-0.0218380345769×i,-0.0254777070064+9.0991810737ᴇ-4×i][0."
      "00333636639369-0.00181983621474×i,0.36093418259+0.534728541098×i,-0."
      "130118289354-0.357597816197×i]]",
      cartesianCtx, 12);

  // We do not map on matrices anymore
  approximates_to<float>("abs([[1,-2][3,-4]])", "undef");
  approximates_to<double>("abs([[1,-2][3,-4]])", "undef");

  // Undefined
  approximates_to<float>("cross([[0]],[[0]])", "undef");
  approximates_to<float>("det(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("dim(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("inverse(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("norm(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("ref(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("trace(cross([[0]],[[0]]))", "undef");
  approximates_to<float>("transpose(cross([[0]],[[0]]))", "undef");
}

QUIZ_CASE(pcj_approximation_point) {
  approximates_to<float>("(1,2)", "(1,2)");
  approximates_to<float>("(1/0,2)", "(undef,2)");
  approximates_to<float>("(1,2)+3", "undef");
  approximates_to<float>("abs((1.23,4.56))", "undef");
  approximates_to<float>("{(1+2,3+4),(5+6,7+8)}", "{(3,7),(11,15)}");

  approximates_to<double>("(1,2)", "(1,2)");
  approximates_to<double>("(1/0,2)", "(undef,2)");
  approximates_to<double>("(1,2)+3", "undef");
  approximates_to<double>("abs((1.23,4.56))", "undef");
  approximates_to<double>("{(1+2,3+4),(5+6,7+8)}", "{(3,7),(11,15)}");

  // undef is not bubbled up in points
  approximates_to<double>("(undef,i)", "(undef,nonreal)", realCtx);
  approximates_to<double>("(undef,i)", "(undef,i)", cartesianCtx);
}

QUIZ_CASE(pcj_approximation_infinity) {
  approximates_to<float>("inf", "∞");
  approximates_to<float>("inf*1", "∞");
  approximates_to<float>("inf(-1)", "-∞");
  approximates_to<float>("inf/1", "∞");
  approximates_to<float>("inf/(-1)", "-∞");
  approximates_to<float>("-inf+1", "-∞");
  approximates_to<float>("inf-inf", "undef");
  approximates_to<float>("-inf+inf", "undef");
  approximates_to<float>("inf*(-π)", "-∞");
  approximates_to<float>("inf*2*inf", "∞");
  approximates_to<float>("0×inf", "undef");
  approximates_to<float>("3×inf", "∞");
  approximates_to<float>("-3×inf", "-∞");
  approximates_to<float>("inf×(-inf)", "-∞");
  approximates_to<float>("1/inf", "0");
  approximates_to<float>("0/inf", "0");
  approximates_to<double>("10^1000", INFINITY);
  approximates_to<double>("10^1000", "∞");
  approximates_to<double>("2*10^1000", "∞");
  approximates_to<double>("(10^1000)/2", "∞");
  approximates_to<double>("(∞)×(i)", "∞×i", cartesianCtx);
  approximates_to<double>("(inf×i)×(i)", "-∞", cartesianCtx);
  approximates_to<double>("(inf×i)×(2)", "∞×i", cartesianCtx);
  // (inf+i)×(2) = inf * 2 - 1 * 0 + i * (inf * 0 + 1 * 2), inf * 0 return NAN
  approximates_to<double>("(inf+i)×(2)", "undef", cartesianCtx);

  // x^inf
  approximates_to<float>("(-2)^inf", "undef");  // complex inf
  approximates_to<float>("(-2)^(-inf)", "0");
  approximates_to<float>("(-1)^inf", "undef");
  approximates_to<float>("(-1)^(-inf)", "undef");
  approximates_to<float>("(-0.3)^inf", "0");
  approximates_to<float>("(-0.3)^(-inf)", "undef");  // complex inf
  approximates_to<float>("0^inf", "0");
  approximates_to<float>("0^(-inf)", "undef");  // complex inf
  approximates_to<float>("0.3^inf", "0");
  approximates_to<float>("0.3^(-inf)", "∞");
  approximates_to<float>("1^inf", "undef");
  approximates_to<float>("1^(-inf)", "undef");
  approximates_to<float>("2^inf", "∞");
  approximates_to<float>("2^(-inf)", "0");
  approximates_to<double>("(-i)^inf", "undef", cartesianCtx);
  approximates_to<double>("(-i)^(-inf)", "undef", cartesianCtx);
  approximates_to<double>("i^inf", "undef", cartesianCtx);
  approximates_to<double>("i^(-inf)", "undef", cartesianCtx);
  approximates_to<double>("(3+4i)^inf", "undef", cartesianCtx);
  approximates_to<double>("(3+4i)^(-inf)", "0", cartesianCtx);

  // inf^x
  approximates_to<float>("inf^i", "undef");
  approximates_to<float>("inf^6", "∞");
  approximates_to<float>("(-inf)^6", "∞");
  approximates_to<float>("inf^7", "∞");
  approximates_to<float>("(-inf)^7", "-∞");
  approximates_to<float>("inf^-6", "0");
  approximates_to<float>("(-inf)^-6", "0");
  approximates_to<float>("inf^-7", "0");
  approximates_to<float>("(-inf)^-7", "0");
  approximates_to<float>("inf^0", "undef");
  approximates_to<float>("(-inf)^0", "undef");
  approximates_to<float>("inf^inf", "∞");
  approximates_to<float>("inf^(-inf)", "0");
  approximates_to<float>("(-inf)^inf", "undef");  // complex inf
  approximates_to<float>("(-inf)^(-inf)", "0");

  // functions
  approximates_to<float>("exp(inf)", "∞");
  approximates_to<float>("exp(-inf)", "0");
  approximates_to<float>("log(inf,0)", "undef", cartesianCtx);
  approximates_to<float>("log(-inf,0)", "undef", cartesianCtx);
  approximates_to<float>("log(inf,1)", "undef", cartesianCtx);
  approximates_to<float>("log(-inf,1)", "undef", cartesianCtx);
  approximates_to<float>("log(inf,0.3)", "-∞");
  approximates_to<float>("log(-inf,0.3)", "nonreal");
  approximates_to<float>("log(-inf,0.3)", "-∞-2.609355×i", cartesianCtx);
  approximates_to<float>("log(inf,3)", "∞");
  approximates_to<float>("log(-inf,3)", "nonreal");
  approximates_to<float>("log(-inf,3)", "∞+2.859601×i", cartesianCtx);
  approximates_to<float>("log(inf,-3)", "nonreal");
  approximates_to<float>("log(inf,-3)", "∞-∞×i", cartesianCtx);
  approximates_to<float>("log(0,inf)", "undef", cartesianCtx);
  approximates_to<float>("log(0,-inf)", "undef", cartesianCtx);
  approximates_to<float>("log(1,inf)", "0");
  approximates_to<float>("log(1,-inf)", "0");
  approximates_to<float>("log(0.3,inf)", "0");
  approximates_to<float>("log(0.3,-inf)", "0");
  approximates_to<float>("log(3,inf)", "0");
  approximates_to<float>("log(3,-inf)", "0");
  approximates_to<float>("log(inf,inf)", "undef", cartesianCtx);
  approximates_to<float>("log(-inf,inf)", "undef", cartesianCtx);
  approximates_to<float>("log(inf,-inf)", "undef", cartesianCtx);
  approximates_to<float>("log(-inf,-inf)", "undef", cartesianCtx);
  approximates_to<float>("ln(inf)", "∞");
  approximates_to<float>("ln(-inf)", "nonreal");
  approximates_to<float>("ln(-inf)", "∞+3.141593×i", cartesianCtx);
  approximates_to<float>("cos(inf)", "undef");
  approximates_to<float>("cos(-inf)", "undef", cartesianCtx);
  approximates_to<float>("sin(inf)", "undef", cartesianCtx);
  approximates_to<float>("sin(-inf)", "undef", cartesianCtx);
  approximates_to<float>("atan(inf)", "1.570796");
  approximates_to<float>("atan(-inf)", "-1.570796");

  // nonreal vs undef
  approximates_to<float>("nonreal", "nonreal");
  approximates_to<float>("√(-1)", "nonreal");
  approximates_to<float>("√(-1)+1/0", "undef");
  // TODO_PCJ: this test returns 0 with emscripten
  // approximates_to<float>("1/(√(-1)^2+1)", "nonreal");
  approximates_to<float>("{√(-1),1/0}", "{nonreal,undef}");
  approximates_to<float>("(√(-1),2)", "(nonreal,2)");
  approximates_to<float>("(1/0,2)", "(undef,2)");
  approximates_to<float>("[[√(-1),2]]", "[[nonreal,2]]");
}

QUIZ_CASE(pcj_approximation_constant) {
  approximates_to<double>("π", "3.1415926535898");
  approximates_to<float>("e", "2.718282");
  approximates_to<float>("i", NAN);
  approximates_to<double>("i", NAN);
  approximates_to<float>("_c", 299792458.0);
  approximates_to<float>("_e", 1.602176634e-19);
  approximates_to<float>("_G", 6.67430e-11);
  approximates_to<float>("_g0", 9.80665);
  approximates_to<float>("_k", 1.380649e-23);
  approximates_to<float>("_ke", 8.9875517873681764e9);
  approximates_to<float>("_me", 9.1093837015e-31);
  approximates_to<float>("_mn", 1.67492749804e-27);
  approximates_to<float>("_mp", 1.67262192369e-27);
  approximates_to<float>("_Na", 6.02214076e23);
  approximates_to<float>("_R", 8.31446261815324);
  approximates_to<float>("_ε0", 8.854187817e-12);
  approximates_to<float>("_μ0", 1.25663706212e-6);
  approximates_to<float>("_hplanck", 6.62607015e-34);
}

QUIZ_CASE(pcj_approximation_units) {
  // Make input scalar to bypass hindered unit approximation.
  approximates_to<float>("(12_m)/(_m)", "12");
  approximates_to<float>("(1_s)/(_s)", "1");
  approximates_to<float>("1_m+1_s", "undef");
  approximates_to<float>("(1_m+1_yd)/(_m)", "1.9144");
  approximates_to<float>("1_m+x", "undef");
  approximates_to<float>("(1_mm+1_km)/(_m)", "1000.001");
  approximates_to<float>("(2_month×7_dm)/(_s×_m)", "3681720");
  approximates_to<float>("(1234_g)/(_kg)", "1.234");
  approximates_to<float>("(sum(_s,x,0,1))/(_s)", "2");

  // Temperature
#if 0  // Cannot turn these temperature expressions into scalars
  approximates_to<float>("4_°C", "277.15×_K");
  // Note: this used to be undef in previous Poincare.
  approximates_to<float>("((4-2)_°C)×2", "277.15×_K");
  approximates_to<float>("((4-2)_°F)×2", "257.5945×_K");
  approximates_to<float>("8_°C/2", "277.15×_K");
  approximates_to<float>("2_K+2_K", "4×_K");
  approximates_to<float>("2_K×2_K", "4×_K^2");
  approximates_to<float>("1/_K", "1×_K^-1");
  approximates_to<float>("(2_K)^2", "4×_K^2");
#endif

  // Undefined
  approximates_to<float>("2_°C-1_°C", "undef");
  approximates_to<float>("2_°C+2_K", "undef");
  approximates_to<float>("2_K+2_°C", "undef");
  approximates_to<float>("2_K×2_°C", "undef");
  approximates_to<float>("1/_°C", "undef");
  approximates_to<float>("(1_°C)^2", "undef");
  approximates_to<float>("tan(2_m)", "undef");
  approximates_to<float>("tan(2_rad^2)", "undef");

  // Constants
  approximates_to<float>("(_mn + _mp)/(_kg)", "3.34755ᴇ-27");
  approximates_to<float>("_mn + _G", "undef");
}

QUIZ_CASE(pcj_approximation_trigonometry) {
  approximates_to<double>("sin(3)2(4+2)", "1.6934400967184");
  approximates_to<double>("-sin(3)×2-3", "-3.2822400161197");
  approximates_to<float>("arccot(0)", "1.570796");
  approximates_to<float>("arccot(0)", "90", degreeCtx);
  approximates_to<float>("arccot(0.5)", "1.107149");
  approximates_to<float>("arccot(-0.5)", "2.034444");
  approximates_to<float>("arcsec(0)", "undef");
  approximates_to<float>("arccsc(0)", "undef");

  /* cos: R  ->  R (oscillator)
   *      Ri ->  R (even)
   */
  // On R
  approximates_to<double>("cos(2)", "-0.41614683654714");
  approximates_to<double>("cos(2)", "0.9993908270191", degreeCtx);
  approximates_to<double>("cos(2)", "0.99950656036573", gradianCtx);
  // Oscillator
  approximates_to<float>("cos(π/2)", "0");
  approximates_to<float>("cos(100)", "0", gradianCtx);
  approximates_to<double>("cos(3×π/2)", "0");
  approximates_to<float>("cos(300)", "0", gradianCtx);
  approximates_to<float>("cos(3×π)", "-1");
  approximates_to<float>("cos(-540)", "-1", degreeCtx);
  approximates_to<float>("cos(-600)", "-1", gradianCtx);
  // On R×i
  approximates_to<double>("cos(-2×i)", "3.7621956910836", cartesianCtx);
  approximates_to<double>("cos(-2×i)", "1.0006092967033", degreeCartesianCtx);
  approximates_to<double>("cos(-2×i)", "1.0004935208085", gradianCartesianCtx);
  // Symmetry: even
  approximates_to<double>("cos(2×i)", "3.7621956910836", cartesianCtx);
  approximates_to<double>("cos(2×i)", "1.0006092967033", degreeCartesianCtx);
  approximates_to<double>("cos(2×i)", "1.0004935208085", gradianCartesianCtx);

  // On C
  approximates_to<float>("cos(i-4)", "-1.008625-0.8893952×i", cartesianCtx);
  approximates_to<float>("cos(i-4)", "0.997716+0.00121754×i",
                         degreeCartesianCtx, 6);
  approximates_to<float>("cos(i-4)", "0.99815+9.86352ᴇ-4×i",
                         gradianCartesianCtx, 6);

  // Advanced function : sec
  approximates_to<double>("sec(2)", "-2.4029979617224");
  approximates_to<float>("sec(200)", "-1", gradianCtx);
  approximates_to<float>("sec(3×i)", "0.9986307", degreeCartesianCtx);
  approximates_to<float>("sec(-3×i)", "0.9986307", degreeCartesianCtx);
  approximates_to<float>("sec(i-4)", "-0.5577604+0.4918275×i", cartesianCtx, 7);

  /* sin: R  ->  R (oscillator)
   *      Ri ->  Ri (odd)
   */
  // On R
  approximates_to<double>("sin(2)", "0.90929742682568");
  approximates_to<double>("sin(2)", "0.034899496702501", degreeCtx);
  approximates_to<double>("sin(2)", "0.031410759078128", gradianCtx);
  // Oscillator
  approximates_to<float>("sin(π/2)", "1");
  approximates_to<double>("sin(3×π/2)", "-1");
  approximates_to<float>("sin(3×π)", "0");
  approximates_to<float>("sin(-540)", "0", degreeCtx);
  approximates_to<float>("sin(-600)", "0", gradianCtx);
  approximates_to<float>("sin(300)", "-1", gradianCtx);
  approximates_to<float>("sin(100)", "1", gradianCtx);
  // On R×i
  approximates_to<double>("sin(3×i)", "10.01787492741×i", cartesianCtx);
  approximates_to<float>("sin(3×i)", "0.05238381×i", degreeCartesianCtx);
  approximates_to<double>("sin(3×i)", "0.047141332771113×i",
                          gradianCartesianCtx);
  // Symmetry: odd
  approximates_to<double>("sin(-3×i)", "-10.01787492741×i", cartesianCtx);
  approximates_to<float>("sin(-3×i)", "-0.05238381×i", degreeCartesianCtx);
  approximates_to<double>("sin(-3×i)", "-0.047141332771113×i",
                          gradianCartesianCtx);

  // On: C
  approximates_to<float>("sin(i-4)", "1.16781-0.768163×i", cartesianCtx, 6);
  approximates_to<float>("sin(i-4)", "-0.0697671+0.0174117×i",
                         degreeCartesianCtx, 6);
  approximates_to<float>("sin(i-4)", "-0.0627983+0.0156776×i",
                         gradianCartesianCtx, 6);
  approximates_to<float>("sin(1.234567890123456ᴇ-15)", "1.23457ᴇ-15",
                         cartesianCtx, 6);

  // Advanced function : csc
  approximates_to<double>("csc(2)", "1.0997501702946");
  approximates_to<float>("csc(100)", "1", gradianCtx);
  approximates_to<float>("csc(3×i)", "-19.08987×i", degreeCartesianCtx);
  approximates_to<float>("csc(-3×i)", "19.08987×i", degreeCartesianCtx);
  approximates_to<float>("csc(i-4)", "0.597696+0.393154×i", cartesianCtx, 6);

  /* tan: R  ->  R (tangent-style)
   *      Ri ->  Ri (odd)
   */
  // On R
  approximates_to<double>("tan(2)", "-2.1850398632615");
  approximates_to<double>("tan(2)", "0.034920769491748", degreeCtx);
  approximates_to<double>("tan(2)", "0.031426266043351", gradianCtx);
  // Tangent-style
  approximates_to<float>("tan(3×π)", "0");
  approximates_to<float>("tan(-540)", "0", degreeCtx);
  approximates_to<float>("tan(-600)", "0", gradianCtx);
  // On R×i
  approximates_to<double>("tan(-2×i)", "-0.96402758007582×i", cartesianCtx);
  approximates_to<float>("tan(-2×i)", "-0.03489241×i", degreeCartesianCtx);
  approximates_to<float>("tan(-3×i)", "-0.04708904×i", gradianCartesianCtx);
  // Symmetry: odd
  approximates_to<double>("tan(2×i)", "0.96402758007582×i", cartesianCtx);
  approximates_to<float>("tan(2×i)", "0.03489241×i", degreeCartesianCtx);
  approximates_to<float>("tan(3×i)", "0.04708904×i", gradianCartesianCtx);
  // On C
  approximates_to<float>("tan(i-4)", "-0.273553+1.00281×i", cartesianCtx, 6);
  approximates_to<float>("tan(i-4)", "-0.0699054+0.0175368×i",
                         degreeCartesianCtx, 6);
  approximates_to<float>("tan(i-4)", "-0.0628991+0.0157688×i",
                         gradianCartesianCtx, 6);
  // tan(π/2)
  approximates_to<double>("tan(π/2)", "undef");
  approximates_to<double>("sin(π/2)/cos(π/2)", "undef");
  approximates_to<double>("1/cot(π/2)", "undef");
  // tan(almost π/2)
  approximates_to<double>("tan(1.57079632)", "147169276.57047");
  approximates_to<double>("sin(1.57079632)/cos(1.57079632)", "147169276.57047");
  approximates_to<double>("1/cot(1.57079632)", "147169276.57047");

  // Advanced function : cot
  approximates_to<double>("cot(2)", "-0.45765755436029");
  approximates_to<float>("cot(100)", "0", gradianCtx);
  approximates_to<float>("cot(3×i)", "-19.11604×i", degreeCartesianCtx);
  approximates_to<float>("cot(-3×i)", "19.11604×i", degreeCartesianCtx);
  approximates_to<float>("cot(i-4)", "-0.253182-0.928133×i", cartesianCtx, 6);
  // cot(π/2)
  approximates_to<double>("1/tan(π/2)", "undef");
  approximates_to<double>("cos(π/2)/sin(π/2)", "0");
  approximates_to<double>("cot(π/2)", "0");
  // cot(almost π/2)
  approximates_to<double>("1/tan(1.57079632)", "6.7948964845335ᴇ-9");
  approximates_to<double>("cos(1.57079632)/sin(1.57079632)",
                          "6.7948964845335ᴇ-9");
  approximates_to<double>("cot(1.57079632)", "6.7948964845335ᴇ-9");

  /* acos: [-1,1]    -> R
   *       ]-inf,-1[ -> π+R×i (odd imaginary)
   *       ]1, inf[  -> R×i (odd imaginary)
   *       R×i       -> π/2+R×i (odd imaginary)
   */
  // On [-1, 1]
  approximates_to<double>("acos(0.5)", "1.0471975511966");
  approximates_to<double>("acos(0.03)", "1.5407918249714");
  approximates_to<double>("acos(0.5)", "60", degreeCtx);
  approximates_to<double>("acos(0.5)", "66.666666666667", gradianCtx);
  // On [1, inf[
  approximates_to<double>("acos(2)", "nonreal");
  approximates_to<double>("acos(2)", "1.3169578969248×i", cartesianCtx);
  approximates_to<double>("acos(2)", "75.456129290217×i", degreeCartesianCtx);
  approximates_to<double>("acos(2)", "83.84×i", gradianCartesianCtx, 4);
  // Symmetry: odd on imaginary
  approximates_to<double>("acos(-2)", "3.1415926535898-1.3169578969248×i",
                          cartesianCtx);
  approximates_to<double>("acos(-2)", "180-75.456129290217×i",
                          degreeCartesianCtx);
  approximates_to<double>("acos(-2)", "200-83.84×i", gradianCartesianCtx, 4);
  // On ]-inf, -1[
  approximates_to<double>("acos(-32)", "3.14159265359-4.158638853279×i",
                          cartesianCtx, 13);
  approximates_to<float>("acos(-32)", "180-238.3×i", degreeCartesianCtx, 4);
  approximates_to<float>("acos(-32)", "200-264.7×i", gradianCartesianCtx, 4);
  // On R×i
  approximates_to<float>("acos(3×i)", "1.5708-1.8184×i", cartesianCtx, 5);
  approximates_to<float>("acos(3×i)", "90-104.19×i", degreeCartesianCtx, 5);
  approximates_to<float>("acos(3×i)", "100-115.8×i", gradianCartesianCtx, 4);
  // Symmetry: odd on imaginary
  approximates_to<float>("acos(-3×i)", "1.5708+1.8184×i", cartesianCtx, 5);
  approximates_to<float>("acos(-3×i)", "90+104.19×i", degreeCartesianCtx, 5);
  approximates_to<float>("acos(-3×i)", "100+115.8×i", gradianCartesianCtx, 4);
  // On C
  approximates_to<float>("acos(i-4)", "2.8894-2.0966×i", cartesianCtx, 5);
  approximates_to<float>("acos(i-4)", "165.551-120.126×i", degreeCartesianCtx,
                         6);
  approximates_to<float>("acos(i-4)", "183.9-133.5×i", gradianCartesianCtx, 4);
  // Key values
  approximates_to<double>("acos(0)", "90", degreeCtx);
  approximates_to<float>("acos(-1)", "180", degreeCtx);
  approximates_to<double>("acos(1)", "0", degreeCtx);
  approximates_to<double>("acos(0)", "100", gradianCtx);
  approximates_to<float>("acos(-1)", "200", gradianCtx);
  approximates_to<double>("acos(1)", "0", gradianCtx);
  // Advanced function : asec
  approximates_to<double>("arcsec(-2.4029979617224)", "2");
  approximates_to<float>("arcsec(-1)", "200", gradianCtx);
  approximates_to<float>("arcsec(0.9986307857)", "2.9999×i", degreeCartesianCtx,
                         5);
  approximates_to<float>("arcsec(-0.55776+0.491828×i)", "2.28318+1×i",
                         cartesianCtx, 6);

  /* asin: [-1,1]    -> R
   *       ]-inf,-1[ -> -π/2+R×i (odd)
   *       ]1, inf[  -> π/2+R×i (odd)
   *       R×i       -> R×i (odd)
   */
  // On [-1, 1]
  approximates_to<double>("asin(0.5)", "0.5235987755983");
  approximates_to<double>("asin(0.03)", "0.030004501823477");
  approximates_to<double>("asin(0.5)", "30", degreeCtx);
  approximates_to<double>("asin(0.5)", "33.3333", gradianCtx, 6);
  // On [1, inf[
  approximates_to<double>("asin(2)", "nonreal");
  approximates_to<double>("asin(2)", "1.5707963267949-1.3169578969248×i",
                          cartesianCtx);
  approximates_to<double>("asin(2)", "90-75.456129290217×i",
                          degreeCartesianCtx);
  approximates_to<double>("asin(2)", "100-83.84×i", gradianCartesianCtx, 4);
  // Symmetry: odd
  approximates_to<double>("asin(-2)", "-1.5707963267949+1.3169578969248×i",
                          cartesianCtx);
  approximates_to<double>("asin(-2)", "-90+75.456129290217×i",
                          degreeCartesianCtx);
  approximates_to<double>("asin(-2)", "-100+83.84×i", gradianCartesianCtx, 4);
  // On ]-inf, -1[
  approximates_to<float>("asin(-32)", "-1.571+4.159×i", cartesianCtx, 4);
  approximates_to<float>("asin(-32)", "-90+238×i", degreeCartesianCtx, 3);
  approximates_to<float>("asin(-32)", "-100+265×i", gradianCartesianCtx, 3);
  // On R×i
  approximates_to<double>("asin(3×i)", "1.8184464592321×i", cartesianCtx);
  approximates_to<double>("asin(3×i)", "115.8×i", gradianCartesianCtx, 4);
  // Symmetry: odd
  approximates_to<double>("asin(-3×i)", "-1.8184464592321×i", cartesianCtx);
  approximates_to<double>("asin(-3×i)", "-115.8×i", gradianCartesianCtx, 4);
  // On C
  approximates_to<float>("asin(i-4)", "-1.3186+2.0966×i", cartesianCtx, 5);
  approximates_to<float>("asin(i-4)", "-75.551+120.13×i", degreeCartesianCtx,
                         5);
  approximates_to<float>("asin(i-4)", "-83.95+133.5×i", gradianCartesianCtx, 4);
  // Key values
  approximates_to<double>("asin(0)", "0", degreeCtx);
  approximates_to<double>("asin(0)", "0", gradianCtx);
  approximates_to<float>("asin(-1)", "-90", degreeCtx, 6);
  approximates_to<float>("asin(-1)", "-100", gradianCtx, 6);
  approximates_to<double>("asin(1)", "90", degreeCtx);
  approximates_to<double>("asin(1)", "100", gradianCtx);
  // Advanced function : acsc
  approximates_to<double>("arccsc(1.0997501702946)", "1.1415926535898");
  approximates_to<double>("arccsc(1)", "100", gradianCtx, 6);
  approximates_to<double>("arccsc(-19.08987×i)", "3×i", degreeCartesianCtx, 6);
  approximates_to<double>("arccsc(19.08987×i)", "-3×i", degreeCartesianCtx, 6);
  approximates_to<double>("arccsc(0.5+0.4×i)", "0.792676-1.13208×i",
                          cartesianCtx, 6);

  /* atan: R         ->  R (odd)
   *       [-i,i]    ->  R×i (odd)
   *       ]-inf×i,-i[ -> -π/2+R×i (odd)
   *       ]i, inf×i[  -> π/2+R×i (odd)
   */
  // On R
  approximates_to<double>("atan(2)", "1.1071487177941");
  approximates_to<double>("atan(0.01)", "0.0099996666866652");
  approximates_to<double>("atan(2)", "63.434948822922", degreeCtx);
  approximates_to<double>("atan(2)", "70.48", gradianCtx, 4);
  approximates_to<float>("atan(0.5)", "0.4636476");
  // Symmetry: odd
  approximates_to<double>("atan(-2)", "-1.1071487177941");
  approximates_to<double>("atan(-2)", "-63.434948822922", degreeCtx);
  // On [-i, i]
  approximates_to<float>("atan(0.2×i)", "0.202733×i", cartesianCtx, 6);
  approximates_to<float>("atan(i)", "undef");
  // Symmetry: odd
  approximates_to<float>("atan(-0.2×i)", "-0.202733×i", cartesianCtx, 6);
  // On [i, inf×i[
  approximates_to<double>("atan(26×i)", "1.5707963267949+0.038480520568064×i",
                          cartesianCtx);
  approximates_to<double>("atan(26×i)", "90+2.2047714220164×i",
                          degreeCartesianCtx);
  approximates_to<double>("atan(26×i)", "100+2.45×i", gradianCartesianCtx, 3);
  // Symmetry: odd
  approximates_to<double>("atan(-26×i)", "-1.5707963267949-0.038480520568064×i",
                          cartesianCtx);
  approximates_to<double>("atan(-26×i)", "-90-2.2047714220164×i",
                          degreeCartesianCtx);
  approximates_to<double>("atan(-26×i)", "-100-2.45×i", gradianCartesianCtx, 3);
  // On ]-inf×i, -i[
  approximates_to<float>("atan(-3.4×i)", "-1.570796-0.3030679×i", cartesianCtx);
  approximates_to<float>("atan(-3.4×i)", "-90-17.3645×i", degreeCartesianCtx,
                         6);
  approximates_to<float>("atan(-3.4×i)", "-100-19.29×i", gradianCartesianCtx,
                         4);
  // On C
  approximates_to<float>("atan(i-4)", "-1.338973+0.05578589×i", cartesianCtx);
  approximates_to<float>("atan(i-4)", "-76.7175+3.1963×i", degreeCartesianCtx,
                         6);
  approximates_to<float>("atan(i-4)", "-85.24+3.551×i", gradianCartesianCtx, 4);
  // Key values
  approximates_to<float>("atan(0)", "0", degreeCtx);
  approximates_to<float>("atan(0)", "0", gradianCtx);
  approximates_to<double>("atan(-i)", "undef", cartesianCtx);
  approximates_to<double>("atan(i)", "undef", cartesianCtx);
  // Advanced function : acot
  approximates_to<double>("arccot(-0.45765755436029)", "2");
  approximates_to<double>("arccot(0)", "90", degreeCtx);
  approximates_to<float>("arccot(0)", "1.570796");
  approximates_to<float>("arccot(-19.11604×i)", "180+3×i", degreeCartesianCtx,
                         6);
  // TODO_PCJ: small real part instead of 0
  // approximates_to<float>("arccot(19.11604×i)", "-3×i", degreeCartesianCtx,
  // 6);
  approximates_to<float>("arccot(-0.253182-0.928133×i)", "2.28319+1×i",
                         cartesianCtx, 6);

  /* cosh: R         -> R (even)
   *       R×i       -> R (oscillator)
   */
  // On R
  approximates_to<double>("cosh(2)", "3.7621956910836");
  approximates_to<double>("cosh(2)", "3.7621956910836", degreeCtx);
  approximates_to<double>("cosh(2)", "3.7621956910836", gradianCtx);
  // Symmetry: even
  approximates_to<double>("cosh(-2)", "3.7621956910836");
  approximates_to<double>("cosh(-2)", "3.7621956910836", degreeCtx);
  approximates_to<double>("cosh(-2)", "3.7621956910836", gradianCtx);
  // On R×i
  approximates_to<double>("cosh(43×i)", "0.55511330152063", cartesianCtx);
  // Oscillator
  approximates_to<float>("cosh(π×i/2)", "0", cartesianCtx);
  approximates_to<float>("cosh(5×π×i/2)", "0", cartesianCtx);
  approximates_to<float>("cosh(8×π×i/2)", "1", cartesianCtx);
  approximates_to<float>("cosh(9×π×i/2)", "0", cartesianCtx);
  // On C
  approximates_to<float>("cosh(i-4)", "14.7547-22.9637×i", cartesianCtx, 6);
  approximates_to<float>("cosh(i-4)", "14.7547-22.9637×i", degreeCartesianCtx,
                         6);
  approximates_to<float>("cosh(i-4)", "14.7547-22.9637×i", gradianCartesianCtx,
                         6);

  /* sinh: R         -> R (odd)
   *       R×i       -> R×i (oscillator)
   */
  // On R
  approximates_to<double>("sinh(2)", "3.626860407847");
  approximates_to<double>("sinh(2)", "3.626860407847", degreeCtx);
  approximates_to<double>("sinh(2)", "3.626860407847", gradianCtx);
  // Symmetry: odd
  approximates_to<double>("sinh(-2)", "-3.626860407847");
  // On R×i
  approximates_to<double>("sinh(43×i)", "-0.8317747426286×i", cartesianCtx);
  // Oscillator
  approximates_to<float>("sinh(π×i/2)", "i", cartesianCtx);
  approximates_to<float>("sinh(5×π×i/2)", "i", cartesianCtx);
  approximates_to<float>("sinh(7×π×i/2)", "-i", cartesianCtx);
  approximates_to<float>("sinh(8×π×i/2)", "0", cartesianCtx);
  approximates_to<float>("sinh(9×π×i/2)", "i", cartesianCtx);
  // On C
  approximates_to<float>("sinh(i-4)", "-14.7448+22.9791×i", cartesianCtx, 6);
  approximates_to<float>("sinh(i-4)", "-14.7448+22.9791×i", degreeCartesianCtx,
                         6);

  /* tanh: R         -> R (odd)
   *       R×i       -> R×i (tangent-style)
   */
  // On R
  approximates_to<double>("tanh(2)", "0.96402758007582");
  approximates_to<double>("tanh(2)", "0.96402758007582", gradianCtx);
  // Symmetry: odd
  approximates_to<double>("tanh(-2)", "-0.96402758007582", degreeCtx);
  // On R×i
  approximates_to<double>("tanh(43×i)", "-1.4983873388552×i", cartesianCtx);
  // Tangent-style
  // FIXME: this depends on the libm implementation and does not work on
  // travis/appveyor servers
  /* approximates_to<float>("tanh(π×i/2)", "undef", cartesianCtx);
   * approximates_to<float>("tanh(5×π×i/2)","undef", cartesianCtx);
   * approximates_to<float>("tanh(7×π×i/2)", "undef", cartesianCtx);
   * approximates_to<float>("tanh(8×π×i/2)", "0", cartesianCtx);
   * approximates_to<float>("tanh(9×π×i/2)", "undef", cartesianCtx); */
  // On C
  approximates_to<float>("tanh(i-4)", "-1.00028+6.10241ᴇ-4×i", cartesianCtx, 6);
  approximates_to<float>("tanh(i-4)", "-1.00028+6.10241ᴇ-4×i",
                         degreeCartesianCtx, 6);

  /* acosh: [-1,1]       -> R×i
   *        ]-inf,-1[    -> π×i+R (even on real)
   *        ]1, inf[     -> R (even on real)
   *        ]-inf×i, 0[  -> -π/2×i+R (even on real)
   *        ]0, inf*i[   -> π/2×i+R (even on real)
   */
  // On [-1,1]
  approximates_to<double>("arcosh(2)", "1.3169578969248");
  approximates_to<double>("arcosh(2)", "1.3169578969248", degreeCtx);
  approximates_to<double>("arcosh(2)", "1.3169578969248", gradianCtx);
  // On ]-inf, -1[
  approximates_to<double>("arcosh(-4)", "2.0634370688956+3.1415926535898×i",
                          cartesianCtx);
  approximates_to<float>("arcosh(-4)", "2.06344+3.14159×i", cartesianCtx, 6);
  // On ]1,inf[: Symmetry: even on real
  approximates_to<double>("arcosh(4)", "2.0634370688956");
  approximates_to<float>("arcosh(4)", "2.063437");
  // On ]-inf×i, 0[
  approximates_to<double>("arcosh(-42×i)", "4.43095849208-1.57079632679×i",
                          cartesianCtx, 12);
  approximates_to<float>("arcosh(-42×i)", "4.431-1.571×i", cartesianCtx, 4);
  // On ]0, i×inf[: Symmetry: even on real
  approximates_to<double>("arcosh(42×i)", "4.4309584920805+1.5707963267949×i",
                          cartesianCtx);
  approximates_to<float>("arcosh(42×i)", "4.431+1.571×i", cartesianCtx, 4);
  // On C
  approximates_to<float>("arcosh(i-4)", "2.0966+2.8894×i", cartesianCtx, 5);
  approximates_to<float>("arcosh(i-4)", "2.0966+2.8894×i", degreeCartesianCtx,
                         5);
  // Key values
  // approximates_to<double>("arcosh(-1)", "3.1415926535898×i", cartesianCtx);
  approximates_to<double>("arcosh(1)", "0", cartesianCtx);
  approximates_to<float>("arcosh(0)", "1.570796×i", cartesianCtx);

  /* asinh: R            -> R (odd)
   *        [-i,i]       -> R*i (odd)
   *        ]-inf×i,-i[  -> -π/2×i+R (odd)
   *        ]i, inf×i[   -> π/2×i+R (odd)
   */
  // On R
  approximates_to<double>("arsinh(2)", "1.4436354751788");
  approximates_to<double>("arsinh(2)", "1.4436354751788", degreeCtx);
  approximates_to<double>("arsinh(2)", "1.4436354751788", gradianCtx);
  // Symmetry: odd
  approximates_to<double>("arsinh(-2)", "-1.4436354751788");
  approximates_to<double>("arsinh(-2)", "-1.4436354751788", degreeCtx);
  // On [-i,i]
  approximates_to<double>("arsinh(0.2×i)", "0.20135792079033×i", cartesianCtx);
  // arsinh(0.2*i) has a too low precision in float on the web platform
  approximates_to<float>("arsinh(0.3×i)", "0.3046927×i", degreeCartesianCtx);
  // Symmetry: odd
  approximates_to<double>("arsinh(-0.2×i)", "-0.20135792079033×i",
                          cartesianCtx);
  // arsinh(-0.2*i) has a too low precision in float on the web platform
  approximates_to<float>("arsinh(-0.3×i)", "-0.3046927×i", degreeCartesianCtx);
  // On ]-inf×i, -i[
  approximates_to<double>("arsinh(-22×i)", "-3.78367270433-1.57079632679×i",
                          cartesianCtx, 12);
  approximates_to<float>("arsinh(-22×i)", "-3.784-1.571×i", degreeCartesianCtx,
                         4);
  // On ]i, inf×i[, Symmetry: odd
  approximates_to<double>("arsinh(22×i)", "3.7836727043295+1.5707963267949×i",
                          cartesianCtx);
  approximates_to<float>("arsinh(22×i)", "3.784+1.571×i", degreeCartesianCtx,
                         4);
  // On C
  approximates_to<float>("arsinh(i-4)", "-2.123+0.2383×i", cartesianCtx, 4);
  approximates_to<float>("arsinh(i-4)", "-2.123+0.2383×i", degreeCartesianCtx,
                         4);

  /* atanh: [-1,1]       -> R (odd)
   *        ]-inf,-1[    -> π/2*i+R (odd)
   *        ]1, inf[     -> -π/2×i+R (odd)
   *        R×i          -> R×i (odd)
   */
  // On [-1,1]
  approximates_to<double>("artanh(0.4)", "0.4236489301936");
  approximates_to<double>("artanh(0.4)", "0.4236489301936", degreeCtx);
  approximates_to<double>("artanh(0.4)", "0.4236489301936", gradianCtx);
  // Symmetry: odd
  approximates_to<double>("artanh(-0.4)", "-0.4236489301936");
  approximates_to<double>("artanh(-0.4)", "-0.4236489301936", degreeCtx);
  // On ]1, inf[
  approximates_to<double>("artanh(4)", "0.255412811883-1.5707963267949×i",
                          cartesianCtx);
  approximates_to<float>("artanh(4)", "0.2554128-1.570796×i", cartesianCtx);
  // On ]-inf,-1[, Symmetry: odd
  approximates_to<double>("artanh(-4)", "-0.255412811883+1.5707963267949×i",
                          cartesianCtx);
  approximates_to<float>("artanh(-4)", "-0.2554128+1.570796×i",
                         degreeCartesianCtx);
  // On R×i
  approximates_to<double>("artanh(4×i)", "1.325817663668×i", cartesianCtx);
  approximates_to<float>("artanh(4×i)", "1.325818×i", cartesianCtx);
  // Symmetry: odd
  approximates_to<double>("artanh(-4×i)", "-1.325817663668×i", cartesianCtx);
  approximates_to<float>("artanh(-4×i)", "-1.325818×i", cartesianCtx);
  // On C
  approximates_to<float>("artanh(i-4)", "-0.238878+1.50862×i", cartesianCtx, 6);
  approximates_to<float>("artanh(i-4)", "-0.238878+1.50862×i",
                         degreeCartesianCtx, 6);

  // Check that the complex part is not neglected
  approximates_to<double>("artanh(0.99999999999+1.0ᴇ-26×i)", "13+5ᴇ-16×i",
                          cartesianCtx, 3);
  approximates_to<double>("artanh(0.99999999999+1.0ᴇ-60×i)", "13+5ᴇ-50×i",
                          cartesianCtx, 3);
  approximates_to<double>("artanh(0.99999999999+1.0ᴇ-150×i)", "13+5ᴇ-140×i",
                          cartesianCtx, 3);
  approximates_to<double>("artanh(0.99999999999+1.0ᴇ-250×i)", "13+5ᴇ-240×i",
                          cartesianCtx, 3);
  approximates_to<double>("artanh(0.99999999999+1.0ᴇ-300×i)", "13+5ᴇ-290×i",
                          cartesianCtx, 3);

  // WARNING: evaluate on branch cut can be multi-valued
  approximates_to<double>("acos(2)", "1.3169578969248×i", cartesianCtx);
  approximates_to<double>("acos(2)", "75.456129290217×i", degreeCartesianCtx);
  approximates_to<double>("asin(2)", "1.5707963267949-1.3169578969248×i",
                          cartesianCtx);
  approximates_to<double>("asin(2)", "90-75.456129290217×i",
                          degreeCartesianCtx);
  approximates_to<double>("artanh(2)", "0.54930614433405-1.5707963267949×i",
                          cartesianCtx);
  approximates_to<double>("atan(2i)", "1.5707963267949+0.54930614433405×i",
                          cartesianCtx);
  approximates_to<double>("atan(2i)", "90+31.472923730945×i",
                          degreeCartesianCtx);
  approximates_to<double>("arsinh(2i)", "1.3169578969248+1.5707963267949×i",
                          cartesianCtx);
  approximates_to<double>("arcosh(-2)", "1.3169578969248+3.1415926535898×i",
                          cartesianCtx);
}

QUIZ_CASE(pcj_approximation_arithmetic) {
  approximates_to<float>("floor(π)", "3");
  approximates_to<float>("floor(-π)", "-4");
  approximates_to<float>("log(floor(2^64),2)", "64");
  // TODO_PCJ: this test returns nonreal with emscripten
  // approximates_to<float>("floor(1+i)", "undef");
  approximates_to<float>("ceil(3)", "3");
  approximates_to<float>("ceil(π)", "4");
  approximates_to<float>("ceil(-π)", "-3");
  approximates_to<float>("ceil(1+i)", "undef");
  approximates_to<float>("ceil(0.2)", "1");
  approximates_to<double>("ceil(0.2)", "1");
  approximates_to<float>("floor(2.3)", "2");
  approximates_to<double>("floor(2.3)", "2");
  approximates_to<float>("frac(2.3)", "0.3");
  approximates_to<double>("frac(2.3)", "0.3");
  approximates_to<float>("round(2.3246,3)", "2.325");
  approximates_to<double>("round(2.3245,3)", "2.325");
  approximates_to<float>("round(2.3245)", "2");
  approximates_to<double>("round(2.3245)", "2");

  approximates_to<float>("6!", "720");
  approximates_to<double>("6!", "720");

  approximates_to<float>("gcd(234,394)", "2");
  approximates_to<double>("gcd(234,394)", "2");
  approximates_to<float>("gcd(-234,394)", "2");
  approximates_to<double>("gcd(234,-394)", "2");
  approximates_to<float>("gcd(-234,-394)", "2");
  approximates_to<float>("gcd(-234,-394, -16)", "2");
  approximates_to<double>("gcd(-234,-394, -16)", "2");
  approximates_to<float>("gcd(6,15,10)", "1");
  approximates_to<double>("gcd(6,15,10)", "1");
  approximates_to<float>("gcd(30,105,70,42)", "1");
  approximates_to<double>("gcd(30,105,70,42)", "1");

  approximates_to<float>("lcm(234,394)", "46098");
  approximates_to<double>("lcm(234,394)", "46098");
  approximates_to<float>("lcm(-234,394)", "46098");
  approximates_to<double>("lcm(234,-394)", "46098");
  approximates_to<float>("lcm(-234,-394)", "46098");
  approximates_to<float>("lcm(-234,-394, -16)", "368784");
  approximates_to<double>("lcm(-234,-394, -16)", "368784");
  approximates_to<float>("lcm(6,15,10)", "30");
  approximates_to<double>("lcm(6,15,10)", "30");
  approximates_to<float>("lcm(30,105,70,42)", "210");
  approximates_to<double>("lcm(30,105,70,42)", "210");

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
  // 1
  approximates_to<float>("gcd(16777219,13)", "undef");
  approximates_to<double>("gcd(16777219,13)", "1");
  // 16777219
  approximates_to<float>("lcm(1549, 10831)", "undef");
  approximates_to<double>("lcm(1549, 10831)", "16777219");
  // Integers overflowing int, but not uint32_t
  // 13
  approximates_to<float>("gcd(2147483650,13)", "undef");
  approximates_to<double>("gcd(2147483650,13)", "13");
  // 2147483650
  approximates_to<float>("lcm(2,25,13,41,61,1321)", "undef");
  approximates_to<double>("lcm(2,25,13,41,61,1321)", "2147483650");
  // Integers overflowing uint32_t
  // 13
  approximates_to<float>("gcd(4294967300,13)", "undef");
  // 13
  approximates_to<double>("gcd(4294967300,13)", "undef");
  // 4294967300
  approximates_to<float>("lcm(4,25,13,41,61,1321)", "undef");
  // 4294967300
  approximates_to<double>("lcm(4,25,13,41,61,1321)", "undef");
  // Integers that can't be accurately represented as double
  approximates_to<float>("gcd(1ᴇ16,10)", "undef");
  approximates_to<double>("gcd(1ᴇ16,10)", "undef");

  approximates_to<float>("quo(29, 10)", "2");
  approximates_to<double>("quo(29, 10)", "2");
  approximates_to<float>("rem(29, 10)", "9");
  approximates_to<double>("rem(29, 10)", "9");

  approximates_to<float>("factor(-23/4)", "-5.75");
  approximates_to<double>("factor(-123/24)", "-5.125");
  approximates_to<float>("factor(i)", "undef");
}

QUIZ_CASE(pcj_approximation_mixed_fraction) {
  GlobalPreferences::SharedGlobalPreferences()->setCountry(I18n::Country::US);
  assert(Poincare::SharedPreferences->mixedFractionsAreEnabled());
  approximates_to<double>("1 1/2", 1.5);
  approximates_to<double>("-1 1/2", -1.5);
  GlobalPreferences::SharedGlobalPreferences()->setCountry(I18n::Country::WW);
}

QUIZ_CASE(pcj_approximation_parametrics) {
  approximates_to<float>("int(x, x, 1, 2)", "1.5");
  approximates_to<float>("diff(2×x, x, 2)", "2");
  approximates_to<float>("sum(k/2, k, 1, 12)", "39");
}

QUIZ_CASE(pcj_approximation_integrals) {
  approximates_to<double>("int(e^x, x, 0, 200)", "7.2259737681256ᴇ86");
  approximates_to<double>("int(e^(x^3), x, 0, 4)", "1.312888495564ᴇ26");
  approximates_to<double>("int(√(x^2-3)/x, x, √(3), 2)", "0.093100317882891");

  approximates_to<double>("int(710×x×e^(-710×x), x, 0, 10)",
                          "0.0014084507042254");
  approximates_to<double>("710*int(x×e^(-710×x), x, 0, 10)",
                          "0.0014084507042254");
  approximates_to<float>("int(710×x×e^(-710×x), x, 0, 10)", "0.001408451");
  approximates_to<float>("710*int(x×e^(-710×x), x, 0, 10)", "0.001408451");
  approximates_to<double>("int(√(5-x^2), x, -√(5), √(5))", "7.8539816339745");
  approximates_to<float>("int(2*(2x-2)^3,x,1,2)", "4");

  simplified_approximates_to<double>("int(3x^2+x-4,x,-2,2)", "0");

  // Double integration
  approximates_to<float>("int(int(t^2,t,0,x),x,0,4)", "21.33333");
  approximates_to<float>("int(int(t,t,0,x),x,0,1)", "0.1666667");

  approximates_to<float>("int(x,x, 1, 2)", "1.5");
  approximates_to<double>("int(x,x, 1, 2)", "1.5");
  approximates_to<float>("int(1/x,x,0,1)", "undef");

  // Ensure this does not take up too much time
  approximates_to<double>("int(e^(x^3),x,0,6)", "5.9639380918976ᴇ91");

  approximates_to<float>("int(1+cos(a),a, 0, 180)", "180", degreeCtx);
  approximates_to<double>("int(1+cos(a),a, 0, 180)", "180", degreeCtx);

  // former precision issues
  // #1912
  approximates_to<double>("int(abs(2*sin(e^(x/4))+1),x,0,6)",
                          "12.573260585347");
  // #1912
  approximates_to<double>("int(4*√(1-x^2),x,0,1)", "3.1415926535898");
  // Runge function
  approximates_to<double>("int(1/(1+25*x^2),x,0,1)", "0.274680153389");
  // #1378
  approximates_to<double>("int(2*√(9-(x-3)^2),x,0,6)", "28.274333882309");

  // far bounds
  approximates_to<double>("int(e^(-x^2),x,0,100000)", "0.88622692545276");
  approximates_to<double>("int(1/(x^2+1),x,0,100000)", "1.5707863267949");
  approximates_to<double>("int(1/(x^2+1),x,0,inf)", "1.5707963267949");

  approximates_to<double>("int(1/x,x,1,1000000)", "13.81551056", realCtx, 10);
  approximates_to<double>("int(e^(-x)/√(x),x,0,inf)", "1.7724", realCtx, 5);
  approximates_to<double>("int(1,x,inf,0)", "undef");
  approximates_to<double>("int(e^(-x),x,inf,0)", "-1");

  // singularities, project for better precision
  projected_approximates_to<double>("int(ln(x)*√(x),x,0,1)", "-0.444444444444",
                                    realCtx, 12);
  projected_approximates_to<double>("int(1/√(x),x,0,1)", "2", realCtx, 12);
  projected_approximates_to<double>("int(1/√(1-x),x,0,1)", "2", realCtx, 12);
  projected_approximates_to<double>("int(1/√(x)+1/√(1-x),x,0,1)", "4", realCtx,
                                    12);
  projected_approximates_to<double>("int(ln(x)^2,x,0,1)", "2", realCtx, 12);
  // #596
  projected_approximates_to<double>("int(1/√(x-1),x,1,2)", "2", realCtx, 12);
  // #1780
  projected_approximates_to<double>("int(2/√(1-x^2),x,0,1)", "3.1415926535898");
  // #1780
  projected_approximates_to<double>("int(4x/√(1-x^4),x,0,1)",
                                    "3.1415926535898");
  // convergence is slow with 1/x^k k≈1, therefore precision is poor
  projected_approximates_to<float>("int(1/x^0.9,x,0,1)", "10", realCtx, 3);

  // double integration
  approximates_to<float>("int(int(x×x,x,0,x),x,0,4)", "21.33333");
  approximates_to<double>("int(int(x×x,x,0,x),x,0,4)", "21.333333333333");
  approximates_to<double>("int(sum(sin(x)^k,k,0,100),x,0,π)", "48.3828",
                          realCtx, 6);
  approximates_to<double>("int(int(1/(1-x)^k,x,0,1),k,0.5,0.25)", "-0.405465",
                          realCtx, 6);
  // this integral on R2 takes about one minute to compute on N110
  // approximates_to<double>("int(int(e^(-(x^2+y^2)),x,-inf,inf),y,-inf,inf)",
  //                         "3.141592654", realCtx, 10);
  // approximates_to<double>("int(int(e^(-x*t)/t^2,t,1,inf),x,0,1)",
  //                         "0.3903080328", realCtx, 10);

  // oscillator
  // poor precision
  approximates_to<double>("int((sin(x)/x)^2,x,0,inf)", "1.5708", realCtx, 5);
  approximates_to<double>("int(x*sin(1/x)*√(abs(1-x)),x,0,3)", "1.9819412",
                          realCtx, 8);

  /* The integral approximation escapes before finding a result, because it
   * would take too much time. */
  approximates_to<float>("int(sin((10^7)*x),x,0,1)", "undef");
}

QUIZ_CASE(pcj_approximation_derivatives) {
  approximates_to<float>("diff(ln(x), x, -1)", "undef");

  approximates_to_float_and_double("diff(2×x, x, 2)", "2");
  approximates_to_float_and_double("diff(2×\"TO\"^2, \"TO\", 7)", "28");
  approximates_to_float_and_double("diff(ln(x),x,1)", "1");
  approximates_to_float_and_double("diff(ln(x),x,2.2)", "0.455");
  approximates_to_float_and_double("diff(ln(x),x,0)", "undef");
  approximates_to_float_and_double("diff(ln(x),x,-3.1)", "undef");
  approximates_to_float_and_double("diff(log(x),x,-10)", "undef");
  approximates_to_float_and_double("diff(abs(x),x,123)", "1");
  approximates_to_float_and_double("diff(abs(x),x,-2.34)", "-1");
  approximates_to_float_and_double("diff(1/x,x,-2)", "-0.25");
  approximates_to_float_and_double("diff(x^3+5*x^2,x,0)", "0");
  approximates_to_float_and_double("diff(abs(x),x,0)", "0");  // "undef");
  // TODO_PCJ: error too big on floats
  // approximates_to<float>("diff(-1/3×x^3+6x^2-11x-50,x,11)", "0");
  approximates_to<double>("diff(-1/3×x^3+6x^2-11x-50,x,11)", "0");
  // On points
  approximates_to_float_and_double("diff((sin(t),cos(t)),t,π/2)", "(0,-1)");

  // Higher order
  // We have to approximate to double because error is too big on floats
  approximates_to<double>("diff(x^3,x,10,2)", "60");
  approximates_to<double>("diff(x^3,x,1,4)", "0");
  approximates_to<double>("diff(e^(2x),x,0,4)", "16");
  approximates_to_float_and_double("diff(x^3,x,3,0)", "27");
  approximates_to_float_and_double("diff(x^3,x,3,-1)", "undef");
  approximates_to_float_and_double("diff(x^3,x,3,1.3)", "undef");
  // Order 5 and above are not handled because recursively too long
  approximates_to_float_and_double("diff(e^(2x),x,0,5)", "undef");
  // On points
  approximates_to<double>("diff((2t,ln(t)),t,2,2)", "(0,-0.25)");
}

QUIZ_CASE(pcj_approximation_percent) {
  simplified_approximates_to<double>("900(1+1.5%/365)^(18*365)",
                                     "1178.9614650069");
  approximates_to<float>("20%", "0.2");
  approximates_to<float>("20%%", "0.002");
  approximates_to<float>("80*20%", "16");
  approximates_to<float>("80/(20%)", "400");
  approximates_to<float>("80+20%", "96");
  approximates_to<float>("20%+80+20%", "96.24");
  approximates_to<float>("80+20%+20%", "115.2");
  approximates_to<float>("80-20%", "64");
  approximates_to<float>("80+20-20%", "80");
  approximates_to<float>("10+2*3%", "10.06");
  approximates_to<float>("10+3%3", "10.09");
  approximates_to<float>("10+3%%", "10.0003");
  approximates_to<float>("10+3%%2", "10.0006");
  approximates_to<float>("10+3%-1", "9.3");
  approximates_to<float>("{10+3%,5+4%}", "{10.3,5.2}");
  approximates_to<float>("10+98%^2", "10.9604");
}

QUIZ_CASE(pcj_approximation_probability) {
  // FIXME: precision problem
  approximates_to<float>("binomcdf(5.3, 9, 0.7)", "0.270341", realCtx, 6);
  // FIXME precision problem
  approximates_to<double>("binomcdf(5.3, 9, 0.7)", "0.270340902", realCtx, 10);
  // FIXME: precision problem
  approximates_to<float>("binompdf(4.4, 9, 0.7)", "0.0735138", realCtx, 6);
  approximates_to<double>("binompdf(4.4, 9, 0.7)", "0.073513818");
  approximates_to<float>("invbinom(0.9647324002, 15, 0.7)", "13");
  approximates_to<double>("invbinom(0.9647324002, 15, 0.7)", "13");
  approximates_to<float>("invbinom(0.95,100,0.42)", "50");
  approximates_to<double>("invbinom(0.95,100,0.42)", "50");
  approximates_to<float>("invbinom(0.01,150,0.9)", "126");
  approximates_to<double>("invbinom(0.01,150,0.9)", "126");

  approximates_to<double>("geompdf(1,1)", "1");
  approximates_to<double>("geompdf(2,0.5)", "0.25");
  approximates_to<double>("geompdf(2,1)", "0");
  approximates_to<double>("geompdf(1,0)", "undef");
  approximates_to<double>("geomcdf(2,0.5)", "0.75");
  approximates_to<double>("geomcdfrange(2,3,0.5)", "0.375");
  approximates_to<double>("geomcdfrange(2,2,0.5)", "0.25");
  approximates_to<double>("invgeom(1,1)", "1");
  approximates_to<double>("invgeom(0.825,0.5)", "3");

  approximates_to<double>("hgeompdf(-1,2,1,1)", "0");
  approximates_to<double>("hgeompdf(0,2,1,1)", "0.5");
  approximates_to<double>("hgeompdf(1,2,1,1)", "0.5");
  approximates_to<double>("hgeompdf(2,2,1,1)", "0");
  approximates_to<double>("hgeompdf(3,2,1,1)", "0");
  approximates_to<double>("hgeompdf(0,2,1,2)", "0");
  approximates_to<double>("hgeompdf(1,2,1,2)", "1");
  approximates_to<double>("hgeompdf(2,2,1,2)", "0");
  approximates_to<double>("hgeompdf(0,42,0,42)", "1");
  approximates_to<double>("hgeompdf(24,42,24,42)", "1");
  approximates_to<double>("hgeomcdf(1,4,2,3)", "0.5");
  approximates_to<double>("hgeomcdf(24,42,24,34)", "1");
  approximates_to<double>("hgeomcdfrange(2,3,6,3,4)", "0.8");
  approximates_to<double>("hgeomcdfrange(13,15,40,20,30)", "0.60937014162821");
  approximates_to<double>("invhgeom(.5,4,2,3)", "1");
  approximates_to<double>("invhgeom(.6,40,20,30)", "15");
  approximates_to<double>("invhgeom(-1,4,2,3)", "undef");
  approximates_to<double>("invhgeom(0,4,2,2)", "undef");
  approximates_to<double>("invhgeom(0,4,2,3)", "0");
  approximates_to<double>("invhgeom(1,4,2,3)", "2");

  approximates_to<double>("normcdf(5, 7, 0.3162)", "1.265256ᴇ-10", realCtx, 7);
  approximates_to<float>("normcdf(1.2, 3.4, 5.6)", "0.3472125");
  approximates_to<double>("normcdf(1.2, 3.4, 5.6)", "0.34721249841587");
  approximates_to<float>("normcdf(-1ᴇ99,3.4,5.6)", "0");
  approximates_to<float>("normcdf(1ᴇ99,3.4,5.6)", "1");
  approximates_to<float>("normcdf(-6,0,1)", "0");
  approximates_to<float>("normcdf(6,0,1)", "1");
  approximates_to<float>("normcdfrange(0.5, 3.6, 1.3, 3.4)", "0.3436388");
  approximates_to<double>("normcdfrange(0.5, 3.6, 1.3, 3.4)",
                          "0.34363881299147");
  approximates_to<float>("normpdf(1.2, 3.4, 5.6)", "0.06594901");
  approximates_to<float>("invnorm(0.56, 1.3, 2.4)", "1.662326");
  // FIXME precision error
  // approximates_to<double>("invnorm(0.56, 1.3, 2.4)", "1.6623258450088");

  approximates_to<float>("poissonpdf(2,1)", "0.1839397");
  approximates_to<double>("poissonpdf(2,1)", "0.18393972058572");
  approximates_to<float>("poissonpdf(2,2)", "0.2706706");
  approximates_to<float>("poissoncdf(2,2)", "0.6766764");
  approximates_to<double>("poissoncdf(2,2)", "0.67667641618306");

  approximates_to<float>("tpdf(1.2, 3.4)", "0.1706051");
  approximates_to<double>("tpdf(1.2, 3.4)", "0.17060506917323");
  approximates_to<float>("tcdf(0.5, 2)", "0.6666667");
  approximates_to<float>("tcdf(1.2, 3.4)", "0.8464878");
  // FIXME: precision problem
  approximates_to<double>("tcdf(1.2, 3.4)", "0.8464877995", realCtx, 10);
  approximates_to<float>("invt(0.8464878,3.4)", "1.2");
  approximates_to<double>("invt(0.84648779949601043,3.4)", "1.2", realCtx, 10);
}

QUIZ_CASE(pcj_approximation_decimal) {
  approximates_to<float>("-0", "0");
  approximates_to<float>("-0.1", "-0.1");
  approximates_to<float>("-1.", "-1");
  approximates_to<float>("-.1", "-0.1");
  approximates_to<float>("-0ᴇ2", "0");
  approximates_to<float>("-0.1ᴇ2", "-10");
  approximates_to<float>("-1.ᴇ2", "-100");
  approximates_to<float>("-.1ᴇ2", "-10");
  approximates_to<float>("-0ᴇ-2", "0");
  approximates_to<float>("-0.1ᴇ-2", "-0.001");
  approximates_to<float>("-1.ᴇ-2", "-0.01");
  approximates_to<float>("-.1ᴇ-2", "-0.001");
  approximates_to<float>("-.003", "-0.003");
  approximates_to<float>("1.2343ᴇ-2", "0.012343");
  approximates_to<double>("-567.2ᴇ2", "-56720");

  approximates_to(283495231345_e, 283495231345.f);
  approximates_to(283495231345._e, 283495231345.f);
  approximates_to(283495231345.0_e, 283495231345.f);
  approximates_to(283495231345.00_e, 283495231345.f);

  approximates_to(0.028349523100_e, 0.0283495231f);
  approximates_to(0.28349523100_e, 0.283495231f);
  approximates_to(2.8349523100_e, 2.83495231f);
  approximates_to(283495231.00_e, 283495231.f);
  approximates_to(2834952310.0_e, 2834952310.f);
  approximates_to(28349523100_e, 28349523100.f);
}

QUIZ_CASE(pcj_approximation_based_integer) {
  approximates_to<float>("1232", "1232");
  approximates_to<double>("0b110101", "53");
  approximates_to<double>("0xabc1234", "180097588");
}

QUIZ_CASE(pcj_approximation_random) {
  /* All random nodes need to be at least projected so they can be seeded.
   * Randint of list also needs to be simplified to bubble up the list */
  simplified_approximates_to<double>("randint({1,1})", "{1,1}");
  simplified_approximates_to<double>("randint({1,2},{1,2})", "{1,2}");
  projected_approximates_to<double>("1/randint(2,2)+1/2", "1");
  projected_approximates_to<double>("randint(45,4)", "undef");
  projected_approximates_to<double>("randint(1, inf)", "undef");
  projected_approximates_to<double>("randint(-inf, 3)", "undef");
  projected_approximates_to<double>("randint(4, 3)", "undef");
#if 0  // TODO_PCJ: limit randint bounds
  projected_approximates_to<double>("randint(2, 23345678909876545678)",
                                    "undef");
#endif

  projected_approximates_to<double>("randintnorep(10,1,3)", "undef");
  projected_approximates_to<double>("randintnorep(1,10,100)", "undef");
  projected_approximates_to<double>("randintnorep(1,10,-1)", "undef");
  projected_approximates_to<double>("randintnorep(1,10,0)", "{}");
  projected_approximates_to<double>("randintnorep(536427840,-2145711360,4)",
                                    "undef");

  // Random lists can be sorted
  projected_approximates_to<float>("sort(randintnorep(1,4,4))", "{1,2,3,4}");
  projected_approximates_to<double>("sort(randintnorep(5,8,4))", "{5,6,7,8}");

  /* The simplification process should understand that the expression is not a
   * scalar if it encounters a randintnorep. */
  simplified_approximates_to<double>("rem(randintnorep(1,10,5),1)",
                                     "{0,0,0,0,0}");
}

QUIZ_CASE(pcj_approximation_function) {
  approximates_to<float>("abs(-1)", "1");
  approximates_to<double>("abs(-1)", "1");
  approximates_to<double>("abs(-2.3ᴇ-39)", "2.3ᴇ-39");
  approximates_to<float>("abs(-2.3ᴇ-39)", "2.3ᴇ-39", realCtx, 5);
  approximates_to<float>("abs(3+2i)", "3.605551", cartesianCtx);
  approximates_to<double>("abs(3+2i)", "3.605551275464", cartesianCtx);

  approximates_to<float>("binomial(10, 4)", "210");
  approximates_to<double>("binomial(10, 4)", "210");
  approximates_to<float>("binomial(12, 3)", "220");
  approximates_to<double>("binomial(12, 3)", "220");
  approximates_to<float>("binomial(-4.6, 3)", "-28.336");
  approximates_to<double>("binomial(-4.6, 3)", "-28.336");
  approximates_to<float>("binomial(π, 3)", "1.280108");
  approximates_to<double>("binomial(π, 3)", "1.2801081307019");
  approximates_to<float>("binomial(7, 9)", "0");
  approximates_to<double>("binomial(7, 9)", "0");
  approximates_to<float>("binomial(-7, 9)", "-5005");
  approximates_to<double>("binomial(-7, 9)", "-5005");
  approximates_to<float>("binomial(13, 0)", "1");
  approximates_to<double>("binomial(13, 0)", "1");
  approximates_to<float>("binomial(10, -1)", "0");
  approximates_to<double>("binomial(10, -1)", "0");
  approximates_to<float>("binomial(-5, -10)", "0");
  approximates_to<double>("binomial(-5, -10)", "0");
  approximates_to<float>("binomial(10, 2.1)", "undef");
  approximates_to<double>("binomial(10, 2.1)", "undef");

  approximates_to<float>("permute(10, 4)", "5040");
  approximates_to<double>("permute(10, 4)", "5040");

  approximates_to<float>("re(2+i)", "2", cartesianCtx);
  approximates_to<double>("re(2+i)", "2", cartesianCtx);
  approximates_to<float>("im(2+3i)", "3", cartesianCtx);
  approximates_to<double>("im(2+3i)", "3", cartesianCtx);
  approximates_to<float>("conj(3+2×i)", "3-2×i", cartesianCtx);
  approximates_to<double>("conj(3+2×i)", "3-2×i", cartesianCtx);

  approximates_to<float>("ln(2)", "0.6931472");
  approximates_to<double>("ln(2)", "0.69314718055995");
  approximates_to<float>("log(2)", "0.30103");
  approximates_to<double>("log(2)", "0.30102999566398");

  approximates_to<float>("root(2,3)", "1.259921");
  approximates_to<double>("root(2,3)", "1.2599210498949");
  approximates_to<float>("√(2)", "1.414214");
  approximates_to<double>("√(2)", "1.4142135623731");
  approximates_to<float>("√(5ᴇ-37)", "7.071068ᴇ-19");
  approximates_to<double>("√(5ᴇ-79)", "7.0710678118655ᴇ-40");
  approximates_to<float>("√(-1)", "i", cartesianCtx);
  approximates_to<double>("√(-1)", "i", cartesianCtx);
  approximates_to<float>("√(i)", "0.7071068+0.7071068×i", cartesianCtx);
  approximates_to<double>("√(i)", "0.70710678118655+0.70710678118655×i",
                          cartesianCtx);
  approximates_to<float>("√(-1-i)", "0.4550898-1.098684×i", cartesianCtx);
  approximates_to<double>("√(-1-i)", "0.45508986056223-1.0986841134678×i",
                          cartesianCtx);
  approximates_to<float>("root(3+i, 3)", "1.459366+0.1571201×i", cartesianCtx);
  approximates_to<double>("root(3+i, 3)", "1.4593656008684+0.15712012294394×i",
                          cartesianCtx);
  approximates_to<float>("root(3, 3+i)", "1.382007-0.1524428×i", cartesianCtx);
  approximates_to<double>("root(3, 3+i)", "1.3820069623326-0.1524427794159×i",
                          cartesianCtx);
  approximates_to<float>("√(3+i)", "1.755317+0.2848488×i", cartesianCtx);
  approximates_to<double>("√(3+i)", "1.7553173018244+0.28484878459314×i",
                          cartesianCtx);
  approximates_to<float>("√(-1)", "i", cartesianCtx);
  approximates_to<double>("√(-1)", "i", cartesianCtx);
  approximates_to<float>("root(-1,3)", "0.5+0.8660254×i", cartesianCtx);
  approximates_to<double>("root(-1,3)", "0.5+0.86602540378444×i", cartesianCtx);
  approximates_to<float>("root(5^((-i)3^9),i)", "3.504", cartesianCtx, 4);
  approximates_to<double>("root(5^((-i)3^9),i)", "3.5039410843", cartesianCtx,
                          11);

  approximates_to<float>("sign(-23+1)", "-1");
  approximates_to<float>("sign(inf)", "1");
  approximates_to<float>("sign(-inf)", "-1");
  approximates_to<float>("sign(0)", "0");
  approximates_to<float>("sign(-0)", "0");
  approximates_to<float>("sign(x)", "undef");
  approximates_to<double>("sign(2+i)", "undef");
  approximates_to<double>("sign(undef)", "undef");

  approximates_to<float>("sum(r,r, 4, 10)", "49");
  approximates_to<double>("sum(k,k, 4, 10)", "49");
  approximates_to<double>("sum(2+n×i,n,1,5)", "10+15×i", cartesianCtx);
  approximates_to<double>("sum(2+n×i,n,1,5)", "10+15×i", cartesianCtx);
  approximates_to<float>("product(n,n, 4, 10)", "604800");
  approximates_to<double>("product(n,n, 4, 10)", "604800");
  approximates_to<float>("product(2+k×i,k, 1, 5)", "-100-540×i", cartesianCtx);
  approximates_to<double>("product(2+o×i,o, 1, 5)", "-100-540×i", cartesianCtx);
}

QUIZ_CASE(pcj_approximation_context) {
  PoincareTest::SymbolStore symbolStore;
  ProjectionContext cartesianCtx = {
      .m_complexFormat = ComplexFormat::Cartesian,
      .m_context = symbolStore,
  };
  ProjectionContext realCtx = {
      .m_complexFormat = ComplexFormat::Real,
      .m_context = symbolStore,
  };

  PoincareTest::store("2x+5→f(x)", symbolStore);
  PoincareTest::store("π+1→a", symbolStore);
  PoincareTest::store("[[4]]→b", symbolStore);
  approximates_to<float>("a", "4.141593", cartesianCtx);
  approximates_to<float>("f(a)", "13.28319", cartesianCtx);
  approximates_to<float>("f(a+i)", "13.28319+2×i", cartesianCtx);
  approximates_to<float>("z", "undef", cartesianCtx);
  approximates_to<float>("b*[[5]]", "[[20]]", cartesianCtx);

  PoincareTest::store("x>0→g(x)", symbolStore);
  PoincareTest::store("true→t", symbolStore);
  approximates_to_boolean("g(2)", true, cartesianCtx);
  approximates_to_boolean("g(1/0)", Approximation::BooleanOrUndefined::Undef(),
                          cartesianCtx);
  approximates_to_boolean("t", true, cartesianCtx);

  PoincareTest::store("[[x,0][0,x]]→h(x)", symbolStore);
  approximates_to<float>("h(3)", "[[3,0][0,3]]", cartesianCtx);
  approximates_to<float>("h(1/0)", "undef", cartesianCtx);

  symbolStore.reset();
  // f : x→ x^2
  PoincareTest::store("x^2→f(x)", symbolStore);
  // Approximate f(x-2) with x = 5
  PoincareTest::store("5→x", symbolStore);
  approximates_to<double>("f(x-2)", 9.0, cartesianCtx);
  // Approximate f(x-1)+f(x+1) with x = 3
  PoincareTest::store("3→x", symbolStore);
  approximates_to<double>("f(x-1)+f(x+1)", 20.0, cartesianCtx);

  symbolStore.reset();
  // f : x → √(-1)×√(-1)
  PoincareTest::store("√(-1)×√(-1)→f(x)", symbolStore);
  // Approximate f(x) with x = 1
  PoincareTest::store("1→x", symbolStore);
  // Cartesian
  approximates_to<double>("f(x)", -1.0, cartesianCtx);
  // Real
  approximates_to<double>("f(x)", (double)NAN, realCtx);
}

QUIZ_CASE(pcj_approximation_floor_ceil_integer) {
  const char* expr1 = "floor(x * (x+1)^(-1) + x^2 * (x+1)^(-1))";
  const char* expr2 = "ceil(x * (x+1)^(-1) + x^2 * (x+1)^(-1))";
  Tree* t1 = parse(expr1);
  Tree* t2 = parse(expr2);
  Variables::ReplaceSymbol(t1, "x"_e, 0, ComplexSign::RealFinite());
  Variables::ReplaceSymbol(t2, "x"_e, 0, ComplexSign::RealFinite());
  Approximation::Context ctx;
  Approximation::Private::LocalContext localCtx =
      Approximation::Private::LocalContext(0.0, nullptr);
  ctx.m_localContext = &localCtx;
  constexpr double upperBound = 1000.0;
  for (double d = 0.0; d < upperBound; d += 1.0) {
    localCtx.setLocalValue(d);
    double result1 =
        Approximation::To<double>(t1, Approximation::Parameters{}, ctx);
    double result2 =
        Approximation::To<double>(t2, Approximation::Parameters{}, ctx);
    quiz_assert_print_if_failure(
        roughly_equal(result1, d, OMG::Float::EpsilonLax<double>(), true),
        expr1);
    quiz_assert_print_if_failure(
        roughly_equal(result2, d, OMG::Float::EpsilonLax<double>(), true),
        expr2);
  }
}

// Use projected trees
void assert_function_prepares_to(const Tree* input, const Tree* expected) {
  Tree* clone = input->cloneTree();
  Approximation::PrepareFunctionForApproximation(clone, "x",
                                                 ComplexFormat::Real);
  assert_trees_are_equal(clone, expected);
}

QUIZ_CASE(pcj_prepare_function) {
  assert_function_prepares_to(KAdd(2_e, 3_e, "x"_e), KAdd(5.0_de, KVarX));
  assert_function_prepares_to(KAdd(2_e, "a"_e, "x"_e), KUndef);
}

void assert_expression_prepares_to(const Tree* input, const Tree* expected) {
  Tree* clone = input->cloneTree();
  [[maybe_unused]] ProjectionContext ctx = {};
  assert(!Simplification::ToSystem(clone, &ctx));
  Approximation::Private::PrepareExpressionForApproximation(clone);
  assert_trees_are_equal(clone, expected);
}

QUIZ_CASE(pcj_prepare_expression) {
  constexpr uint8_t k_varId = Parametric::k_localVariableId + 1;
  assert_expression_prepares_to(
      KIntegral("t"_e, 0_e, KVarX, KMult(KVarX, KContinuousVar<k_varId>)),
      KIntegralWithAlternatives("t"_e, 0_e, KVarX,
                                KMult(KVarX, KContinuousVar<k_varId>),
                                KMult(KVarX, KContinuousVar<k_varId>),
                                KAdd(KMult(KVarX, KContinuousVar<k_varId>),
                                     KPow(KContinuousVar<k_varId>, 2_e))));
}
