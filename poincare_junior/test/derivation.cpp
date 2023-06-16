#include <poincare_junior/src/expression/derivation.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/simplification.h>

#include "helper.h"

using namespace PoincareJ;

void assertDerivationInplaceIs(Node expression, Node expected) {
  EditionReference ref(expression);
  Derivation::Reduce(&ref);
  assert_trees_are_equal(ref, expected);
  ref.removeTree();
}

void assertDerivationIs(Node expression, Node expected, Node symbol = Node(),
                        Node symbolValue = Node()) {
  if (symbol.isUninitialized()) {
    symbol = "x"_e;
  }
  if (symbolValue.isUninitialized()) {
    symbolValue = "y"_e;
  }
  Node result = EditionPool::sharedEditionPool()->lastBlock();
  Derivation::Derivate(expression, symbol, symbolValue);
  EditionReference simplifiedResult(result);
  Simplification::Simplify(&simplifiedResult);
  quiz_assert(expected.treeIsIdenticalTo(simplifiedResult));
  simplifiedResult.removeTree();
}

QUIZ_CASE(pcj_derivation) {
  assertDerivationInplaceIs(KDiff("x"_e, "x"_e, 2_e), 1_e);
  assertDerivationInplaceIs(KDiff(23_e, "x"_e, 1_e), 0_e);

  // TODO: Activate these tests once systemReduce is fully operational
#if 0
  assertDerivationIs(KAdd(1_e, "x"_e), 1_e);
  assertDerivationIs(
      KMult(KPow("x"_e, 4_e), KLn("x"_e), KExp(KMult(3_e, "x"_e))),
      KAdd(KMult(4_e, KPow("y"_e, 3_e), KExp(KMult(3_e, "y"_e))),
           KMult(KPow("y"_e, 3_e), KExp(KMult(3_e, "y"_e))),
           KMult(3_e, KExp(KMult(3_e, "y"_e)), KPow("y"_e, 4_e), KLn("y"_e))));
  assertDerivationIs(KTrig(KLn("x"_e), 1_e),
                     KMult(KPow("y"_e, -1_e), KTrig(KLn("y"_e), 0_e)));
#endif
}
