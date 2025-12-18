#include <apps/global_preferences.h>
#include <poincare/src/expression/k_tree.h>

#include <cmath>

#include "../helpers/symbol_store.h"
#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

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
  approximates_to(KAdd(KMult(2_e, 4_e), KPow(1.5_fe, 3.0_fe)), 11.375f);
  approximates_to(KDiv(KSub(2_e, 4_e), 10.0_fe), -0.2f);
  approximates_to(KTrig(KDiv(π_e, 2_e), 1_e), 1.f);
  approximates_to(KLogBase(KMult(13_e, 13_e), 13_e), 2.f);
  approximates_to(KExp(2_e), M_E * M_E);
  approximates_to(KLog(100_e), 2.f);
  approximates_to(KLn(e_e), 1.f);
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
  approximates_to<float>("piecewise(True,1<0,False,3=4)", "undef");
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
  approximates_to<double>("(undef,i)", "(undef,nonreal)", k_realCtx);
  approximates_to<double>("(undef,i)", "(undef,i)", k_cartesianCtx);
}

QUIZ_CASE(pcj_approximation_piecewise) {
  approximates_to<float>("piecewise(3,1<0,2)", "2");
  approximates_to<float>("piecewise(3,1>0,2)", "3");
  approximates_to<float>("piecewise(3,0>1,4,0>2,5,0<6,2)", "5");
  approximates_to<float>("piecewise(3,0<1,4,0<2,5,0<6,2)", "3");
  approximates_to<float>("piecewise(3,1<0,2,3=4)", "undef");
  approximates_to<float>("piecewise([[1,2]],sin(3)<0,[[2,1]])", "[[2,1]]");
}

QUIZ_CASE(pcj_approximation_logarithm) {
  approximates_to<float>("ln(2)", "0.6931472");
  approximates_to<double>("ln(2)", "0.69314718055995");
  approximates_to<float>("log(2)", "0.30103");
  approximates_to<double>("log(2)", "0.30102999566398");
  approximates_to<float>("log(2,64)", "0.1666667");
  approximates_to<double>("log(6,7)", "0.9207822211616");
  approximates_to<float>("log(5)", "0.69897");
  approximates_to<double>("ln(5)", "1.6094379124341");
  simplified_approximates_to<float>("ln(10^2025)", "4662.735");

  approximates_to<float>("log(2+5×i,64)", "0.4048317+0.2862042×i",
                         k_cartesianCtx);
  approximates_to<double>("log(6,7+4×i)", "0.80843880717528-0.20108238082167×i",
                          k_cartesianCtx);
  approximates_to<float>("log(5+2×i)", "0.731199+0.1652518×i", k_cartesianCtx);
  approximates_to<double>("ln(5+2×i)", "1.6836479149932+0.38050637711236×i",
                          k_cartesianCtx);
  projected_approximates_to<double>("log(0,0)", "undef");
  projected_approximates_to<double>("log(0)", "undef");
  projected_approximates_to<double>("log(2,0)", "undef");

  // WARNING: evaluate on branch cut can be multi-valued
  approximates_to<double>("ln(-4)", "1.3862943611199+3.1415926535898×i",
                          k_cartesianCtx);

  // Calculations involving a potential integer overflow
  simplified_approximates_to<float>("-8.314×ln(5/558)", "39.19985");
  simplified_approximates_to<float>("(1/((1/900)-(1/769)))×ln(5.2)",
                                    "-8710.202");
  simplified_approximates_to<float>("(-8.314×(5/6))×ln(5/6)", "1.263184");
}

QUIZ_CASE(pcj_approximation_abs) {
  approximates_to(KAbs(100_e), 100.f);
  approximates_to(KAbs(-2.31_fe), 2.31f);

  approximates_to<float>("abs(-1)", "1");
  approximates_to<double>("abs(-1)", "1");
  approximates_to<double>("abs(-2.3ᴇ-39)", "2.3ᴇ-39");
  approximates_to<float>("abs(-2.3ᴇ-39)", "2.3ᴇ-39", k_realCtx, 5);
  approximates_to<float>("abs(3+2i)", "3.605551", k_cartesianCtx);
  approximates_to<double>("abs(3+2i)", "3.605551275464", k_cartesianCtx);
}

