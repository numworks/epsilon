#ifndef POINCARE_EXPRESSION_ADVANCED_REDUCTION_H
#define POINCARE_EXPRESSION_ADVANCED_REDUCTION_H

#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>

#include "advanced_operation.h"
#include "arithmetic.h"
#include "logarithm.h"
#include "parametric.h"
#include "projection.h"
#include "trigonometry.h"

// Max number of trees advanced reduction can handle
#define ADVANCED_MAX_BREADTH 32
// Max depth of path advanced reduction can handle
#define ADVANCED_MAX_DEPTH 8

namespace Poincare::Internal {

class AdvancedReduction {
 public:
  static bool Reduce(Tree* e);
  // Bottom-up deep contract
  static bool DeepContract(Tree* e);
  // Top-Bottom deep expand
  static bool DeepExpand(Tree* e) { return PrivateDeepExpand(e); };
  // Top-Bottom deep expand using only algebraic operations
  static bool DeepExpandAlgebraic(Tree* e) {
    return PrivateDeepExpand(e, true);
  };

 private:
  // Ordered list of hashes encountered during advanced reduction.
  class CrcCollection {
   public:
    CrcCollection() : m_length(0), m_maxDepth(ADVANCED_MAX_DEPTH) {}
    // Return false if hash was already explored
    bool add(uint32_t crc, uint8_t depth);
    bool isFull() const { return m_length >= k_size; }
    // Ignore dependencies in hash
    static uint32_t AdvancedHash(const Tree* e) {
      return (e->isDep() ? e->child(0) : e)->hash();
    }
    uint8_t maxDepth() const { return m_maxDepth; }
#if POINCARE_TREE_LOG
    void log() const {
      std::cout << "Max depth is " << (int)m_maxDepth << ". Capacity is "
                << m_length << '/' << k_size << ".\n";
    }
#endif

   private:
    /* Discard the hashes encountered with the highest depth, to continue
     * advanced reduction with a lower max depth. BestPath won't be discarded,
     * but remaining branches will be explored at lower depth. */
    void decreaseMaxDepth();
    // Max Expand/Contract combination possibilities
    constexpr static size_t k_size = ADVANCED_MAX_BREADTH;
    uint32_t m_collection[k_size];
    // Depth and Available depth at which each possibility has been explored
    // TODO: Since ADVANCED_MAX_DEPTH is small, we could use a bitfield here.
    uint8_t m_depth[k_size];
    uint8_t m_availableDepth[k_size];
    size_t m_length;
    // Maximal depth at which a hash will be handled.
    uint8_t m_maxDepth;
  };

  // Store a direction. NextNode can be applied multiple times.
  class Direction {
   public:
    static Direction Contract() { return Direction(k_contractType); }
    static Direction Expand() { return Direction(k_expandType); }
    static Direction NextNode(uint8_t i) {
      assert(i >= k_baseNextNodeType && i <= k_maxNextNodeAmount);
      return Direction(i);
    }
    // Return true if direction was applied.
    bool apply(Tree** e, Tree* root, bool* treeChanged) const;
    // Return true if direction was applied.
    bool applyNextNode(Tree** e, const Tree* root) const;
    // Return true if direction was applied.
    bool applyContractOrExpand(Tree** e, Tree* root) const;
    // Constructor needed for Path::m_stack
    Direction() : m_type(0) {}
    bool isNextNode() const { return !isContract() && !isExpand(); }
#if POINCARE_TREE_LOG
    void log(bool addLineReturn = true) const;
#endif
    // If possible, combine the other direction into this one and return true.
    bool combine(Direction other);
    // If possible, decrement the weight of the direction and return true.
    bool decrement();

   private:
    Direction(uint8_t type) : m_type(type) {}
    bool isContract() const { return m_type == k_contractType; }
    bool isExpand() const { return m_type == k_expandType; }

    /* Contract and Expand use 0 and UINT8_MAX so that NextNode can be weighted
     * between 1 and UINT8_MAX-1. */
    constexpr static uint8_t k_contractType = 0;
    constexpr static uint8_t k_baseNextNodeType = 1;
    constexpr static uint8_t k_expandType = UINT8_MAX;
    uint8_t m_type;

   public:
    constexpr static uint8_t k_maxNextNodeAmount = k_expandType - 1;
  };
  static_assert(sizeof(uint8_t) == sizeof(Direction));

  // Path in exploration of a tree's advanced reduction.
  class Path {
   public:
    Path() : m_length(0) {}
    // Return true if tree has changed. Path is expected to be valid on root.
    bool apply(Tree* root) const;
    // Pop last direction if it is [NextNode x1], [Contract] or [Expand].
    // If it is [NextNode xN], reduce [NextNode] amount by 1.
    void popBaseDirection();
    // Remove last direction of [Path]. Could be [Contract], [Expand] or
    // [NextNode xN].
    void popWholeDirection();
    // Return true if direction was appended
    bool append(Direction direction);
    uint8_t length() const { return m_length; }
#if POINCARE_TREE_LOG
    void logLastDirection(bool addNewLine = true) {
      m_stack[m_length - 1].log(addNewLine);
    }
    void log() const;
    Direction* getStack() { return m_stack; }
#endif

