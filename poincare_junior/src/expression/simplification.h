#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <omgpj/enums.h>
#include <poincare_junior/src/expression/arithmetic.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/expression/logarithm.h>
#include <poincare_junior/src/expression/metric.h>
#include <poincare_junior/src/expression/trigonometry.h>
#include <poincare_junior/src/memory/edition_reference.h>

#include "arithmetic.h"
#include "beautification.h"
#include "context.h"
#include "float.h"
#include "parametric.h"
#include "projection.h"

namespace PoincareJ {

/* TODO : Implement PolynomialInterpretation. Prepare the expression for
 * Polynomial interpretation (expand TranscendentalOnRationals and algebraic
 * trees.) */

class Simplification {
 public:
  /* New advanced reduction */

  // Ordered list of CRC encountered during advanced reduction.
  class CrcCollection {
   public:
    CrcCollection() : m_length(0) {}
    // Return false if CRC was already explored
    bool add(uint32_t crc, uint8_t depth);
    bool isFull() const { return m_length >= k_size; }

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
  // Recursive advanced reduction
  static void AdvancedReductionRec(Tree *u, Tree *root, const Tree *original,
                                   Path *path, Path *bestPath, int *bestMetric,
                                   CrcCollection *crcCollection,
                                   bool *didOverflowPath);
  // Return true if tree has changed. path is expected to be valid.
  static bool ApplyPath(Tree *u, const Path *path);
  // Return true if direction was applied.
  static bool ApplyDirection(Tree **u, Tree *root, Direction direction,
                             bool *rootChanged);
  // Return true if can apply direction.
  static bool CanApplyDirection(const Tree *u, const Tree *root,
                                Direction direction);
  static bool AdvancedReduction(Tree *u);
  // Metric of given tree. The smaller is the better.
  static float GetMetric(const Tree *u) { return u->treeSize(); }
  // Bottom-up ShallowReduce starting from tree. Output is unrelated to change.
  static bool UpwardSystematicReduction(Tree *root, const Tree *tree);

  /* End of new advanced reduction */

