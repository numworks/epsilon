#include <poincare/src/expression/k_tree.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(pcj_simplification_probability) {
  simplifies_to("binompdf(2,4,-1)", "undef");
  simplifies_to("binompdf(2,4,1.2)", "undef");
  simplifies_to("binompdf(-inf,4,0.4)", "0");
  simplifies_to("binompdf(inf,4,0.4)", "0");
  simplifies_to("binomcdf(2,4,-1)", "undef");
  simplifies_to("binomcdf(2,4,1.2)", "undef");
  simplifies_to("binomcdf(-inf,4,0.4)", "0");
  simplifies_to("binomcdf(inf,4,0.4)", "1");
  simplifies_to("invbinom(1,4,-1)", "undef");
  simplifies_to("invbinom(0,4,1.2)", "undef");
  simplifies_to("invbinom(1,4,0.4)", "4");
  simplifies_to("invbinom(0,4,0.4)", "undef");
  simplifies_to("invbinom(1,4,1)", "4");
  simplifies_to("invbinom(0,4,1)", "0");

  simplifies_to("geompdf(1,0)", "undef");
  simplifies_to("geomcdf(inf,0.5)", "1");
  simplifies_to("invgeom(1,1)", "1");

  simplifies_to("invnorm(-1.3,2,3)", "undef");
  simplifies_to("invnorm(-1.3,2,3)", "undef");
  simplifies_to("invnorm(-1.3,2,3)", "undef");
  simplifies_to("invnorm(0,2,3)", "-∞");
  simplifies_to("invnorm(0.5,2,3)", "2");
  simplifies_to("invnorm(1,2,3)", "∞");
  simplifies_to("invnorm(1.3,2,3)", "undef");
  // random can be 0
  simplifies_to("invnorm(3/4,2,random())", "invnorm(3/4,2,random())");
  simplifies_to("invnorm(0.5,2,0)", "undef");
  simplifies_to("invnorm(0.5,2,-1)", "undef");
  simplifies_to("normcdf(2,0,1)", "normcdf(2,0,1)");
  simplifies_to("normcdfrange(-inf,inf,20,4)", "1");
  simplifies_to("normcdfrange(1,2,0,1)", "normcdfrange(1,2,0,1)");
  simplifies_to("normpdf(2,0,1)", "normpdf(2,0,1)");

  simplifies_to("poissonpdf(1,0)", "undef");
  simplifies_to("poissoncdf(inf,0.5)", "1");

  simplifies_to("tpdf(-inf,0)", "undef");
  simplifies_to("tpdf(-inf,2)", "0");
  simplifies_to("tpdf(inf,2)", "0");
  simplifies_to("tcdf(-inf,2)", "0");
  simplifies_to("tcdf(inf,2)", "1");
  simplifies_to("invt(0.5,0)", "undef");
  simplifies_to("invt(0.5,12)", "0");
  simplifies_to("invt(0.5,0)", "undef");

  simplifies_to("invt(0.3,4)+normcdf(2,0,1)+invt(0.3,4)",
                "2×invt(3/10,4)+normcdf(2,0,1)");
}

QUIZ_CASE(pcj_simplification_random) {
  simplifies_to("1/random()+1/3+1/4", "7/12+1/random()");
  simplifies_to("random()+random()", "random()+random()");
  simplifies_to("random()-random()", "random()-random()");
  simplifies_to("abs(random()-random())", "abs(random()-random())");
  simplifies_to("1/random()+1/3+1/4+1/random()", "7/12+1/random()+1/random()");
  simplifies_to("random()×random()", "random()×random()");
  simplifies_to("random()/random()", "random()/random()");
  simplifies_to("3^random()×3^random()", "3^(random()+random())");
  simplifies_to("random()×ln(2)×3+random()×ln(2)×5",
                "(3×random()+5×random())×ln(2)");

  // TODO: Handle them with {.m_strategy = Strategy::ApproximateToFloat}
  simplifies_to("randintnorep(1,10,5)", "randintnorep(1,10,5)");
  simplifies_to("randintnorep(1,10,11)", "undef");
  simplifies_to("randintnorep(1.5,10, 10)", "undef");
  simplifies_to("random()", "random()");
  simplifies_to("randint(1,10)", "randint(1,10)");
  simplifies_to("diff(random()+1,x,2)", "undef");
  simplifies_to("sum(k+randint(1,10),k,2,5)-14", "sum(randint(1,10),k,2,5)");
  simplifies_to("sequence(2*k+random(),k,3)+1", "1+sequence(2×k+random(),k,3)");
  simplifies_to("random()<acos(40)", "undef");
  Tree* randIntList =
      (KAdd(KRandInt(0_e, KList(0_e, 1_e)), KRandom))->cloneTree();
  simplify(randIntList, k_realCtx, true);
  assert_trees_are_equal(
      randIntList, KList(KAdd(KRandomSeeded<3>, KRandIntSeeded<1>(0_e, 0_e)),
                         KAdd(KRandomSeeded<3>, KRandIntSeeded<2>(0_e, 1_e))));

  Tree* randIntNoRep = (KRandIntNoRep(0_e, 1000_e, 100_e))->cloneTree();
  simplify(randIntNoRep, k_realCtx, true);
  assert_trees_are_equal(randIntNoRep,
                         KRandIntNoRepSeeded<1>(0_e, 1000_e, 100_e));
  randIntNoRep = (KRandIntNoRep(0_e, 1000_e, 101_e))->cloneTree();
  simplify(randIntNoRep, k_realCtx, true);
  /* NOTE: RandIntNoRep size is limited by
   * Random::Context::k_maxNumberOfVariables ->
   * 0-seeded RandIntNoRep means it will approximate to a list of undef */
  assert_trees_are_equal(randIntNoRep,
                         KRandIntNoRepSeeded<0>(0_e, 1000_e, 101_e));

  SharedTreeStack->flushFromBlock(randIntList);

  // TODO_PCJ: used to simplify to 1
  simplifies_to("randint(1,1)", "randint(1,1)");
  //  Randint is not simplified if ReductionTarget = SystemForApproximation
  simplifies_to_no_beautif(
      "randint(1,3)", "randint(1,3)",
      {.m_reductionTarget = ReductionTarget::SystemForApproximation});
}

QUIZ_CASE(pcj_simplification_combinatorics) {
  simplifies_to("binomial(4,2)", "6");
  simplifies_to("binomial(20,3)", "1140");
  simplifies_to("binomial(20,10)", "184756");
  simplifies_to("binomial(10,20)", "0");
  simplifies_to("binomial(10.34,0)", "1");
  simplifies_to("binomial(3.34,-1)", "0");
  simplifies_to("binomial(-10,10)", "92378");
  simplifies_to("binomial(2.5,3)", "binomial(5/2,3)");
  simplifies_to("binomial(-200,120)", "binomial(-200,120)");
  simplifies_to("binomial(400,1)", "binomial(400,1)");

  simplifies_to("permute(4,2)", "12");
  simplifies_to("permute(99,4)", "90345024");
  simplifies_to("permute(20,-10)", "undef");
}
