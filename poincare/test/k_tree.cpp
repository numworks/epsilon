#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>

#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_k_tree) {
  constexpr KTree tree = 3_e;

  const Tree* node = KMult(KAdd(5_e, 8_e, 4_e), 3_e, tree);
  quiz_assert(node->numberOfChildren() == 3);
  quiz_assert(node->numberOfDescendants(true) == 7);

  quiz_assert(("x"_e)->nodeSize() == 4);
  quiz_assert(("var"_e)->nodeSize() == 6);

  const Tree* poly = KPol(Exponents<2, 3>(), "x"_e, 2_e, "a"_e);
  quiz_assert(poly->numberOfChildren() == 3);
  quiz_assert(poly->nodeSize() == 4);
  quiz_assert(poly->treeSize() == 13);

  (void)KPol(Exponents<1>(), "x"_e, 2_e);

  quiz_assert(Approximation::RootTreeToReal<float>(0.125_fe) == 0.125);
  quiz_assert(Approximation::RootTreeToReal<float>(-2.5_fe) == -2.5);

  const Tree* rational = -3_e / 8_e;
  quiz_assert(rational->isRational());
  quiz_assert(Approximation::RootTreeToReal<float>(rational) == -0.375);
}

QUIZ_CASE(pcj_k_tree_integer) {
  quiz_assert((1_e)->nodeSize() == 1);
  quiz_assert((12_e)->nodeSize() == 2);
  quiz_assert((1234_e)->nodeSize() == 4);
  quiz_assert((-12345_e)->nodeSize() == 4);
  quiz_assert((123456_e)->nodeSize() == 5);
  quiz_assert((-123456_e)->nodeSize() == 5);
  quiz_assert((123456789_e)->nodeSize() == 6);
  quiz_assert((-123456789_e)->nodeSize() == 6);

  quiz_assert(Integer::Handler(1_e).to<double>() == 1.0);
  quiz_assert(Integer::Handler(12_e).to<double>() == 12.0);
  quiz_assert(Integer::Handler(-12_e).to<double>() == -12.0);
  quiz_assert(Integer::Handler(1234_e).to<double>() == 1234.0);
  quiz_assert(Integer::Handler(-1234_e).to<double>() == -1234.0);
  quiz_assert(Integer::Handler(123456_e).to<double>() == 123456.0);
  quiz_assert(Integer::Handler(-123456_e).to<double>() == -123456.0);
  quiz_assert(Integer::Handler(123456789_e).to<double>() == 123456789.0);
  quiz_assert(Integer::Handler(-123456789_e).to<double>() == -123456789.0);
}

QUIZ_CASE(pcj_k_tree_ternary) {
  /* Ternaries are guessing a common type for both expressions which led to
   * issues with an IntegerLiteral and a KTree, the deduction guide of the
   * literal was bypassed. */

  // KTree vs IntegerLiteral<U>
  quiz_assert((true ? i_e : 1_e)->treeIsIdenticalTo(i_e));
  quiz_assert((false ? 1_e : i_e)->treeIsIdenticalTo(i_e));
  quiz_assert((true ? 1_e : i_e)->treeIsIdenticalTo(1_e));
  quiz_assert((false ? i_e : 1_e)->treeIsIdenticalTo(1_e));

  // IntegerLiteral<U> vs IntegerLiteral<V>
  quiz_assert((true ? 2_e : 1_e)->treeIsIdenticalTo(2_e));
  quiz_assert((false ? 1_e : 2_e)->treeIsIdenticalTo(2_e));
  quiz_assert((true ? 1_e : 2_e)->treeIsIdenticalTo(1_e));
  quiz_assert((false ? 2_e : 1_e)->treeIsIdenticalTo(1_e));

  // IntegerLiteral<U> vs RationalLiteral<N, D>
  quiz_assert((true ? 2_e : (1_e / 3_e))->treeIsIdenticalTo(2_e));
  quiz_assert((false ? (1_e / 3_e) : 2_e)->treeIsIdenticalTo(2_e));
  quiz_assert((true ? (1_e / 3_e) : 2_e)->treeIsIdenticalTo((1_e / 3_e)));
  quiz_assert((false ? 2_e : (1_e / 3_e))->treeIsIdenticalTo((1_e / 3_e)));
}

QUIZ_CASE(pcj_k_codepoints) {
  quiz_assert("a"_cl->type() == Type::AsciiCodePointLayout);
  quiz_assert("π"_cl->type() == Type::UnicodeCodePointLayout);
  quiz_assert("aπc"_l->treeSize() == 3 + 2 + 5 + 2);
}

QUIZ_CASE(pcj_k_rack) {
  quiz_assert(KCodePointL<'a'>()->treeIsIdenticalTo("a"_cl));
  quiz_assert(KRackL("a"_cl, "b"_cl)->treeIsIdenticalTo("ab"_l));
  quiz_assert(("a"_l ^ "b"_l)->treeIsIdenticalTo("ab"_l));
  quiz_assert(("a"_cl ^ "b"_l)->treeIsIdenticalTo("ab"_l));
  quiz_assert(("a"_l ^ "b"_cl)->treeIsIdenticalTo("ab"_l));
  quiz_assert(("a"_cl ^ "b"_cl)->treeIsIdenticalTo("ab"_l));
}
