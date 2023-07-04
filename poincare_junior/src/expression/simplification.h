#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <omgpj/enums.h>
#include <poincare_junior/src/memory/edition_reference.h>

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
  static bool Simplify(Node *node);
  static bool AdvancedReduction(Node *node);
  static bool ShallowAdvancedReduction(Node *node, bool change);

  static bool ShallowBeautify(Node *node, void *context = nullptr);
  static bool DeepBeautify(Node *node,
                           ProjectionContext projectionContext = {}) {
    return ApplyShallowInDepth(node, ShallowBeautify, &projectionContext);
  }
  static EditionReference DistributeMultiplicationOverAddition(
      EditionReference reference);

  // TODO : Ensure NAry children are sorted before and after Expand/Contract.
  static bool ShallowContract(Node *e, void *context = nullptr) {
    return TryAllOperations(e, k_contractOperations,
                            std::size(k_contractOperations));
  }
  static bool ShallowExpand(Node *e, void *context = nullptr) {
    return TryAllOperations(e, k_expandOperations,
                            std::size(k_expandOperations));
  }
  static bool ShallowAlgebraicExpand(Node *e, void *context = nullptr) {
    return TryAllOperations(e, k_algebraicExpandOperations,
                            std::size(k_algebraicExpandOperations));
  }

  static bool DeepSystemProjection(Node *reference,
                                   ProjectionContext projectionContext = {});
  static bool ShallowSystemProjection(Node *reference, void *projectionContext);

  static bool SystematicReduce(Node *u);

 private:
  static bool SimplifyTrig(Node *u);
  static bool SimplifyTrigDiff(Node *u);
  static bool SimplifyAddition(Node *u);
  static bool MergeAdditionChildren(Node *u1, Node *u2);
  static bool SimplifyMultiplication(Node *u);
  static bool MergeMultiplicationChildren(Node *u1, Node *u2);
  static bool SimplifyPower(Node *u);

  typedef bool (*ShallowOperation)(Node *node, void *context);
  static bool ApplyShallowInDepth(Node *node, ShallowOperation shallowOperation,
                                  void *context = nullptr);
  /* Replace target(..., naryTarget(A, B, ...), ...)
   * into    naryOutput(target(..., A, ...), target(..., B, ...), ...) */
  static bool DistributeOverNAry(Node *node, BlockType target,
                                 BlockType naryTarget, BlockType naryOutput,
                                 int childIndex = 0);

  static bool AdvanceReduceOnTranscendental(Node *node, bool change);
  static bool AdvanceReduceOnAlgebraic(Node *node, bool change);
  static bool ReduceInverseFunction(Node *node);
  static bool ExpandTranscendentalOnRational(Node *node);
  static bool PolynomialInterpretation(Node *node);

  typedef bool (*Operation)(Node *node);
  // Try all Operations until they all fail consecutively.
  static bool TryAllOperations(Node *node, const Operation *operations,
                               int numberOfOperations);

  static bool ContractAbs(Node *node);
  static bool ExpandAbs(Node *node);
  static bool ContractLn(Node *node);
  static bool ExpandLn(Node *node);
  static bool ContractExpMult(Node *node);
  static bool ContractExpPow(Node *node);
  static bool ExpandExp(Node *node);
  static bool ContractTrigonometric(Node *node);
  static bool ExpandTrigonometric(Node *node);
  static bool ExpandMult(Node *node);
  static bool ExpandPower(Node *node);

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
