#include "print.h"
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/tree_constructor.h>
#include <quiz.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_placeholders) {
  using namespace PatternMatching::Placeholders;
  constexpr CTree a = A;
  (void) Addi(2_e, a, A);
}

QUIZ_CASE(pcj_context) {
  using namespace PatternMatching::Placeholders;
  PatternMatching::Context ctx;
  ctx[A] = Addi(2_e, 1_e);
  Node structure = Multi(5_e, Addi(A, A));
  EditionReference exp = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(exp, Multi(5_e, Addi(Addi(2_e, 1_e), Addi(2_e, 1_e))));
}

QUIZ_CASE(pcj_match) {
  using namespace PatternMatching::Placeholders;
  Node t = Addi(2_e, 1_e);
  PatternMatching::Context ctx = PatternMatching::Match(A, t);
  assert_trees_are_equal(ctx[A], t);
  PatternMatching::Context ctx2 = PatternMatching::Match(Addi(A, 1_e), t);
  assert_trees_are_equal(ctx2[A], 2_e);
  PatternMatching::Context ctx3 = PatternMatching::Match(Addi(A, 2_e), t);
  quiz_assert(ctx3.isUninitialized());
}

QUIZ_CASE(pcj_rewrite) {
  using namespace PatternMatching::Placeholders;
  Node p = Addi(A, A);
  Node s = Multi(2_e, A);
  EditionReference ref = EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference result = ref.matchAndRewrite(p, s);
  assert_trees_are_equal(result, Multi(2_e, 5_e));
}
