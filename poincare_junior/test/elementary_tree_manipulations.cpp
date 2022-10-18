#include "print.h"
#include <poincare_junior/edition_reference.h>
#include <poincare_junior/node_iterator.h>

using namespace Poincare;

void elementaryTreeManipulation() {
  CachePool * cache = CachePool::sharedCachePool();
  EditionPool * editionPool = cache->editionPool();

  createSimpleExpression();

  print();

  std::cout << "\n---------------- Store (1+2)*3*4 ----------------" << std::endl;
  uint16_t treeId = cache->storeEditedTree();
  print();

  std::cout << "\n---------------- Edit (1+2)*3*4 ----------------" << std::endl;
  editionPool->initFromTree(cache->nodeForIdentifier(treeId));
  print();


  std::cout << "\n---------------- Develop (1+2)*3*4 ----------------" << std::endl;
  TypeBlock * root = editionPool->firstBlock();
  assert(root->type() == BlockType::Multiplication);
  Multiplication::DistributeOverAddition(root);
  print();

  std::cout << "\n---------------- Store developped 1*3*4+2*3*4 ----------------" << std::endl;
  treeId = cache->storeEditedTree();
  print();

  std::cout << "\n---------------- Create 1-2/3 ----------------" << std::endl;
  Node subtraction = Node::Push<Subtraction>();
  Node::Push<IntegerShort>(1);
  Node::Push<Division>();
  Node::Push<IntegerShort>(2);
  Node::Push<IntegerShort>(3);
  print();

  std::cout << "\n---------------- Projection to internal nodes 1-2/3 ----------------" << std::endl;
  subtraction.recursivelyEdit([](Node node) {
      Expression::BasicReduction(node.block());
    });
  print();
}
