#include <poincare_junior/src/expression/beautification.h>
#include <poincare_junior/src/expression/k_tree.h>

#include "helper.h"

QUIZ_CASE(pcj_beautification) {
  Tree* ref0 = KMult(3_e, KPow("x"_e, -2_e))->clone();
  Beautification::DeepBeautify(ref0);
  assert_trees_are_equal(ref0, KDiv(3_e, KPow("x"_e, 2_e)));

  Tree* ref30 = KMult(3_e, KPow("x"_e, 2_e))->clone();
  Beautification::DeepBeautify(ref30);
  assert_trees_are_equal(ref30, KMult(3_e, KPow("x"_e, 2_e)));

  TreeRef ref1(KAdd(KCos(3_e), KSin("x"_e),
                    KMult(-1_e, 2_e, KExp(KMult(KLn(5_e), "y"_e))),
                    KMult(KLn(2_e), KPow(KLn(4_e), -1_e))));
  Beautification::DeepBeautify(ref1);
  assert_trees_are_equal(
      ref1, KAdd(KCos(3_e), KOpposite(KMult(2_e, KPow(5_e, "y"_e))),
                 KSin("x"_e), KDiv(KLn(2_e), KLn(4_e))));

  TreeRef ref3(KExp(KMult(KHalf, KLn("y"_e))));
  Beautification::DeepBeautify(ref3);
  assert_trees_are_equal(ref3, KSqrt("y"_e));

  TreeRef ref4(KExp(KMult(2.5_fe, KLn("y"_e))));
  Beautification::DeepBeautify(ref4);
  assert_trees_are_equal(ref4, KPow("y"_e, 2.5_fe));

  TreeRef ref5(
      KAdd(KMult(-1_e, "w"_e), "x"_e, KMult(-1_e, "y"_e), KMult(-1_e, "z"_e)));
  Beautification::DeepBeautify(ref5);
  assert_trees_are_equal(
      ref5, KAdd(KOpposite("w"_e), "x"_e, KOpposite("y"_e), KOpposite("z"_e)));
}
