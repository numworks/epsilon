#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_edition_pool) {
  CachePool* cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  EditionPool* pool = cachePool->editionPool();

  constexpr Tree k_expression = KMult(KAdd(1_e, 2_e), 3_e, 4_e);
  const Node handingNode = static_cast<Node>(k_expression);
  const Node editedNode = pool->clone(handingNode);
  assert(pool->size() == handingNode.treeSize());
  assert(pool->numberOfTrees() == 1);

  // References
  assert(pool->nodeForIdentifier(pool->referenceNode(editedNode)) ==
         editedNode);

  pool->flush();
  assert(pool->size() == 0);

  pool->pushBlock(ZeroBlock);
  pool->pushBlock(OneBlock);
  assert(*pool->firstBlock() == ZeroBlock &&
         *(pool->lastBlock() - 1) == OneBlock && pool->size() == 2);
  pool->popBlock();
  assert(*(pool->lastBlock() - 1) == ZeroBlock && pool->size() == 1);
  pool->replaceBlock(pool->firstBlock(), TwoBlock);
  assert(*pool->blockAtIndex(0) == TwoBlock);
  pool->insertBlock(pool->firstBlock(), OneBlock);
  assert(*pool->firstBlock() == OneBlock && pool->size() == 2);
  pool->insertBlocks(pool->blockAtIndex(2), pool->blockAtIndex(0), 2);
  assert(*(pool->blockAtIndex(2)) == OneBlock &&
         *(pool->blockAtIndex(3)) == TwoBlock && pool->size() == 4);
  pool->removeBlocks(pool->firstBlock(), 3);
  assert(*(pool->firstBlock()) == TwoBlock && pool->size() == 1);
  pool->pushBlock(ZeroBlock);
  pool->pushBlock(OneBlock);
  pool->pushBlock(HalfBlock);
  //[ 2 0 1 1/2 ]--> [ 2 1 1/2 0 ]
  pool->moveBlocks(pool->firstBlock() + 1, pool->blockAtIndex(2), 2);
  assert(*(pool->blockAtIndex(0)) == TwoBlock &&
         *(pool->blockAtIndex(1)) == OneBlock &&
         *(pool->blockAtIndex(2)) == HalfBlock &&
         *(pool->blockAtIndex(3)) == ZeroBlock && pool->size() == 4);
  assert(pool->contains(pool->blockAtIndex(2)));
  assert(!pool->contains(pool->blockAtIndex(5)));
}

