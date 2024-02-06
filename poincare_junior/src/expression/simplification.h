#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "arithmetic.h"
#include "logarithm.h"
#include "parametric.h"
#include "projection.h"
#include "trigonometry.h"

namespace PoincareJ {

/* TODO: Implement PolynomialInterpretation. Prepare the expression for
 * Polynomial interpretation (expand TranscendentalOnRationals and algebraic
 * trees.) */

class Simplification {
 public:
  static bool Simplify(Tree *node, ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(Simplify, ProjectionContext, {});

  static bool ShallowApplyMatrixOperators(Tree *u, void *context = nullptr);
  EDITION_REF_WRAP_1D(ShallowApplyMatrixOperators, void *, nullptr);
  static bool DeepApplyMatrixOperators(Tree *u);
  EDITION_REF_WRAP(DeepApplyMatrixOperators);

  static bool ShallowSystemReduce(Tree *u);
  EDITION_REF_WRAP(ShallowSystemReduce);
  static bool DeepSystemReduce(Tree *u);
  EDITION_REF_WRAP(DeepSystemReduce);

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
  static bool SimplifyLnReal(Tree *u);
  EDITION_REF_WRAP(SimplifyLnReal);
  static bool SimplifyExp(Tree *u);
  EDITION_REF_WRAP(SimplifyExp);
  static bool SimplifyComplexArgument(Tree *t);
  EDITION_REF_WRAP(SimplifyComplexArgument);
  static bool SimplifyComplexPart(Tree *t);
  EDITION_REF_WRAP(SimplifyComplexPart);
  static bool SimplifySign(Tree *t);
  EDITION_REF_WRAP(SimplifySign);

  static bool ShallowContract(Tree *e, bool tryAll) {
    return (tryAll ? TryAllOperations : TryOneOperation)(
        e, k_contractOperations, std::size(k_contractOperations));
  }
  EDITION_REF_WRAP_1(ShallowContract, bool);
  static bool ShallowExpand(Tree *e, bool tryAll) {
    return (tryAll ? TryAllOperations : TryOneOperation)(
        e, k_expandOperations, std::size(k_expandOperations));
  }
  EDITION_REF_WRAP_1(ShallowExpand, bool);

  // Bottom-up deep contract
  static bool DeepContract(Tree *e);
  EDITION_REF_WRAP(DeepContract);
  // Top-Bottom deep expand
  static bool DeepExpand(Tree *e);
  EDITION_REF_WRAP(DeepExpand);

  typedef bool (*Operation)(Tree *node);

 private:
  /* These private methods should never be called on EditionReferences.
   * TODO: ensure it cannot. */
  static bool SimplifyLastTree(Tree *node,
                               ProjectionContext projectionContext = {});
  static bool SimplifySwitch(Tree *u);
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

  /* Expand/Contract operations */

  // Try all Operations until they all fail consecutively.
  static bool TryAllOperations(Tree *node, const Operation *operations,
                               int numberOfOperations);
  // Try all Operations until one of them succeed.
  static bool TryOneOperation(Tree *node, const Operation *operations,
                              int numberOfOperations);

  static bool ExpandImRe(Tree *node);
  static bool ContractAbs(Tree *node);
  static bool ExpandAbs(Tree *node);
  static bool ContractExpMult(Tree *node);
  static bool ExpandExp(Tree *node);
  static bool ContractMult(Tree *node);
  static bool ExpandMult(Tree *node);
  static bool ExpandPower(Tree *node);

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
      Parametric::ExpandSum,
      Parametric::ExpandProduct,
      Arithmetic::ExpandBinomial,
      Arithmetic::ExpandPermute,
      Projection::Expand,
      ExpandPower,
      ExpandMult,
      ExpandImRe,
  };
};

}  // namespace PoincareJ

#endif
