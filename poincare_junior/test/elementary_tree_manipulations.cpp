#include "print.h"
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/n_ary.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node_iterator.h>

using namespace PoincareJ;

inline EditionReference createSimpleExpression() {
  std::cout << "\n---------------- Create (1 + 2) * 3 * 4 ----------------" << std::endl;
  EditionReference multiplication = EditionReference::Push<BlockType::Multiplication>(3);
  EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(4));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  return multiplication;
}

void elementaryTreeManipulation() {
  CachePool * cache = CachePool::sharedCachePool();
  EditionPool * editionPool = cache->editionPool();

  createSimpleExpression();

  log_edition_pool();
  log_cache_pool();

  std::cout << "\n---------------- Store (1+2)*3*4 ----------------" << std::endl;
  uint16_t treeId = cache->storeEditedTree();

  log_edition_pool();
  log_cache_pool();

  std::cout << "\n---------------- Edit (1+2)*3*4 ----------------" << std::endl;
  editionPool->initFromTree(cache->nodeForIdentifier(treeId));

  log_edition_pool();
  log_cache_pool();

  std::cout << "\n---------------- Develop (1+2)*3*4 ----------------" << std::endl;
  TypeBlock * root = editionPool->firstBlock();
  assert(root->type() == BlockType::Multiplication);
  Simplification::DistributeMultiplicationOverAddition(EditionReference(root));

  log_edition_pool();

  std::cout << "\n---------------- Store developped 1*3*4+2*3*4 ----------------" << std::endl;
  treeId = cache->storeEditedTree();

  log_edition_pool();
  log_cache_pool();

  std::cout << "\n---------------- Create 1-2/3 ----------------" << std::endl;
  EditionReference subtraction = EditionReference::Push<BlockType::Subtraction>();
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  EditionReference::Push<BlockType::Division>();
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(3));

  log_edition_pool();

  std::cout << "\n---------------- Projection to internal nodes 1-2/3 ----------------" << std::endl;
  subtraction.recursivelyEdit([](EditionReference reference) {
      Simplification::BasicReduction(reference);
    });
  log_edition_pool();

  std::cout << "\n---------------- Create 1+(2+3) ----------------" << std::endl;
  EditionReference addition = EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  log_edition_pool();

  std::cout << "\n---------------- Flatten 1+(2+3) ----------------" << std::endl;
  NAry::Flatten(addition);
  log_edition_pool();
}
