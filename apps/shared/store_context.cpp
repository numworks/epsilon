#include "store_context.h"
#include <poincare/decimal.h>
#include <assert.h>
#include <string.h>

using namespace Poincare;

namespace Shared {

void StoreContext::setExpressionForSymbolName(const Expression & expression, const char * symbolName, Context & context) {
  m_parentContext->setExpressionForSymbolName(expression, symbolName, context);
}

}
