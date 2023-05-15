#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;
using namespace Placeholders;

void assert_no_match(Node source, Node pattern) {
  PatternMatching::Context ctx;
  quiz_assert(!PatternMatching::Match(pattern, source, &ctx));
  quiz_assert(ctx.isUninitialized());
}

// TODO : Factorize more tests with assert_match_and_create
void assert_match_and_create(Node source, Node pattern, Node structure,
                             Node output) {
  PatternMatching::Context ctx;
  quiz_assert(PatternMatching::Match(pattern, source, &ctx));
  // Also test with an already matching context
  quiz_assert(PatternMatching::Match(pattern, source, &ctx));

  EditionReference createdRef = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(createdRef, output);
  createdRef.removeTree();
  // Also test with matchAndReplace
  EditionReference replacedSourceClone =
      EditionReference(EditionPool::sharedEditionPool()->clone(source));
  replacedSourceClone.matchAndReplace(pattern, structure);
  assert_trees_are_equal(replacedSourceClone, output);
  replacedSourceClone.removeTree();
}

QUIZ_CASE(pcj_context) {
  PatternMatching::Context ctx;
  ctx.setNode(Placeholder::A, KAdd(2_e, 1_e), 1);
  Node structure = KMult(5_e, KAdd(KPlaceholder<A>(), KPlaceholder<A>()));
  EditionReference exp = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(exp, KMult(5_e, KAdd(2_e, 1_e, 2_e, 1_e)));
}

QUIZ_CASE(pcj_match) {
  Node t = KAdd(2_e, 1_e);
  PatternMatching::Context ctx;
  quiz_assert(PatternMatching::Match(KPlaceholder<A>(), t, &ctx));
  assert_trees_are_equal(ctx.getNode(Placeholder::A), t);
  PatternMatching::Context ctx2;
  quiz_assert(PatternMatching::Match(KAdd(KPlaceholder<A>(), 1_e), t, &ctx2));
  assert_trees_are_equal(ctx2.getNode(Placeholder::A), 2_e);
  PatternMatching::Context ctx3;
  quiz_assert(!PatternMatching::Match(KAdd(KPlaceholder<A>(), 2_e), t, &ctx3));
  quiz_assert(ctx3.isUninitialized());

  Node t2 = KAdd(1_e, 1_e, 2_e);
  Node p = KAdd(KPlaceholder<A>(), KPlaceholder<A>(), KPlaceholder<B>());
  PatternMatching::Context ctx4;
  quiz_assert(PatternMatching::Match(p, t2, &ctx4));
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
  assert_no_match(
      KMult(KAdd(1_e, 2_e, 3_e), KAdd(1_e, 2_e)),
      KMult(KAdd(KAnyTreesPlaceholder<A>()), KAdd(KPlaceholder<A>())));

  assert_no_match(KAdd(1_e, 2_e, 3_e, 4_e),
                  KAdd(KAnyTreesPlaceholder<A>(), 3_e, KPlaceholder<B>(), 4_e));

  assert_match_and_create(
      KMult(KAdd(1_e, 2_e, 3_e), KAdd(1_e, 2_e)),
      KMult(KAdd(KPlaceholder<A>(), KAnyTreesPlaceholder<B>()),
            KPlaceholder<C>()),
      KAdd(KMult(KPlaceholder<A>(), KPlaceholder<C>()),
           KMult(KAdd(KPlaceholder<B>()), KPlaceholder<C>())),
      KAdd(KMult(1_e, KAdd(1_e, 2_e)), KMult(KAdd(2_e, 3_e), KAdd(1_e, 2_e))));

  assert_match_and_create(KAdd(1_e, 2_e, 3_e),
                          KAdd(KAnyTreesPlaceholder<A>(), KPlaceholder<B>(),
                               3_e, KAnyTreesPlaceholder<C>()),
                          KAdd(KPlaceholder<A>(), 0_e, KPlaceholder<B>(), 0_e,
                               KPlaceholder<C>(), 0_e),
                          KAdd(1_e, 0_e, 2_e, 0_e, 0_e));

  assert_match_and_create(
      KAdd(1_e, 2_e, 3_e, KMult(2_e, 3_e), 3_e),
      KAdd(KAnyTreesPlaceholder<A>(), KAnyTreesPlaceholder<B>(),
           KMult(KAnyTreesPlaceholder<C>(), KPlaceholder<B>()),
           KPlaceholder<B>()),
      KAdd(KPlaceholder<A>(), 0_e, KPlaceholder<B>(), 0_e, KPlaceholder<C>(),
           0_e),
      KAdd(1_e, 2_e, 0_e, 3_e, 0_e, 2_e, 0_e));

  assert_match_and_create(
      KMult(KAdd(1_e, 2_e, 3_e), 4_e, KAdd(2_e, 3_e)),
      KMult(KAdd(KAnyTreesPlaceholder<A>(), KAnyTreesPlaceholder<B>(),
                 KAnyTreesPlaceholder<C>()),
            KAnyTreesPlaceholder<D>(), KAdd(KPlaceholder<B>())),
      KAdd(KAnyTreesPlaceholder<D>(), KPlaceholder<A>(), KPlaceholder<C>()),
      KAdd(4_e, 1_e));

  /* TODO: In this example we first try with 0 trees in A and 1 tree in B.
   *       Then, we perform a costly match and fail at the very end.
   *       We try again with 1 tree in A and 0 in B, and uselessly perform the
   *       exact same costly Match with no success.
   *       This should be optimized. */
  assert_no_match(
      KAdd(1_e, 1_e, 2_e, KMult(1_e, 2_e, 3_e, 3_e), 2_e),
      KAdd(KAnyTreesPlaceholder<A>(), 1_e, KAnyTreesPlaceholder<B>(), 2_e,
           KMult(KAnyTreesPlaceholder<C>(), KPlaceholder<D>(),
                 KAnyTreesPlaceholder<E>(), KPlaceholder<D>()),
           1_e));
}
