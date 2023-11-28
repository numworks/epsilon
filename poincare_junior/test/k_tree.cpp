#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/k_tree.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_k_tree) {
  constexpr KTree tree = 3_e;

  const Tree* node = KMult(KAdd(5_e, 8_e, 4_e), 3_e, tree);
  quiz_assert(node->numberOfChildren() == 3);
  quiz_assert(node->numberOfDescendants(true) == 7);

  quiz_assert(("x"_e)->nodeSize() == 3);
  quiz_assert(("var"_e)->nodeSize() == 5);

  const Tree* poly = KPol(Exponents<2, 3>(), "x"_e, 2_e, "a"_e);
  quiz_assert(poly->numberOfChildren() == 3);
  quiz_assert(poly->nodeSize() == 4);
  quiz_assert(poly->treeSize() == 11);

  (void)KPol(Exponents<1>(), "x"_e, 2_e);

  quiz_assert(Approximation::RootTreeTo<float>(0.125_fe) == 0.125);
  quiz_assert(Approximation::RootTreeTo<float>(-2.5_fe) == -2.5);

  const Tree* rational = -3_e / 8_e;
  quiz_assert(rational->isRational());
  quiz_assert(Approximation::RootTreeTo<float>(rational) == -0.375);
}

QUIZ_CASE(pcj_k_tree_integer) {
  quiz_assert((1_e)->nodeSize() == 1);
  quiz_assert((12_e)->nodeSize() == 2);
  quiz_assert((1234_e)->nodeSize() == 4);
  quiz_assert((-12345_e)->nodeSize() == 4);
  quiz_assert((123456_e)->nodeSize() == 5);
  quiz_assert((-123456_e)->nodeSize() == 5);
  quiz_assert((123456789_e)->nodeSize() == 6);
  quiz_assert((-123456789_e)->nodeSize() == 6);

  quiz_assert(Integer::Handler(1_e).to<double>() == 1.0);
  quiz_assert(Integer::Handler(12_e).to<double>() == 12.0);
  quiz_assert(Integer::Handler(-12_e).to<double>() == -12.0);
  quiz_assert(Integer::Handler(1234_e).to<double>() == 1234.0);
  quiz_assert(Integer::Handler(-1234_e).to<double>() == -1234.0);
  quiz_assert(Integer::Handler(123456_e).to<double>() == 123456.0);
  quiz_assert(Integer::Handler(-123456_e).to<double>() == -123456.0);
  quiz_assert(Integer::Handler(123456789_e).to<double>() == 123456789.0);
  quiz_assert(Integer::Handler(-123456789_e).to<double>() == -123456789.0);
}
