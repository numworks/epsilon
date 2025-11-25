#include <poincare/function_properties/integral.h>
#include <poincare/sign.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/system_expression.h>

namespace Poincare {

using namespace Internal;

Tree* BuildIntegralTree(const SystemExpression& expression,
                        const char* variableName,
                        const SystemExpression& lowerBound,
                        const SystemExpression& upperBound) {
  Tree* function = expression.tree()->cloneTree();
  Variables::ReplaceSymbol(function, variableName, 0,
                           ComplexProperties::Real());
  // Sort again, since Var and UserSymbol don't sort the same way.
  NAry::DeepSort(function);

  if (!Internal::Dimension::Get(function).isScalar()) {
    // Non-scalar functions cannot be integrated
    function->cloneTreeOverTree(KUndefUnhandledDimension);
    return function;
  }

  TreeRef result = PatternMatching::CreateReduce(
      // Unknown doesn't matter and could be anything
      KIntegral(KTemporaryUnknownSymbol, KA, KB, KC),
      PatternMatching::Context(
          ContextTrees{.KA = lowerBound, .KB = upperBound, .KC = function},
          ContextScopes{.KC = 1}));
  function->removeTree();
  return result;
}

SystemExpression IntegralBetweenBounds(const SystemExpression& expression,
                                       const char* variableName,
                                       const SystemExpression& lowerBound,
                                       const SystemExpression& upperBound) {
  Tree* integral =
      BuildIntegralTree(expression, variableName, lowerBound, upperBound);
  return SystemExpression::Builder(integral);
}

template <typename T>
T ApproximateIntegralBetweenBounds(const SystemExpression& expression,
                                   const char* variableName,
                                   const SystemExpression& lowerBound,
                                   const SystemExpression& upperBound) {
  Tree* integral =
      BuildIntegralTree(expression, variableName, lowerBound, upperBound);
  T result = Approximation::To<T>(integral, Approximation::Parameters{
                                                .isRootAndCanHaveRandom = true,
                                                .prepare = true,
                                            });
  integral->removeTree();
  return result;
}

template float ApproximateIntegralBetweenBounds<float>(
    const SystemExpression& expression, const char* symbolName,
    const SystemExpression& lowerBound, const SystemExpression& upperBound);
template double ApproximateIntegralBetweenBounds<double>(
    const SystemExpression& expression, const char* symbolName,
    const SystemExpression& lowerBound, const SystemExpression& upperBound);

}  // namespace Poincare
