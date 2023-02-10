#include "print.h"
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

void testTreeConstructor() {
  constexpr CTree tree = 3_n;

  Node node = (5_n + 8_n + 4_n) * 3_n * tree;
  assert(node.numberOfChildren() == 3);
  assert(node.numberOfDescendants(true) == 7);

  assert(Node("x"_v).nodeSize() == 5);
  assert(Node("var"_v).nodeSize() == 7);

  Node poly = Poly(CTree<2, 3>(), "x"_v, 2_n, "a"_v);
  assert(poly.numberOfChildren() == 3);
  assert(poly.nodeSize() == 6);
  assert(poly.treeSize() == 17);

  // These tests are at least useful at compile time
  (void) (5_n - 8_n - 4_n);
  (void) (4_n * 3_n + 2_n * 1_n);
  (void) (4_n * 3_n / 2_n * 1_n);

  (void) Poly(CTree<1>(), "x"_v, 2_n);
}
QUIZ_CASE(pcj_tree_constructor) { testTreeConstructor(); }

void testTreeIntegerConstructor() {
  assert(Node(1_n).nodeSize() == 1);
  assert(Node(12_n).nodeSize() == 3);
  assert(Node(1234_n).nodeSize() == 6);
  assert(Node(-12345_n).nodeSize() == 6);
  assert(Node(123456_n).nodeSize() == 7);
  assert(Node(-123456_n).nodeSize() == 7);
  assert(Node(123456789_n).nodeSize() == 8);
  assert(Node(-123456789_n).nodeSize() == 8);

  assert(Integer::Handler(1_n).to<double>() == 1.0);
  assert(Integer::Handler(12_n).to<double>() == 12.0);
  assert(Integer::Handler(-12_n).to<double>() == -12.0);
  assert(Integer::Handler(1234_n).to<double>() == 1234.0);
  assert(Integer::Handler(-1234_n).to<double>() == -1234.0);
  assert(Integer::Handler(123456_n).to<double>() == 123456.0);
  assert(Integer::Handler(-123456_n).to<double>() == -123456.0);
  assert(Integer::Handler( 123456789_n).to<double>() == 123456789.0);
  assert(Integer::Handler(-123456789_n).to<double>() == -123456789.0);
}
QUIZ_CASE(pcj_tree_integer_constructor) { testTreeIntegerConstructor(); }
