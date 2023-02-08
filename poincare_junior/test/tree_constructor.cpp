#include "print.h"
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_tree_constructor) {
  constexpr CTree tree = 3_e;

  Node node = Multi(Addi(5_e, 8_e, 4_e), 3_e, tree);
  assert(node.numberOfChildren() == 3);
  assert(node.numberOfDescendants(true) == 7);

  assert(Node("x"_v).nodeSize() == 5);
  assert(Node("var"_v).nodeSize() == 7);

  Node poly = Poly(Exponents<2, 3>(), "x"_v, 2_e, "a"_v);
  assert(poly.numberOfChildren() == 3);
  assert(poly.nodeSize() == 6);
  assert(poly.treeSize() == 17);

  (void) Poly(Exponents<1>(), "x"_v, 2_e);
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
