#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/k_creator.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_k_creator) {
  constexpr Tree tree = 3_e;

  const Node* node = KMult(KAdd(5_e, 8_e, 4_e), 3_e, tree);
  quiz_assert(node->numberOfChildren() == 3);
  quiz_assert(node->numberOfDescendants(true) == 7);

  quiz_assert(static_cast<const Node*>("x"_e)->nodeSize() == 5);
  quiz_assert(static_cast<const Node*>("var"_e)->nodeSize() == 7);

  const Node* poly = KPol(Exponents<2, 3>(), "x"_e, 2_e, "a"_e);
  quiz_assert(poly->numberOfChildren() == 3);
  quiz_assert(poly->nodeSize() == 6);
  quiz_assert(poly->treeSize() == 17);

  (void)KPol(Exponents<1>(), "x"_e, 2_e);

  quiz_assert(Approximation::To<float>(0.125_e) == 0.125);
  quiz_assert(Approximation::To<float>(-2.5_e) == -2.5);
}

QUIZ_CASE(pcj_k_creator_integer) {
  quiz_assert(static_cast<const Node*>(1_e)->nodeSize() == 1);
  quiz_assert(static_cast<const Node*>(12_e)->nodeSize() == 3);
  quiz_assert(static_cast<const Node*>(1234_e)->nodeSize() == 6);
  quiz_assert(static_cast<const Node*>(-12345_e)->nodeSize() == 6);
  quiz_assert(static_cast<const Node*>(123456_e)->nodeSize() == 7);
  quiz_assert(static_cast<const Node*>(-123456_e)->nodeSize() == 7);
  quiz_assert(static_cast<const Node*>(123456789_e)->nodeSize() == 8);
  quiz_assert(static_cast<const Node*>(-123456789_e)->nodeSize() == 8);

  quiz_assert(Integer::Handler(1_e).to<double>() == 1.0);
  quiz_assert(Integer::Handler(12_e).to<double>() == 12.0);
  quiz_assert(Integer::Handler(-12_e).to<double>() == -12.0);
  quiz_assert(Integer::Handler(1234_e).to<double>() == 1234.0);
  quiz_assert(Integer::Handler(-1234_e).to<double>() == -1234.0);
  quiz_assert(Integer::Handler(123456_e).to<double>() == 123456.0);
  quiz_assert(Integer::Handler(-123456_e).to<double>() == -123456.0);
  quiz_assert(Integer::Handler(123456789_e).to<double>() == 123456789.0);
  quiz_assert(Integer::Handler(-123456789_e).to<double>() == -123456789.0);

  quiz_assert(!static_cast<const Node*>(12_e)->parent());
}
