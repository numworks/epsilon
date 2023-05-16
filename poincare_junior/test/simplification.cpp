#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/simplification.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_simplification_expansion) {
  EditionReference ref1(KExp(KAdd(1_e, 2_e)));
  quiz_assert(Simplification::Expand(&ref1));
  assert_trees_are_equal(ref1, KMult(KExp(1_e), KExp(2_e)));

  EditionReference ref2(KTrig(KAdd(π_e, KPow("x"_e, 2_e)), 0_e));
  quiz_assert(Simplification::Expand(&ref2));
  assert_trees_are_equal(
      ref2,
      KAdd(KMult(KTrig(π_e, 0_e), KTrig(KPow("x"_e, 2_e), 0_e)),
           KMult(KTrig(π_e, KAdd(0_e, -1_e)), KTrig(KPow("x"_e, 2_e), 1_e))));

  EditionReference ref3(KExp(KAdd(1_e, 2_e, 3_e)));
  quiz_assert(Simplification::Expand(&ref3));
  assert_trees_are_equal(ref3, KMult(KExp(1_e), KExp(KAdd(2_e, 3_e))));

  EditionReference ref4(KAbs(KMult(1_e, 2_e)));
  quiz_assert(Simplification::Expand(&ref4));
  assert_trees_are_equal(ref4, KMult(KAbs(1_e), KAbs(2_e)));

  EditionReference ref5(KLn(KMult(1_e, 2_e, 3_e)));
  quiz_assert(Simplification::Expand(&ref5));
  assert_trees_are_equal(ref5, KAdd(KLn(1_e), KLn(KMult(2_e, 3_e))));
}

QUIZ_CASE(pcj_simplification_contraction) {
  EditionReference ref1(KMult(KExp(1_e), KExp(2_e)));
  quiz_assert(Simplification::Contract(&ref1));
  assert_trees_are_equal(ref1, KExp(KAdd(1_e, 2_e)));

  EditionReference ref2(
      KMult(KTrig(KLog(3_e), 1_e), KTrig(KAdd(1_e, KLog("x"_e)), 0_e)));
  quiz_assert(Simplification::Contract(&ref2));
  assert_trees_are_equal(
      ref2,
      KMult(0.5_e,
            KAdd(KTrig(KAdd(KLog(3_e), KMult(-1_e, KAdd(1_e, KLog("x"_e)))),
                       KTrigDiff(1_e, 0_e)),
                 KTrig(KAdd(KLog(3_e), 1_e, KLog("x"_e)), KAdd(0_e, 1_e)))));

  EditionReference ref4(KMult(KAbs(1_e), KAbs(KMult(2_e, 3_e))));
  quiz_assert(Simplification::Contract(&ref4));
  assert_trees_are_equal(ref4, KAbs(KMult(1_e, 2_e, 3_e)));

  EditionReference ref5(KAdd(KLn(1_e), KLn(2_e), 3_e, 4_e));
  quiz_assert(Simplification::Contract(&ref5));
  assert_trees_are_equal(ref5, KAdd(KLn(KMult(1_e, 2_e)), 3_e, 4_e));
}

QUIZ_CASE(pcj_simplification_projection) {
  EditionReference ref1(KCos(KSin(KTan(
      KPow(KPow(KPow(e_e, KLogarithm(KLogarithm(KLog(π_e), 2_e), e_e)), π_e),
           3_e)))));
  ref1 = Simplification::SystemProjection(ref1);
  assert_trees_are_equal(
      ref1,
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

  EditionReference ref2(KAdd(KCos(KSub(2065_e, 2065_e)), KPow(e_e, "x"_e)));
  ref2 = Simplification::SystemProjection(
      ref2, Simplification::ProjectionContext::NumbersToFloat);
  assert_trees_are_equal(
      ref2,
      KAdd(KTrig(KAdd(2065.0_e, KMult(-1.0_e, 2065.0_e)), 0.0_e), KExp("x"_e)));
  ref2 = Simplification::SystemProjection(
      ref2, Simplification::ProjectionContext::ApproximateToFloat);
  assert_trees_are_equal(ref2, KAdd(1.0_e, KExp("x"_e)));
}
