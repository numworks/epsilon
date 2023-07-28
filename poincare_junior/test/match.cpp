#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;
using namespace Placeholders;

void assert_no_match(const Tree* source, const Tree* pattern) {
  PatternMatching::Context ctx;
  quiz_assert(!PatternMatching::Match(pattern, source, &ctx));
  quiz_assert(ctx.isUninitialized());
}

// TODO : Factorize more tests with assert_match_and_create
void assert_match_and_create(const Tree* source, const Tree* pattern,
                             const Tree* structure, const Tree* output) {
  int numberOfTrees = SharedEditionPool->numberOfTrees();
  PatternMatching::Context ctx;
  quiz_assert(PatternMatching::Match(pattern, source, &ctx));
  // Also test with an already matching context
  quiz_assert(PatternMatching::Match(pattern, source, &ctx));

  EditionReference createdRef = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(createdRef, output);
  createdRef->removeTree();
  // Also test with matchAndReplace
  EditionReference replacedSourceClone =
      EditionReference(SharedEditionPool->clone(source));
  MatchAndReplace(replacedSourceClone, pattern, structure);
  assert_trees_are_equal(replacedSourceClone, output);
  replacedSourceClone->removeTree();
  // Nothing has leaked
  quiz_assert(numberOfTrees == SharedEditionPool->numberOfTrees());
}

QUIZ_CASE(pcj_context) {
  PatternMatching::Context ctx;
  ctx.setNode(Placeholder::A, KAdd(2_e, 1_e), 1, false);
  const Tree* structure =
      KMult(5_e, KAdd(KPlaceholder<A>(), KPlaceholder<A>()));
  EditionReference exp = PatternMatching::Create(structure, ctx);
  assert_trees_are_equal(exp, KMult(5_e, KAdd(2_e, 1_e, 2_e, 1_e)));
}

QUIZ_CASE(pcj_match) {
  const Tree* t = KAdd(2_e, 1_e);
  PatternMatching::Context ctx;
  quiz_assert(PatternMatching::Match(KPlaceholder<A>(), t, &ctx));
  assert_trees_are_equal(ctx.getNode(Placeholder::A), t);
  PatternMatching::Context ctx2;
  quiz_assert(PatternMatching::Match(KAdd(KPlaceholder<A>(), 1_e), t, &ctx2));
  assert_trees_are_equal(ctx2.getNode(Placeholder::A), 2_e);
  PatternMatching::Context ctx3;
  quiz_assert(!PatternMatching::Match(KAdd(KPlaceholder<A>(), 2_e), t, &ctx3));
  quiz_assert(ctx3.isUninitialized());

  const Tree* t2 = KAdd(1_e, 1_e, 2_e);
  const Tree* p = KAdd(KPlaceholder<A>(), KPlaceholder<A>(), KPlaceholder<B>());
  PatternMatching::Context ctx4;
  quiz_assert(PatternMatching::Match(p, t2, &ctx4));
  assert_trees_are_equal(ctx4.getNode(Placeholder::A), 1_e);
  assert_trees_are_equal(ctx4.getNode(Placeholder::B), 2_e);

  PatternMatching::Context ctx5;
  const Tree* n5 = KExp(KMult(KFact(1_e)));
  quiz_assert(
      PatternMatching::Match(KExp(KMult(KAnyTreesPlaceholder<A>(), KFact(1_e),
                                        KAnyTreesPlaceholder<C>())),
                             n5, &ctx5));
  PatternMatching::Context ctx6;
  const Tree* n6 = EditionReference(KMult(1_e, KAdd(1_e, KMult(1_e, 2_e))));
  quiz_assert(PatternMatching::Match(
      KMult(1_e, KAdd(1_e, KMult(1_e, 2_e, KAnyTreesPlaceholder<A>()))), n6,
      &ctx6));
  PatternMatching::Context ctx7;
  quiz_assert(PatternMatching::Match(
      KMult(1_e, KAdd(1_e, KMult(1_e, 2_e), KAnyTreesPlaceholder<A>())), n6,
      &ctx7));
  PatternMatching::Context ctx8;
  quiz_assert(PatternMatching::Match(
      KMult(1_e, KAdd(1_e, KMult(1_e, 2_e)), KAnyTreesPlaceholder<A>()), n6,
      &ctx8));
}

