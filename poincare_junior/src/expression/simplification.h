#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <omgpj/enums.h>
#include <poincare_junior/src/expression/arithmetic.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/expression/metric.h>
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
      ContractLn,
      ContractAbs,
      ContractExpMult,
      ContractTrigonometric,
      Arithmetic::ContractDecimals,
  };
  constexpr static Operation k_expandOperations[] = {
      ExpandAbs,
      ExpandLn,
      ExpandExp,
      ExpandTrigonometric,
      Parametric::ExpandSum,
      Parametric::ExpandProduct,
      Arithmetic::ExpandDecimals,
      Arithmetic::ExpandLnOnRational,
  };
  constexpr static Operation k_algebraicExpandOperations[] = {
      ExpandPower, ExpandPowerComplex, ExpandMult};
};

}  // namespace PoincareJ

#endif
