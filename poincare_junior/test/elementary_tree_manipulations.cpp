#include "print.h"
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/n_ary.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node_iterator.h>

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
  Simplification::DistributeMultiplicationOverAddition(EditionReference(root));
  print();

  std::cout << "\n---------------- Store developped 1*3*4+2*3*4 ----------------" << std::endl;
  treeId = cache->storeEditedTree();
  print();

  std::cout << "\n---------------- Create 1-2/3 ----------------" << std::endl;
  Node subtraction = Node::Push<BlockType::Subtraction>();
  Node::Push<BlockType::IntegerShort>(1);
  Node::Push<BlockType::Division>();
  Node::Push<BlockType::IntegerShort>(2);
  Node::Push<BlockType::IntegerShort>(3);
  print();

  std::cout << "\n---------------- Projection to internal nodes 1-2/3 ----------------" << std::endl;
  EditionReference(subtraction).recursivelyEdit([](EditionReference reference) {
      Simplification::BasicReduction(reference);
    });
  print();

  std::cout << "\n---------------- Create 1+(2+3) ----------------" << std::endl;
  Node addition = Node::Push<BlockType::Addition>(2);
  Node::Push<BlockType::IntegerShort>(1);
  Node::Push<BlockType::Addition>(2);
  Node::Push<BlockType::IntegerShort>(2);
  Node::Push<BlockType::IntegerShort>(3);
  print();

  std::cout << "\n---------------- Flatten 1+(2+3) ----------------" << std::endl;
  NAry::Flatten(EditionReference(addition));
  print();
}
