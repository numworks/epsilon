#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/projection.h>

#include "helper.h"

QUIZ_CASE(pcj_simplification_projection) {
  EditionReference ref(KCos(KSin(KTan(
      KPow(KPow(KPow(e_e, KLogarithm(KLogarithm(KLog(π_e), 2_e), e_e)), π_e),
           3_e)))));
  Projection::DeepSystemProjection(
      ref, {.m_complexFormat = ComplexFormat::Cartesian});
  assert_trees_are_equal(
      ref,
      KTrig(
          KTrig(
              KMult(
                  KTrig(
                      KPow(KPow(KExp(KLn(KMult(
                                    KLn(KMult(KLn(π_e), KPow(KLn(10_e), -1_e))),
                                    KPow(KLn(2_e), -1_e)))),
                                π_e),
                           3_e),
                      1_e),
                  KPow(KTrig(KPow(KPow(KExp(KLn(KMult(
                                           KLn(KMult(KLn(π_e),
                                                     KPow(KLn(10_e), -1_e))),
                                           KPow(KLn(2_e), -1_e)))),
                                       π_e),
                                  3_e),
                             0_e),
                       -1_e)),
              1_e),
          0_e));

  CloneTreeOverTree(ref, KAdd(KCos(KSub(2065_e, 2065_e)), KPow(e_e, "x"_e)));
  Projection::DeepSystemProjection(ref,
                                   {.m_complexFormat = ComplexFormat::Cartesian,
                                    .m_strategy = Strategy::NumbersToFloat});
  assert_trees_are_equal(
      ref,
      KAdd(KTrig(KAdd(2065.0_e, KMult(-1.0_e, 2065.0_e)), 0.0_e), KExp("x"_e)));

  CloneTreeOverTree(ref, KAdd(KCos(KSub(2065_e, 2065_e)), KPow(2_e, "x"_e),
                              KPow(KLn(e_e), KDiv(1_e, 10_e))));
  Projection::DeepSystemProjection(
      ref, {.m_complexFormat = ComplexFormat::Cartesian,
            .m_strategy = Strategy::ApproximateToFloat});
  assert_trees_are_equal(ref, KAdd(1.0_e, KPow(2.0_e, "x"_e), 1.0_e));

  CloneTreeOverTree(ref, KCos(100_e));
  Projection::DeepSystemProjection(ref, {.m_angleUnit = AngleUnit::Degree});
  assert_trees_are_equal(ref,
                         KTrig(KMult(100_e, π_e, KPowReal(180_e, -1_e)), 0_e));

  CloneTreeOverTree(ref, KSqrt("y"_e));
  Projection::DeepSystemProjection(
      ref, {.m_complexFormat = ComplexFormat::Cartesian});
  assert_trees_are_equal(ref, KPow("y"_e, KHalf));

  CloneTreeOverTree(ref, KSqrt("y"_e));
  Projection::DeepSystemProjection(ref,
                                   {.m_complexFormat = ComplexFormat::Real});
  assert_trees_are_equal(ref, KPowReal("y"_e, KHalf));

  ref->removeTree();
}
