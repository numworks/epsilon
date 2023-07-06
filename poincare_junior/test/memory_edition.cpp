#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_edition_pool) {
  CachePool* cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  EditionPool* pool = cachePool->editionPool();

  constexpr KTree k_expression = KMult(KAdd(1_e, 2_e), 3_e, 4_e);
  const Tree* handingNode = k_expression;
  Tree* editedNode = pool->clone(handingNode);
  assert(pool->size() == handingNode->treeSize());
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

  constexpr KTree k_expr0 = KMult(KAdd(1_e, 2_e), 3_e, 4_e);
  constexpr KTree k_subExpr1 = 6_e;
  constexpr KTree k_expr1 = KPow(KSub(5_e, k_subExpr1), 7_e);

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
  EditionReference ref2 = EditionReference(
      editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(8)));
  assert_pool_contains(editionPool, {k_expr0, k_expr1, k_expr0, 8_e});

  // Insertions
  ref2->cloneNodeAfterNode(9_e);
  assert_pool_contains(editionPool, {k_expr0, k_expr1, k_expr0, 8_e, 9_e});
  ref2->cloneNodeAfterNode(10_e);
  assert_pool_contains(editionPool,
                       {k_expr0, k_expr1, k_expr0, 8_e, 10_e, 9_e});
  ref2->moveTreeAfterNode(ref0);
  assert_pool_contains(editionPool,
                       {k_expr1, k_expr0, 8_e, k_expr0, 10_e, 9_e});
  ref2->cloneNodeBeforeNode(10_e);
  assert_pool_contains(editionPool,
                       {k_expr1, k_expr0, 10_e, 8_e, k_expr0, 10_e, 9_e});
  ref2->moveTreeBeforeNode(ref1);
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, 8_e, k_expr0, 10_e, 9_e});

  // Replacements
  ref0 = ref2;  // 8_e
  assert_trees_are_equal(ref0, 8_e);
  ref1 = ref0->nextTree();  // k_expr0
  ref2 = ref1->nextTree();  // 10_e

  // Replacements by same
  ref2 = ref2->moveTreeOverNode(ref2);
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, 8_e, k_expr0, 10_e, 9_e});

  // Replacements from nodes outside of the EditionPool
  ref0 = ref0->cloneNodeOverNode(9_e);  // Same size
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, 9_e, k_expr0, 10_e, 9_e});
  ref1 = ref1->cloneNodeOverTree(10_e);  // Smaller size
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, 9_e, 10_e, 10_e, 9_e});
  ref2 = ref2->cloneTreeOverNode(k_expr1);  // Bigger size
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, 9_e, 10_e, k_expr1, 9_e});

  // Replacements from nodes living in the EditionPool
  EditionReference subRef0(ref2->childAtIndex(0)->childAtIndex(1));
  EditionReference subRef1(ref2->childAtIndex(0));
  ref2 = ref2->moveTreeOverTree(subRef0);  // Child
  quiz_assert(subRef1.isUninitialized());
  ref1->moveNodeOverNode(ref0);  // Before
  assert_pool_contains(editionPool,
                       {k_expr0, 10_e, k_expr1, 9_e, k_subExpr1, 9_e});
  ref0->moveTreeOverTree(ref2);  // After
  assert_pool_contains(editionPool, {k_expr0, 10_e, k_expr1, k_subExpr1, 9_e});

  // Removals
  ref0 = EditionReference(editionPool->firstBlock());  // k_expr0
  ref1 = ref0->nextTree();                             // 10_e
  ref2 = ref1->nextTree();                             // k_expr1

  ref2->removeTree();
  ref1->removeNode();
  assert_pool_contains(editionPool, {k_expr0, k_subExpr1, 9_e});

  // Detach
  subRef0 = EditionReference(ref0->childAtIndex(0));
  subRef0->cloneTreeBeforeNode(13_e);
  subRef0->detachTree();
  assert_pool_contains(
      editionPool, {KMult(13_e, 3_e, 4_e), k_subExpr1, 9_e, KAdd(1_e, 2_e)});
}

QUIZ_CASE(pcj_edition_reference_reallocation) {
  CachePool::sharedCachePool()->reset();
  EditionReference reference0(KAdd(1_e, 1_e));
  EditionReference referenceSub0(reference0->childAtIndex(0));
  EditionReference referenceSub1(reference0->childAtIndex(1));
  for (size_t i = 0; i < EditionPool::k_maxNumberOfReferences - 5; i++) {
    EditionReference reference1(1_e);
  }
  /* The reference table is now full but we can reference a new node of another
   * one is out-dated. */
  reference0->removeTree();
  assert(referenceSub0.isUninitialized());
  EditionReference reference2(2_e);
}
