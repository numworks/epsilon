#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include "helper.h"

using namespace PoincareJ;

static constexpr Tree bigTree = KAdd(3_e, 4_e);
static constexpr Tree smallTree = 4_e;

void execute_push_tree_and_modify() {
  PoincareJ::Reference::InitializerFromTreeInplace treeModifier =
      [](Node *tree) {
        EditionReference(tree).replaceNodeByNode(
            EditionPool::sharedEditionPool()->push<BlockType::Multiplication>(
                2));
      };
  EditionPool::sharedEditionPool()->executeAndCache(
      [](void *context, const void *data) {
        Node *editedTree =
            EditionPool::sharedEditionPool()->initFromAddress(data);
        return (
            reinterpret_cast<PoincareJ::Reference::InitializerFromTreeInplace>(
                context))(editedTree);
      },
      reinterpret_cast<void *>(treeModifier), &bigTree.k_blocks);
}

QUIZ_CASE(pcj_cache_pool) {
  CachePool *cachePool = CachePool::sharedCachePool();
  EditionPool *editionPool = cachePool->editionPool();
  size_t treeSize = static_cast<const Node *>(bigTree)->treeSize();
  cachePool->reset();

  // storeEditedTree
  editionPool->clone(bigTree);
  assert_pools_tree_sizes_are(0, 1);
  cachePool->storeEditedTree();
  assert_pools_tree_sizes_are(1, 0);

  // freeBlocks
  editionPool->clone(bigTree);
  cachePool->storeEditedTree();
  assert_pools_tree_sizes_are(2, 0);
  cachePool->freeBlocks(1);
  assert_pools_tree_sizes_are(1, 0);
  cachePool->freeBlocks(treeSize - 1);
  assert_pools_tree_sizes_are(0, 0);
  for (int i = 0; i < 3; i++) {
    editionPool->clone(bigTree);
    cachePool->storeEditedTree();
  }
  assert_pools_tree_sizes_are(3, 0);
  cachePool->freeBlocks(treeSize + 1);
  assert_pools_tree_sizes_are(1, 0);
  editionPool->clone(bigTree);
  assert_pools_tree_sizes_are(1, 1);
  EditionReference ref(editionPool->firstBlock());
  cachePool->freeBlocks(1, false);
  assert_trees_are_equal(ref, bigTree);
  assert_pools_tree_sizes_are(0, 1);
  cachePool->storeEditedTree();
  assert_pools_tree_sizes_are(1, 0);

  // reset
  editionPool->clone(bigTree);
  assert_pools_tree_sizes_are(1, 1);
  cachePool->reset();
  assert_pools_tree_sizes_are(0, 0);

  // execute
  execute_push_tree_and_modify();
  assert_pool_contains(cachePool, {KMult(3_e, 4_e)});
  assert_pools_tree_sizes_are(1, 0);
}

QUIZ_CASE(pcj_cache_pool_limits) {
  CachePool *cachePool = CachePool::sharedCachePool();
  EditionPool *editionPool = cachePool->editionPool();
  cachePool->reset();
  size_t treeSize = static_cast<const Node *>(bigTree)->treeSize();

  /* test overflowing the edition pool */
  // 1. Almost fill the whole cache
  // Fill the cache
  size_t maxNumberOfTreesInCache = CachePool::k_maxNumberOfBlocks / treeSize;
  for (int i = 0; i < maxNumberOfTreesInCache; i++) {
    editionPool->clone(bigTree);
    cachePool->storeEditedTree();
  }
  assert_pools_tree_sizes_are(maxNumberOfTreesInCache, 0);

  // 2. Edit another tree triggering a cache invalidation
  execute_push_tree_and_modify();
  assert(cachePool->numberOfTrees() <= maxNumberOfTreesInCache);
  Node *lastTree = Node::FromBlocks(cachePool->lastBlock())->previousTree();
  assert_trees_are_equal(lastTree, KMult(3_e, 4_e));

  /* test overflowing the cache identifier */
  cachePool->reset();
  // 1. Fill the cache with the max number of identifiers
  for (int i = 0; i < CachePool::k_maxNumberOfReferences; i++) {
    editionPool->clone(smallTree);
    cachePool->storeEditedTree();
  }
  assert_pools_tree_sizes_are(CachePool::k_maxNumberOfReferences, 0);
  // 2. Edit and cache a new tree triggering a cache invalidation
  execute_push_tree_and_modify();
  assert_pools_tree_sizes_are(CachePool::k_maxNumberOfReferences, 0);
}

