#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/function.h>
#include <poincare/symbol.h>

namespace Poincare {

const Expression Context::expressionForSymbolAbstract(
    const SymbolAbstract& symbol, bool clone) {
  return protectedExpressionForSymbolAbstract(symbol, clone, nullptr);
}

const PoincareJ::Tree* Context::treeForSymbolIdentifier(
    const char* identifier, int length, SymbolAbstractType type) {
  if (type == SymbolAbstractType::Symbol) {
    return expressionForSymbolAbstract(Symbol::Builder(identifier, length),
                                       false)
        .tree();
  } else if (type == SymbolAbstractType::Function) {
    return expressionForSymbolAbstract(
               Function::Builder(identifier, length,
                                 Symbol::Builder(UCodePointUnknown)),
               false)
        .tree();
  }
  // TODO_PCJ: Not implemented.
  assert(false);
  return nullptr;
}

}  // namespace Poincare
