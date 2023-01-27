#include "print.h"
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

void testTreeConstructor() {
  constexpr CTree tree = Int<3>();
  const Node node = Addi(tree, Fact(Int<8>()));
  node.log();

  Node(Multi(Int<5>(), Int<8>(), tree)).log();
}
QUIZ_CASE(pcj_tree_constructor) { testTreeConstructor(); }

void testTreeConstructorInt() {
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
QUIZ_CASE(pcj_tree_constructor_int) { testTreeConstructorInt(); }
