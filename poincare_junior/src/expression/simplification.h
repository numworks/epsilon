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
  static bool Simplify(EditionReference *reference);
  static bool AdvancedReduction(EditionReference *reference);
  static bool ShallowAdvancedReduction(EditionReference *reference,
                                       bool change);

  static bool ShallowBeautify(EditionReference *reference,
                              void *context = nullptr);
  static bool DeepBeautify(EditionReference *reference,
                           ProjectionContext projectionContext = {}) {
    return ApplyShallowInDepth(reference, ShallowBeautify, &projectionContext);
  }
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

  static bool DeepSystemProjection(EditionReference *reference,
                                   ProjectionContext projectionContext = {});
  static bool ShallowSystemProjection(EditionReference *reference,
                                      void *projectionContext);

  static bool SystematicReduce(EditionReference *u);

 private:
  static bool SimplifyRationalTree(EditionReference *u);
  static bool SimplifyTrig(EditionReference *u);
  static bool SimplifyTrigDiff(EditionReference *u);
  static bool SimplifyAddition(EditionReference *u);
  static bool MergeAdditionChildren(Node *u1, Node *u2);
  static bool SimplifyMultiplication(EditionReference *u);
  static bool MergeMultiplicationChildren(Node *u1, Node *u2);
  static bool SimplifyPower(EditionReference *u);

  typedef bool (*ShallowOperation)(EditionReference *reference, void *context);
  static bool ApplyShallowInDepth(EditionReference *reference,
                                  ShallowOperation shallowOperation,
                                  void *context = nullptr);
  /* Replace target(..., naryTarget(A, B, ...), ...)
   * into    naryOutput(target(..., A, ...), target(..., B, ...), ...) */
  static bool DistributeOverNAry(EditionReference *reference, BlockType target,
                                 BlockType naryTarget, BlockType naryOutput,
                                 int childIndex = 0);

  static bool AdvanceReduceOnTranscendental(EditionReference *reference,
                                            bool change);
  static bool AdvanceReduceOnAlgebraic(EditionReference *reference,
                                       bool change);
  static bool ReduceInverseFunction(EditionReference *reference);
  static bool ExpandTranscendentalOnRational(EditionReference *reference);
  static bool PolynomialInterpretation(EditionReference *reference);

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
