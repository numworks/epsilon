#include <poincare/src/expression/symbol.h>
#include <poincare/src/memory/tree.h>
#include <poincare/symbol_context.h>

#include <string_view>

using namespace Poincare::Internal;

namespace Poincare {

const Tree* SymbolContext::expressionForSymbol(const Tree* symbol) const {
  /* Sequence don't have a Tree*. Ensure this method isn't called on them.
   * TODO: We could add support for explicit sequences here */
  assert(symbol->isUserSymbol() || symbol->isUserFunction());

  const char* identifier = Symbol::GetName(symbol);
  SymbolDescription description = symbolDescriptionForIdentifier(identifier);

  bool isFunction = description.type == SymbolType::Function;
  bool isSymbolOrList = description.type == SymbolType::Variable ||
                        description.type == SymbolType::List;

  if ((isFunction && symbol->isUserFunction()) ||
      (isSymbolOrList && symbol->isUserSymbol())) {
    return description.expression;
  }

  // If the description type doesn't match the Tree* type, return nullptr
  return nullptr;
}

}  // namespace Poincare
