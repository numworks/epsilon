#include <apps/global_preferences.h>
#include <apps/shared/global_context.h>
#include <float.h>
#include <omg/float.h>
#include <poincare/sign.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/projection.h>

#include <cmath>

#include "apps/shared/poincare_helpers.h"
#include "helper.h"

using namespace Poincare::Internal;

constexpr ProjectionContext cartesianCtx = {.m_complexFormat =
                                                ComplexFormat::Cartesian};
constexpr ProjectionContext realCtx = {.m_complexFormat = ComplexFormat::Real};
constexpr ProjectionContext degreeCtx = {.m_angleUnit = AngleUnit::Degree};
constexpr ProjectionContext gradianCtx = {.m_angleUnit = AngleUnit::Gradian};

void approximates_to_boolean(const Tree* n,
                             Approximation::BooleanOrUndefined expected) {
  Approximation::BooleanOrUndefined approx = Approximation::ToBoolean<float>(
      n,
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(AngleUnit::Radian, ComplexFormat::Real));
  quiz_assert(approx.isUndefined() == expected.isUndefined());
  if (!approx.isUndefined()) {
    quiz_assert(approx.value() == expected.value());
  }
}

void approximates_to_boolean(const char* input,
                             Approximation::BooleanOrUndefined expected,
                             const ProjectionContext& projectionContext) {
  Tree* expression = parse(input, projectionContext.m_context);
  Approximation::BooleanOrUndefined approx = Approximation::ToBoolean<float>(
      expression,
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(projectionContext.m_angleUnit,
                             projectionContext.m_complexFormat,
                             projectionContext.m_context));
  quiz_assert(approx.isUndefined() == expected.isUndefined());
  if (!approx.isUndefined()) {
    quiz_assert(approx.value() == expected.value());
  }
  expression->removeTree();
}

template <typename T>
void approximates_to(const Tree* n, T f) {
  T approx = Approximation::To<T>(
      n,
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(AngleUnit::Radian, ComplexFormat::Real));
  bool result =
      OMG::Float::RoughlyEqual<T>(approx, f, OMG::Float::EpsilonLax<T>(), true);
#if POINCARE_TREE_LOG
  if (!result) {
    std::cout << "Approximation test failure with: \n";
    n->log();
    std::cout << "Approximated to " << approx << " instead of " << f << "\n";
    std::cout << "Absolute difference is : " << std::fabs(approx - f) << "\n";
    std::cout << "Relative difference is : " << std::fabs((approx - f) / f)
              << "\n";
  }
#endif
  quiz_assert(result);
}

template <typename T>
void approximates_to(const char* input, T f,
                     const ProjectionContext& projectionContext = realCtx) {
  Tree* expression = parse(input, projectionContext.m_context);
  T approx = Approximation::To<T>(
      expression,
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(projectionContext.m_angleUnit,
                             projectionContext.m_complexFormat,
                             projectionContext.m_context));
  bool result =
      OMG::Float::RoughlyEqual<T>(approx, f, OMG::Float::EpsilonLax<T>(), true);
#if POINCARE_TREE_LOG
  if (!result) {
    std::cout << "Approximation test failure with: " << input << "\n";
    std::cout << "Approximated to " << approx << " instead of " << f << "\n";
    std::cout << "Absolute difference is : " << std::fabs(approx - f) << "\n";
    std::cout << "Relative difference is : " << std::fabs((approx - f) / f)
              << "\n";
  }
#endif
  quiz_assert(result);
  expression->removeTree();
}

template <typename T>
void approximates_to(const char* input, const char* output,
                     const ProjectionContext& projectionContext = realCtx) {
  // TODO: use same test and log as approximates_to?
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        tree->moveTreeOverTree(Approximation::ToTree<T>(
            tree,
            Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                      .projectLocalVariables = true},
            Approximation::Context(projectionContext.m_angleUnit,
                                   projectionContext.m_complexFormat,
                                   projectionContext.m_context)));
        Beautification::DeepBeautify(tree, projectionContext);
      },
      projectionContext);
}

template <typename T>
void simplified_approximates_to(
    const char* input, const char* output,
    const ProjectionContext& projectionContext = realCtx) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        simplify(tree, projectionContext, false);
        tree->moveTreeOverTree(Approximation::ToTree<T>(
            tree,
            Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                      .projectLocalVariables = true},
            Approximation::Context(projectionContext.m_angleUnit,
                                   projectionContext.m_complexFormat,
                                   projectionContext.m_context)));
        Beautification::DeepBeautify(tree, projectionContext);
      },
      projectionContext);
}

