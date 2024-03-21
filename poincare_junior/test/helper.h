#ifndef POINCAREJ_TEST_HELPER_H
#define POINCAREJ_TEST_HELPER_H

#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <quiz.h>

#if POINCARE_TREE_LOG
#include <iostream>
#endif

using namespace PoincareJ;

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
  if (!Comparison::AreEqual(tree0, tree1)) {
#if POINCARE_TREE_LOG
    tree0->logDiffWith(tree1);
#endif
    quiz_assert(false);
  }
}

using FunctionSize = size_t (Pool::*)() const;
inline void assert_pools_sizes_are(size_t cachePoolSize, size_t editionPoolSize,
                                   FunctionSize functionSize) {
  CachePool* cachePool = CachePool::SharedCachePool;
  Pool* pools[] = {cachePool, SharedEditionPool};
  size_t theoreticalSizes[] = {cachePoolSize, editionPoolSize};
  for (size_t i = 0; i < sizeof(theoreticalSizes) / sizeof(size_t); i++) {
#if POINCARE_TREE_LOG
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
  SharedEditionPool->flush();
  CachePool::SharedCachePool->reset();
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
#include <chrono>
#include <iomanip>
#include <iostream>
#define QUIZ_ASSERT(test)                                                     \
  if (!(test)) {                                                              \
    std::cerr << __FILE__ << ':' << __LINE__ << ": test failed" << std::endl; \
    abort();                                                                  \
  }

inline void assertionsWarn() {
#if ASSERTIONS
  std::cout << "Compile with DEBUG=0 and ASSERTIONS=0 for more precise results"
            << std::endl;
#endif
}

#define PCJ_METRICS 0

#if PCJ_METRICS
#define METRICS(F)                                                          \
  {                                                                         \
    Tree::nextNodeCount = 0;                                                \
    Tree::nextNodeInPoolCount = 0;                                          \
    int refId;                                                              \
    {                                                                       \
      EditionReference r(0_e);                                              \
      refId = r.identifier();                                               \
      r->removeNode();                                                      \
    }                                                                       \
    auto startTime = std::chrono::high_resolution_clock::now();             \
    F;                                                                      \
    auto elapsed = std::chrono::high_resolution_clock::now() - startTime;   \
    {                                                                       \
      EditionReference r(0_e);                                              \
      refId = r.identifier() - refId;                                       \
      r->removeNode();                                                      \
    }                                                                       \
    if (refId != 0) {                                                       \
      std::cout << "WARNING ! " << refId << " references have leaked.\n";   \
    }                                                                       \
    std::cout << "Metrics [" << #F << "]\n"                                 \
              << "  nextNode:      " << std::right << std::setw(6)          \
              << Tree::nextNodeCount << "\n  nextNodeInPool:" << std::right \
              << std::setw(6) << Tree::nextNodeInPoolCount                  \
              << "\n  microseconds:  " << std::right << std::setw(6)        \
              << std::chrono::duration_cast<std::chrono::microseconds>(     \
                     elapsed)                                               \
                     .count()                                               \
              << std::endl;                                                 \
    assertionsWarn();                                                       \
  }
#endif
#endif

Tree* parse(const char* input);

// Integer

const char* MaxIntegerString();            // (2^8)^k_maxNumberOfDigits-1
const char* AlmostMaxIntegerString();      // (2^8)^k_maxNumberOfDigits-2
const char* OverflowedIntegerString();     // (2^8)^k_maxNumberOfDigits
const char* BigOverflowedIntegerString();  // OverflowedIntegerString with a 2
                                           // on first digit
const char* MaxParsedIntegerString();
const char* ApproximatedParsedIntegerString();

// Parsing

Tree* TextToTree(const char* input);

#endif
