#ifndef SIMPLIFICATION_HELPER_H
#define SIMPLIFICATION_HELPER_H

#include "expression_node.h"
#include "expression.h"

namespace Poincare {

/* Class holding helper functions (mostly defaults)
 * for expression simplication (reduce / beautify). */
class SimplificationHelper {
public:
  enum class UnitReduction : uint8_t {
    KeepUnits = 0,
    BanUnits,
    ExtractUnitsOfFirstChild
  };

  enum class MatrixReduction : bool {
    DefinedOnMatrix = 0,
    UndefinedOnMatrix
  };

  enum class ListReduction : bool {
    DoNotDistributeOverLists = 0,
    DistributeOverLists
  };

  enum class BooleanReduction : bool {
    DefinedOnBooleans = 0,
    UndefinedOnBooleans
  };

  static void defaultDeepReduceChildren(Expression e, const ReductionContext& reductionContext);
  // DeepBeautify children and add parentheses if needed.
  static void deepBeautifyChildren(Expression e, const ReductionContext& reductionContext);

  /* Apply the default reduction that almost all nodes do.
   * Step 1. shallowReduceUndefined
   * Step 2. bubble up dependencies
   * Step 3. Apply the unit reduction depending on parameter
   * Step 4. Apply the matrix reduction depending on parameter
   * Step 5. Apply the list reduction depending on parameter
   * Step 6. Apply the boolean reduction depending on parameter
   * (Steps 3, 4, 5 and 6 do nothing if parameter = 0) */
  static Expression defaultShallowReduce(Expression e, ReductionContext * reductionContext, BooleanReduction booleanParameter = BooleanReduction::DefinedOnBooleans, UnitReduction unitParameter = UnitReduction::KeepUnits, MatrixReduction matrixParameter = MatrixReduction::DefinedOnMatrix, ListReduction listParameter = ListReduction::DoNotDistributeOverLists);

  // This will shallowReduce the resulting expression.
  static Expression bubbleUpDependencies(Expression e, const ReductionContext& reductionContext);

  /* This method should be called only on expressions which have all their
   * children reduced */
  static Expression distributeReductionOverLists(Expression e, const ReductionContext& reductionContext);

  // Returns true if the child is a symbol or an integer.
  static bool extractIntegerChildAtIndex(Expression e, int integerChildIndex, int * integerChildReturnValue, bool * isSymbolReturnValue);

private:
  /* Handle circuit breaker and early reduce if should be undefined
   * Returns uninitialized handle if nothing was done, the resulting expression
   * otherwise. */
  static Expression shallowReduceUndefined(Expression e);
  /* If `e` contains units, replaces with undefined to parent and returns the
   * undefined handle. Returns uninitialized handle otherwise. */
  static Expression shallowReduceBanningUnits(Expression e);
  /* *In place* shallowReduce while keeping the units from first child.
   * The returned expression is the result with the units if units were handled.
   * Otherwise returns uninitialized handle. */
  static Expression shallowReduceKeepingUnitsFromFirstChild(Expression e, const ReductionContext& reductionContext);

  static Expression undefinedOnBooleans(Expression e);
  static Expression undefinedOnMatrix(Expression e, ReductionContext * reductionContext);
};
}

#endif
