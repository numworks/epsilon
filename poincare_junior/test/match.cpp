#include "print.h"
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/tree_constructor.h>
#include <quiz.h>

using namespace PoincareJ;

void testPlaceholders() {
  using namespace PatternMatching::Placeholders;
  constexpr CTree a = A;
  (void) Addi(2_n, a, A);
}
QUIZ_CASE(pcj_placeholders) { testPlaceholders(); }

void testContext() {
  using namespace PatternMatching::Placeholders;
  PatternMatching::Context ctx;
  ctx[A] = Addi(2_n, 1_n);
  Node structure = Multi(5_n, Addi(A, A));
  EditionReference exp = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(exp, Multi(5_n, Addi(Addi(2_n, 1_n), Addi(2_n, 1_n))));
}
QUIZ_CASE(pcj_context) { testContext(); }

void testMatch() {
  using namespace PatternMatching::Placeholders;
  Node t = Addi(2_n, 1_n);
  PatternMatching::Context ctx = PatternMatching::Match(A, t);
  assert_trees_are_equal(ctx[A], t);
  PatternMatching::Context ctx2 = PatternMatching::Match(Addi(A, 1_n), t);
  assert_trees_are_equal(ctx2[A], 2_n);
  PatternMatching::Context ctx3 = PatternMatching::Match(Addi(A, 2_n), t);
  quiz_assert(ctx3.isUninitialized());
}
QUIZ_CASE(pcj_match) { testMatch(); }

void testRewrite() {
  using namespace PatternMatching::Placeholders;
  Node p = Addi(A, A);
  Node s = Multi(2_n, A);
  EditionReference ref = EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference result = ref.matchAndRewrite(p, s);
  assert_trees_are_equal(result, Multi(2_n, 5_n));
}
QUIZ_CASE(pcj_rewrite) { testRewrite(); }
