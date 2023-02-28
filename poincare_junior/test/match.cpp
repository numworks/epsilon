#include "print.h"
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/expression/constructor.h>
#include <quiz.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_placeholders) {
  using namespace PatternMatching::Placeholders;
  constexpr Tree a = A;
  (void) Add(2_e, a, A);
}

QUIZ_CASE(pcj_context) {
  using namespace PatternMatching::Placeholders;
  PatternMatching::Context ctx;
  ctx[A] = Add(2_e, 1_e);
  Node structure = Mult(5_e, Add(A, A));
  EditionReference exp = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(exp, Mult(5_e, Add(Add(2_e, 1_e), Add(2_e, 1_e))));
}

QUIZ_CASE(pcj_match) {
  using namespace PatternMatching::Placeholders;
  Node t = Add(2_e, 1_e);
  PatternMatching::Context ctx = PatternMatching::Match(A, t);
  assert_trees_are_equal(ctx[A], t);
  PatternMatching::Context ctx2 = PatternMatching::Match(Add(A, 1_e), t);
  assert_trees_are_equal(ctx2[A], 2_e);
  PatternMatching::Context ctx3 = PatternMatching::Match(Add(A, 2_e), t);
  quiz_assert(ctx3.isUninitialized());
}

QUIZ_CASE(pcj_rewrite) {
  using namespace PatternMatching::Placeholders;
  Node p = Add(A, A);
  Node s = Mult(2_e, A);
  EditionReference ref = EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference result = ref.matchAndRewrite(p, s);
  assert_trees_are_equal(result, Mult(2_e, 5_e));
}
