#ifndef POINCARE_EXPRESSION_ADVANCED_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_ADVANCED_SIMPLIFICATION_H

#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class AdvancedSimplification {
 public:
  static bool AdvancedReduce(Tree *u);
  EDITION_REF_WRAP(AdvancedReduce);

 private:
  // Ordered list of hashes encountered during advanced reduction.
  class CrcCollection {
   public:
    CrcCollection() : m_length(0) {}
    // Return false if hash was already explored
    bool add(uint32_t crc, uint8_t depth);
    bool isFull() const { return m_length >= k_size; }
    static uint32_t Hash(const Tree *tree);

   private:
    // Max Expand/Contract combination possibilities
    constexpr static size_t k_size = 128;
    uint32_t m_collection[k_size];
    // Depth at which each crc has been explored
    uint8_t m_depth[k_size];
    size_t m_length;
  };

  // Store a direction. NextNode can be applied multiple times.
  class Direction {
   public:
    constexpr static uint8_t k_numberOfBaseDirections = 3;
    // Constructor needed for Path::m_stack
    Direction() : m_type(0) {}
    bool isNextNode() const { return !isContract() && !isExpand(); }
    bool isContract() const { return m_type == k_contractType; }
    bool isExpand() const { return m_type == k_expandType; }
#if POINCARE_MEMORY_TREE_LOG
    void log();
#endif
    // Returns one of the three base direction (NextNode, Contract and Expand).
    static Direction SingleDirectionForIndex(int index) {
      assert(index >= 0 && index < k_numberOfBaseDirections);
      return Direction(index == 0
                           ? k_baseNextNodeType
                           : (index == 1 ? k_contractType : k_expandType));
    }
    // If possible, combine the other direction into this one and return true.
    bool combine(Direction other);
    // If possible, decrement the weight of the direction and return true.
    bool decrement();

   private:
    /* Contract and Expand use 0 and UINT8_MAX so that NextNode can be weighted
     * between 1 and UINT8_MAX-1. */
    constexpr static uint8_t k_contractType = 0;
    constexpr static uint8_t k_baseNextNodeType = 1;
    constexpr static uint8_t k_expandType = UINT8_MAX;
    Direction(uint8_t type) : m_type(type) {}
    uint8_t m_type;
  };
  static_assert(sizeof(uint8_t) == sizeof(Direction));

  // Path in exploration of a tree's advanced reduction.
  class Path {
   public:
    Path() : m_length(0) {}
    // Pop NextNode directions one at a time.
    void popBaseDirection();
    // Return if any base direction can be added.
    bool canAddNewDirection() const { return m_length < k_size; }
    bool append(Direction direction);
    Direction direction(uint8_t index) const {
      assert(index < m_length);
      return m_stack[index];
    }
    uint8_t length() const { return m_length; }

   private:
    // Path max length (~= 2 * max number of allowed Expand/Contract)
    constexpr static uint8_t k_size = 12;
    Direction m_stack[k_size];
    uint8_t m_length;
  };

  struct Context {
    Context(Tree *root, const Tree *original, int bestMetric)
        : m_root(root),
          m_original(original),
          m_bestMetric(bestMetric),
          m_mustResetRoot(false) {}

    Tree *m_root;
    const Tree *m_original;
    Path m_path;
    Path m_bestPath;
    int m_bestMetric;
    CrcCollection m_crcCollection;
    bool m_mustResetRoot;
  };
  /* Recursive advanced reduction. Return true if advanced reduction
   * possibilities have all been explored. */
  static bool AdvancedReduceRec(Tree *u, Context *ctx);
  // Return true if tree has changed. path is expected to be valid.
  static bool ApplyPath(Tree *root, const Path *path, bool keepDependencies);
  // Return true if direction was applied.
  static bool ApplyDirection(Tree **u, Tree *root, Direction direction,
                             bool *rootChanged, bool keepDependencies);
  // Return true if can apply direction.
  static bool CanApplyDirection(const Tree *u, const Tree *root,
                                Direction direction);
  // Bottom-up ShallowReduce starting from tree. Output is unrelated to change.
  static bool UpwardSystemReduce(Tree *root, const Tree *tree);
};

}  // namespace PoincareJ

#endif
