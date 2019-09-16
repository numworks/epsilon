#include "store_context.h"
#include <poincare/decimal.h>
#include <assert.h>
#include <string.h>

using namespace Poincare;

namespace Shared {

void StoreContext::setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol) {
  m_parentContext->setExpressionForSymbolAbstract(expression, symbol);
}

}
