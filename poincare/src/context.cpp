#include <poincare/context.h>
#include <poincare/expression.h>

namespace Poincare {

const Expression Context::expressionForSymbolAbstract(
    const SymbolAbstract& symbol, bool clone) {
  return protectedExpressionForSymbolAbstract(symbol, clone, nullptr);
}

}  // namespace Poincare
