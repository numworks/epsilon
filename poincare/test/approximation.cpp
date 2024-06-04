#include <float.h>
#include <omg/float.h>
#include <poincare/src/expression/approximation.h>
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
  bool result = OMG::Float::RoughlyEqual<T>(approx, f, FLT_EPSILON, true);
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
                     ProjectionContext projectionContext = realCtx) {
  // TODO: use same test and log as approximates_to?
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        tree->moveTreeOverTree(Approximation::RootTreeToTree<T>(
            tree, projectionContext.m_angleUnit,
            projectionContext.m_complexFormat));
      },
      projectionContext);
}

QUIZ_CASE(pcj_approximation) {
  approximates_to(123_e, 123.f);
  approximates_to(-123.21_fe, -123.21f);
  approximates_to(π_e, M_PI);
  approximates_to(e_e, M_E);
  approximates_to(KAdd(KMult(2_e, 4_e), KPow(1.5_fe, 3.0_fe)), 11.375f);
  approximates_to(KDiv(KSub(2_e, 4_e), 10.0_fe), -0.2f);
  approximates_to(KTrig(KDiv(π_e, 2_e), 1_e), 1.f);
  approximates_to(KLogarithm(KMult(13_e, 13_e), 13_e), 2.f);
  approximates_to(KExp(2_e), M_E * M_E);
  approximates_to(KLog(100_e), 2.f);
  approximates_to(KLn(e_e), 1.f);
  approximates_to(KAbs(100_e), 100.f);
  approximates_to(KAbs(-2.31_fe), 2.31f);
  approximates_to(KCos(π_e), -1.f);
  approximates_to(KSin(π_e), 0.f);
  approximates_to(KTan(0_e), 0.f);
  approximates_to(KPowReal(1_e, KDiv(1_e, 3_e)), 1.f);
  approximates_to(KPowReal(-1_e, KDiv(1_e, 3_e)), NAN);
  approximates_to(KPowReal(-1_e, 2_e), 1.f);
  approximates_to(KSum("k"_e, 1_e, 3_e, KVarK), 6.f);
  approximates_to("x"_e, NAN);
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
  approximates_to<float>("0^(3+4i)", "0");
  approximates_to<float>("0^(3-4i)", "0");
  approximates_to<float>("0^(-3+4i)", "undef");
  approximates_to<float>("0^(-3-4i)", "undef");
}

QUIZ_CASE(pcj_approximation_list) {
  approximates_to<float>("{1,2,3,4}(-5,1)", "undef");
  approximates_to<float>("{1,2,3,4}(0,2)", "{1,2}");
  // TODO_PCJ: approximates_to<float>("sort(randintnorep(1,4,4))", "{1,2,3,4}");
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
  // TODO_PCJ: approximates_to<float>("inf^0", "undef");
  // TODO_PCJ: approximates_to<float>("(-inf)^0", "undef");
  approximates_to<float>("inf^inf", "∞");
  approximates_to<float>("inf^(-inf)", "0");
  approximates_to<float>("(-inf)^inf", "undef");  // complex inf
  approximates_to<float>("(-inf)^(-inf)", "0");

  // functions
  approximates_to<float>("exp(inf)", "∞");
  approximates_to<float>("exp(-inf)", "0");
  approximates_to<float>("log(inf,0)", "undef");
  approximates_to<float>("log(-inf,0)", "undef");
  approximates_to<float>("log(inf,1)", "undef");
  approximates_to<float>("log(-inf,1)", "undef");
  approximates_to<float>("log(inf,0.3)", "-∞");
  approximates_to<float>("log(-inf,0.3)", "nonreal");
  approximates_to<float>("log(-inf,0.3)", "-∞-2.609355×i", cartesianCtx);
  approximates_to<float>("log(inf,3)", "∞");
  approximates_to<float>("log(-inf,3)", "nonreal");
  approximates_to<float>("log(-inf,3)", "∞+2.859601×i", cartesianCtx);
  approximates_to<float>("log(inf,-3)", "nonreal");
  approximates_to<float>("log(inf,-3)", "∞-∞×i", cartesianCtx);
  approximates_to<float>("log(0,inf)", "undef");
  approximates_to<float>("log(0,-inf)", "undef");
  approximates_to<float>("log(1,inf)", "0");
  approximates_to<float>("log(1,-inf)", "0");
  approximates_to<float>("log(0.3,inf)", "0");
  approximates_to<float>("log(0.3,-inf)", "0");
  approximates_to<float>("log(3,inf)", "0");
  approximates_to<float>("log(3,-inf)", "0");
  approximates_to<float>("log(inf,inf)", "undef");
  approximates_to<float>("log(-inf,inf)", "undef");
  approximates_to<float>("log(inf,-inf)", "undef");
  approximates_to<float>("log(-inf,-inf)", "undef");
  approximates_to<float>("ln(inf)", "∞");
  approximates_to<float>("ln(-inf)", "nonreal");
  approximates_to<float>("ln(-inf)", "∞+3.141593×i", cartesianCtx);
  approximates_to<float>("cos(inf)", "undef");
  approximates_to<float>("cos(-inf)", "undef");
  approximates_to<float>("sin(inf)", "undef");
  approximates_to<float>("sin(-inf)", "undef");
  approximates_to<float>("atan(inf)", "1.570796");
  approximates_to<float>("atan(-inf)", "-1.570796");
}
