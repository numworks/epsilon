#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/simplification.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_elementary_tree_manipulation) {
  CachePool* cache = CachePool::sharedCachePool();
  reset_pools();

  // Create (1 + 2) * 3 * (4 + 5)
  const Tree* simpleExpression = KMult(KAdd(1_e, 2_e), 3_e, KAdd(4_e, 5_e));
  Tree* createdExpr = SharedEditionPool->push<BlockType::Multiplication>(3);
  SharedEditionPool->push<BlockType::Addition>(2);
  SharedEditionPool->push<BlockType::One>();
  SharedEditionPool->push<BlockType::Two>();
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  SharedEditionPool->push<BlockType::Addition>(2);
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(4));
  SharedEditionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(5));

  quiz_assert(cache->numberOfTrees() == 0);
  quiz_assert(SharedEditionPool->numberOfTrees() == 1);
  assert_trees_are_equal(createdExpr, simpleExpression);

  // Store (1 + 2) * 3 * (4 + 5)
  uint16_t treeId = cache->storeEditedTree();

  quiz_assert(cache->numberOfTrees() == 1);
  quiz_assert(SharedEditionPool->numberOfTrees() == 0);
  assert_trees_are_equal(cache->nodeForIdentifier(treeId), simpleExpression);

  // Edit (1 + 2) * 3 * (4 + 5)
  Tree* editedExpr = SharedEditionPool->clone(cache->nodeForIdentifier(treeId));

  quiz_assert(cache->numberOfTrees() == 1);
  quiz_assert(SharedEditionPool->numberOfTrees() == 1);
  assert_trees_are_equal(editedExpr, simpleExpression);
  quiz_assert(Tree::FromBlocks(SharedEditionPool->firstBlock()) == editedExpr);

  // Simplify (1 + 2) * 3 * (4 + 5)
  Simplification::Simplify(editedExpr);

  quiz_assert(cache->numberOfTrees() == 1);
  quiz_assert(SharedEditionPool->numberOfTrees() == 1);
  assert_trees_are_equal(editedExpr, 81_e);

  // Store simplified 81
  treeId = cache->storeEditedTree();

  quiz_assert(cache->numberOfTrees() == 2);
  quiz_assert(SharedEditionPool->numberOfTrees() == 0);
  assert_trees_are_equal(cache->nodeForIdentifier(treeId), 81_e);
}
