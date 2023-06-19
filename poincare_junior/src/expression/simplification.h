#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <omgpj/enums.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Simplification {
 public:
  static EditionReference SystematicReduction(EditionReference reference);
  static bool ShallowBeautify(EditionReference *reference,
                              void *context = nullptr);
  static EditionReference DeepBeautify(EditionReference reference) {
    return ApplyShallowInDepth(reference, ShallowBeautify);
  }
  static EditionReference DivisionReduction(EditionReference reference);
  static EditionReference SubtractionReduction(EditionReference reference);
  static EditionReference DistributeMultiplicationOverAddition(
      EditionReference reference);

  // TODO : Ensure NAry children are sorted before and after Expand/Contract.
  static bool ShallowContract(EditionReference *e, void *context = nullptr) {
    return TryAllOperations(e, k_contractOperations,
                            std::size(k_contractOperations));
  }
  static bool ShallowExpand(EditionReference *e, void *context = nullptr) {
    return TryAllOperations(e, k_expandOperations,
                            std::size(k_expandOperations));
  }
  static bool ShallowAlgebraicExpand(EditionReference *e,
                                     void *context = nullptr) {
    return TryAllOperations(e, k_algebraicExpandOperations,
                            std::size(k_algebraicExpandOperations));
  }

  enum class ProjectionContext {
    Default,
    NumbersToFloat,
    ApproximateToFloat,
  };
  static EditionReference DeepSystemProjection(
      EditionReference reference,
      ProjectionContext projectionContext = ProjectionContext::Default);
  static bool ShallowSystemProjection(EditionReference *reference,
                                      void *projectionContext);

  static bool SystematicReduce(EditionReference *u);

 private:
  static bool SimplifyRationalTree(EditionReference *u);
  static bool SimplifySum(EditionReference *u);
  // SimplifySumRec expects an Add and returns an Add
  static bool SimplifySumRec(EditionReference *u);
  static bool MergeSums(EditionReference *p, EditionReference *q);
  static bool SimplifyProduct(EditionReference *u);
  // SimplifyProductRec expects a Mult and returns a Mult
  static bool SimplifyProductRec(EditionReference *u);
  static bool MergeProducts(EditionReference *p, EditionReference *q);
  static bool SimplifyPower(EditionReference *u);

  typedef EditionReference (*NumberOperation)(const Node, const Node);
  static void ReduceNumbersInNAry(EditionReference reference,
                                  NumberOperation operation);
  static EditionReference ProjectionReduction(
      EditionReference reference, Node (*PushProjectedEExpression)(),
      Node (*PushInverse)());

  typedef bool (*ShallowOperation)(EditionReference *reference, void *context);
  static EditionReference ApplyShallowInDepth(EditionReference reference,
                                              ShallowOperation shallowOperation,
                                              void *context = nullptr);
  static bool SmartExpand(EditionReference *reference, Node pattern,
                          Node structure);

  typedef bool (*Operation)(EditionReference *reference);
  // Try all Operations until they all fail consecutively.
  static bool TryAllOperations(EditionReference *e, const Operation *operations,
                               int numberOfOperations);

  static bool ContractAbs(EditionReference *reference);
  static bool ExpandAbs(EditionReference *reference);
  static bool ContractLn(EditionReference *reference);
  static bool ExpandLn(EditionReference *reference);
  static bool ContractExpMult(EditionReference *reference);
  static bool ContractExpPow(EditionReference *reference);
  static bool ExpandExp(EditionReference *reference);
  static bool ContractTrigonometric(EditionReference *reference);
  static bool ExpandTrigonometric(EditionReference *reference);
  static bool ExpandMult(EditionReference *reference);
  static bool ExpandPower(EditionReference *reference);

  constexpr static Operation k_contractOperations[] = {
      ContractLn, ContractExpPow, ContractAbs, ContractExpMult,
      ContractTrigonometric};
  constexpr static Operation k_expandOperations[] = {
      ExpandAbs, ExpandLn, ExpandExp, ExpandTrigonometric};
  constexpr static Operation k_algebraicExpandOperations[] = {ExpandPower,
                                                              ExpandMult};
};

}  // namespace PoincareJ

#endif