  static bool Simplify(Tree *node, ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(Simplify, ProjectionContext, {});

  /* Ignoring EDITION_REF_WRAP_1 wrapper here so ternary can be used on these
   * methods. TODO: Remove other EditionReference wrappers on private methods if
   * they are indeed unused. */
  static bool ShallowContract(Tree *e, bool tryAll) {
    return (tryAll ? TryAllOperations : TryOneOperations)(
        e, k_contractOperations, std::size(k_contractOperations));
  }
  static bool ShallowExpand(Tree *e, bool tryAll) {
    return (tryAll ? TryAllOperations : TryOneOperations)(
        e, k_expandOperations, std::size(k_expandOperations));
  }

  // Bottom-up deep contract
  static bool DeepContract(Tree *e);
  EDITION_REF_WRAP(DeepContract);
  // Top-Bottom deep expand
  static bool DeepExpand(Tree *e);
  EDITION_REF_WRAP(DeepExpand);

  static bool ShallowApplyMatrixOperators(Tree *u, void *context = nullptr);
  EDITION_REF_WRAP_1D(ShallowApplyMatrixOperators, void *, nullptr);
  static bool DeepApplyMatrixOperators(Tree *u);
  EDITION_REF_WRAP(DeepApplyMatrixOperators);

  static bool ShallowSystematicReduce(Tree *u);
  EDITION_REF_WRAP(ShallowSystematicReduce);
  static bool DeepSystematicReduce(Tree *u);
  EDITION_REF_WRAP(DeepSystematicReduce);

  static bool SimplifyAbs(Tree *u);
  EDITION_REF_WRAP(SimplifyAbs);
  static bool SimplifyAddition(Tree *u);
  EDITION_REF_WRAP(SimplifyAddition);
  static bool SimplifyMultiplication(Tree *u);
  EDITION_REF_WRAP(SimplifyMultiplication);
  static bool SimplifyPower(Tree *u);
  EDITION_REF_WRAP(SimplifyPower);
  static bool SimplifyPowerReal(Tree *u);
  EDITION_REF_WRAP(SimplifyPowerReal);
  static bool SimplifyExp(Tree *u);
  EDITION_REF_WRAP(SimplifyExp);
  static bool SimplifyComplex(Tree *t);
  EDITION_REF_WRAP(SimplifyComplex);
  static bool SimplifyComplexArgument(Tree *t);
  EDITION_REF_WRAP(SimplifyComplexArgument);
  static bool SimplifyRealPart(Tree *t);
  EDITION_REF_WRAP(SimplifyRealPart);
  static bool SimplifyImaginaryPart(Tree *t);
  EDITION_REF_WRAP(SimplifyImaginaryPart);
  static bool SimplifySign(Tree *t);
  EDITION_REF_WRAP(SimplifySign);

  typedef bool (*Operation)(Tree *node);
  /* Replace target(..., naryTarget(A, B, ...), ...)
   * into    naryOutput(target(..., A, ...), target(..., B, ...), ...) */
  static bool DistributeOverNAry(Tree *node, BlockType target,
                                 BlockType naryTarget, BlockType naryOutput,
                                 Operation operation = ShallowSystematicReduce,
                                 int childIndex = 0);

 private:
  static bool ApplyIfMetricImproved(Tree *ref, const Tree *root,
                                    Operation operation, const Metric metric);
  static bool SimplifyLastTree(Tree *node,
                               ProjectionContext projectionContext = {});
  static bool SimplifySwitch(Tree *u);
  EDITION_REF_WRAP(SimplifySwitch);
  /* The following methods should not be called with EditionReferences.
   * TODO : ensure it cannot. */
  // Return true if child has been merged with next sibling.
  static bool MergeAdditionChildWithNext(Tree *child, Tree *next);
  // Return true if child has been merged with next sibling.
  static bool MergeMultiplicationChildWithNext(Tree *child);
  // Return true if child has been merged with one or more next siblings.
  static bool MergeMultiplicationChildrenFrom(Tree *child, int index,
                                              int *numberOfSiblings,
                                              bool *zero);
  /* Return true if child has been merged with siblings. Recursively merge next
   * siblings. */
  static bool SimplifyMultiplicationChildRec(Tree *child, int index,
                                             int *numberOfSiblings, bool *zero,
                                             bool *multiplicationChanged);
  // Simplify a sorted and sanitized multiplication.
  static bool SimplifySortedMultiplication(Tree *multiplication);
  static void ConvertPowerRealToPower(Tree *u);

  // Try all Operations until they all fail consecutively.
  static bool TryAllOperations(Tree *node, const Operation *operations,
                               int numberOfOperations);
  // Try all Operations until one of them succeed.
  static bool TryOneOperations(Tree *node, const Operation *operations,
                               int numberOfOperations);

  static bool ContractAbs(Tree *node);
  EDITION_REF_WRAP(ContractAbs);
  static bool ExpandAbs(Tree *node);
  EDITION_REF_WRAP(ExpandAbs);
  static bool ContractExpMult(Tree *node);
  EDITION_REF_WRAP(ContractExpMult);
  static bool ExpandExp(Tree *node);
  EDITION_REF_WRAP(ExpandExp);
  static bool ContractMult(Tree *node);
  EDITION_REF_WRAP(ContractMult);
  static bool ExpandMult(Tree *node);
  EDITION_REF_WRAP(ExpandMult);
  static bool ExpandMultSubOperation(Tree *node) {
    return SimplifyMultiplication(node) + ExpandMult(node);
  }
  EDITION_REF_WRAP(ExpandMultSubOperation);
  static bool ExpandPowerComplex(Tree *node);
  EDITION_REF_WRAP(ExpandPowerComplex);
  static bool ExpandPower(Tree *node);
  EDITION_REF_WRAP(ExpandPower);

  constexpr static Operation k_contractOperations[] = {
      Logarithm::ContractLn,
      ContractAbs,
      ContractExpMult,
      Trigonometry::ContractTrigonometric,
      Parametric::ContractProduct,
      ContractMult,
  };
  constexpr static Operation k_expandOperations[] = {
      ExpandAbs,
      Logarithm::ExpandLn,
      ExpandExp,
      Trigonometry::ExpandTrigonometric,
      Trigonometry::ExpandATrigonometric,
      Parametric::ExpandSum,
      Parametric::ExpandProduct,
      Arithmetic::ExpandBinomial,
      Arithmetic::ExpandPermute,
      Projection::Expand,
      ExpandPower,
      ExpandPowerComplex,
      ExpandMult,
  };
};

}  // namespace PoincareJ

#endif
