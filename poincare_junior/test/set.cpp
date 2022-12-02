#include "print.h"
#include <poincare_junior/src/expression/set.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void testSet() {
  std::cout << "\n---------------- Create {1, 2, 3} ----------------" << std::endl;
  Node set0 = Node::Push<BlockType::Set>(0);

  Set::Add(set0, 1_sn);
  Set::Add(set0, 2_sn);
  Set::Add(set0, 3_n);

  std::cout << "\n---------------- Create {-1, 3, 4} ----------------" << std::endl;
  Node set1 = Node::Push<BlockType::Set>(3);
  Node::Push<BlockType::MinusOne>();
  Node::Push<BlockType::IntegerShort>(3);
  Node::Push<BlockType::IntegerShort>(5);

  print();

  std::cout << "\n---------------- {1, 2, 3} U {-1, 3, 5} ----------------" << std::endl;
  Set::Union(set0, set1);
  print();
}

