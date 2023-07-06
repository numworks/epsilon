#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <omgpj/enums.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

enum class ComplexFormat { Real, Cartesian, Polar };
enum class AngleUnit : uint8_t { Radian = 0, Degree = 1, Gradian = 2 };
enum class Strategy { Default, NumbersToFloat, ApproximateToFloat };

struct ProjectionContext {
  ComplexFormat m_complexFormat = ComplexFormat::Real;
  AngleUnit m_angleUnit = AngleUnit::Radian;
  Strategy m_strategy = Strategy::Default;
};

class Simplification {
 public:
  static bool Simplify(Tree *node);
  static bool AdvancedReduction(Tree *node);
  static bool ShallowAdvancedReduction(Tree *node, bool change);

  static bool ShallowBeautify(Tree *node, void *context = nullptr);
  static bool DeepBeautify(Tree *node,
                           ProjectionContext projectionContext = {}) {
    return ApplyShallowInDepth(node, ShallowBeautify, &projectionContext);
  }

  // TODO : Ensure NAry children are sorted before and after Expand/Contract.
  static bool ShallowContract(Tree *e, void *context = nullptr) {
    return TryAllOperations(e, k_contractOperations,
                            std::size(k_contractOperations));
  }
  static bool ShallowExpand(Tree *e, void *context = nullptr) {
    return TryAllOperations(e, k_expandOperations,
                            std::size(k_expandOperations));
  }
  static bool ShallowAlgebraicExpand(Tree *e, void *context = nullptr) {
    return TryAllOperations(e, k_algebraicExpandOperations,
                            std::size(k_algebraicExpandOperations));
  }

  static bool DeepSystemProjection(Tree *reference,
                                   ProjectionContext projectionContext = {});
  static bool ShallowSystemProjection(Tree *reference, void *projectionContext);

  static bool SystematicReduce(Tree *u);

 private:
  static bool SimplifyTrig(Tree *u);
  static bool SimplifyTrigDiff(Tree *u);
  static bool SimplifyAddition(Tree *u);
  static bool MergeAdditionChildren(Tree *u1, Tree *u2);
  static bool SimplifyMultiplication(Tree *u);
  static bool MergeMultiplicationChildren(Tree *u1, Tree *u2);
  static bool SimplifyPower(Tree *u);

  typedef bool (*ShallowOperation)(Tree *node, void *context);
  static bool ApplyShallowInDepth(Tree *node, ShallowOperation shallowOperation,
                                  void *context = nullptr);
  /* Replace target(..., naryTarget(A, B, ...), ...)
   * into    naryOutput(target(..., A, ...), target(..., B, ...), ...) */
  static bool DistributeOverNAry(Tree *node, BlockType target,
                                 BlockType naryTarget, BlockType naryOutput,
                                 int childIndex = 0);

  static bool AdvanceReduceOnTranscendental(Tree *node, bool change);
  static bool AdvanceReduceOnAlgebraic(Tree *node, bool change);
  static bool ReduceInverseFunction(Tree *node);
  static bool ExpandTranscendentalOnRational(Tree *node);
  static bool PolynomialInterpretation(Tree *node);

  typedef bool (*Operation)(Tree *node);
  // Try all Operations until they all fail consecutively.
  static bool TryAllOperations(Tree *node, const Operation *operations,
                               int numberOfOperations);

  static bool ContractAbs(Tree *node);
  static bool ExpandAbs(Tree *node);
  static bool ContractLn(Tree *node);
  static bool ExpandLn(Tree *node);
  static bool ContractExpMult(Tree *node);
  static bool ContractExpPow(Tree *node);
  static bool ExpandExp(Tree *node);
  static bool ContractTrigonometric(Tree *node);
  static bool ExpandTrigonometric(Tree *node);
  static bool ExpandMult(Tree *node);
  static bool ExpandPower(Tree *node);

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
