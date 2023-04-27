#include <float.h>
#include <omgpj/float.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/k_creator.h>

#include <cmath>

#include "helper.h"

using namespace PoincareJ;

void assert_approximation_is(Node n, float f) {
  float approx = Approximation::To<float>(n);
  bool result = Float::RoughlyEqual<float>(approx, f, FLT_EPSILON);
  if (!result) {
    std::cout << "Approximation test failure with: \n";
    n.log();
    std::cout << "Approximated to " << approx << " instead of " << f << "\n";
    std::cout << "Absolute difference is : " << std::fabs(approx - f) << "\n";
    std::cout << "Relative difference is : " << std::fabs((approx - f) / f)
              << "\n";
  }
  quiz_assert(result);
}

QUIZ_CASE(pcj_approximation) {
  assert_approximation_is(123_e, 123.f);
  assert_approximation_is(-123.21_e, -123.21f);
  assert_approximation_is(π_e, M_PI);
  assert_approximation_is(e_e, M_E);
  assert_approximation_is(KAdd(KMult(2_e, 4_e), KPow(1.5_e, 3.0_e)), 11.375f);
  assert_approximation_is(KDiv(KSub(2_e, 4_e), 10.0_e), -0.2f);
  assert_approximation_is(KTrig(KDiv(π_e, 2_e), 1_e), 1.f);
  assert_approximation_is(KLogarithm(KMult(13_e, 13_e), 13_e), 2.f);
  assert_approximation_is(KExp(2_e), M_E * M_E);
  assert_approximation_is(KLog(100_e), 2.f);
  assert_approximation_is(KLn(e_e), 1.f);
  assert_approximation_is(KCos(π_e), -1.f);
  assert_approximation_is(KSin(π_e), 0.f);
  assert_approximation_is(KTan(0_e), 0.f);

  quiz_assert(std::isnan(Approximation::To<float>("x"_e)));
}
