#include "print.h"
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/n_ary.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <quiz.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_elementary_tree_manipulation) {
  CachePool * cache = CachePool::sharedCachePool();
  EditionPool * editionPool = cache->editionPool();

  createSimpleExpression();

  log_edition_pool();
  log_cache_pool();

#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n--- Store (1+2)*3*4 ---" << std::endl;
#endif
  uint16_t treeId = cache->storeEditedTree();

  log_edition_pool();
  log_cache_pool();

#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n--- Edit (1+2)*3*4 ---" << std::endl;
#endif
  editionPool->initFromTree(cache->nodeForIdentifier(treeId));

  log_edition_pool();
  log_cache_pool();

#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n--- Develop (1+2)*3*4 ---" << std::endl;
#endif
  TypeBlock * root = editionPool->firstBlock();
  assert(root->type() == BlockType::Multiplication);
  Simplification::DistributeMultiplicationOverAddition(EditionReference(root));

  log_edition_pool();

#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n--- Store developped 1*3*4+2*3*4 ---" << std::endl;
#endif
  treeId = cache->storeEditedTree();

  log_edition_pool();
  log_cache_pool();

#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n--- Create 1-2/3 ---" << std::endl;
#endif
  EditionReference subtraction = EditionReference::Push<BlockType::Subtraction>();
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  EditionReference::Push<BlockType::Division>();
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(3));

  log_edition_pool();

#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n--- Projection to internal nodes 1-2/3 ---" << std::endl;
#endif
  subtraction.recursivelyEdit([](EditionReference reference) {
      Simplification::BasicReduction(reference);
    });
  log_edition_pool();

#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n--- Create 1+(2+3) ---" << std::endl;
#endif
  EditionReference addition = EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  log_edition_pool();

#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n--- Flatten 1+(2+3) ---" << std::endl;
#endif
  NAry::Flatten(addition);
  log_edition_pool();
}
