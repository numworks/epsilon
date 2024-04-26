#include <poincare/old/context_with_parent.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>

namespace Poincare {

const JuniorExpression EmptyContext::protectedExpressionForSymbolAbstract(
    const SymbolAbstract& symbol, bool clone,
    ContextWithParent* lastDescendantContext) {
  return JuniorExpression();
}

}  // namespace Poincare
