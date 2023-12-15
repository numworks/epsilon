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

class Simplification {
 public:
  /* New advanced reduction */

  // Ordered list of CRC encountered during advanced reduction.
  class CrcCollection {
   public:
    CrcCollection() : m_length(0) {}
    // Return false if CRC was already explored
    bool add(uint32_t crc);
    // Max Expand/Contract combination possibilities
    static constexpr size_t k_size = 1024;
    uint32_t collection[k_size];
    size_t m_length;
  };
  enum class Direction : uint8_t {
    NextNode = 0,
    Contract = 1,
    Expand = 2,
  };
  static constexpr uint8_t k_numberOfDirection =
      static_cast<uint8_t>(Direction::Expand) + 1;
  // Path in exploration of a tree's advanced reduction.
  class Path {
   public:
    Path() : m_length(0) {}
    Direction pop() {
      assert(m_length > 0);
      return m_stack[--m_length];
    }
    void append(Direction direction);
    Direction direction(size_t index) const {
      assert(index < m_length);
      return m_stack[index];
    }
    size_t length() const { return m_length; }
    // Path max length
    static constexpr size_t k_size = 1024;
    /* TODO : This structure isn't optimized.
     * [NextNode, NextNode, NextNode,Contract, NextNode, Expand, Expand]
     * could be stored as
     * [NextNode,Contract, NextNode, Expand] and [3,1,1,2] or
     * [3, 0, 1, UINT8_MAX, UINT8_MAX] (0 is Contract, UINT8_MAX Expand). */
    Direction m_stack[k_size];
    size_t m_length;
  };
  // Recursive new advanced reduction
  static void NewAdvancedReductionRec(Tree *u, Tree *root, const Tree *original,
                                      Path *path, Path *bestPath,
                                      int *bestMetric,
                                      CrcCollection *crcCollection);
  // Return true if tree has changed. path is expected to be valid.
  static bool ApplyPath(Tree *u, const Path *path);
  // Return true if direction was applied.
  static bool ApplyDirection(Tree **u, Tree *root, Direction direction,
                             bool *rootChanged);
  // Return true if can apply direction.
  static bool CanApplyDirection(const Tree *u, const Tree *root,
                                Direction direction);
  // ShallowAlgebraicExpand(u) + ShallowExpand(u). TODO : Merge both.
  static bool NewShallowExpand(Tree *u);
  static bool NewAdvancedReduction(Tree *u);
  // Metric of given tree. The smaller is the better.
  static float GetMetric(const Tree *u) { return u->treeSize(); }
  // Bottom-up ShallowReduce starting from tree. Output is unrelated to change.
  static bool UpwardSystematicReduction(Tree *root, const Tree *tree);

  /* End of new advanced reduction */

  static bool Simplify(Tree *node, ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(Simplify, ProjectionContext, {});
  static bool AdvancedReduction(Tree *node, const Tree *root);
  EDITION_REF_WRAP_1(AdvancedReduction, const Tree *);
  static bool ShallowAdvancedReduction(Tree *node, const Tree *root);
  EDITION_REF_WRAP_1(ShallowAdvancedReduction, const Tree *);

  // TODO : Ensure NAry children are sorted before and after Expand/Contract.
  static bool ShallowContract(Tree *e) {
    return TryAllOperations(e, k_contractOperations,
                            std::size(k_contractOperations));
  }
  EDITION_REF_WRAP(ShallowContract);
  static bool ShallowExpand(Tree *e) {
    return TryAllOperations(e, k_expandOperations,
                            std::size(k_expandOperations));
  }
  EDITION_REF_WRAP(ShallowExpand);
  static bool ShallowAlgebraicExpand(Tree *e) {
    return TryAllOperations(e, k_algebraicExpandOperations,
                            std::size(k_algebraicExpandOperations));
  }
  EDITION_REF_WRAP(ShallowAlgebraicExpand);

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

  static bool AdvanceReduceOnTranscendental(Tree *node, const Tree *root);
  EDITION_REF_WRAP_1(AdvanceReduceOnTranscendental, const Tree *);
  static bool AdvanceReduceOnAlgebraic(Tree *node, const Tree *root);
  EDITION_REF_WRAP_1(AdvanceReduceOnAlgebraic, const Tree *);
  static bool ReduceInverseFunction(Tree *node);
  EDITION_REF_WRAP(ReduceInverseFunction);
  static bool ExpandTranscendentalOnRational(Tree *node);
  EDITION_REF_WRAP(ExpandTranscendentalOnRational);
  static bool PolynomialInterpretation(Tree *node);
  EDITION_REF_WRAP(PolynomialInterpretation);

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
  };
  constexpr static Operation k_algebraicExpandOperations[] = {
      ExpandPower, ExpandPowerComplex, ExpandMult};
};

}  // namespace PoincareJ

#endif
