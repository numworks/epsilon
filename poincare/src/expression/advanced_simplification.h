#ifndef POINCARE_EXPRESSION_ADVANCED_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_ADVANCED_SIMPLIFICATION_H

#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>

#include "advanced_operation.h"
#include "arithmetic.h"
#include "logarithm.h"
#include "parametric.h"
#include "projection.h"
#include "trigonometry.h"

// Max number of trees advanced reduction can handle
#define ADVANCED_MAX_BREADTH 64
// Max depth of path advanced reduction can handle
#define ADVANCED_MAX_DEPTH 6

namespace Poincare::Internal {

class AdvancedSimplification {
 public:
  static bool AdvancedReduce(Tree* u);
  EDITION_REF_WRAP(AdvancedReduce);

  // Bottom-up deep contract
  static bool DeepContract(Tree* e);
  EDITION_REF_WRAP(DeepContract);
  // Top-Bottom deep expand
  static bool DeepExpand(Tree* e);
  EDITION_REF_WRAP(DeepExpand);

 private:
  // Ordered list of hashes encountered during advanced reduction.
  class CrcCollection {
   public:
    CrcCollection() : m_length(0) {}
    // Return false if hash was already explored
    bool add(uint32_t crc, uint8_t depth);
    bool isFull() const { return m_length >= k_size; }

   private:
    // Max Expand/Contract combination possibilities
    constexpr static size_t k_size = ADVANCED_MAX_BREADTH;
    uint32_t m_collection[k_size];
    // Depth at which each crc has been explored
    uint8_t m_depth[k_size];
    size_t m_length;
  };

  // Store a direction. NextNode can be applied multiple times.
  class Direction {
   public:
    constexpr static uint8_t k_numberOfBaseDirections = 3;
    // Return true if direction was applied.
    bool apply(Tree** u, Tree* root, bool* rootChanged) const;
    // Return true if can apply direction.
    bool canApply(const Tree* u, const Tree* root) const;
    // Constructor needed for Path::m_stack
    Direction() : m_type(0) {}
    bool isNextNode() const { return !isContract() && !isExpand(); }
#if POINCARE_TREE_LOG
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
    bool isContract() const { return m_type == k_contractType; }
    bool isExpand() const { return m_type == k_expandType; }

    uint8_t m_type;
  };
  static_assert(sizeof(uint8_t) == sizeof(Direction));

  // Path in exploration of a tree's advanced reduction.
  class Path {
   public:
    Path() : m_length(0) {}
    // Return true if tree has changed. Path is expected to be valid on root.
    bool apply(Tree* root) const;
    // Pop NextNode directions one at a time.
    void popBaseDirection();
    // Return if any base direction can be added.
    bool canAddNewDirection() const { return m_length < k_size; }
    // Return true if direction was appended
    bool append(Direction direction);
    uint8_t length() const { return m_length; }

   private:
    // Path max length (~= 2 * max number of allowed Expand/Contract)
    constexpr static uint8_t k_size = ADVANCED_MAX_DEPTH;
    Direction m_stack[k_size];
    uint8_t m_length;
  };

  struct Context {
    Context(Tree* root, const Tree* original, int bestMetric)
        : m_root(root),
          m_original(original),
          m_bestMetric(bestMetric),
          m_mustResetRoot(false) {}

    Tree* m_root;
    const Tree* m_original;
    Path m_path;
    Path m_bestPath;
    int m_bestMetric;
    CrcCollection m_crcCollection;
    bool m_mustResetRoot;
  };
  /* Recursive advanced reduction. Return true if advanced reduction
   * possibilities have all been explored. */
  static bool AdvancedReduceRec(Tree* u, Context* ctx);
  // Bottom-up ShallowReduce starting from tree. Output is unrelated to change.
  static bool UpwardSystematicReduce(Tree* root, const Tree* tree);

  /* Expand/Contract operations */
  static bool ShallowContract(Tree* e, bool tryAll) {
    return (tryAll ? TryAllOperations : TryOneOperation)(
        e, k_contractOperations, std::size(k_contractOperations));
  }
  static bool ShallowExpand(Tree* e, bool tryAll) {
    return (tryAll ? TryAllOperations : TryOneOperation)(
        e, k_expandOperations, std::size(k_expandOperations));
  }

  // Try all Operations until they all fail consecutively.
  static bool TryAllOperations(Tree* node, const Tree::Operation* operations,
                               int numberOfOperations);
  // Try all Operations until one of them succeed.
  static bool TryOneOperation(Tree* node, const Tree::Operation* operations,
                              int numberOfOperations);

  constexpr static Tree::Operation k_contractOperations[] = {
      Logarithm::ContractLn,
      AdvancedOperation::ContractAbs,
      AdvancedOperation::ContractExpMult,
      Trigonometry::ContractTrigonometric,
      Parametric::ContractProduct,
      AdvancedOperation::ContractMult,
  };
  constexpr static Tree::Operation k_expandOperations[] = {
      AdvancedOperation::ExpandAbs,
      Logarithm::ExpandLn,
      AdvancedOperation::ExpandExp,
      Trigonometry::ExpandTrigonometric,
      Parametric::ExpandSum,
      Parametric::ExpandProduct,
      Arithmetic::ExpandBinomial,
      Arithmetic::ExpandPermute,
      Projection::Expand,
      AdvancedOperation::ExpandPower,
      AdvancedOperation::ExpandMult,
      AdvancedOperation::ExpandImRe,
  };
};

}  // namespace Poincare::Internal

#endif
