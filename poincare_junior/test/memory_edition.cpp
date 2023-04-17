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
  CachePool::sharedCachePool()->reset();
  EditionPool* editionPool = EditionPool::sharedEditionPool();

  constexpr Tree k_expr0 = KMult(KAdd(1_e, 2_e), 3_e, 4_e);
  constexpr Tree k_subExpr1 = 6_e;
  constexpr Tree k_expr1 = KPow(KSub(5_e, k_subExpr1), 7_e);

  // Operator ==
  EditionReference ref0;
  EditionReference ref1;
  assert(ref0 == ref1);
  assert(ref0.isUninitialized());
  ref0 = EditionReference(k_expr0);
  assert(!ref0.isUninitialized());
  assert(ref0 != ref1);
  ref1 = EditionReference(ref0);
  assert(ref0 == ref1);
  ref1 = EditionReference(k_expr1);
  assert(ref0 != ref1);

  // Constructors
  editionPool->clone(ref0);
  EditionReference ref3 = EditionReference(
      editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(8)));
  assert_pool_contains(editionPool, {k_expr0, k_expr1, k_expr0, 8_e});

  // Insertions
  ref3.insertNodeAfterNode(9_e);
  assert_pool_contains(editionPool, {k_expr0, k_expr1, k_expr0, 8_e, 9_e});
  ref3.insertNodeAfterNode(EditionReference(10_e));
  assert_pool_contains(editionPool,
                       {k_expr0, k_expr1, k_expr0, 8_e, 10_e, 9_e});
  ref3.insertTreeAfterNode(ref0);
  assert_pool_contains(editionPool,
                       {k_expr1, k_expr0, 8_e, k_expr0, 10_e, 9_e});
  ref3.insertNodeBeforeNode(EditionReference(10_e));
  assert_pool_contains(editionPool,
                       {k_expr1, k_expr0, 10_e, 8_e, k_expr0, 10_e, 9_e});
  ref3.insertTreeBeforeNode(ref1);
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, 8_e, k_expr0, 10_e, 9_e});

  // Replacements
  ref3.replaceNodeByNode(ref3);
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, 8_e, k_expr0, 10_e, 9_e});
  // TODO: Restore dangling reference on replaceBy
  ref3 = ref3.replaceNodeByNode(11_e);
  // quiz_assert(static_cast<Node>(ref3).isUninitialized());
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, 11_e, k_expr0, 10_e, 9_e});

  EditionReference ref5 = ref3.replaceNodeByTree(k_expr1);
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, k_expr1, k_expr0, 10_e, 9_e});

  EditionReference subref5(ref5.childAtIndex(0).childAtIndex(1));
  assert_trees_are_equal(subref5, k_subExpr1);
  ref5.replaceTreeByTree(subref5);
  assert_pool_contains(
      editionPool, {k_expr0, 10_e, k_expr1, k_subExpr1, k_expr0, 10_e, 9_e});

  EditionReference ref6(
      editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(12)));
  ref5.insertTreeBeforeNode(ref6);
  EditionReference ref7(
      editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(13)));
  ref5.insertTreeBeforeNode(ref7);

  assert_pool_contains(editionPool, {k_expr0, 10_e, k_expr1, 12_e, 13_e,
                                     k_subExpr1, k_expr0, 10_e, 9_e});

  ref7.replaceTreeByTree(ref6);
  assert_pool_contains(editionPool, {k_expr0, 10_e, k_expr1, 12_e, k_subExpr1,
                                     k_expr0, 10_e, 9_e});

  ref6.replaceTreeByTree(ref5);
  assert_pool_contains(
      editionPool, {k_expr0, 10_e, k_expr1, k_subExpr1, k_expr0, 10_e, 9_e});

  ref5.replaceTreeByTree(k_expr1);
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, k_expr1, k_expr0, 10_e, 9_e});

  ref0.replaceTreeByNode(12_e);
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, k_expr1, 12_e, 10_e, 9_e});

  EditionReference subRef1(ref1.childAtIndex(0).childAtIndex(1));
  assert_trees_are_equal(subRef1, k_subExpr1);

  EditionReference ref8(k_expr0);
  ref1.replaceTreeByTree(ref8);
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr0, k_expr1, 12_e, 10_e, 9_e});
  quiz_assert(subRef1.isUninitialized());

  // Removals
  ref0.removeNode();
  quiz_assert(ref0.isUninitialized());
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr0, k_expr1, 10_e, 9_e});
  ref8.removeTree();
  assert_pool_contains(editionPool, {k_expr0, 10_e, k_expr1, 10_e, 9_e});

  // Detach
  EditionReference(ref5.childAtIndex(0)).detachTree();
  ref5.insertTreeAfterNode(13_e);
  assert_pool_contains(
      editionPool, {k_expr0, 10_e, KPow(13_e, 7_e), 10_e, 9_e, KSub(5_e, 6_e)});
  EditionReference(ref5.childAtIndex(1)).detachNode();
  ref5.insertTreeAfterNode(14_e);
  assert_pool_contains(editionPool, {k_expr0, 10_e, KPow(14_e, 13_e), 10_e, 9_e,
                                     KSub(5_e, 6_e), 7_e});
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
