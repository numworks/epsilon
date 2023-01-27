#include "print.h"
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

void testTreeConstructor() {
  constexpr CTree tree = 3_n;
  const Node node = Addi(tree, Fact(8_n));
  node.log();

  Node((5_n + 8_n + 4_n) * 3_n * tree).log();
  Node(5_n - 8_n - 4_n).log();
  Node(4_n * 3_n + 2_n * 1_n).log();
  Node(4_n * 3_n / 2_n * 1_n).log();
}
QUIZ_CASE(pcj_tree_constructor) { testTreeConstructor(); }

void testTreeIntegerConstructor() {
  assert(Node(Int<1>()).nodeSize() == 1);
  assert(Node(Int<12>()).nodeSize() == 3);
  assert(Node(Int<1234>()).nodeSize() == 6);
  assert(Node(Int<-12345>()).nodeSize() == 6);
  assert(Node(Int<123456>()).nodeSize() == 7);
  assert(Node(Int<-123456>()).nodeSize() == 7);
  assert(Node(Int<123456789>()).nodeSize() == 8);
  assert(Node(Int<-123456789>()).nodeSize() == 8);

  assert(Integer::Handler(Int<1>()).to<double>() == 1.0);
  assert(Integer::Handler(Int<12>()).to<double>() == 12.0);
  assert(Integer::Handler(Int<-12>()).to<double>() == -12.0);
  assert(Integer::Handler(Int<1234>()).to<double>() == 1234.0);
  assert(Integer::Handler(Int<-1234>()).to<double>() == -1234.0);
  assert(Integer::Handler(Int<123456>()).to<double>() == 123456.0);
  assert(Integer::Handler(Int<-123456>()).to<double>() == -123456.0);
  assert(Integer::Handler(Int< 123456789>()).to<double>() == 123456789.0);
  assert(Integer::Handler(Int<-123456789>()).to<double>() == -123456789.0);
}
QUIZ_CASE(pcj_tree_integer_constructor) { testTreeIntegerConstructor(); }
