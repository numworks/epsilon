#include "print.h"
#include <poincare_junior/src/expression/constructor.h>
#include <poincare_junior/src/expression/approximation.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_tree_constructor) {
  constexpr Tree tree = 3_e;

  Node node = Mult(Add(5_e, 8_e, 4_e), 3_e, tree);
  assert(node.numberOfChildren() == 3);
  assert(node.numberOfDescendants(true) == 7);

  assert(Node("x"_e).nodeSize() == 5);
  assert(Node("var"_e).nodeSize() == 7);

  Node poly = Pol(Exponents<2, 3>(), "x"_e, 2_e, "a"_e);
  assert(poly.numberOfChildren() == 3);
  assert(poly.nodeSize() == 6);
  assert(poly.treeSize() == 17);

  (void) Pol(Exponents<1>(), "x"_e, 2_e);

  assert(Approximation::To<float>(0.125_e) == 0.125);
  assert(Approximation::To<float>(-2.5_e) == -2.5);
}

QUIZ_CASE(pcj_tree_integer_constructor) {
  assert(Node(1_e).nodeSize() == 1);
  assert(Node(12_e).nodeSize() == 3);
  assert(Node(1234_e).nodeSize() == 6);
  assert(Node(-12345_e).nodeSize() == 6);
  assert(Node(123456_e).nodeSize() == 7);
  assert(Node(-123456_e).nodeSize() == 7);
  assert(Node(123456789_e).nodeSize() == 8);
  assert(Node(-123456789_e).nodeSize() == 8);

  assert(Integer::Handler(1_e).to<double>() == 1.0);
  assert(Integer::Handler(12_e).to<double>() == 12.0);
  assert(Integer::Handler(-12_e).to<double>() == -12.0);
  assert(Integer::Handler(1234_e).to<double>() == 1234.0);
  assert(Integer::Handler(-1234_e).to<double>() == -1234.0);
  assert(Integer::Handler(123456_e).to<double>() == 123456.0);
  assert(Integer::Handler(-123456_e).to<double>() == -123456.0);
  assert(Integer::Handler( 123456789_e).to<double>() == 123456789.0);
  assert(Integer::Handler(-123456789_e).to<double>() == -123456789.0);
}
