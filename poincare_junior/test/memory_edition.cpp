#include "print.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_edition_pool) {
  CachePool * cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  EditionPool * pool = cachePool->editionPool();

  constexpr Tree k_expression = Mult(Add("1"_e, "2"_e), "3"_e, "4"_e);
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

QUIZ_CASE(pcj_edition_reference) {
  CachePool * cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  EditionPool * editionPool = cachePool->editionPool();

  constexpr Tree k_expression0 = Mult(Add("1"_e, "2"_e), "3"_e, "4"_e);
  constexpr Tree k_expression1 = Pow(Sub("5"_e, "6"_e), "7"_e);

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
  EditionReference::Clone(reference0);
  EditionReference reference3 = EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(8));
  assert_pool_contains(editionPool, {k_expression0, k_expression1, k_expression0, "8"_e});

  // Insertions
  reference3.insertNodeAfterNode("9"_e);
  assert_pool_contains(editionPool, {k_expression0, k_expression1, k_expression0, "8"_e, "9"_e});
  reference3.insertNodeAfterNode(EditionReference("10"_e));
  assert_pool_contains(editionPool, {k_expression0, k_expression1, k_expression0, "8"_e, "10"_e, "9"_e});
  reference3.insertTreeAfterNode(reference0);
  assert_pool_contains(editionPool, {k_expression1, k_expression0, "8"_e, k_expression0, "10"_e, "9"_e});
  reference3.insertNodeBeforeNode(EditionReference("10"_e));
  assert_pool_contains(editionPool, {k_expression1, k_expression0, "10"_e, "8"_e, k_expression0, "10"_e, "9"_e});
  reference3.insertTreeBeforeNode(reference1);
  assert_pool_contains(editionPool, {k_expression0, "10"_e, k_expression1, "8"_e, k_expression0, "10"_e, "9"_e});

  // Replacements
  reference3.replaceNodeByNode(reference3);
  assert_pool_contains(editionPool, {k_expression0, "10"_e, k_expression1, "8"_e, k_expression0, "10"_e, "9"_e});
  reference3.replaceNodeByNode("11"_e);
  assert_pool_contains(editionPool, {k_expression0, "10"_e, k_expression1, "11"_e, k_expression0, "10"_e, "9"_e});
  reference3.replaceNodeByTree(k_expression1);
  assert_pool_contains(editionPool, {k_expression0, "10"_e, k_expression1, k_expression1, k_expression0, "10"_e, "9"_e});
  reference0.replaceTreeByNode("12"_e);
  assert_pool_contains(editionPool, {k_expression0, "10"_e, k_expression1, k_expression1, "12"_e, "10"_e, "9"_e});
  reference1.replaceTreeByTree(k_expression0);
  assert_pool_contains(editionPool, {k_expression0, "10"_e, k_expression0, k_expression1, "12"_e, "10"_e, "9"_e});

  // Removals
  reference0.removeNode();
  assert_pool_contains(editionPool, {k_expression0, "10"_e, k_expression0, k_expression1, "10"_e, "9"_e});
  reference1.removeTree();
  assert_pool_contains(editionPool, {k_expression0, "10"_e, k_expression1, "10"_e, "9"_e});

  // Detach
  reference3.childAtIndex(0).detachTree();
  reference3.insertTreeAfterNode("13"_e);
  assert_pool_contains(editionPool, {k_expression0, "10"_e, Pow("13"_e, "7"_e), "10"_e, "9"_e, Sub("5"_e, "6"_e)});
  reference3.childAtIndex(1).detachNode();
  reference3.insertTreeAfterNode("14"_e);
  assert_pool_contains(editionPool, {k_expression0, "10"_e, Pow("14"_e, "13"_e), "10"_e, "9"_e, Sub("5"_e, "6"_e), "7"_e});
}

QUIZ_CASE(pcj_edition_reference_reallocation) {
  CachePool::sharedCachePool()->reset();
  constexpr Tree k_expression = "1"_e;

  EditionReference reference0("0"_e);
  for (size_t i = 0; i < EditionPool::k_maxNumberOfReferences - 1; i++) {
    EditionReference reference1("1"_e);
  }
  /* The reference table is now full but we can reference a new node of another
   * one is out-dated. */
  reference0.removeTree();
  EditionReference reference2("2"_e);
}
