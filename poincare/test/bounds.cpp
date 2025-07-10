#include <poincare/src/expression/bounds.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/layout/k_tree.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

void assert_bounds_sign_is(const Tree* t, Sign expectedSign) {
  Sign obtained = Bounds::Sign(t);
  bool assert = obtained == expectedSign;
#if POINCARE_TREE_LOG
  if (!assert) {
    std::cout << "Bounds::Sign on: ";
    t->logBlocks();
    std::cout << "\tExpected: ";
    expectedSign.log();
    std::cout << "\tGot: ";
    obtained.log();
  }
#endif
  quiz_assert(assert);
}

void assert_bounds_sign_is(const char* treeToParse, Sign expectedSign) {
  assert_bounds_sign_is(parse_and_reduce(treeToParse), expectedSign);
}

QUIZ_CASE(pcj_bounds_sign) {
  assert_bounds_sign_is(0_e, Sign::Zero());
  assert_bounds_sign_is(1_e, Sign::FiniteStrictlyPositive());
  assert_bounds_sign_is("π-1/2", Sign::FiniteStrictlyPositive());
  assert_bounds_sign_is("ln(2)-π/3", Sign::FiniteStrictlyNegative());
  assert_bounds_sign_is("2^e-2π", Sign::FiniteStrictlyPositive());

  // Trig
  assert_bounds_sign_is(KTrig(1_e, 1_e), Sign::FiniteStrictlyPositive());
  assert_bounds_sign_is(KTrig(2_e, 0_e), Sign::FiniteStrictlyNegative());
  const Tree* cosPI = KTrig(KMult(1_e, π_e), 0_e);
  const Tree* expCosPI = KExp(KTrig(KMult(1_e, π_e), 0_e));
  assert_bounds_sign_is(cosPI, Sign::Unknown());
  assert_bounds_sign_is(expCosPI, Sign::FiniteStrictlyPositive());

  // Unkown
  assert_bounds_sign_is(i_e, Sign::Unknown());
  assert_bounds_sign_is("1+i", Sign::Unknown());
  assert_bounds_sign_is("{1}", Sign::Unknown());
  assert_bounds_sign_is(KAbs(-1_e), Sign::Unknown());
}