template <typename T>
void projected_approximates_to(
    const char* input, const char* output,
    const ProjectionContext& projectionContext = realCtx) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        Simplification::ToSystem(tree, &projectionContext);
        tree->moveTreeOverTree(Approximation::ToTree<T>(
            tree,
            Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                      .prepare = true},
            Approximation::Context(projectionContext.m_angleUnit,
                                   projectionContext.m_complexFormat,
                                   projectionContext.m_context)));
        Beautification::DeepBeautify(tree, projectionContext);
      },
      projectionContext);
}

QUIZ_CASE(pcj_approximation_can_approximate) {
  quiz_assert(Approximation::CanApproximate(KAdd(2_e, 3_e)));
  quiz_assert(!Approximation::CanApproximate(KAdd(2_e, "x"_e)));
  quiz_assert(!Approximation::CanApproximate(KAdd(KVarX, 3_e)));
  quiz_assert(
      Approximation::CanApproximate(KSum("k"_e, 2_e, 8_e, KAdd(KVarK, 3_e))));
  quiz_assert(!Approximation::CanApproximate(
      KSum("k"_e, 2_e, 8_e, KAdd(KVarK, KVar<1, 0, 0>, 3_e))));
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
  /* TODO: this tests fails because of negligible real or imaginary parts.
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
  // TODO: should be a pure imaginary
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
  approximates_to<float>("sort({True,False,True})", "{False,True,True}");
  approximates_to<float>("sort({1,3,4,2})", "{1,2,3,4}");
  approximates_to<float>("sort({(3,2),(1,4),(2,0),(1,1)})",
                         "{(1,1),(1,4),(2,0),(3,2)}");
  approximates_to<float>("sequence(k<=1, k, 2)", "{True,False}");
  approximates_to<float>("sequence(1/(k-2)<=3, k, 5)",
                         "{True,undef,True,True,True}");
  approximates_to<float>("sequence(1/(k-2)=3, k, 5)",
                         "{False,undef,False,False,False}");
  // TODO_PCJ: approximates_to<float>("sort(randintnorep(1,4,4))", "{1,2,3,4}");

  approximates_to<float>("{1,2,3,4}(-5,1)", "undef");
  approximates_to<float>("{1,2,3,4}(0,2)", "{1,2}");

  Shared::GlobalContext globalContext;
  store("{1,2,3,4,5}→L", &globalContext);
  approximates_to<float>("L(1)", "1", {.m_context = &globalContext});
  approximates_to<float>("L(0)", "undef", {.m_context = &globalContext});
  approximates_to<float>("L(7)", "undef", {.m_context = &globalContext});
  approximates_to<double>("L(1)", "1", {.m_context = &globalContext});
  approximates_to<double>("L(0)", "undef", {.m_context = &globalContext});
  approximates_to<double>("L(7)", "undef", {.m_context = &globalContext});
  approximates_to<float>("L(1,3)", "{1,2,3}", {.m_context = &globalContext});
  approximates_to<float>("L(1,9)", "{1,2,3,4,5}",
                         {.m_context = &globalContext});
  approximates_to<float>("L(-5,3)", "undef", {.m_context = &globalContext});
  approximates_to<float>("L(3,1)", "{}", {.m_context = &globalContext});
  approximates_to<float>("L(8,9)", "{}", {.m_context = &globalContext});
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
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
  approximates_to<double>("cos(-2×i)", "1.0006092967033",
                          {.m_angleUnit = AngleUnit::Degree,
                           .m_complexFormat = ComplexFormat::Cartesian});
  approximates_to<double>("cos(-2×i)", "1.0004935208085",
                          {.m_angleUnit = AngleUnit::Gradian,
                           .m_complexFormat = ComplexFormat::Cartesian});
  // Symmetry: even
  approximates_to<double>("cos(2×i)", "3.7621956910836", cartesianCtx);
  approximates_to<double>("cos(2×i)", "1.0006092967033",
                          {.m_angleUnit = AngleUnit::Degree,
                           .m_complexFormat = ComplexFormat::Cartesian});
  approximates_to<double>("cos(2×i)", "1.0004935208085",
                          {.m_angleUnit = AngleUnit::Gradian,
                           .m_complexFormat = ComplexFormat::Cartesian});

  // Advanced function : sec
  approximates_to<double>("sec(2)", "-2.4029979617224");
  approximates_to<float>("sec(200)", "-1", gradianCtx);
  approximates_to<float>("sec(3×i)", "0.9986307",
                         {.m_angleUnit = AngleUnit::Degree,
                          .m_complexFormat = ComplexFormat::Cartesian});
  approximates_to<float>("sec(-3×i)", "0.9986307",
                         {.m_angleUnit = AngleUnit::Degree,
                          .m_complexFormat = ComplexFormat::Cartesian});

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
  approximates_to<float>("sin(3×i)", "0.05238381×i",
                         {.m_angleUnit = AngleUnit::Degree,
                          .m_complexFormat = ComplexFormat::Cartesian});
  approximates_to<double>("sin(3×i)", "0.047141332771113×i",
                          {.m_angleUnit = AngleUnit::Gradian,
                           .m_complexFormat = ComplexFormat::Cartesian});
  // Symmetry: odd
  approximates_to<double>("sin(-3×i)", "-10.01787492741×i", cartesianCtx);
  approximates_to<float>("sin(-3×i)", "-0.05238381×i",
                         {.m_angleUnit = AngleUnit::Degree,
                          .m_complexFormat = ComplexFormat::Cartesian});
  approximates_to<double>("sin(-3×i)", "-0.047141332771113×i",
                          {.m_angleUnit = AngleUnit::Gradian,
                           .m_complexFormat = ComplexFormat::Cartesian});

  // Advanced function : csc
  approximates_to<double>("csc(2)", "1.0997501702946");
  approximates_to<float>("csc(100)", "1", gradianCtx);
  approximates_to<float>("csc(3×i)", "-19.08987×i",
                         {.m_angleUnit = AngleUnit::Degree,
                          .m_complexFormat = ComplexFormat::Cartesian});
  approximates_to<float>("csc(-3×i)", "19.08987×i",
                         {.m_angleUnit = AngleUnit::Degree,
                          .m_complexFormat = ComplexFormat::Cartesian});
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
  assert(
      Poincare::Preferences::SharedPreferences()->mixedFractionsAreEnabled());
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
}

QUIZ_CASE(pcj_approximation_derivatives) {
  approximates_to<float>("diff(ln(x), x, -1)", "undef");
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

QUIZ_CASE(pcj_approximation_function) {
  approximates_to<float>("abs(-1)", "1");
  approximates_to<double>("abs(-1)", "1");
  approximates_to<double>("abs(-2.3ᴇ-39)", "2.3ᴇ-39");
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

  approximates_to<float>("invbinom(0.9647324002, 15, 0.7)", "13");
  approximates_to<double>("invbinom(0.9647324002, 15, 0.7)", "13");
  approximates_to<float>("invbinom(0.95,100,0.42)", "50");
  approximates_to<double>("invbinom(0.95,100,0.42)", "50");
  approximates_to<float>("invbinom(0.01,150,0.9)", "126");
  approximates_to<double>("invbinom(0.01,150,0.9)", "126");

  approximates_to<float>("invnorm(0.56, 1.3, 2.4)", "1.662326");
  // approximates_to<double>("invnorm(0.56, 1.3, 2.4)",
  // "1.6623258450088"); FIXME precision error

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

QUIZ_CASE(pcj_approximation_with_context) {
  Shared::GlobalContext globalContext;
  store("2x+5→f(x)", &globalContext);
  store("π+1→a", &globalContext);
  store("[[4]]→b", &globalContext);
  ProjectionContext ctx = {
      .m_complexFormat = ComplexFormat::Cartesian,
      .m_context = &globalContext,
  };

  approximates_to<float>("a", "4.141593", ctx);
  approximates_to<float>("f(a)", "13.28319", ctx);
  approximates_to<float>("f(a+i)", "13.28319+2×i", ctx);
  approximates_to<float>("z", "undef", ctx);
  approximates_to<float>("b*[[5]]", "[[20]]", ctx);

  store("x>0→g(x)", &globalContext);
  store("true→t", &globalContext);
  approximates_to_boolean("g(2)", true, ctx);
  approximates_to_boolean("g(1/0)", Approximation::BooleanOrUndefined::Undef(),
                          ctx);
  approximates_to_boolean("t", true, ctx);

  store("[[x,0][0,x]]→h(x)", &globalContext);
  approximates_to<float>("h(3)", "[[3,0][0,3]]", ctx);
  approximates_to<float>("h(1/0)", "undef", ctx);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
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
