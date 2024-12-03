#include <poincare/expression.h>
#include <poincare/old/context.h>
#include <poincare/old/function.h>
#include <poincare/old/symbol.h>
#include <poincare/src/expression/symbol.h>

namespace Poincare {

Context* Context::GlobalContext = nullptr;

const Internal::Tree* Context::expressionForSymbolAbstract(
    const Internal::Tree* symbol) {
  return protectedExpressionForSymbolAbstract(symbol, nullptr);
}

}  // namespace Poincare
