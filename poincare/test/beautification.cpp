#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/k_tree.h>

#include "helper.h"

QUIZ_CASE(pcj_beautification) {
  Tree* e1 = KMult(3_e, KPow("x"_e, -2_e))->cloneTree();
  Beautification::DeepBeautify(e1);
  assert_trees_are_equal(e1, KDiv(3_e, KPow("x"_e, 2_e)));

  Tree* e2 = KMult(3_e, KPow("x"_e, 2_e))->cloneTree();
  Beautification::DeepBeautify(e2);
  assert_trees_are_equal(e2, KMult(3_e, KPow("x"_e, 2_e)));

  TreeRef ref1(KAdd(KCos(3_e), KSin("x"_e),
                    KMult(-1_e, 2_e, KExp(KMult(KLn(5_e), "y"_e))),
                    KMult(KLn(2_e), KPow(KLn(4_e), -1_e))));
  Beautification::DeepBeautify(ref1);
  assert_trees_are_equal(ref1, KAdd(KCos(3_e), KSin("x"_e),
                                    KOpposite(KMult(2_e, KPow(5_e, "y"_e))),
                                    KDiv(KLn(2_e), KLn(4_e))));

  TreeRef ref3(KExp(KMult(1_e / 2_e, KLn("y"_e))));
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

  TreeRef ref6((-3_e / 2_e)->cloneTree());
  Beautification::DeepBeautify(ref6);
  assert_trees_are_equal(ref6, KOpposite(KDiv(3_e, 2_e)));

  TreeRef ref7((-3_e)->cloneTree());
  Beautification::DeepBeautify(ref7);
  assert_trees_are_equal(ref7, KOpposite(3_e));
}

void beautifies_to(const char* input, const char* output) {
  process_tree_and_compare(input, output,
                           [](Tree* e, ProjectionContext ctx) {
                             Projection::DeepSystemProject(e, ctx);
                             Beautification::DeepBeautify(e, ctx);
                           },
                           {});
}

QUIZ_CASE(pcj_beautification_complex) {
  beautifies_to("i/2", "1/2×i");
  beautifies_to("2*i*π", "2×π×i");
}
