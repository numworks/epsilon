#include "print.h"
#include <poincare_junior/src/memory/context.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <poincare_junior/src/memory/tree_constructor.h>
#include <quiz.h>

using namespace PoincareJ;

void testPlaceholders() {
  using enum Placeholder;
  constexpr Tree a = A;
  (void) Add("2"_n, a);
  (void) Add("2"_n, Tree(A));
}
QUIZ_CASE(pcj_placeholders) { testPlaceholders(); }

void testContext() {
  using enum Placeholder;
  Context ctx;
  constexpr Tree t = Add("2"_n, "1"_n);
  ctx[A] = t;
  constexpr Tree structure = Mult("5"_n, Add(Tree(A), Tree(A)));
  EditionReference exp = ctx.build(structure);
  exp.log();
}
QUIZ_CASE(pcj_context) { testContext(); }

void testMatch() {
  using enum Placeholder;
  constexpr Tree t = Add("2"_n, "1"_n);
  constexpr Tree p = A;
  Context ctx = Context::Match(p, t);
  ctx[A].log();
  constexpr Tree p2 = Add(p, "1"_n);
  Context ctx2 = Context::Match(p2, t);
  ctx2[A].log();
}
QUIZ_CASE(pcj_match) { testMatch(); }

void testRewrite() {
  using enum Placeholder;
  constexpr Tree p = Add(Tree(A), Tree(A));
  constexpr Tree s = Mult("2"_n, Tree(A));
  EditionReference ref = EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference result = ref.rewrite(p, s);

  constexpr Tree a = Tree(A);
  EditionReference result2 = ref.rewrite(Add(a, a), Mult("2"_n, a));
  result2.log();
}
QUIZ_CASE(pcj_rewrite) { testRewrite(); }
