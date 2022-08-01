#include "print.h"

using namespace Poincare;

void elementaryTreeManipulation(TreeCache * cache, TreeSandbox * sandbox) {
  std::cout << "\n---------------- Create (1 + 2) * 3 * 4 ----------------" << std::endl;
  Multiplication::PushNode(sandbox, 3);
  Addition::PushNode(sandbox, 2);
  Integer::PushNode(sandbox, 1);
  Integer::PushNode(sandbox, 2);
  Integer::PushNode(sandbox, 3);
  Integer::PushNode(sandbox, 4);

  print();

  std::cout << "\n---------------- Scan children backward ----------------" << std::endl;
  TypeTreeBlock * root = sandbox->firstBlock();
  for (TypeTreeBlock * subTree : root->backwardsDirectChildren()) {
    subTree->log(std::cout);
  }

  std::cout << "\n---------------- Store (1+2)*3*4 ----------------" << std::endl;
  int treeId = cache->storeLastTree();
  print();

  std::cout << "\n---------------- Edit (1+2)*3*4 ----------------" << std::endl;
  sandbox->copyTreeFromAddress(cache->treeForIdentifier(treeId));
  print();


  std::cout << "\n---------------- Develop (1+2)*3*4 ----------------" << std::endl;
  root = sandbox->firstBlock();
  assert(root->type() == BlockType::Multiplication);
  Multiplication::DistributeOverAddition(root, sandbox);
  print();

  std::cout << "\n---------------- Store developped 1*3*4+2*3*4 ----------------" << std::endl;
  treeId = cache->storeLastTree();
  print();

  std::cout << "\n---------------- Create 1-2/3 ----------------" << std::endl;
  TypeTreeBlock * subtraction = Subtraction::PushNode(sandbox);
  Integer::PushNode(sandbox, 1);
  Division::PushNode(sandbox);
  Integer::PushNode(sandbox, 2);
  Integer::PushNode(sandbox, 3);
  print();

  std::cout << "\n---------------- Projection to internal nodes 1-2/3 ----------------" << std::endl;
  subtraction->recursivelyApply(sandbox, &TypeTreeBlock::basicReduction);
  print();
}
