#ifndef SIMPLIFICATION_HELPER_H
#define SIMPLIFICATION_HELPER_H

#include "expression_node.h"
#include "old_expression.h"

namespace Poincare {

class OList;

/* Class holding helper functions (mostly defaults)
 * for expression simplication (reduce / beautify). */
class SimplificationHelper {
  friend class PiecewiseOperator;

 public:
  enum class UnitReduction : uint8_t {
    KeepUnits = 0,
    BanUnits,
    ExtractUnitsOfFirstChild
  };

  enum class MatrixReduction : bool { DefinedOnMatrix = 0, UndefinedOnMatrix };

  enum class ListReduction : bool {
    DoNotDistributeOverLists = 0,
    DistributeOverLists
  };

  enum class BooleanReduction : bool {
    DefinedOnBooleans = 0,
    UndefinedOnBooleans
  };

  enum class PointReduction : bool { UndefinedOnPoint = 0, DefinedOnPoint };

  enum class UndefReduction : bool { BubbleUpUndef = 0, DoNotBubbleUpUndef };

  enum class DependencyReduction : bool { BubbleUp = 0, DoNotBubbleUp };

  static void defaultDeepReduceChildren(
      OExpression e, const ReductionContext& reductionContext);
  // DeepBeautify children and add parentheses if needed.
  static void deepBeautifyChildren(OExpression e,
                                   const ReductionContext& reductionContext);

  /* This method applies the default reduction that almost all nodes need:
   * - It bubbles up dependencies
   * - Depending on its parameters, it applies different default reductions
   *   on expressions containing Undef, OBoolean, Unit, OMatrix, OList and
   * OPoint
   */
  static OExpression defaultShallowReduce(
      OExpression e, ReductionContext* reductionContext,
      BooleanReduction booleanParameter = BooleanReduction::DefinedOnBooleans,
      UnitReduction unitParameter = UnitReduction::KeepUnits,
      MatrixReduction matrixParameter = MatrixReduction::DefinedOnMatrix,
      ListReduction listParameter = ListReduction::DoNotDistributeOverLists,
      PointReduction pointParameter = PointReduction::UndefinedOnPoint,
      UndefReduction undefParameter = UndefReduction::BubbleUpUndef,
      DependencyReduction dependencyParameter = DependencyReduction::BubbleUp);

  // This will shallowReduce the resulting expression.
  static OExpression bubbleUpDependencies(
      OExpression e, const ReductionContext& reductionContext);

  /* This method should be called only on expressions which have all their
   * children reduced */
  static OExpression distributeReductionOverLists(
      OExpression e, const ReductionContext& reductionContext);

  // Returns true if expression is a symbol or an integer.
  static bool extractInteger(OExpression e, int* integerReturnValue,
                             bool* isSymbolReturnValue);
  // Returns true if the child is a symbol or an integer.
  static bool extractIntegerChildAtIndex(OExpression e, int integerChildIndex,
                                         int* integerChildReturnValue,
                                         bool* isSymbolReturnValue);

 private:
  /* Handle circuit breaker and early reduce if should be undefined
   * Returns uninitialized handle if nothing was done, the resulting expression
   * otherwise. */
  static OExpression shallowReduceUndefined(OExpression e);
  /* If `e` contains units, replaces with undefined to parent and returns the
   * undefined handle. Returns uninitialized handle otherwise. */
  static OExpression shallowReduceBanningUnits(OExpression e);
  /* *In place* shallowReduce while keeping the units from first child.
   * The returned expression is the result with the units if units were handled.
   * Otherwise returns uninitialized handle. */
  static OExpression shallowReduceKeepingUnitsFromFirstChild(
      OExpression e, const ReductionContext& reductionContext);

  static OExpression reduceAfterBubblingUpDependencies(
      OExpression e, OList dependencies,
      const ReductionContext& reductionContext);

  static OExpression undefinedOnBooleans(OExpression e);
  static OExpression undefinedOnPoint(OExpression e);
  static OExpression undefinedOnMatrix(OExpression e,
                                       ReductionContext* reductionContext);
};
}  // namespace Poincare

#endif
