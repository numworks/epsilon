#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/placeholder.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_context) {
  PatternMatching::Context ctx;
  ctx[Placeholder::NodeToTag(A_e)] = KAdd(2_e, 1_e);
  Node structure = KMult(5_e, KAdd(A_e, A_e));
  EditionReference exp = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(exp, KMult(5_e, KAdd(KAdd(2_e, 1_e), KAdd(2_e, 1_e))));
}

QUIZ_CASE(pcj_match) {
  Node t = KAdd(2_e, 1_e);
  PatternMatching::Context ctx = PatternMatching::Match(A_e, t);
  assert_trees_are_equal(ctx[Placeholder::NodeToTag(A_e)], t);
  PatternMatching::Context ctx2 = PatternMatching::Match(KAdd(A_e, 1_e), t);
  assert_trees_are_equal(ctx2[Placeholder::NodeToTag(A_e)], 2_e);
  PatternMatching::Context ctx3 = PatternMatching::Match(KAdd(A_e, 2_e), t);
  quiz_assert(ctx3.isUninitialized());

  Node t2 = KAdd(1_e, 1_e, 2_e);
  Node p = KAdd(A_e, A_e, B_e);
  PatternMatching::Context ctx4 = PatternMatching::Match(p, t2);
  assert_trees_are_equal(ctx4[Placeholder::NodeToTag(A_e)], 1_e);
  assert_trees_are_equal(ctx4[Placeholder::NodeToTag(B_e)], 2_e);
}

QUIZ_CASE(pcj_rewrite_replace) {
  EditionPool* editionPool = EditionPool::sharedEditionPool();
  Node p = KAdd(A_e, A_e);
  Node s = KMult(2_e, A_e);
  EditionReference ref(editionPool->push<BlockType::Addition>(2));
  editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference result = ref.matchAndRewrite(p, s);
  assert_trees_are_equal(result, KMult(2_e, 5_e));
  ref.matchAndReplace(p, s);
  assert_trees_are_equal(result, ref);
}
