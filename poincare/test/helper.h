#ifndef POINCAREJ_TEST_HELPER_H
#define POINCAREJ_TEST_HELPER_H

#include <poincare/old/context.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/parsing/rack_parser.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/src/memory/tree_stack.h>
#include <quiz.h>

#if POINCARE_TREE_LOG
#include <iostream>
#endif

using namespace Poincare::Internal;

typedef void (*ProcessTree)(Tree*, ProjectionContext projectionContext);
void process_tree_and_compare(const char* input, const char* output,
                              ProcessTree process,
                              ProjectionContext projectionContext);

void quiz_assert_print_if_failure(bool test, const char* information);

inline void assert_tree_equals_blocks(const Tree* tree,
                                      std::initializer_list<Block> blocks) {
  const Block* block = tree->block();
  for (Block b : blocks) {
    quiz_assert(*block == b);
    block = block->next();
  }
  quiz_assert(tree->treeSize() == blocks.size());
}

inline void assert_trees_are_equal(const Tree* tree0, const Tree* tree1) {
  quiz_assert((tree0 == nullptr) == (tree1 == nullptr));
  if (!tree0->treeIsIdenticalTo(tree1)) {
#if POINCARE_TREE_LOG
    tree0->logDiffWith(tree1);
#endif
    quiz_assert(false);
  }
}

inline void flush_stack() { SharedTreeStack->flush(); }

inline void assert_tree_stack_contains(
    std::initializer_list<const Tree*> trees) {
  quiz_assert(SharedTreeStack->size() > 0);
  Tree* tree = Tree::FromBlocks(SharedTreeStack->firstBlock());
  for (const Tree* t : trees) {
    assert_trees_are_equal(t, tree);
    tree = tree->nextTree();
  }
  quiz_assert(tree->block() == SharedTreeStack->lastBlock());
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
      TreeRef r(0_e);                                                       \
      refId = r.identifier();                                               \
      r->removeNode();                                                      \
    }                                                                       \
    auto startTime = std::chrono::high_resolution_clock::now();             \
    F;                                                                      \
    auto elapsed = std::chrono::high_resolution_clock::now() - startTime;   \
    {                                                                       \
      TreeRef r(0_e);                                                       \
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

Tree* parse(const char* input, Poincare::Context* context = nullptr,
            bool parseForAssignment = false);

// Integer

const char* MaxIntegerString();            // (2^8)^k_maxNumberOfDigits-1
const char* AlmostMaxIntegerString();      // (2^8)^k_maxNumberOfDigits-2
const char* OverflowedIntegerString();     // (2^8)^k_maxNumberOfDigits
const char* BigOverflowedIntegerString();  // OverflowedIntegerString with a 2
                                           // on first digit
const char* MaxParsedIntegerString();
const char* ApproximatedParsedIntegerString();

// Parsing

Tree* TextToTree(const char* input, Poincare::Context* context = nullptr);

void store(const char* storeExpression, Poincare::Context* ctx);

inline Tree* parseAndPrepareForApproximation(const char* function,
                                             ProjectionContext ctx = {}) {
  constexpr const char* k_symbol = "x";
  Tree* e = parse(function, ctx.m_context);
  Simplification::ToSystem(e, &ctx);
  Approximation::PrepareFunctionForApproximation(e, k_symbol,
                                                 ctx.m_complexFormat);
  return e;
}

#endif