   private:
    // Path max length (~= 2 * max number of allowed Expand/Contract)
    constexpr static uint8_t k_size = ADVANCED_MAX_DEPTH;
    Direction m_stack[k_size];
    uint8_t m_length;
  };

  struct Context {
    Context(Tree* root, const Tree* original, int bestMetric, uint32_t bestHash)
        : m_root(root),
          m_original(original),
          m_bestMetric(bestMetric),
          m_bestHash(bestHash),
          m_mustResetRoot(false) {}

    Tree* m_root;
    const Tree* m_original;
    Path m_path;
    Path m_bestPath;
    int m_bestMetric;
    uint32_t m_bestHash;
    CrcCollection m_crcCollection;
    bool m_mustResetRoot;
    /* Reset ctx->m_root to current [Path] if needed */
    void resetIfNeeded();
    bool canAppendDirection() const {
      return m_path.length() < m_crcCollection.maxDepth();
    }
  };

  [[nodiscard]] static Path FindBestReduction(const Tree* e);

  static bool ReduceIndependantElement(Tree* e);

  /* Internal entrypoint for AdvancedReduction. Handles NextNode operations.
   * After each NextNode, calls [ReduceContractThenExpand].
   * Return true if advanced reduction possibilities have all been explored. */
  static bool PrivateReduce(Tree* e, Context* ctx,
                            bool zeroNextNodeAllowed = true);
  /* Try one reduction step with given [direction], if successful calls
   * [PrivateReduce].
   * [direction] should be Contract or Expand only. NextNode are handled in
   * [PrivateReduce]. */
  static bool ReduceDirection(Tree* e, Context* ctx, Direction direction);
  /* Auxiliary method to [PrivateReduce], handles Contract and Expand
   * operations. Return true if advanced reduction possibilities have all been
   * explored. */
  static bool ReduceContractThenExpand(Tree* e, Context* ctx);
  /* Compute the metric of ctx->m_root and update ctx accordingly */
  static void UpdateBestMetric(Context* ctx);
  // Bottom-up ShallowReduce starting from tree. Output is unrelated to change.
  static bool UpwardSystematicReduce(Tree* root, const Tree* tree);

  /* Expand/Contract operations */
  static bool PrivateDeepExpand(Tree* e, bool onlyAlgebraicOperations = false);
  static bool ShallowContract(Tree* e, bool tryAll) {
    return (tryAll ? TryAllOperations : TryOneOperation)(
        e, k_contractOperations, std::size(k_contractOperations));
  }
  static bool ShallowExpand(Tree* e, bool tryAll) {
    return (tryAll ? TryAllOperations : TryOneOperation)(
        e, k_expandOperations, std::size(k_expandOperations));
  }
  static bool ShallowExpandAlgebraic(Tree* e, bool tryAll) {
    return (tryAll ? TryAllOperations : TryOneOperation)(
        e, k_expandAlgebraicOperations, std::size(k_expandAlgebraicOperations));
  }

  // Try all Operations until they all fail consecutively.
  static bool TryAllOperations(Tree* e, const Tree::Operation* operations,
                               int numberOfOperations);
  // Try all Operations until one of them succeed.
  static bool TryOneOperation(Tree* e, const Tree::Operation* operations,
                              int numberOfOperations);

  constexpr static Tree::Operation k_contractOperations[] = {
      Logarithm::ContractLn,           AdvancedOperation::ContractAbs,
      AdvancedOperation::ContractExp,  Trigonometry::ContractTrigonometric,
      Parametric::ContractSum,         Parametric::ContractProductOfExp,
      Parametric::ContractProduct,     AdvancedOperation::ContractMult,
      AdvancedOperation::ContractImRe, AdvancedOperation::ContractCeilFloor,
  };
  constexpr static Tree::Operation k_expandOperations[] = {
      AdvancedOperation::ExpandAbs,
      Logarithm::ExpandLn,
      AdvancedOperation::ExpandExp,
      Trigonometry::ExpandTrigonometric,
      Parametric::ExpandExpOfSum,
      Parametric::ExpandSum,
      Parametric::ExpandProduct,
      Arithmetic::ExpandBinomial,
      Arithmetic::ExpandPermute,
      Projection::Expand,
      AdvancedOperation::ExpandPower,
      AdvancedOperation::ExpandMult,
      AdvancedOperation::ExpandImRe,
      AdvancedOperation::ExpandCeilFloor,
      AdvancedOperation::ExpandComplexArgument,
  };
  constexpr static Tree::Operation k_expandAlgebraicOperations[] = {
      AdvancedOperation::ExpandPower,
      AdvancedOperation::ExpandMult,
      Parametric::ExpandSum,
      Parametric::ExpandProduct,
  };
};

}  // namespace Poincare::Internal

#endif
