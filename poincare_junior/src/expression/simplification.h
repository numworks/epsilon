#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <omgpj/enums.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/memory/edition_reference.h>

#include "context.h"
#include "parametric.h"

namespace PoincareJ {

struct ProjectionContext {
  ComplexFormat m_complexFormat = ComplexFormat::Real;
  AngleUnit m_angleUnit = AngleUnit::Radian;
  Strategy m_strategy = Strategy::Default;
  Dimension m_dimension = Dimension();
};

class Simplification {
 public:
  static bool Simplify(Tree *node, ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(Simplify, ProjectionContext, {});
  static bool AdvancedReduction(Tree *node, const Tree *root);
  EDITION_REF_WRAP_1(AdvancedReduction, const Tree *);
  static bool ShallowAdvancedReduction(Tree *node, const Tree *root,
                                       bool change);
  EDITION_REF_WRAP_2(ShallowAdvancedReduction, const Tree *, bool);

  static bool ShallowBeautify(Tree *node, void *context = nullptr);
  EDITION_REF_WRAP_1D(ShallowBeautify, void *, nullptr);
  static bool DeepBeautify(Tree *node,
                           ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(DeepBeautify, ProjectionContext, {});

  // TODO : Ensure NAry children are sorted before and after Expand/Contract.
  static bool ShallowContract(Tree *e, void *context = nullptr) {
    return TryAllOperations(e, k_contractOperations,
                            std::size(k_contractOperations));
  }
  EDITION_REF_WRAP_1D(ShallowContract, void *, nullptr);
  static bool ShallowExpand(Tree *e, void *context = nullptr) {
    return TryAllOperations(e, k_expandOperations,
                            std::size(k_expandOperations));
  }
  EDITION_REF_WRAP_1D(ShallowExpand, void *, nullptr);
  static bool ShallowAlgebraicExpand(Tree *e, void *context = nullptr) {
    return TryAllOperations(e, k_algebraicExpandOperations,
                            std::size(k_algebraicExpandOperations));
  }
  EDITION_REF_WRAP_1D(ShallowAlgebraicExpand, void *, nullptr);

  static bool DeepSystemProjection(Tree *reference,
                                   ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(DeepSystemProjection, ProjectionContext, {});

  static bool ShallowSystemProjection(Tree *reference,
                                      void *projectionContext = nullptr);
  EDITION_REF_WRAP_1D(ShallowSystemProjection, void *, nullptr);

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
  static bool SimplifyTrig(Tree *u);
  EDITION_REF_WRAP(SimplifyTrig);
  static bool SimplifyTrigDiff(Tree *u);
  EDITION_REF_WRAP(SimplifyTrigDiff);
  static bool SimplifyAddition(Tree *u);
  EDITION_REF_WRAP(SimplifyAddition);
  static bool SimplifyMultiplication(Tree *u);
  EDITION_REF_WRAP(SimplifyMultiplication);
  static bool SimplifyPower(Tree *u);
  EDITION_REF_WRAP(SimplifyPower);
  static bool SimplifyPowerReal(Tree *u);
  EDITION_REF_WRAP(SimplifyPowerReal);
  static bool SimplifyLn(Tree *u);
  EDITION_REF_WRAP(SimplifyLn);
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

  typedef bool (*Operation)(Tree *node);
  /* Replace target(..., naryTarget(A, B, ...), ...)
   * into    naryOutput(target(..., A, ...), target(..., B, ...), ...) */
  static bool DistributeOverNAry(Tree *node, BlockType target,
                                 BlockType naryTarget, BlockType naryOutput,
                                 Operation operation = ShallowSystematicReduce,
                                 int childIndex = 0);

 private:
  static bool SimplifySwitch(Tree *u);
  EDITION_REF_WRAP(SimplifySwitch);
  static bool SimplifyTrigSecondElement(Tree *u, bool *isOpposed);
  EDITION_REF_WRAP_1(SimplifyTrigSecondElement, bool *);
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

  // Apply Shallow operation in a Top Down manner.
  typedef bool (*ShallowOperation)(Tree *node, void *context);
  static bool ApplyShallowInDepth(Tree *node, ShallowOperation shallowOperation,
                                  void *context = nullptr);
  // Try all Operations until they all fail consecutively.
  static bool TryAllOperations(Tree *node, const Operation *operations,
                               int numberOfOperations);

  static bool AdvanceReduceOnTranscendental(Tree *node, const Tree *root,
                                            bool change);
  EDITION_REF_WRAP_2(AdvanceReduceOnTranscendental, const Tree *, bool);
  static bool AdvanceReduceOnAlgebraic(Tree *node, const Tree *root,
                                       bool change);
  EDITION_REF_WRAP_2(AdvanceReduceOnAlgebraic, const Tree *, bool);
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
  static bool ContractLn(Tree *node);
  EDITION_REF_WRAP(ContractLn);
  static bool ExpandLn(Tree *node);
  EDITION_REF_WRAP(ExpandLn);
  static bool ContractExpMult(Tree *node);
  EDITION_REF_WRAP(ContractExpMult);
  static bool ExpandExp(Tree *node);
  EDITION_REF_WRAP(ExpandExp);
  static bool ContractTrigonometric(Tree *node);
  EDITION_REF_WRAP(ContractTrigonometric);
  static bool ExpandTrigonometric(Tree *node);
  EDITION_REF_WRAP(ExpandTrigonometric);
  static bool ExpandMult(Tree *node);
  static bool ExpandMultSubOperation(Tree *node) {
    return SimplifyMultiplication(node) + ExpandMult(node);
  }
  EDITION_REF_WRAP(ExpandMult);
  static bool ExpandPowerComplex(Tree *node);
  EDITION_REF_WRAP(ExpandPowerComplex);
  static bool ExpandPower(Tree *node);
  EDITION_REF_WRAP(ExpandPower);

  constexpr static Operation k_contractOperations[] = {
      ContractLn, ContractAbs, ContractExpMult, ContractTrigonometric};
  constexpr static Operation k_expandOperations[] = {ExpandAbs,
                                                     ExpandLn,
                                                     ExpandExp,
                                                     ExpandTrigonometric,
                                                     Parametric::ExpandSum,
                                                     Parametric::ExpandProduct};
  constexpr static Operation k_algebraicExpandOperations[] = {
      ExpandPower, ExpandPowerComplex, ExpandMult};
};

}  // namespace PoincareJ

#endif
