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
  static bool Simplify(Tree *node, ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(Simplify, ProjectionContext, {});
  static bool AdvancedReduction(Tree *node);
  EDITION_REF_WRAP(AdvancedReduction);
  static bool ShallowAdvancedReduction(Tree *node, bool change);
  EDITION_REF_WRAP_1(ShallowAdvancedReduction, bool);

  static bool ShallowBeautify(Tree *node, void *context = nullptr);
  EDITION_REF_WRAP_1D(ShallowBeautify, void *, nullptr);
  static bool DeepBeautify(Tree *node,
                           ProjectionContext projectionContext = {}) {
    return ApplyShallowInDepth(node, ShallowBeautify, &projectionContext);
  }
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

  static bool ShallowSystemProjection(Tree *reference, void *projectionContext);
  EDITION_REF_WRAP_1D(ShallowSystemProjection, void *, nullptr);

  static bool ShallowSystematicReduce(Tree *u);
  EDITION_REF_WRAP(ShallowSystematicReduce);
  static bool DeepSystematicReduce(Tree *u);
  EDITION_REF_WRAP(DeepSystematicReduce);

  static bool SimplifyAbs(Tree *u);
  static bool SimplifyTrig(Tree *u);
  EDITION_REF_WRAP(SimplifyTrig);
  static bool SimplifyTrigDiff(Tree *u);
  static bool SimplifyAddition(Tree *u);
  static bool SimplifyMultiplication(Tree *u);
  EDITION_REF_WRAP(SimplifyMultiplication);
  static bool SimplifyPower(Tree *u);
  static bool SimplifyPowerReal(Tree *u);

 private:
  static bool SimplifyTrigSecondElement(Tree *u, bool *isOpposed);
  static bool MergeAdditionChildren(Tree *u1, Tree *u2);
  static bool MergeMultiplicationChildren(Tree *u1, Tree *u2);
  static void ConvertPowerRealToPower(Tree *u);

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
  EDITION_REF_WRAP_1(AdvanceReduceOnAlgebraic, bool);
  static bool ReduceInverseFunction(Tree *node);
  static bool ExpandTranscendentalOnRational(Tree *node);
  EDITION_REF_WRAP(ExpandTranscendentalOnRational);
  static bool PolynomialInterpretation(Tree *node);
  EDITION_REF_WRAP(PolynomialInterpretation);

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
  EDITION_REF_WRAP(ContractTrigonometric);
  static bool ExpandTrigonometric(Tree *node);
  EDITION_REF_WRAP(ExpandTrigonometric);
  static bool ExpandMult(Tree *node);
  EDITION_REF_WRAP(ExpandMult);
  static bool ExpandPower(Tree *node);
  EDITION_REF_WRAP(ExpandPower);

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
