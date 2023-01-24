#include "print.h"
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/tree_constructor.h>
#include <quiz.h>

using namespace PoincareJ;

void assert_nodes_are_equal(const Node node0, const Node node1) {
  quiz_assert(Comparison::Compare(node0, node1) == 0);
}

void testPlaceholders() {
  using enum PatternMatching::Placeholder;
  constexpr Tree a = A;
  (void) Add("2"_n, a);
  (void) Add("2"_n, Tree(A));
}
QUIZ_CASE(pcj_placeholders) { testPlaceholders(); }

void testContext() {
  using enum PatternMatching::Placeholder;
  PatternMatching::Context ctx;
  constexpr Tree t = Add("2"_n, "1"_n);
  ctx[A] = t;
  constexpr Tree structure = Mult("5"_n, Add(Tree(A), Tree(A)));
  EditionReference exp = PatternMatching::Create(structure, ctx);
  assert_nodes_are_equal(exp, Mult("5"_n, Add(Add("2"_n, "1"_n), Add("2"_n, "1"_n))));
}
QUIZ_CASE(pcj_context) { testContext(); }

void testMatch() {
  using enum PatternMatching::Placeholder;
  constexpr Tree t = Add("2"_n, "1"_n);
  constexpr Tree p = A;
  PatternMatching::Context ctx = PatternMatching::Match(p, t);
  assert_nodes_are_equal(ctx[A], t);
  constexpr Tree p2 = Add(p, "1"_n);
  PatternMatching::Context ctx2 = PatternMatching::Match(p2, t);
  assert_nodes_are_equal(ctx2[A], "2"_n);
}
QUIZ_CASE(pcj_match) { testMatch(); }

void testRewrite() {
  using enum PatternMatching::Placeholder;
  constexpr Tree p = Add(Tree(A), Tree(A));
  constexpr Tree s = Mult("2"_n, Tree(A));
  EditionReference ref = EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference result = ref.matchAndRewrite(p, s);
  assert_nodes_are_equal(result, Mult("2"_n, "5"_n));
}
QUIZ_CASE(pcj_rewrite) { testRewrite(); }