QUIZ_CASE(pcj_rewrite_replace) {
  const Tree* p = KAdd(KPlaceholder<A>(), KPlaceholder<A>());
  const Tree* s = KMult(2_e, KPlaceholder<A>());
  EditionReference ref(SharedEditionPool->push<BlockType::Addition>(2));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  EditionReference result = PatternMatching::MatchAndCreate(ref, p, s);
  assert_trees_are_equal(result, KMult(2_e, 5_e));
  MatchAndReplace(ref, p, s);
  assert_trees_are_equal(result, ref);
  result = PatternMatching::MatchAndCreate(ref, p, s);
  quiz_assert(result.isUninitialized());
}

QUIZ_CASE(pcj_match_n_ary) {
  assert_no_match(
      KMult(KAdd(1_e, 2_e, 3_e), KAdd(1_e, 2_e)),
      KMult(KAdd(KAnyTreesPlaceholder<A>()), KAdd(KAnyTreesPlaceholder<A>())));

  assert_match_and_create(KAdd(1_e),
                          KAdd(KPlaceholder<A>(), KAnyTreesPlaceholder<B>()),
                          KAdd(KAnyTreesPlaceholder<B>()), 0_e);

  assert_no_match(KAdd(1_e, 2_e, 3_e, 4_e),
                  KAdd(KAnyTreesPlaceholder<A>(), 3_e, KPlaceholder<B>(), 4_e));

  assert_match_and_create(KAdd(1_e, 2_e), KPlaceholder<A>(),
                          KLn(KPlaceholder<A>()), KLn(KAdd(1_e, 2_e)));

  assert_match_and_create(
      KMult(KAdd(1_e, 2_e, 3_e), KAdd(1_e, 2_e)),
      KMult(KAdd(KPlaceholder<A>(), KAnyTreesPlaceholder<B>()),
            KPlaceholder<C>()),
      KAdd(KMult(KPlaceholder<A>(), KPlaceholder<C>()),
           KMult(KAdd(KAnyTreesPlaceholder<B>()), KPlaceholder<C>())),
      KAdd(KMult(1_e, KAdd(1_e, 2_e)), KMult(KAdd(2_e, 3_e), KAdd(1_e, 2_e))));

  assert_match_and_create(
      KAdd(1_e, 2_e, 3_e),
      KAdd(KAnyTreesPlaceholder<A>(), KPlaceholder<B>(), 3_e,
           KAnyTreesPlaceholder<C>()),
      KAdd(KAnyTreesPlaceholder<A>(), 0_e, KPlaceholder<B>(), 0_e,
           KAnyTreesPlaceholder<C>(), 0_e),
      KAdd(1_e, 0_e, 2_e, 0_e, 0_e));

  assert_match_and_create(
      KAdd(1_e, 2_e, 3_e, KMult(2_e, 3_e), 3_e),
      KAdd(KAnyTreesPlaceholder<A>(), KAnyTreesPlaceholder<B>(),
           KMult(KAnyTreesPlaceholder<C>(), KAnyTreesPlaceholder<B>()),
           KAnyTreesPlaceholder<B>()),
      KAdd(KAnyTreesPlaceholder<A>(), 0_e, KAnyTreesPlaceholder<B>(), 0_e,
           KAnyTreesPlaceholder<C>(), 0_e),
      KAdd(1_e, 2_e, 0_e, 3_e, 0_e, 2_e, 0_e));

  assert_match_and_create(
      KMult(KAdd(1_e, 2_e, 3_e), 4_e, KAdd(2_e, 3_e)),
      KMult(KAdd(KAnyTreesPlaceholder<A>(), KAnyTreesPlaceholder<B>(),
                 KAnyTreesPlaceholder<C>()),
            KAnyTreesPlaceholder<D>(), KAdd(KAnyTreesPlaceholder<B>())),
      KAdd(KAnyTreesPlaceholder<D>(), KAnyTreesPlaceholder<A>(),
           KAnyTreesPlaceholder<C>()),
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
