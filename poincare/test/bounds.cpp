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

void assert_reduced_bounds_sign_is(const char* treeToParse, Sign expectedSign) {
  assert_bounds_sign_is(parse_and_reduce(treeToParse), expectedSign);
}

QUIZ_CASE(pcj_bounds_sign) {
  /* This test sometimes uses a KTrees to ensure reduction does not trivialize
   * the input tree */
  assert_bounds_sign_is(0_e, Sign::Zero());
  assert_bounds_sign_is(1_e, Sign::FiniteStrictlyPositive());
  assert_reduced_bounds_sign_is("π-1/2", Sign::FiniteStrictlyPositive());
  assert_reduced_bounds_sign_is("ln(2)-π/3", Sign::FiniteStrictlyNegative());
  assert_reduced_bounds_sign_is("2^e-2π", Sign::FiniteStrictlyPositive());
  assert_bounds_sign_is(KAbs(-1_e), Sign::FiniteStrictlyPositive());

  // Trig
  assert_reduced_bounds_sign_is("sin(1)", Sign::FiniteStrictlyPositive());
  assert_reduced_bounds_sign_is("cos(2)", Sign::FiniteStrictlyNegative());
  assert_bounds_sign_is(KTrig(π_e, 0_e), Sign::Finite());
  assert_bounds_sign_is(KAbs(KTrig(π_e, 0_e)), Sign::FinitePositive());

  // Unkown
  assert_bounds_sign_is(i_e, Sign::Unknown());
  assert_reduced_bounds_sign_is("1+i", Sign::Unknown());
  assert_reduced_bounds_sign_is("{1}", Sign::Unknown());
  assert_bounds_sign_is(KLn(KPow(KLn(2_e), 7000_e)), Sign::Unknown());
}
