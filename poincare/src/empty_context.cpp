#include <poincare/context_with_parent.h>
#include <poincare/empty_context.h>
#include <poincare/junior_expression.h>

namespace Poincare {

const JuniorExpression EmptyContext::protectedExpressionForSymbolAbstract(
    const SymbolAbstract& symbol, bool clone,
    ContextWithParent* lastDescendantContext) {
  return JuniorExpression();
}

}  // namespace Poincare
