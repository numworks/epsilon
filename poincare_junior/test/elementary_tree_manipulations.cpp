#include "print.h"
#include <poincare_junior/edition_reference.h>
#include <poincare_junior/node_iterator.h>

using namespace Poincare;

void elementaryTreeManipulation() {
  CachePool * cache = CachePool::sharedCachePool();
  EditionPool * editionPool = cache->editionPool();

  std::cout << "\n---------------- Create (1 + 2) * 3 * 4 ----------------" << std::endl;
  TypeBlock * root = MultiplicationInterface::PushNode(3);
  AdditionInterface::PushNode(2);
  IntegerInterface::PushNode(1);
  IntegerInterface::PushNode(2);
  IntegerInterface::PushNode(3);
  IntegerInterface::PushNode(4);

  print();

  std::cout << "\n---------------- Store (1+2)*3*4 ----------------" << std::endl;
  uint16_t treeId = cache->storeEditedTree();
  print();

  std::cout << "\n---------------- Edit (1+2)*3*4 ----------------" << std::endl;
  editionPool->initFromTree(cache->nodeForIdentifier(treeId));
  print();


  std::cout << "\n---------------- Develop (1+2)*3*4 ----------------" << std::endl;
  root = editionPool->firstBlock();
  assert(root->type() == BlockType::Multiplication);
  MultiplicationExpressionInterface::DistributeOverAddition(root);
  print();

  std::cout << "\n---------------- Store developped 1*3*4+2*3*4 ----------------" << std::endl;
  treeId = cache->storeEditedTree();
  print();

  std::cout << "\n---------------- Create 1-2/3 ----------------" << std::endl;
  TypeBlock * subtraction = SubtractionInterface::PushNode();
  IntegerInterface::PushNode(1);
  DivisionInterface::PushNode();
  IntegerInterface::PushNode(2);
  IntegerInterface::PushNode(3);
  print();

  std::cout << "\n---------------- Projection to internal nodes 1-2/3 ----------------" << std::endl;
  Node(subtraction).recursivelyEdit([](Node node) {
      node.expressionInterface()->basicReduction(node.block()); });
  print();
}