void assert_check_cache_reference(
    Reference reference, std::initializer_list<const Node *> cacheTrees) {
  CachePool *cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  assert_pools_tree_sizes_are(0, 0);
  reference.send([](const Node *tree, void *result) {}, nullptr);
  assert_pool_contains(cachePool, cacheTrees);
  assert_pools_tree_sizes_are(cacheTrees.size(), 0);
  cachePool->reset();
}

QUIZ_CASE(pcj_cache_references) {
  // Constructors
  Reference reference0([]() {
    EditionPool::sharedEditionPool()->push<BlockType::IntegerShort>(
        static_cast<int8_t>(4));
  });
  assert_check_cache_reference(reference0, {4_e});

  Reference reference1(
      [](Node *node) { EditionReference(node).replaceNodeByNode(5_e); },
      static_cast<const Node *>(smallTree)->block());
  assert_check_cache_reference(reference1, {5_e});

  Reference reference2(
      [](Node *node) {
        EditionReference ref(node);
        ref.insertNodeBeforeNode(
            EditionPool::sharedEditionPool()->push<BlockType::Addition>(2));
        ref.insertNodeAfterNode(6_e);
      },
      &reference1);
  assert_check_cache_reference(reference2, {5_e, KAdd(5_e, 6_e)});

  Reference reference3(
      [](const char *string) {
        EditionPool::sharedEditionPool()->push<BlockType::Addition>(string[0] -
                                                                    '0');
      },
      "0");
  assert_check_cache_reference(reference3, {KAdd()});
}

void check_reference_invalidation_and_reconstruction(Reference reference,
                                                     uint16_t identifier,
                                                     Node *node) {
  CachePool *cachePool = CachePool::sharedCachePool();
  // reference has been invalidated
  quiz_assert(!cachePool->nodeForIdentifier(identifier));
  // reference is regenerated on demand
  reference.send([](const Node *tree, void *result) {}, nullptr);
  assert_trees_are_equal(
      Node::FromBlocks(cachePool->lastBlock())->previousTree(), node);
}

void fill_cache_and_assert_invalidation(int maxNumberOfTreesInCache,
                                        const Node *tree) {
  CachePool *cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  Reference firstReference([]() {
    EditionPool::sharedEditionPool()->push<BlockType::IntegerShort>(
        static_cast<int8_t>(28));
  });
  assert_pools_tree_sizes_are(0, 0);
  firstReference.send([](const Node *tree, void *result) {}, nullptr);
  uint16_t identifier = firstReference.id();
  // Fill the cache
  for (int i = 1; i < maxNumberOfTreesInCache; i++) {
    assert_pools_tree_sizes_are(i, 0);
    Reference([](Node *node) {}, tree->block())
        .send([](const Node *tree, void *result) {}, nullptr);
  }
  assert_pools_tree_sizes_are(maxNumberOfTreesInCache, 0);
  // Sending this reference will require freeing cache nodes
  Reference([](Node *node) {}, tree->block())
      .send([](const Node *tree, void *result) {}, nullptr);
  quiz_assert(cachePool->numberOfTrees() <= maxNumberOfTreesInCache);
  check_reference_invalidation_and_reconstruction(firstReference, identifier,
                                                  28_e);
  cachePool->reset();
}

QUIZ_CASE(pcj_cache_reference_invalidation) {
  /* Invalidation when cache memory overflows */
  size_t treeSize = static_cast<const Node *>(bigTree)->treeSize();
  fill_cache_and_assert_invalidation(
      1 + CachePool::k_maxNumberOfBlocks / treeSize,
      static_cast<const Node *>(bigTree));
  /* Invalidation when cache identifiers overflow */
  fill_cache_and_assert_invalidation(CachePool::k_maxNumberOfReferences,
                                     static_cast<const Node *>(smallTree));
}

QUIZ_CASE(pcj_cache_reference_shared_data) {
  CachePool *cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  Expression e = Expression::Parse("-1+2*3");
  assert(e.id() != 1);
  // l is created with e.m_id different from 1
  Layout l = e.toLayout();
  // Forcing e.m_id change
  cachePool->freeBlocks(1);
  assert(e.id() == 1);
  // This test should fail if this line is uncommented
  // e = Expression::Parse("2*3");
  // l should handle new e.m_id
  l.id();
}
