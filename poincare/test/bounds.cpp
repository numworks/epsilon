#include <poincare/src/expression/bounds.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/properties.h>
#include <poincare/src/layout/k_tree.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

void assert_bounds_properties_are(const Tree* t,
                                  Properties expectedProperties) {
  Properties obtained = Bounds::Properties(t);
  bool assert = obtained == expectedProperties;
#if POINCARE_TREE_LOG
  if (!assert) {
    std::cout << "Bounds::Properties on: ";
    t->logBlocks();
    std::cout << "\tExpected: ";
    expectedProperties.log();
    std::cout << "\tGot: ";
    obtained.log();
  }
#endif
  quiz_assert(assert);
}

void assert_reduced_bounds_properties_are(const char* treeToParse,
                                          Properties expectedProperties) {
  assert_bounds_properties_are(parse_and_reduce(treeToParse),
                               expectedProperties);
}

QUIZ_CASE(pcj_bounds_properties) {
  /* This test sometimes uses KTrees to ensure reduction does not trivialize
   * the input tree */
  assert_bounds_properties_are(0_e, Properties::Zero());
  assert_bounds_properties_are(1_e, Properties::FiniteStrictlyPositive());
  assert_reduced_bounds_properties_are("π-1/2",
                                       Properties::FiniteStrictlyPositive());
  assert_reduced_bounds_properties_are("ln(2)-π/3",
                                       Properties::FiniteStrictlyNegative());
  assert_reduced_bounds_properties_are("2^e-2π",
                                       Properties::FiniteStrictlyPositive());
  assert_bounds_properties_are(KAbs(-1_e),
                               Properties::FiniteStrictlyPositive());

  // Trig
  assert_reduced_bounds_properties_are("sin(1)",
                                       Properties::FiniteStrictlyPositive());
  assert_reduced_bounds_properties_are("cos(2)",
                                       Properties::FiniteStrictlyNegative());
  assert_bounds_properties_are(KTrig(π_e, 0_e), Properties::Finite());
  assert_bounds_properties_are(KAbs(KTrig(π_e, 0_e)),
                               Properties::FinitePositive());

  // Mult
  assert_bounds_properties_are(KMult(KTrig(1000_e, 0_e), KTrig(1000_e, 1_e)),
                               Properties::Finite());

  // Unknown
  assert_bounds_properties_are(i_e, Properties::Unknown());
  assert_reduced_bounds_properties_are("1+i", Properties::Unknown());
  assert_reduced_bounds_properties_are("{1}", Properties::Unknown());
  assert_bounds_properties_are(KLn(KPow(KLn(2_e), 7000_e)),
                               Properties::Unknown());
  assert_bounds_properties_are(
      KMult(KExp(KMult(3000_e, KTrig(9000_e, 0_e))), 10_e),
      Properties::Unknown());
}
