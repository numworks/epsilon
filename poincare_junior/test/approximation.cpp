#include <float.h>
#include <omgpj/float.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/k_tree.h>

#include <cmath>

#include "helper.h"

using namespace PoincareJ;

void assert_approximation_is(const Tree* n, float f) {
  float approx = Approximation::RootTreeTo<float>(n);
  bool result = Float::RoughlyEqual<float>(approx, f, FLT_EPSILON, true);
#if POINCARE_MEMORY_TREE_LOG
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

QUIZ_CASE(pcj_approximation) {
  assert_approximation_is(123_e, 123.f);
  assert_approximation_is(-123.21_fe, -123.21f);
  assert_approximation_is(π_e, M_PI);
  assert_approximation_is(e_e, M_E);
  assert_approximation_is(KAdd(KMult(2_e, 4_e), KPow(1.5_fe, 3.0_fe)), 11.375f);
  assert_approximation_is(KDiv(KSub(2_e, 4_e), 10.0_fe), -0.2f);
  assert_approximation_is(KTrig(KDiv(π_e, 2_e), 1_e), 1.f);
  assert_approximation_is(KLogarithm(KMult(13_e, 13_e), 13_e), 2.f);
  assert_approximation_is(KExp(2_e), M_E * M_E);
  assert_approximation_is(KLog(100_e), 2.f);
  assert_approximation_is(KLn(e_e), 1.f);
  assert_approximation_is(KAbs(100_e), 100.f);
  assert_approximation_is(KAbs(-2.31_fe), 2.31f);
  assert_approximation_is(KCos(π_e), -1.f);
  assert_approximation_is(KSin(π_e), 0.f);
  assert_approximation_is(KTan(0_e), 0.f);
  assert_approximation_is(KPowReal(1_e, KDiv(1_e, 3_e)), 1.f);
  assert_approximation_is(KPowReal(-1_e, KDiv(1_e, 3_e)), NAN);
  assert_approximation_is(KPowReal(-1_e, 2_e), 1.f);
  assert_approximation_is("x"_e, NAN);
}

QUIZ_CASE(pcj_approximation_replace) {
  EditionReference ref1(KAdd(1_e, 2_e, 10.5_de));
  quiz_assert(Approximation::ApproximateAndReplaceEveryScalar(ref1));
  assert_trees_are_equal(ref1, 13.5_de);

  EditionReference ref2(
      KMult(2.0_de, KDiv("x"_e, KAdd(1_e, 2.0_de)), KAdd(1_e, 2_e, 10.5_de)));
  quiz_assert(Approximation::ApproximateAndReplaceEveryScalar(ref2));
  assert_trees_are_equal(ref2, KMult(2.0_de, KDiv("x"_e, 3.0_de), 13.5_de));
}
