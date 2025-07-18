#include <poincare/expression.h>
#include <poincare/function_properties/integral.h>
#include <poincare/sign.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/pattern_matching.h>

namespace Poincare {

SystemExpression IntegralBetweenBounds(SystemExpression expression,
                                       const char* variableName,
                                       SystemExpression lowerBound,
                                       SystemExpression upperBound) {
  using namespace Internal;
  Tree* function = expression.tree()->cloneTree();
  Variables::ReplaceSymbol(function, variableName, 0, ComplexSign::Real());
  SystemExpression result =
      SystemExpression::Builder(PatternMatching::CreateSimplify(
          KIntegral(KVarX, KA, KB, KC),
          PatternMatching::Context(
              ContextTrees{.KA = lowerBound, .KB = upperBound, .KC = function},
              ContextScopes{.KC = 1})));
  function->removeTree();
  return result;
}

}  // namespace Poincare
