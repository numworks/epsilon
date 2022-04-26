#include "store_context.h"
#include <assert.h>
#include <string.h>
#include <poincare/float.h>

namespace Shared {

Poincare::Context::SymbolAbstractType StoreContext::expressionTypeForIdentifier(const char * identifier, int length) {
  if (length == 2) {
    // Crop identifier
    char name[3] = {identifier[0], identifier[1], 0};
    if (isSymbol(name)) {
      return Context::SymbolAbstractType::Symbol;
    }
  }
  return Poincare::ContextWithParent::expressionTypeForIdentifier(identifier, length);
}

}
