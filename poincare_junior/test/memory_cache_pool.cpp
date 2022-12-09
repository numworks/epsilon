#include "print.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

static CachePool * cachePool = CachePool::sharedCachePool();
static EditionPool * editionPool = cachePool->editionPool();

static constexpr Tree tree = Add(3_n, 4_n);
static size_t treeSize = static_cast<Node>(tree).treeSize();
static constexpr Tree smallTree = 4_n;
static size_t smallTreeSize = static_cast<Node>(smallTree).treeSize();

void execute_push_tree_and_modify() {
  Poincare::CacheReference::InitializerFromTree treeModifier = [](Node tree) { EditionReference(tree).replaceNodeByNode(EditionReference::Push<BlockType::Multiplication>(2)); };
  cachePool->execute(
      [](void * subAction, const void * data) {
        Node editedTree = EditionPool::sharedEditionPool()->initFromAddress(data);
        log_pools();
        return (reinterpret_cast<Poincare::CacheReference::InitializerFromTree>(subAction))(editedTree);
      },
      reinterpret_cast<void *>(treeModifier),
      &tree
    );
}

void testCachePool() {
  // storeEditedTree
  editionPool->initFromTree(tree);
  assert_pools_sizes_are(0, treeSize);
  cachePool->storeEditedTree();
  assert_pools_sizes_are(treeSize, 0);

  // needFreeBlocks
  editionPool->initFromTree(tree);
  cachePool->storeEditedTree();
  assert_pools_sizes_are(2*treeSize, 0);
  cachePool->needFreeBlocks(1);
  assert_pools_sizes_are(treeSize, 0);
  cachePool->needFreeBlocks(treeSize - 1);
  assert_pools_sizes_are(0, 0);
  for (int i = 0; i < 3; i++) {
    editionPool->initFromTree(tree);
    cachePool->storeEditedTree();
  }
  assert_pools_sizes_are(3*treeSize, 0);
  cachePool->needFreeBlocks(treeSize + 1);
  assert_pools_sizes_are(treeSize, 0);

  // reset
  editionPool->initFromTree(tree);
  assert_pools_sizes_are(treeSize, treeSize);
  cachePool->reset();
  assert_pools_sizes_are(0, 0);

  // execute
  execute_push_tree_and_modify();
  assert_pool_contains(cachePool, {Mult(3_n, 4_n)});
  assert_pools_sizes_are(treeSize, 0);
}

void testCachePoolLimits() {
  /* test overflowing the edition pool */
  // 1. Almost fill the whole cache
  int maxNumberOfTreesInCache = CachePool::k_maxNumberOfBlocks/treeSize;
  for (int i = 0; i < maxNumberOfTreesInCache; i++) {
    editionPool->initFromTree(tree);
    cachePool->storeEditedTree();
  }
  assert_pools_sizes_are(maxNumberOfTreesInCache * treeSize, 0);
  // 2. Edit another tree triggering a cache invalidation
  execute_push_tree_and_modify();
  assert(cachePool->size() <= maxNumberOfTreesInCache * treeSize);
  Node lastTree = Node(cachePool->lastBlock()).previousTree();
  assert_trees_are_equal(lastTree, Mult(3_n, 4_n));

  /* test overflowing the cache identifier */
  cachePool->reset();
  // 1. Fill the cache with the max number of identifiers
  for (int i = 0; i < Pool::k_maxNumberOfReferences; i++) {
    editionPool->initFromTree(smallTree);
    cachePool->storeEditedTree();
  }
  assert_pools_sizes_are(Pool::k_maxNumberOfReferences * smallTreeSize, 0);
  // 2. Edit and cache a new tree triggering a cache invalidation
  execute_push_tree_and_modify();
  assert_pools_sizes_are((Pool::k_maxNumberOfReferences - 1) * smallTreeSize + treeSize, 0);


  assert(false);
}
