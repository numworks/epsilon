#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;
using namespace Placeholders;

QUIZ_CASE(pcj_context) {
  PatternMatching::Context ctx;
  ctx.setNode(Placeholder::A, KAdd(2_e, 1_e));
  Node structure = KMult(5_e, KAdd(KPlaceholder<A>(), KPlaceholder<A>()));
  EditionReference exp = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(exp, KMult(5_e, KAdd(KAdd(2_e, 1_e), KAdd(2_e, 1_e))));
}

QUIZ_CASE(pcj_match) {
  Node t = KAdd(2_e, 1_e);
  PatternMatching::Context ctx = PatternMatching::Match(KPlaceholder<A>(), t);
  assert_trees_are_equal(ctx.getNode(Placeholder::A), t);
  PatternMatching::Context ctx2 =
      PatternMatching::Match(KAdd(KPlaceholder<A>(), 1_e), t);
  assert_trees_are_equal(ctx2.getNode(Placeholder::A), 2_e);
  PatternMatching::Context ctx3 =
      PatternMatching::Match(KAdd(KPlaceholder<A>(), 2_e), t);
  quiz_assert(ctx3.isUninitialized());

  Node t2 = KAdd(1_e, 1_e, 2_e);
  Node p = KAdd(KPlaceholder<A>(), KPlaceholder<A>(), KPlaceholder<B>());
  PatternMatching::Context ctx4 = PatternMatching::Match(p, t2);
  assert_trees_are_equal(ctx4.getNode(Placeholder::A), 1_e);
  assert_trees_are_equal(ctx4.getNode(Placeholder::B), 2_e);
}

QUIZ_CASE(pcj_rewrite_replace) {
  EditionPool* editionPool = EditionPool::sharedEditionPool();
  Node p = KAdd(KPlaceholder<A>(), KPlaceholder<A>());
  Node s = KMult(2_e, KPlaceholder<A>());
  EditionReference ref(editionPool->push<BlockType::Addition>(2));
  editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference result = ref.matchAndCreate(p, s);
  assert_trees_are_equal(result, KMult(2_e, 5_e));
  ref.matchAndReplace(p, s);
  assert_trees_are_equal(result, ref);
  result = ref.matchAndCreate(p, s);
  quiz_assert(result.isUninitialized());
}

QUIZ_CASE(pcj_match_n_ary) {
  Node source = KMult(KAdd(1_e, 2_e, 3_e), KAdd(4_e, 5_e));
  quiz_assert(PatternMatching::Match(KPlaceholder<B, FilterAddition>(), source)
                  .isUninitialized());
  quiz_assert(PatternMatching::Match(KMult(KPlaceholder<A, FilterAddition>(),
                                           KPlaceholder<A, FilterAddition>()),
                                     source)
                  .isUninitialized());
  Node pattern = KMult(KPlaceholder<A, FilterAddition>(), KPlaceholder<B>());
  PatternMatching::Context ctx = PatternMatching::Match(pattern, source);
  assert_trees_are_equal(ctx.getNode(Placeholder::A), KAdd(1_e, 2_e, 3_e));
  assert_trees_are_equal(ctx.getNode(Placeholder::B), KAdd(4_e, 5_e));

  Node structure =
      KAdd(KMult(KPlaceholder<A, FilterFirstChild>(), KPlaceholder<B>()),
           KMult(KPlaceholder<A, FilterNonFirstChild>(), KPlaceholder<B>()));
  EditionReference result = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(result, KAdd(KMult(1_e, KAdd(4_e, 5_e)),
                                      KMult(KAdd(2_e, 3_e), KAdd(4_e, 5_e))));
}
