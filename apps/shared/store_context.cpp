#include "store_context.h"
#include <poincare/decimal.h>
#include <assert.h>
#include <string.h>

using namespace Poincare;

namespace Shared {

void StoreContext::setExpressionForSymbolName(const Expression * expression, const Symbol * symbol, Context & context) {
  m_parentContext->setExpressionForSymbolName(expression, symbol, context);
}

}
