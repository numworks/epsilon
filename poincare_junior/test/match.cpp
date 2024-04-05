#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;

void assert_no_match(const Tree* source, const Tree* pattern) {
  PatternMatching::Context ctx;
  quiz_assert(!PatternMatching::Match(pattern, source, &ctx));
  quiz_assert(ctx.isUninitialized());
}

// TODO: Factorize more tests with assert_match_and_create
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
  PatternMatching::MatchAndReplace(replacedSourceClone, pattern, structure);
  assert_trees_are_equal(replacedSourceClone, output);
  replacedSourceClone->removeTree();
  // Nothing has leaked
  quiz_assert(numberOfTrees == SharedEditionPool->numberOfTrees());
}

QUIZ_CASE(pcj_context) {
  EditionReference exp =
      PatternMatching::Create(KMult(5_e, KAdd(KA, KA)), {.KA = KAdd(2_e, 1_e)});
  assert_trees_are_equal(exp, KMult(5_e, KAdd(2_e, 1_e, 2_e, 1_e)));
}

QUIZ_CASE(pcj_match) {
  const Tree* t = KAdd(2_e, 1_e);
  PatternMatching::Context ctx;
  quiz_assert(PatternMatching::Match(KA, t, &ctx));
  assert_trees_are_equal(ctx.getNode(Placeholder::A), t);
  PatternMatching::Context ctx2;
  quiz_assert(PatternMatching::Match(KAdd(KA, 1_e), t, &ctx2));
  assert_trees_are_equal(ctx2.getNode(Placeholder::A), 2_e);
  PatternMatching::Context ctx3;
  quiz_assert(!PatternMatching::Match(KAdd(KA, 2_e), t, &ctx3));
  quiz_assert(ctx3.isUninitialized());
  quiz_assert(!PatternMatching::Match(KAdd(2_e, 1_e, 3_e), t, &ctx3));
  quiz_assert(ctx3.isUninitialized());

  const Tree* t2 = KAdd(1_e, 1_e, 2_e);
  const Tree* p = KAdd(KA, KA, KB);
  PatternMatching::Context ctx4;
  quiz_assert(PatternMatching::Match(p, t2, &ctx4));
  assert_trees_are_equal(ctx4.getNode(Placeholder::A), 1_e);
  assert_trees_are_equal(ctx4.getNode(Placeholder::B), 2_e);

  PatternMatching::Context ctx5;
  const Tree* n5 = KExp(KMult(KFact(1_e)));
  quiz_assert(
      PatternMatching::Match(KExp(KMult(KA_s, KFact(1_e), KC_s)), n5, &ctx5));
  PatternMatching::Context ctx6;
  const Tree* n6 = EditionReference(KMult(1_e, KAdd(1_e, KMult(1_e, 2_e))));
  quiz_assert(PatternMatching::Match(
      KMult(1_e, KAdd(1_e, KMult(1_e, 2_e, KA_s))), n6, &ctx6));
  PatternMatching::Context ctx7;
  quiz_assert(PatternMatching::Match(
      KMult(1_e, KAdd(1_e, KMult(1_e, 2_e), KA_s)), n6, &ctx7));
  PatternMatching::Context ctx8;
  quiz_assert(PatternMatching::Match(
      KMult(1_e, KAdd(1_e, KMult(1_e, 2_e)), KA_s), n6, &ctx8));

  PatternMatching::Context ctx9;
  const Tree* n9 = KAdd(KMult("z"_e, KIm("x"_e)), "y"_e);
  quiz_assert(PatternMatching::Match(KAdd(KMult("z"_e, KIm(KA), KB_s), KC_s),
                                     n9, &ctx9));
  assert_trees_are_equal(ctx9.getNode(Placeholder::A), "x"_e);
  quiz_assert(ctx9.getNumberOfTrees(Placeholder::B) == 0);
  quiz_assert(ctx9.getNumberOfTrees(Placeholder::C) == 1);
  assert_trees_are_equal(ctx9.getNode(Placeholder::C), "y"_e);

  PatternMatching::Context ctx10;
  const Tree* n10 = KAdd(KMult("z"_e, KIm("x"_e)), "y"_e);
  quiz_assert(
      PatternMatching::Match(KAdd(KMult("z"_e, KIm(KA)), KB_s), n10, &ctx10));
  assert_trees_are_equal(ctx10.getNode(Placeholder::A), "x"_e);
  quiz_assert(ctx10.getNumberOfTrees(Placeholder::B) == 1);
  assert_trees_are_equal(ctx10.getNode(Placeholder::B), "y"_e);

  PatternMatching::Context ctx11;
  const Tree* n11 = KATrig(KMult(KHalf, KExp(KMult(KHalf, KLn(3_e)))), 0_e);
  quiz_assert(PatternMatching::Match(n11, n11, &ctx11));

  PatternMatching::Context ctx12;
  quiz_assert(PatternMatching::Match(KMult(KA_s, "x"_e, KB_s), "x"_e, &ctx12));
  quiz_assert(ctx12.getNumberOfTrees(Placeholder::A) == 0);
  quiz_assert(ctx12.getNumberOfTrees(Placeholder::B) == 0);

  PatternMatching::Context ctx13;
  quiz_assert(PatternMatching::Match(KMult(KA_s, KB, KC_s), "x"_e, &ctx13));
  quiz_assert(ctx13.getNumberOfTrees(Placeholder::A) == 0);
  assert_trees_are_equal(ctx13.getNode(Placeholder::B), "x"_e);
  quiz_assert(ctx13.getNumberOfTrees(Placeholder::C) == 0);

  PatternMatching::Context ctx14;
  quiz_assert(
      PatternMatching::Match(KAdd(KMult(KA_s, "x"_e), KB_s), "x"_e, &ctx14));
  quiz_assert(ctx14.getNumberOfTrees(Placeholder::A) == 0);
  quiz_assert(ctx14.getNumberOfTrees(Placeholder::B) == 0);

  PatternMatching::Context ctx15;
  quiz_assert(
      PatternMatching::Match(KAdd(KMult(KA_s, "x"_e), KB_s), "x"_e, &ctx15));
  quiz_assert(ctx15.getNumberOfTrees(Placeholder::A) == 0);
  quiz_assert(ctx15.getNumberOfTrees(Placeholder::B) == 0);

  PatternMatching::Context ctx16;
  quiz_assert(
      PatternMatching::Match(KAdd(KA_s, KMult(KB_s, "x"_e), KC_s, KMult(KB_s)),
                             KAdd("x"_e, KMult(2_e, "x"_e), 2_e), &ctx16));
  quiz_assert(ctx16.getNumberOfTrees(Placeholder::A) == 1);
  assert_trees_are_equal(ctx16.getNode(Placeholder::A), "x"_e);
  quiz_assert(ctx16.getNumberOfTrees(Placeholder::B) == 1);
  assert_trees_are_equal(ctx16.getNode(Placeholder::B), 2_e);
  quiz_assert(ctx16.getNumberOfTrees(Placeholder::C) == 0);

  PatternMatching::Context ctx17;
  quiz_assert(
      PatternMatching::Match(KAdd(KA_s, KMult(KB_s, "x"_e), KC_s, KMult(KB_s)),
                             KAdd("x"_e, KMult(2_e, "x"_e), 1_e), &ctx17));
  quiz_assert(ctx17.getNumberOfTrees(Placeholder::A) == 0);
  quiz_assert(ctx17.getNumberOfTrees(Placeholder::B) == 0);
  quiz_assert(ctx17.getNumberOfTrees(Placeholder::C) == 1);
  assert_trees_are_equal(ctx17.getNode(Placeholder::C), KMult(2_e, "x"_e));
}

