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
