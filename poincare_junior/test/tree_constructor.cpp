#include "print.h"
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

void testTreeConstructor() {
  constexpr CTree tree = 3_n;
  const Node node = Addi(tree, Fact(8_n));
  node.log();
  Node(Poly(CTree<1>(), "x"_v, CTree(2_n))).log();
  Node(Poly(CTree<2, 3>(), "x"_v, CTree(2_n), "a"_v)).log();

  Node((5_n + 8_n + 4_n) * 3_n * tree).log();
  Node(5_n - 8_n - 4_n).log();
  Node(4_n * 3_n + 2_n * 1_n).log();
  Node(4_n * 3_n / 2_n * 1_n).log();
  assert(Node("x"_v).nodeSize() == 5);
  assert(Node("var"_v).nodeSize() == 7);
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