QUIZ_CASE(pcj_edition_reference) {
  CachePool* cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  EditionPool* editionPool = cachePool->editionPool();

  constexpr Tree k_expression0 = KMult(KAdd(1_e, 2_e), 3_e, 4_e);
  constexpr Tree k_subExpression1 = 6_e;
  constexpr Tree k_expression1 = KPow(KSub(5_e, k_subExpression1), 7_e);

  // Operator ==
  EditionReference reference0;
  EditionReference reference1;
  assert(reference0 == reference1);
  assert(reference0.isUninitialized());
  reference0 = EditionReference(k_expression0);
  assert(!reference0.isUninitialized());
  assert(reference0 != reference1);
  reference1 = EditionReference(reference0);
  assert(reference0 == reference1);
  reference1 = EditionReference(k_expression1);
  assert(reference0 != reference1);

  // Constructors
  editionPool->clone(reference0);
  EditionReference reference3 = EditionReference(
      editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(8)));
  assert_pool_contains(editionPool,
                       {k_expression0, k_expression1, k_expression0, 8_e});

  // Insertions
  reference3.insertNodeAfterNode(9_e);
  assert_pool_contains(editionPool,
                       {k_expression0, k_expression1, k_expression0, 8_e, 9_e});
  reference3.insertNodeAfterNode(EditionReference(10_e));
  assert_pool_contains(editionPool, {k_expression0, k_expression1,
                                     k_expression0, 8_e, 10_e, 9_e});
  reference3.insertTreeAfterNode(reference0);
  assert_pool_contains(editionPool, {k_expression1, k_expression0, 8_e,
                                     k_expression0, 10_e, 9_e});
  reference3.insertNodeBeforeNode(EditionReference(10_e));
  assert_pool_contains(editionPool, {k_expression1, k_expression0, 10_e, 8_e,
                                     k_expression0, 10_e, 9_e});
  reference3.insertTreeBeforeNode(reference1);
  assert_pool_contains(editionPool, {k_expression0, 10_e, k_expression1, 8_e,
                                     k_expression0, 10_e, 9_e});

  // Replacements
  reference3.replaceNodeByNode(reference3);
  assert_pool_contains(editionPool, {k_expression0, 10_e, k_expression1, 8_e,
                                     k_expression0, 10_e, 9_e});
  // TODO: Restore dangling reference on replaceBy
  reference3 = reference3.replaceNodeByNode(11_e);
  // quiz_assert(static_cast<Node>(reference3).isUninitialized());
  assert_pool_contains(editionPool, {k_expression0, 10_e, k_expression1, 11_e,
                                     k_expression0, 10_e, 9_e});
  EditionReference reference5 = reference3.replaceNodeByTree(k_expression1);
  assert_pool_contains(editionPool, {k_expression0, 10_e, k_expression1,
                                     k_expression1, k_expression0, 10_e, 9_e});

  EditionReference subReference5(reference5.childAtIndex(0).childAtIndex(1));
  assert_trees_are_equal(subReference5, k_subExpression1);
  reference5.replaceTreeByTree(subReference5);
  assert_pool_contains(editionPool,
                       {k_expression0, 10_e, k_expression1, k_subExpression1,
                        k_expression0, 10_e, 9_e});

  EditionReference reference6 =
      editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(12));
  reference5.insertTreeBeforeNode(reference6);
  EditionReference reference7 =
      editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(13));
  reference5.insertTreeBeforeNode(reference7);

  assert_pool_contains(editionPool,
                       {k_expression0, 10_e, k_expression1, 12_e, 13_e,
                        k_subExpression1, k_expression0, 10_e, 9_e});

  reference7.replaceTreeByTree(reference6);
  assert_pool_contains(
      editionPool, {k_expression0, 10_e, k_expression1, 12_e, k_subExpression1,
                    k_expression0, 10_e, 9_e});

  reference6.replaceTreeByTree(reference5);
  assert_pool_contains(editionPool,
                       {k_expression0, 10_e, k_expression1, k_subExpression1,
                        k_expression0, 10_e, 9_e});

  reference5.replaceTreeByTree(k_expression1);
  assert_pool_contains(editionPool, {k_expression0, 10_e, k_expression1,
                                     k_expression1, k_expression0, 10_e, 9_e});

  reference0.replaceTreeByNode(12_e);
  assert_pool_contains(editionPool, {k_expression0, 10_e, k_expression1,
                                     k_expression1, 12_e, 10_e, 9_e});

  EditionReference subReference1(reference1.childAtIndex(0).childAtIndex(1));
  assert_trees_are_equal(subReference1, k_subExpression1);

  EditionReference reference8(k_expression0);
  reference1.replaceTreeByTree(reference8);
  assert_pool_contains(editionPool, {k_expression0, 10_e, k_expression0,
                                     k_expression1, 12_e, 10_e, 9_e});
  quiz_assert(subReference1.isUninitialized());

  // Removals
  reference0.removeNode();
  quiz_assert(static_cast<Node>(reference0).isUninitialized());
  assert_pool_contains(editionPool, {k_expression0, 10_e, k_expression0,
                                     k_expression1, 10_e, 9_e});
  reference8.removeTree();
  assert_pool_contains(editionPool,
                       {k_expression0, 10_e, k_expression1, 10_e, 9_e});

  // Detach
  EditionReference(reference5.childAtIndex(0)).detachTree();
  reference5.insertTreeAfterNode(13_e);
  assert_pool_contains(editionPool, {k_expression0, 10_e, KPow(13_e, 7_e), 10_e,
                                     9_e, KSub(5_e, 6_e)});
  EditionReference(reference5.childAtIndex(1)).detachNode();
  reference5.insertTreeAfterNode(14_e);
  assert_pool_contains(editionPool, {k_expression0, 10_e, KPow(14_e, 13_e),
                                     10_e, 9_e, KSub(5_e, 6_e), 7_e});
}

QUIZ_CASE(pcj_edition_reference_reallocation) {
  CachePool::sharedCachePool()->reset();
  constexpr Tree k_expression = 1_e;

  EditionReference reference0(0_e);
  for (size_t i = 0; i < EditionPool::k_maxNumberOfReferences - 1; i++) {
    EditionReference reference1(1_e);
  }
  /* The reference table is now full but we can reference a new node of another
   * one is out-dated. */
  reference0.removeTree();
  EditionReference reference2(2_e);
}
