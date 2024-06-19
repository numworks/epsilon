#include <poincare/expression.h>
#include <poincare/old/context.h>
#include <poincare/old/function.h>
#include <poincare/old/symbol.h>
#include <poincare/src/expression/symbol.h>

namespace Poincare {

Context* Context::GlobalContext = nullptr;

const Expression Context::expressionForSymbolAbstract(
    const SymbolAbstract& symbol, bool clone) {
  return protectedExpressionForSymbolAbstract(symbol, clone, nullptr);
}

const Internal::Tree* Context::treeForSymbolIdentifier(
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

const Internal::Tree* Context::treeForSymbolIdentifier(
    const Internal::Tree* t) {
  return Context::treeForSymbolIdentifier(
      Internal::Symbol::GetName(t), Internal::Symbol::Length(t),
      t->isUserFunction() ? Poincare::Context::SymbolAbstractType::Function
      : t->isUserSymbol() ? Poincare::Context::SymbolAbstractType::Symbol
                          : Poincare::Context::SymbolAbstractType::Sequence);
}

}  // namespace Poincare
