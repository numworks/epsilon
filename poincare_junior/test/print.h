#include <iostream>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>

using namespace Poincare;

inline EditionReference createSimpleExpression() {
  std::cout << "\n---------------- Create (1 + 2) * 3 * 4 ----------------" << std::endl;
  EditionReference multiplication = EditionReference::Push<BlockType::Multiplication>(3);
  EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(1);
  EditionReference::Push<BlockType::IntegerShort>(2);
  EditionReference::Push<BlockType::IntegerShort>(3);
  EditionReference::Push<BlockType::Addition>(2);
  EditionReference::Push<BlockType::IntegerShort>(4);
  EditionReference::Push<BlockType::IntegerShort>(5);
  return multiplication;
}

inline void print() {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * editionPool = cachePool->editionPool();

  std::cout << "\n\n\n\nxxxxxxxxxxx MEMORY DUMP xxxxxxxxxxxx" << std::endl;
  std::cout << "\n========= CACHE POOL =========" << std::endl;
  cachePool->treeLog(std::cout);

  std::cout << "\n========= EDITION POOL =========" << std::endl;
  editionPool->treeLog(std::cout);
  std::cout << "\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << std::endl;
}

inline void intermediaryPrint() {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * editionPool = cachePool->editionPool();

  std::cout << "\n========= CACHE POOL =========" << std::endl;
  cachePool->treeLog(std::cout);

  std::cout << "\n========= INCOMPLETE EDITION POOL =========" << std::endl;
  editionPool->flatLog(std::cout);
  std::cout << "\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << std::endl;
}

#if POINCARE_MEMORY_TREE_LOG
inline void log_pools(bool corruptedEditionPool = false) {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * editionPool = cachePool->editionPool();

  std::cout << "\n========= CACHE POOL ===========" << std::endl;
  cachePool->treeLog(std::cout);

  std::cout << "\n========= EDITION POOL =========" << std::endl;
  if (corruptedEditionPool) {
    editionPool->flatLog(std::cout);
  } else {
    editionPool->treeLog(std::cout);
  }
  std::cout << "\n================================" << std::endl;
}
#endif

inline void assert_node_equals_blocks(const Node node, std::initializer_list<Block> blocks) {
  Block * block = node.block();
  for (Block b : blocks) {
    assert(*block == b);
    block = block->next();
  }
  assert(node.treeSize() == blocks.size());
}

inline void assert_trees_are_equal(const Node tree0, const Node tree1) {
  assert(Simplification::Compare(tree0, tree1) == 0);
}

inline void assert_pools_sizes_are(size_t cachePoolSize, size_t editionPoolSize) {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * editionPool = cachePool->editionPool();
  Pool * pools[] = {cachePool, editionPool};
  size_t theoreticalSizes[] = {cachePoolSize, editionPoolSize};
  for (size_t i = 0; i < sizeof(theoreticalSizes)/sizeof(size_t); i++) {
#if POINCARE_MEMORY_TREE_LOG
    const char * poolNames[] = {"cache pool", "edition Pool"};
    if (pools[i]->size() != theoreticalSizes[i]) {
      std::cout << "Expected "<< poolNames[i] <<" of size " << theoreticalSizes[i] << " but got " << pools[i]->size() << std::endl;
      pools[i]->treeLog(std::cout);
      assert(false);
    }
#else
    assert(pools[i]->size() == theoreticalSizes[i]);
#endif
  }
}

inline void reset_pools() {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * editionPool = cachePool->editionPool();
  editionPool->flush();
  cachePool->reset();
}

inline void assert_pool_contains(Pool * pool, std::initializer_list<const Node> nodes) {
  Node tree(pool->firstBlock());
  for (const Node n : nodes) {
    assert_trees_are_equal(n, tree);
    tree = tree.nextTree();
  }
  assert(tree.block() == pool->lastBlock());
}
