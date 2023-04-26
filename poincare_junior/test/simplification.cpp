#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/simplification.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_simplification_expansion) {
  EditionReference ref1(KExp(KAdd(1_e, 2_e)));
  ref1 = Simplification::ExpandExp(ref1);
  assert_trees_are_equal(ref1, KMult(KExp(1_e), KExp(2_e)));

  EditionReference ref2(KTrig(KAdd(π_e, KPow("x"_e, 2_e)), 0_e));
  ref2 = Simplification::ExpandTrigonometric(ref2);
  assert_trees_are_equal(
      ref2,
      KAdd(KMult(KTrig(π_e, 0_e), KTrig(KPow("x"_e, 2_e), 0_e)),
           KMult(KTrig(π_e, KAdd(0_e, -1_e)), KTrig(KPow("x"_e, 2_e), 1_e))));

  EditionReference ref3(KExp(KAdd(1_e, 2_e, 3_e)));
  ref3 = Simplification::ExpandExp(ref3);
  assert_trees_are_equal(ref3, KMult(KExp(1_e), KExp(KAdd(2_e, 3_e))));
}

QUIZ_CASE(pcj_simplification_contraction) {
  EditionReference ref1(KMult(KExp(1_e), KExp(2_e)));
  ref1 = Simplification::ContractExp(ref1);
  assert_trees_are_equal(ref1, KExp(KAdd(1_e, 2_e)));

  EditionReference ref2(
      KMult(KTrig(KLog(3_e), 1_e), KTrig(KAdd(1_e, KLog("x"_e)), 0_e)));
  ref2 = Simplification::ContractTrigonometric(ref2);
  assert_trees_are_equal(
      ref2,
      KMult(0.5_e,
            KAdd(KTrig(KAdd(KLog(3_e), KMult(-1_e, KAdd(1_e, KLog("x"_e)))),
                       KAdd(1_e, 0_e, KMult(-2_e, 1_e, 0_e))),
                 KTrig(KAdd(KLog(3_e), KAdd(1_e, KLog("x"_e))),
                       KAdd(0_e, 1_e)))));
}

QUIZ_CASE(pcj_simplification_projection) {
  EditionReference ref(KCos(KSin(KTan(
      KPow(KPow(KPow(e_e, KLogarithm(KLogarithm(KLog(π_e), 2_e), e_e)), π_e),
           3_e)))));
  ref = Simplification::SystemProjection(ref);
  assert_trees_are_equal(
      ref,
      KTrig(
          KTrig(
              KMult(KTrig(KPow(KExp(KMult(KLn(KExp(KLn(KMult(
                                              KLn(KMult(KLn(π_e),
                                                        KPow(KLn(10_e), -1_e))),
                                              KPow(KLn(2_e), -1_e))))),
                                          π_e)),
                               3_e),
                          1_e),
                    KPow(KTrig(KPow(KExp(KMult(
                                        KLn(KExp(KLn(KMult(
                                            KLn(KMult(KLn(π_e),
                                                      KPow(KLn(10_e), -1_e))),
                                            KPow(KLn(2_e), -1_e))))),
                                        π_e)),
                                    3_e),
                               0_e),
                         -1_e)),
              1_e),
          0_e));
}
