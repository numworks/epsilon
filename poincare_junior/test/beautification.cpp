#include <poincare_junior/src/expression/beautification.h>
#include <poincare_junior/src/expression/k_tree.h>

#include "helper.h"

QUIZ_CASE(pcj_simplification_beautify) {
  EditionReference ref1(KAdd(KTrig(3_e, 0_e), KTrig("x"_e, 1_e),
                             KMult(-1_e, 2_e, KExp(KMult(KLn(5_e), "y"_e))),
                             KMult(KLn(2_e), KPow(KLn(4_e), -1_e))));
  Beautification::DeepBeautify(ref1);
  assert_trees_are_equal(
      ref1, KAdd(KMult(-1_e, 2_e, KPow(5_e, "y"_e)), KCos(3_e), KSin("x"_e),
                 KLogarithm(2_e, 4_e)));

  EditionReference ref2(KTrig(π_e, 1_e));
  Beautification::DeepBeautify(ref2, {.m_angleUnit = AngleUnit::Gradian});
  assert_trees_are_equal(ref2, KSin(200_e));

  EditionReference ref3(KExp(KMult(KHalf, KLn("y"_e))));
  Beautification::DeepBeautify(ref3);
  assert_trees_are_equal(ref3, KSqrt("y"_e));

  EditionReference ref4(KExp(KMult(2.5_e, KLn("y"_e))));
  Beautification::DeepBeautify(ref4);
  assert_trees_are_equal(ref4, KPow("y"_e, 2.5_e));

  EditionReference ref5(
      KAdd(KMult(-1_e, "w"_e), "x"_e, KMult(-1_e, "y"_e), KMult(-1_e, "z"_e)));
  Beautification::DeepBeautify(ref5);
  assert_trees_are_equal(
      ref5, KSub(KSub(KAdd(KMult(-1_e, "w"_e), "x"_e), "y"_e), "z"_e));
}
