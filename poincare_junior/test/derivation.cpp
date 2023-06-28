#include <poincare_junior/src/expression/derivation.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/simplification.h>

#include "helper.h"

using namespace PoincareJ;

void assert_derivation_inplace_is(const Node *expression,
                                  const Node *expected) {
  EditionReference ref(expression);
  Derivation::Reduce(&ref);
  assert_trees_are_equal(ref, expected);
  ref.removeTree();
}

void assert_derivation_is(const Node *expression, const Node *expected,
                          const Node *symbol = nullptr,
                          const Node *symbolValue = nullptr) {
  if (!symbol) {
    symbol = "x"_e;
  }
  if (!symbolValue) {
    symbolValue = "y"_e;
  }
  Node *result =
      Node::FromBlocks(EditionPool::sharedEditionPool()->lastBlock());
  Derivation::Derivate(expression, symbol, symbolValue);
  EditionReference simplifiedResult(result);
  Simplification::Simplify(&simplifiedResult);
  quiz_assert(expected->treeIsIdenticalTo(simplifiedResult));
  simplifiedResult.removeTree();
}

QUIZ_CASE(pcj_derivation) {
  assert_derivation_inplace_is(KDiff("x"_e, "x"_e, 2_e), 1_e);
  assert_derivation_inplace_is(KDiff(23_e, "x"_e, 1_e), 0_e);

  assert_derivation_is(KAdd(1_e, "x"_e), 1_e);
  assert_derivation_is(KTrig(KLn("x"_e), 1_e),
                       KMult(KCos(KLn("y"_e)), KPow("y"_e, -1_e)));

  // TODO: Improve these results by improving simplification
  assert_derivation_is(
      KMult(KPow("x"_e, 4_e), KLn("x"_e), KExp(KMult(3_e, "x"_e))),
      KAdd(
          KMult(3_e, KPow(e_e, KMult(3_e, "y"_e)), KLn("y"_e),
                KPow("y"_e, 4_e)),
          KMult(4_e, KPow(e_e, KAdd(KMult(3_e, "y"_e), KMult(3_e, KLn("y"_e)))),
                KLn("y"_e)),
          KMult(KPow(e_e, KMult(3_e, "y"_e)), KPow("y"_e, 3_e))));
  assert_derivation_is(KPow(KDiff(KPow("x"_e, 2_e), "x"_e, "x"_e), 2_e),
                       KMult(8_e, "y"_e));
}
