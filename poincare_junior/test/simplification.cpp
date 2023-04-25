#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/simplification.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_simplification_expansion) {
  EditionReference ref1(KPow(e_e, KAdd(1_e, 2_e)));
  ref1 = Simplification::ExpandExp(ref1);
  assert_trees_are_equal(ref1, KMult(KPow(e_e, 1_e), KPow(e_e, 2_e)));

  EditionReference ref2(KCos(KAdd(π_e, KPow("x"_e, 2_e))));
  ref2 = Simplification::ExpandTrigonometric(ref2);
  assert_trees_are_equal(ref2,
                         KAdd(KMult(KCos(π_e), KCos(KPow("x"_e, 2_e))),
                              KMult(-1_e, KSin(π_e), KSin(KPow("x"_e, 2_e)))));

  EditionReference ref3(KPow(e_e, KAdd(1_e, 2_e, 3_e)));
  ref3 = Simplification::ExpandExp(ref3);
  assert_trees_are_equal(ref3,
                         KMult(KPow(e_e, 1_e), KPow(e_e, KAdd(2_e, 3_e))));
}

QUIZ_CASE(pcj_simplification_contraction) {
  EditionReference ref1(KMult(KPow(e_e, 1_e), KPow(e_e, 2_e)));
  ref1 = Simplification::ContractExp(ref1);
  assert_trees_are_equal(ref1, KPow(e_e, KAdd(1_e, 2_e)));

  EditionReference ref2(KMult(KSin(KLog(3_e)), KCos(KAdd(1_e, KLog("x"_e)))));
  ref2 = Simplification::ContractTrigonometric(ref2);
  assert_trees_are_equal(
      ref2,
      KMult(0.5_e,
            KAdd(KSin(KAdd(KLog(3_e), KMult(-1_e, KAdd(1_e, KLog("x"_e))))),
                 KSin(KAdd(KLog(3_e), KAdd(1_e, KLog("x"_e)))))));
}
