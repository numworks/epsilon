#include <float.h>
#include <omg/float.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/projection.h>

#include <cmath>

#include "helper.h"

using namespace Poincare::Internal;

constexpr ProjectionContext cartesianCtx = {.m_complexFormat =
                                                ComplexFormat::Cartesian};
constexpr ProjectionContext realCtx = {.m_complexFormat = ComplexFormat::Real};

template <typename T>
void approximates_to(const Tree* n, T f) {
  T approx = Approximation::RootTreeToReal<T>(n);
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
void approximates_to(const char* input, const char* output,
                     const ProjectionContext& projectionContext = realCtx) {
  // TODO: use same test and log as approximates_to?
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        tree->moveTreeOverTree(Approximation::ToTree<T>(
            tree, Approximation::Parameter(true, true, false, false),
            Approximation::Context(projectionContext.m_angleUnit,
                                   projectionContext.m_complexFormat)));
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

QUIZ_CASE(pcj_approximation) {
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
}

QUIZ_CASE(pcj_approximation_replace) {
  TreeRef ref1(KAdd(1_e, 2_e, 10.5_de));
  quiz_assert(Approximation::ApproximateAndReplaceEveryScalar(ref1));
  assert_trees_are_equal(ref1, 13.5_de);

  TreeRef ref2(
      KMult(2.0_de, KDiv("x"_e, KAdd(1_e, 2.0_de)), KAdd(1_e, 2_e, 10.5_de)));
  quiz_assert(Approximation::ApproximateAndReplaceEveryScalar(ref2));
  assert_trees_are_equal(ref2, KMult(2.0_de, KDiv("x"_e, 3.0_de), 13.5_de));
}

QUIZ_CASE(pcj_approximation_power) {
  approximates_to<float>("0^0", "undef");
  approximates_to<float>("0^0.5", "0");
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
}

QUIZ_CASE(pcj_approximation_list) {
  approximates_to<float>("{1,2,3,4}(-5,1)", "undef");
  approximates_to<float>("{1,2,3,4}(0,2)", "{1,2}");
  approximates_to<float>("sort({True,False,True})", "{False,True,True}");
  approximates_to<float>("sort({1,3,4,2})", "{1,2,3,4}");
  approximates_to<float>("sort({(3,2),(1,4),(2,0),(1,1)})",
                         "{(1,1),(1,4),(2,0),(3,2)}");
  // TODO_PCJ: approximates_to<float>("sort(randintnorep(1,4,4))", "{1,2,3,4}");
}

QUIZ_CASE(pcj_approximation_matrix) {
  approximates_to<float>("trace([[1,2][4,3]])", "4");
  approximates_to<float>("identity(3)", "[[1,0,0][0,1,0][0,0,1]]");
  approximates_to<float>("[[1,2][4,3]]+[[1,2][4,3]]*2*[[1,2][4,3]]",
                         "[[19,18][36,37]]");
  approximates_to<float>("([[0]]^(2π))^2×0×[[0]]^(2π)", "undef");
  approximates_to<float>("[[0]]^(2π)", "undef");
  approximates_to<float>("[[1,2]]^2", "undef");
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
  approximates_to<float>("arccot(0)", "90", {.m_angleUnit = AngleUnit::Degree});
  approximates_to<float>("arccot(0.5)", "1.107149");
  approximates_to<float>("arccot(-0.5)", "2.034444");
  approximates_to<float>("arcsec(0)", "undef");
  approximates_to<float>("arccsc(0)", "undef");
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
}

QUIZ_CASE(pcj_approximation_derivatives) {
  approximates_to<float>("diff(ln(x), x, -1)", "undef");
}

QUIZ_CASE(pcj_approximation_decimal) {
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