QUIZ_CASE(pcj_rewrite_replace) {
  const Tree* p = KAdd(KA, KA);
  const Tree* s = KMult(2_e, KA);
  EditionReference ref(SharedEditionPool->push<Type::Addition>(2));
  SharedEditionPool->push<Type::IntegerShort>(static_cast<int8_t>(5));
  SharedEditionPool->push<Type::IntegerShort>(static_cast<int8_t>(5));
  EditionReference result = PatternMatching::MatchAndCreate(ref, p, s);
  assert_trees_are_equal(result, KMult(2_e, 5_e));
  PatternMatching::MatchAndReplace(ref, p, s);
  assert_trees_are_equal(result, ref);
  result = PatternMatching::MatchAndCreate(ref, p, s);
  quiz_assert(result.isUninitialized());
}

QUIZ_CASE(pcj_match_n_ary) {
  assert_no_match(KMult(KAdd(1_e, 2_e, 3_e), KAdd(1_e, 2_e)),
                  KMult(KAdd(KA_s), KAdd(KA_s)));

  assert_match_and_create(KAdd(1_e), KAdd(KA, KB_s), KAdd(KB_s), 0_e);

  assert_match_and_create(KAdd(1_e), KAdd(KA_p), KAdd(KA_p), 1_e);
  assert_match_and_create(1_e, KAdd(KA, KB_s), KAdd(KB_s), 0_e);

  assert_match_and_create(1_e, KAdd(KA_s), KMult(KA_s), 1_e);
  assert_match_and_create(1_e, KAdd(KA_p), KMult(KA_p), 1_e);
  assert_no_match(1_e, KAdd(KA_s, KA_s));
  assert_match_and_create(0_e, KAdd(KA_s, KA_s), KMult(KA_s), 1_e);

  assert_no_match(KAdd(1_e, 2_e), KAdd(KA, KB, KC_p));
  assert_no_match(KAdd(1_e, 2_e, 3_e, 4_e), KAdd(KA_s, 3_e, KB, 4_e));

  assert_match_and_create(
      KAdd(1_e, 2_e), KAdd(KA_p, KB_s, KC_p),
      KAdd(KAbs(KAdd(KA_p)), KAbs(KAdd(KB_s)), KAbs(KAdd(KC_p))),
      KAdd(KAbs(1_e), KAbs(0_e), KAbs(2_e)));

  assert_no_match(KMult(3_e, KAbs("x"_e)),
                  KMult(KA_s, KAbs(KB), KAbs(KC), KD_s));

  assert_match_and_create(KAdd(1_e, 2_e), KA, KLn(KA), KLn(KAdd(1_e, 2_e)));

  assert_match_and_create(
      KMult(KAdd(1_e, 2_e, 3_e), KAdd(1_e, 2_e)), KMult(KAdd(KA, KB_s), KC),
      KAdd(KMult(KA, KC), KMult(KAdd(KB_s), KC)),
      KAdd(KMult(1_e, KAdd(1_e, 2_e)), KMult(KAdd(2_e, 3_e), KAdd(1_e, 2_e))));

  assert_match_and_create(KAdd(1_e, 2_e, 3_e), KAdd(KA_s, KB, 3_e, KC_s),
                          KAdd(KA_s, 0_e, KB, 0_e, KC_s, 0_e),
                          KAdd(1_e, 0_e, 2_e, 0_e, 0_e));

  assert_match_and_create(KAdd(1_e, 2_e, 3_e, KMult(2_e, 3_e), 3_e),
                          KAdd(KA_s, KB_s, KMult(KC_s, KB_s), KB_s),
                          KAdd(KA_s, 0_e, KB_s, 0_e, KC_s, 0_e),
                          KAdd(1_e, 2_e, 0_e, 3_e, 0_e, 2_e, 0_e));

  assert_match_and_create(KMult(KAdd(1_e, 2_e, 3_e), 4_e, KAdd(2_e, 3_e)),
                          KMult(KAdd(KA_s, KB_s, KC_s), KD_s, KAdd(KB_s)),
                          KAdd(KD_s, KA_s, KC_s), KAdd(4_e, 1_e));

  /* TODO: In this example we first try with 0 trees in A and 1 tree in B.
   *       Then, we perform a costly match and fail at the very end.
   *       We try again with 1 tree in A and 0 in B, and uselessly perform the
   *       exact same costly Match with no success.
   *       This should be optimized. */
  assert_no_match(KAdd(1_e, 1_e, 2_e, KMult(1_e, 2_e, 3_e, 3_e), 2_e),
                  KAdd(KA_s, 1_e, KB_s, 2_e, KMult(KC_s, KD, KE_s, KD), 1_e));
}
