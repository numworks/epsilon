#include "helper.h"
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <quiz.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_placeholders) {
  using namespace PatternMatching::Placeholders;
  constexpr Tree a = A;
  (void) KAdd(2_e, a, A);
}

QUIZ_CASE(pcj_context) {
  using namespace PatternMatching::Placeholders;
  PatternMatching::Context ctx;
  ctx[A] = KAdd(2_e, 1_e);
  Node structure = KMult(5_e, KAdd(A, A));
  EditionReference exp = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(exp, KMult(5_e, KAdd(KAdd(2_e, 1_e), KAdd(2_e, 1_e))));
}

QUIZ_CASE(pcj_match) {
  using namespace PatternMatching::Placeholders;
  Node t = KAdd(2_e, 1_e);
  PatternMatching::Context ctx = PatternMatching::Match(A, t);
  assert_trees_are_equal(ctx[A], t);
  PatternMatching::Context ctx2 = PatternMatching::Match(KAdd(A, 1_e), t);
  assert_trees_are_equal(ctx2[A], 2_e);
  PatternMatching::Context ctx3 = PatternMatching::Match(KAdd(A, 2_e), t);
  quiz_assert(ctx3.isUninitialized());
}

QUIZ_CASE(pcj_rewrite) {
  using namespace PatternMatching::Placeholders;
  Node p = KAdd(A, A);
  Node s = KMult(2_e, A);
  EditionReference ref = EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference result = ref.matchAndRewrite(p, s);
  assert_trees_are_equal(result, KMult(2_e, 5_e));
}
