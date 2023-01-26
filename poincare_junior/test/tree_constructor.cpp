#include "print.h"
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

void testTreeConstructor() {
  constexpr CTree tree = Int<3>();
  const Node node = Addi(tree, Fact(Int<8>()));
  node.log();

  Node(Multi(Int<5>(), Int<8>(), tree)).log();

  static_assert(Node(Int<1>()).nodeSize() == 1);
  static_assert(Node(Int<42>()).nodeSize() == 3);
  assert(Node(Int<424>()).nodeSize() == 6);
  assert(Node(Int<-12345>()).nodeSize() == 6);
}
QUIZ_CASE(pcj_tree_constructor) { testTreeConstructor(); }
