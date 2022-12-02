#include "print.h"
#include <poincare_junior/src/expression/set.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void testSet() {
  std::cout << "\n---------------- Create {1, 2, 3} ----------------" << std::endl;
  Node set0 = Node::Push<BlockType::Set>(0);

  Set::Add(set0, 1_sn);
  Set::Add(set0, 2_sn);
  Set::Add(set0, 3_n);

  std::cout << "\n---------------- Create {-1, 3, 5, 6, 7} ----------------" << std::endl;
  Node set1 = Node::Push<BlockType::Set>(5);
  Node::Push<BlockType::MinusOne>();
  Node::Push<BlockType::IntegerShort>(3);
  Node::Push<BlockType::IntegerShort>(5);
  Node::Push<BlockType::IntegerShort>(6);
  Node::Push<BlockType::IntegerShort>(7);

  print();

  std::cout << "\n---------------- {1, 2, 3} U {-1, 3, 5, 6, 7} ----------------" << std::endl;
  EditionReference unionSet = Set::Union(set0, set1);
  assert(Simplification::Compare(unionSet.node(), Set(1_nsn, 1_sn, 2_sn, 3_n, 5_n, 6_n, 7_n)) == 0);
  print();
}

