#include "print.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

static CachePool * cachePool = CachePool::sharedCachePool();
static EditionPool * editionPool = cachePool->editionPool();

static constexpr Tree tree = Add(3_n, 4_n);
static size_t treeSize = static_cast<Node>(tree).treeSize();
static constexpr Tree smallTree = 4_n;

void execute_push_tree_and_modify() {
  Poincare::CacheReference::InitializerFromTree treeModifier = [](Node tree) { EditionReference(tree).replaceNodeByNode(EditionReference::Push<BlockType::Multiplication>(2)); };
  cachePool->execute(
      [](void * subAction, const void * data) {
        Node editedTree = EditionPool::sharedEditionPool()->initFromAddress(data);
        return (reinterpret_cast<Poincare::CacheReference::InitializerFromTree>(subAction))(editedTree);
      },
      reinterpret_cast<void *>(treeModifier),
      &tree
    );
}

void testCachePool() {
  // storeEditedTree
  editionPool->initFromTree(tree);
  assert_pools_tree_sizes_are(0, 1);
  cachePool->storeEditedTree();
  assert_pools_tree_sizes_are(1, 0);

  // needFreeBlocks
  editionPool->initFromTree(tree);
  cachePool->storeEditedTree();
  assert_pools_tree_sizes_are(2, 0);
  cachePool->needFreeBlocks(1);
  assert_pools_tree_sizes_are(1, 0);
  cachePool->needFreeBlocks(treeSize - 1);
  assert_pools_tree_sizes_are(0, 0);
  for (int i = 0; i < 3; i++) {
    editionPool->initFromTree(tree);
    cachePool->storeEditedTree();
  }
  assert_pools_tree_sizes_are(3, 0);
  cachePool->needFreeBlocks(treeSize + 1);
  assert_pools_tree_sizes_are(1, 0);

  // reset
  editionPool->initFromTree(tree);
  assert_pools_tree_sizes_are(1, 1);
  cachePool->reset();
  assert_pools_tree_sizes_are(0, 0);

  // execute
  execute_push_tree_and_modify();
  assert_pool_contains(cachePool, {Mult(3_n, 4_n)});
  assert_pools_tree_sizes_are(1, 0);
}

void testCachePoolLimits() {
  /* test overflowing the edition pool */
  // 1. Almost fill the whole cache
    // Fill the cache
  int maxNumberOfTreesInCache = CachePool::k_maxNumberOfBlocks/treeSize;
  for (int i = 0; i < maxNumberOfTreesInCache; i++) {
    editionPool->initFromTree(tree);
    cachePool->storeEditedTree();
  }
  assert_pools_tree_sizes_are(maxNumberOfTreesInCache, 0);

  // 2. Edit another tree triggering a cache invalidation
  execute_push_tree_and_modify();
  assert(cachePool->numberOfTrees() < maxNumberOfTreesInCache);
  Node lastTree = Node(cachePool->lastBlock()).previousTree();
  assert_trees_are_equal(lastTree, Mult(3_n, 4_n));

  /* test overflowing the cache identifier */
  cachePool->reset();
  // 1. Fill the cache with the max number of identifiers
  for (int i = 0; i < Pool::k_maxNumberOfReferences; i++) {
    editionPool->initFromTree(smallTree);
    cachePool->storeEditedTree();
  }
  assert_pools_tree_sizes_are(Pool::k_maxNumberOfReferences, 0);
  // 2. Edit and cache a new tree triggering a cache invalidation
  execute_push_tree_and_modify();
  assert_pools_tree_sizes_are(Pool::k_maxNumberOfReferences, 0);
}

void assert_check_cache_reference(CacheReference reference, std::initializer_list<const Node> cacheTrees) {
  cachePool->reset();
  assert_pools_tree_sizes_are(0, 0);
  reference.send([](const Node tree, void * result) {}, nullptr);
  assert_pool_contains(cachePool, cacheTrees);
  assert_pools_tree_sizes_are(cacheTrees.size(), 0);
  cachePool->reset();
}

void testCacheReference() {
  // Constructors
  CacheReference reference0([] (){ EditionReference::Push<BlockType::IntegerShort>(4); });
  assert_check_cache_reference(reference0, {4_n});

  CacheReference reference1([] (Node node){ EditionReference(node).replaceNodeByNode(5_n); }, static_cast<Node>(smallTree).block());
  assert_check_cache_reference(reference1, {5_n});

  CacheReference reference2(
      [] (Node node){
        EditionReference ref(node);
        ref.insertNodeBeforeNode(EditionReference::Push<BlockType::Addition>(2));
        ref.insertNodeAfterNode(6_n);
      }, &reference1);
  assert_check_cache_reference(reference2, {5_n, Add(5_n, 6_n)});

  CacheReference reference3([] (const char * string){ EditionReference::Push<BlockType::Addition>(string[0] - '0'); }, "0");
  assert_check_cache_reference(reference3, {Add()});
}

void check_reference_invalidation_and_reconstruction(CacheReference reference, uint16_t identifier, Node node) {
  // reference has been invalidated
  assert(cachePool->nodeForIdentifier(identifier).isUninitialized());
  // reference is regenerated on demand
  reference.send([](const Node tree, void * result) {}, nullptr);
  assert_trees_are_equal(Node(cachePool->lastBlock()).previousTree(), node);
}

void testCacheReferenceInvalidation() {
  CacheReference reference([] (){ EditionReference::Push<BlockType::IntegerShort>(28); });
  reference.send([](const Node tree, void * result) {}, nullptr);
  uint16_t identifier = reference.id();
  assert_pools_tree_sizes_are(1, 0);

  /* Invalidation when cache memory overflows */
  // Fill the cache
  int maxNumberOfTreesInCache = CachePool::k_maxNumberOfBlocks/treeSize;
  for (int i = 0; i < maxNumberOfTreesInCache + 1; i++) {
    CacheReference reference1([] (Node node){}, static_cast<Node>(tree).block());
    reference1.send([](const Node tree, void * result) {}, nullptr);
  }
  // TODO: factorize
  assert_pools_tree_sizes_are(maxNumberOfTreesInCache, 0);
  check_reference_invalidation_and_reconstruction(reference, identifier, 28_n);

  /* Invalidation when cache identifiers overflow */
  // Fill the cache identifiers
  cachePool->reset();
  assert_pools_tree_sizes_are(0, 0);
  reference.send([](const Node tree, void * result) {}, nullptr);
  identifier = reference.id();
  for (int i = 0; i < Pool::k_maxNumberOfReferences; i++) {
    CacheReference reference1([] (Node node){}, static_cast<Node>(smallTree).block());
    reference1.send([](const Node tree, void * result) {}, nullptr);
  }
  assert_pools_tree_sizes_are(Pool::k_maxNumberOfReferences, 0);
  check_reference_invalidation_and_reconstruction(reference, identifier, 28_n);
}