QUIZ_CASE(pcj_approximation_root) {
  approximates_to<float>("root(2,3)", "1.259921");
  approximates_to<double>("root(2,3)", "1.2599210498949");
  approximates_to<float>("√(2)", "1.414214");
  approximates_to<double>("√(2)", "1.4142135623731");
  approximates_to<float>("√(5ᴇ-37)", "7.071068ᴇ-19");
  approximates_to<double>("√(5ᴇ-79)", "7.0710678118655ᴇ-40");
  approximates_to<float>("√(-1)", "i", k_cartesianCtx);
  approximates_to<double>("√(-1)", "i", k_cartesianCtx);
  approximates_to<float>("√(i)", "0.7071068+0.7071068×i", k_cartesianCtx);
  approximates_to<double>("√(i)", "0.70710678118655+0.70710678118655×i",
                          k_cartesianCtx);
  approximates_to<float>("√(-1-i)", "0.4550898-1.098684×i", k_cartesianCtx);
  approximates_to<double>("√(-1-i)", "0.45508986056223-1.0986841134678×i",
                          k_cartesianCtx);
  approximates_to<float>("root(3+i, 3)", "1.459366+0.1571201×i",
                         k_cartesianCtx);
  approximates_to<double>("root(3+i, 3)", "1.4593656008684+0.15712012294394×i",
                          k_cartesianCtx);
  approximates_to<float>("root(3, 3+i)", "1.382007-0.1524428×i",
                         k_cartesianCtx);
  approximates_to<double>("root(3, 3+i)", "1.3820069623326-0.1524427794159×i",
                          k_cartesianCtx);
  approximates_to<float>("√(3+i)", "1.755317+0.2848488×i", k_cartesianCtx);
  approximates_to<double>("√(3+i)", "1.7553173018244+0.28484878459314×i",
                          k_cartesianCtx);
  approximates_to<float>("√(-1)", "i", k_cartesianCtx);
  approximates_to<double>("√(-1)", "i", k_cartesianCtx);
  approximates_to<float>("root(-1,3)", "0.5+0.8660254×i", k_cartesianCtx);
  approximates_to<double>("root(-1,3)", "0.5+0.86602540378444×i",
                          k_cartesianCtx);
  approximates_to<float>("root(5^((-i)3^9),i)", "3.504", k_cartesianCtx, 4);
  approximates_to<double>("root(5^((-i)3^9),i)", "3.5039410843", k_cartesianCtx,
                          11);
}

QUIZ_CASE(pcj_approximation_mixed_fraction) {
  GlobalPreferences::SharedGlobalPreferences()->setCountry(I18n::Country::US);
  assert(Poincare::SharedPreferences->mixedFractionsAreEnabled());
  approximates_to<double>("1 1/2", 1.5);
  approximates_to<double>("-1 1/2", -1.5);
  GlobalPreferences::SharedGlobalPreferences()->setCountry(I18n::Country::WW);
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

QUIZ_CASE(pcj_approximation_sign) {
  approximates_to<float>("sign(-23+1)", "-1");
  approximates_to<float>("sign(inf)", "1");
  approximates_to<float>("sign(-inf)", "-1");
  approximates_to<float>("sign(0)", "0");
  approximates_to<float>("sign(-0)", "0");
  approximates_to<float>("sign(x)", "undef");
  approximates_to<double>("sign(2+i)", "undef");
  approximates_to<double>("sign(undef)", "undef");
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
                                          "3.287392846+3×x", k_degreeCtx, 10);
  approximates_to_keeping_symbols<double>(
      "cos(4/3+ln(x-1/2))", "cos(1.333333333+ln(x-0.5))", k_degreeCtx, 10);
  approximates_to_keeping_symbols<double>("ln(ln(ln(10+10)))+ln(ln(ln(x+10)))",
                                          "0.09275118142+ln(ln(ln(x+10)))",
                                          k_realCtx, 10);
  approximates_to_keeping_symbols<double>("int(x,x,0,2)+int(x,x,0,x)",
                                          "2+int(x,x,0,x)", k_realCtx, 10);
  approximates_to_keeping_symbols<double>(
      "[[x,cos(10)][1/2+x,cos(4/3+x)]]",
      "[[x,0.984807753][0.5+x,cos(1.333333333+x)]]", k_degreeCtx, 10);
  approximates_to_keeping_symbols<double>("{x,undef,cos(10)+x,cos(10)}",
                                          "{x,undef,0.984807753+x,0.984807753}",
                                          k_degreeCtx, 10);
  approximates_to_keeping_symbols<double>("cos(10)→x", "0.984807753→x",
                                          k_degreeCtx, 10);
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

// Use projected trees
void assert_function_prepares_to(const Tree* input, const Tree* expected) {
  Tree* clone = input->cloneTree();
  Approximation::PrepareFunctionForApproximation(clone, "x", true);
  assert_trees_are_equal(clone, expected);
}

QUIZ_CASE(pcj_prepare_function) {
  assert_function_prepares_to(KAdd(2_e, 3_e, "x"_e), KAdd(5.0_de, KVarX));
  assert_function_prepares_to(KAdd(2_e, "a"_e, "x"_e), KUndef);
}

void assert_expression_prepares_to(const Tree* input, const Tree* expected) {
  Tree* clone = input->cloneTree();
  [[maybe_unused]] ProjectionContext ctx = {};
  assert(!Simplification::ToSystem(clone, &ctx).changed);
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
  assert_expression_prepares_to(KIntegral("t"_e, 0_e, KRandomSeeded<1>, KVarX),
                                KIntegral("t"_e, 0_e, KRandomSeeded<1>, KVarX));
}

QUIZ_CASE(pcj_approximation_capped) {
#ifdef POINCARE_MAX_TREE_SIZE_FOR_APPROXIMATION
  const Tree* bigTree = KFloor->cloneNode();
  const Tree* almostBigTree = KFloor->cloneNode();
  for (int i = 2; i < POINCARE_MAX_TREE_SIZE_FOR_APPROXIMATION; i++) {
    KFloor->cloneNode();
  }
  (1_e)->cloneTree();

  quiz_assert(Approximation::ToComplex<float>(
                  almostBigTree, Approximation::Parameters()) == 1.f);
  quiz_assert(std::isnan(
      Approximation::ToComplex<float>(bigTree, Approximation::Parameters())
          .real()));
#endif
}
