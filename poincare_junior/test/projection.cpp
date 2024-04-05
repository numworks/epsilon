#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/projection.h>

#include "helper.h"

QUIZ_CASE(pcj_projection) {
  TreeRef ref(KCos(KSin(
      KPow(KPow(KPow(e_e, KLogarithm(KLogarithm(KLog(π_e), 2_e), e_e)), π_e),
           3_e))));
  Projection::DeepSystemProject(ref,
                                {.m_complexFormat = ComplexFormat::Cartesian});
  assert_trees_are_equal(
      ref,
      KTrig(KTrig(KPow(KPow(KExp(KLn(KMult(
                                KLn(KMult(KLn(π_e), KPow(KLn(10_e), -1_e))),
                                KPow(KLn(2_e), -1_e)))),
                            π_e),
                       3_e),
                  1_e),
            0_e));

  CloneTreeOverTree(ref, KAdd(KCos(KSub(2065_e, 2065_e)), KPow("x"_e, 2_e)));
  CloneTreeOverTree(ref, KAdd(KCos(KSub(2065_e, 2065_e)), KPow(2_e, "x"_e),
                              KPow(KLn(e_e), KDiv(1_e, 10_e))));
  Projection::DeepSystemProject(ref,
                                {.m_complexFormat = ComplexFormat::Cartesian,
                                 .m_strategy = Strategy::ApproximateToFloat});
  assert_trees_are_equal(ref, KAdd(1_de, KPow(2_de, "x"_e), 1_de));

  CloneTreeOverTree(ref, KCos(100_e));
  Projection::DeepSystemProject(ref, {.m_angleUnit = AngleUnit::Degree});
  assert_trees_are_equal(ref, KTrig(KMult(100_e, 1_e / 180_e, π_e), 0_e));

  CloneTreeOverTree(ref, KSqrt("y"_e));
  Projection::DeepSystemProject(ref,
                                {.m_complexFormat = ComplexFormat::Cartesian});
  assert_trees_are_equal(ref, KPow("y"_e, KHalf));

  CloneTreeOverTree(ref, KSqrt("y"_e));
  Projection::DeepSystemProject(ref, {.m_complexFormat = ComplexFormat::Real});
  assert_trees_are_equal(ref, KPowReal("y"_e, KHalf));

  ref->removeTree();
}
