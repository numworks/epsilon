#ifndef POINCAREJ_TEST_HELPER_H
#define POINCAREJ_TEST_HELPER_H

#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <quiz.h>

#if POINCARE_MEMORY_TREE_LOG
#include <iostream>
#endif

using namespace PoincareJ;

// TODO: remove
inline EditionReference createSimpleExpression() {
#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n--- Create (1 + 2) * 3 * (4 + 5) ---" << std::endl;
#endif
  EditionReference multiplication(
      editionPool->push<BlockType::Multiplication>(3));
  editionPool->push<BlockType::Addition>(2);
  editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  editionPool->push<BlockType::Addition>(2);
  editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(4));
  editionPool->push<BlockType::IntegerShort>(static_cast<int8_t>(5));
  return multiplication;
}

#if POINCARE_MEMORY_TREE_LOG

__attribute__((__used__)) inline void log_edition_pool(
    bool corruptedEditionPool = false) {
  EditionPool* editionPool = CachePool::sharedCachePool()->editionPool();
  editionPool->log(
      std::cout,
      corruptedEditionPool ? Pool::LogFormat::Flat : Pool::LogFormat::Tree,
      true);
}

__attribute__((__used__)) inline void log_cache_pool() {
  CachePool::sharedCachePool()->log(std::cout, Pool::LogFormat::Tree, true);
}

#else

inline void log_edition_pool(bool corruptedEditionPool = false) {}
inline void log_edition_references() {}
inline void log_cache_pool() {}
inline void log_cache_references() {}
#endif

inline void assert_node_equals_blocks(const Tree* node,
                                      std::initializer_list<Block> blocks) {
  const Block* block = node->block();
  for (Block b : blocks) {
    quiz_assert(*block == b);
    block = block->next();
  }
  quiz_assert(node->treeSize() == blocks.size());
}

inline void assert_trees_are_equal(const Tree* tree0, const Tree* tree1) {
  quiz_assert((tree0 == nullptr) == (tree1 == nullptr));
  quiz_assert(Comparison::AreEqual(tree0, tree1));
}

using FunctionSize = size_t (Pool::*)() const;
inline void assert_pools_sizes_are(size_t cachePoolSize, size_t editionPoolSize,
                                   FunctionSize functionSize) {
  CachePool* cachePool = CachePool::sharedCachePool();
  EditionPool* editionPool = cachePool->editionPool();
  Pool* pools[] = {cachePool, editionPool};
  size_t theoreticalSizes[] = {cachePoolSize, editionPoolSize};
  for (size_t i = 0; i < sizeof(theoreticalSizes) / sizeof(size_t); i++) {
#if POINCARE_MEMORY_TREE_LOG
    const char* poolNames[] = {"cache pool", "edition Pool"};
    if ((pools[i]->*functionSize)() != theoreticalSizes[i]) {
      std::cout << "Expected " << poolNames[i] << " of size "
                << theoreticalSizes[i] << " but got "
                << (pools[i]->*functionSize)() << std::endl;
      pools[i]->log(std::cout, Pool::LogFormat::Tree, true);
      quiz_assert(false);
    }
#else
    quiz_assert((pools[i]->*functionSize)() == theoreticalSizes[i]);
#endif
  }
}

inline void assert_pools_block_sizes_are(size_t cachePoolSize,
                                         size_t editionPoolSize) {
  return assert_pools_sizes_are(cachePoolSize, editionPoolSize, &Pool::size);
}

inline void assert_pools_tree_sizes_are(size_t cachePoolSize,
                                        size_t editionPoolSize) {
  return assert_pools_sizes_are(cachePoolSize, editionPoolSize,
                                &Pool::numberOfTrees);
}

inline void reset_pools() {
  CachePool* cachePool = CachePool::sharedCachePool();
  EditionPool* editionPool = cachePool->editionPool();
  editionPool->flush();
  cachePool->reset();
}

inline void assert_pool_contains(Pool* pool,
                                 std::initializer_list<const Tree*> nodes) {
  quiz_assert(pool->size() > 0);
  Tree* tree = Tree::FromBlocks(pool->firstBlock());
  for (const Tree* n : nodes) {
    assert_trees_are_equal(n, tree);
    tree = tree->nextTree();
  }
  quiz_assert(tree->block() == pool->lastBlock());
}

#if PLATFORM_DEVICE
#define QUIZ_ASSERT(test) quiz_assert(test)
#else
#include <iostream>
#define QUIZ_ASSERT(test)                                                     \
  if (!(test)) {                                                              \
    std::cerr << __FILE__ << ':' << __LINE__ << ": test failed" << std::endl; \
    std::exit(1);                                                             \
  }
#endif

// Integer

const char* MaxIntegerString();            // (2^8)^k_maxNumberOfDigits-1
const char* AlmostMaxIntegerString();      // (2^8)^k_maxNumberOfDigits-2
const char* OverflowedIntegerString();     // (2^8)^k_maxNumberOfDigits
const char* BigOverflowedIntegerString();  // OverflowedIntegerString with a 2
                                           // on first digit
const char* MaxParsedIntegerString();
const char* ApproximatedParsedIntegerString();

#endif
