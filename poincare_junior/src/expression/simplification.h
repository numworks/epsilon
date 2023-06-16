#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <omgpj/enums.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Simplification {
 public:
  static EditionReference SystematicReduction(EditionReference reference);
  static EditionReference ShallowBeautify(EditionReference reference) {
    return reference;
  }

  // TODO : Ensure NAry children are sorted before and after Expand/Contract.
  static bool Expand(EditionReference *reference);
  static bool Contract(EditionReference *reference);

  static bool ContractAbs(EditionReference *reference);
  static bool ExpandAbs(EditionReference *reference);
  static bool ContractLn(EditionReference *reference);
  static bool ExpandLn(EditionReference *reference);
  static bool ContractExpMult(EditionReference *reference);
  static bool ContractExpPow(EditionReference *reference);
  static bool ExpandExp(EditionReference *reference);
  static bool ContractTrigonometric(EditionReference *reference);
  static bool ExpandTrigonometric(EditionReference *reference);

  static bool AlgebraicExpand(EditionReference *reference);
  static bool ExpandMult(EditionReference *reference);
  static bool ExpandPower(EditionReference *reference);

  static EditionReference DivisionReduction(EditionReference reference);
  static EditionReference SubtractionReduction(EditionReference reference);
  static EditionReference DistributeMultiplicationOverAddition(
      EditionReference reference);

  enum class ProjectionContext {
    Default,
    NumbersToFloat,
    ApproximateToFloat,
  };
  static EditionReference SystemProjection(
      EditionReference reference,
      ProjectionContext complexity = ProjectionContext::Default);

  static bool AutomaticSimplify(EditionReference *u);

 private:
  static bool SimplifyRational(EditionReference *u) { return false; }
  static bool SimplifyRNE(EditionReference *u);
  static bool SimplifyRNERec(EditionReference *u);
  static bool SimplifySum(EditionReference *u);
  static bool SimplifySumRec(EditionReference *u);
  static bool MergeSums(EditionReference *p, EditionReference *q);
  static bool SimplifyProduct(EditionReference *u);
  static bool SimplifyProductRec(EditionReference *u);
  static bool MergeProducts(EditionReference *p, EditionReference *q);
  static bool SimplifyPower(EditionReference *u);

  typedef EditionReference (*NumberOperation)(const Node, const Node);
  static void ReduceNumbersInNAry(EditionReference reference,
                                  NumberOperation operation);
  static EditionReference ProjectionReduction(
      EditionReference reference, Node (*PushProjectedEExpression)(),
      Node (*PushInverse)());
};

int Compare(Node u, Node v);
}  // namespace PoincareJ

#endif
