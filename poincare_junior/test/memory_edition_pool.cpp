#include "print.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void testEditionPool() {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * pool = cachePool->editionPool();

  constexpr Tree k_expression = Mult(Add(1_sn, 2_sn), 3_n, 4_n);
  const Node handingNode = static_cast<Node>(k_expression);
  const Node editedNode = pool->initFromTree(handingNode);
  assert(pool->size() == handingNode.treeSize());
  assert(pool->numberOfTrees() == 1);

  // References
  assert(pool->nodeForIdentifier(pool->referenceNode(editedNode)) == editedNode);

  pool->flush();
  assert(pool->size() == 0);

  pool->pushBlock(ZeroBlock);
  pool->pushBlock(OneBlock);
  assert(*pool->firstBlock() == ZeroBlock && *(pool->lastBlock() - 1) == OneBlock && pool->size() == 2);
  pool->popBlock();
  assert(*(pool->lastBlock() - 1) == ZeroBlock && pool->size() == 1);
  pool->replaceBlock(pool->firstBlock(), TwoBlock);
  assert(*pool->blockAtIndex(0) == TwoBlock);
  pool->insertBlock(pool->firstBlock(), OneBlock);
  assert(*pool->firstBlock() == OneBlock && pool->size() == 2);
  pool->insertBlocks(pool->blockAtIndex(2), pool->blockAtIndex(0), 2);
  assert(*(pool->blockAtIndex(2)) == OneBlock && *(pool->blockAtIndex(3)) == TwoBlock && pool->size() == 4);
  pool->removeBlocks(pool->firstBlock(), 3);
  assert(*(pool->firstBlock()) == TwoBlock && pool->size() == 1);
  pool->pushBlock(ZeroBlock);
  pool->pushBlock(OneBlock);
  pool->pushBlock(HalfBlock);
  //[ 2 0 1 1/2 ]--> [ 2 1 1/2 0 ]
  pool->moveBlocks(pool->firstBlock() + 1, pool->blockAtIndex(2), 2);
  assert(*(pool->blockAtIndex(0)) == TwoBlock && *(pool->blockAtIndex(1)) == OneBlock && *(pool->blockAtIndex(2)) == HalfBlock && *(pool->blockAtIndex(3)) == ZeroBlock && pool->size() == 4);
  assert(pool->contains(pool->blockAtIndex(2)));
  assert(!pool->contains(pool->blockAtIndex(5)));
}

void testEditionReference() {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * editionPool = cachePool->editionPool();

  constexpr Tree k_expression0 = Mult(Add(1_sn, 2_sn), 3_n, 4_n);
  constexpr Tree k_expression1 = Pow(Sub(5_n, 6_n), 7_n);

  // Operator ==
  EditionReference reference0;
  EditionReference reference1;
  assert(reference0 == reference1);
  assert(reference0.isUninitialized());
  reference0 = EditionReference(k_expression0);
  assert(!reference0.isUninitialized());
  assert(reference0 != reference1);
  reference1 = EditionReference(reference0.node());
  assert(reference0 == reference1);
  reference1 = EditionReference(k_expression1);
  assert(reference0 != reference1);

  // Constructors
  EditionReference::Clone(reference0.node());
  EditionReference reference3 = EditionReference::Push<BlockType::IntegerShort>(8);
  assert_pool_contains(editionPool, {k_expression0, k_expression1, k_expression0, 8_n});

  // Insertions
  reference3.insertNodeAfterNode(9_n);
  assert_pool_contains(editionPool, {k_expression0, k_expression1, k_expression0, 8_n, 9_n});
  reference3.insertNodeAfterNode(EditionReference(10_n));
  assert_pool_contains(editionPool, {k_expression0, k_expression1, k_expression0, 8_n, 10_n, 9_n});
  reference3.insertTreeAfterNode(reference0);
  assert_pool_contains(editionPool, {k_expression1, k_expression0, 8_n, k_expression0, 10_n, 9_n});
  reference3.insertNodeBeforeNode(EditionReference(10_n));
  assert_pool_contains(editionPool, {k_expression1, k_expression0, 10_n, 8_n, k_expression0, 10_n, 9_n});
  reference3.insertTreeBeforeNode(reference1);
  assert_pool_contains(editionPool, {k_expression0, 10_n, k_expression1, 8_n, k_expression0, 10_n, 9_n});

  // Replacements
  reference3.replaceNodeByNode(11_n);
  assert_pool_contains(editionPool, {k_expression0, 10_n, k_expression1, 11_n, k_expression0, 10_n, 9_n});
  reference3.replaceNodeByTree(k_expression1);
  assert_pool_contains(editionPool, {k_expression0, 10_n, k_expression1, k_expression1, k_expression0, 10_n, 9_n});
  reference0.replaceTreeByNode(12_n);
  assert_pool_contains(editionPool, {k_expression0, 10_n, k_expression1, k_expression1, 12_n, 10_n, 9_n});
  reference1.replaceTreeByTree(k_expression0);
  assert_pool_contains(editionPool, {k_expression0, 10_n, k_expression0, k_expression1, 12_n, 10_n, 9_n});

  // Removals
  reference0.removeNode();
  assert_pool_contains(editionPool, {k_expression0, 10_n, k_expression0, k_expression1, 10_n, 9_n});
  reference1.removeTree();
  assert_pool_contains(editionPool, {k_expression0, 10_n, k_expression1, 10_n, 9_n});
}
