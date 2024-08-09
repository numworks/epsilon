#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/simplification.h>

#include "helper.h"

QUIZ_CASE(pcj_projection) {
  TreeRef ref(KCos(KSin(KPow(
      KPow(KPow(e_e, KLogBase(KLogBase(KLog(π_e), 2_e), e_e)), π_e), 3_e))));
  ProjectionContext ctx;
  ctx.m_complexFormat = ComplexFormat::Cartesian;
  ctx.m_strategy = Strategy::Default;
  ctx.m_angleUnit = AngleUnit::Radian;
  Simplification::ToSystem(ref, &ctx);
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
  ctx.m_complexFormat = ComplexFormat::Cartesian;
  ctx.m_strategy = Strategy::ApproximateToFloat;
  ctx.m_angleUnit = AngleUnit::Radian;
  Simplification::ToSystem(ref, &ctx);
  assert_trees_are_equal(ref, KAdd(1_de, KPow(2_de, "x"_e), 1_de));

  ctx.m_complexFormat = ComplexFormat::Cartesian;
  ctx.m_strategy = Strategy::Default;
  ctx.m_angleUnit = AngleUnit::Degree;
  CloneTreeOverTree(ref, KCos(100_e));
  Simplification::ToSystem(ref, &ctx);
  assert_trees_are_equal(ref, KTrig(KMult(100_e, 1_e / 180_e, π_e), 0_e));

  ctx.m_complexFormat = ComplexFormat::Cartesian;
  ctx.m_strategy = Strategy::Default;
  ctx.m_angleUnit = AngleUnit::Radian;
  CloneTreeOverTree(ref, KSqrt(π_e));
  Simplification::ToSystem(ref, &ctx);
  assert_trees_are_equal(ref, KPow(π_e, 1_e / 2_e));

  ctx.m_complexFormat = ComplexFormat::Real;
  ctx.m_strategy = Strategy::Default;
  ctx.m_angleUnit = AngleUnit::Radian;
  CloneTreeOverTree(ref, KSqrt(π_e));
  Simplification::ToSystem(ref, &ctx);
  assert_trees_are_equal(ref, KPowReal(π_e, 1_e / 2_e));

  ref->removeTree();
}
