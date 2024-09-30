#include <poincare/helpers/store.h>
#include <poincare/src/expression/dimension.h>

#include "poincare/old/junior_expression.h"

namespace Poincare {

const JuniorExpression StoreHelper::Value(const UserExpression& e) {
  assert(!e.isUninitialized() && e.isStore());
  return e.cloneChildAtIndex(0);
}

const SymbolAbstract StoreHelper::Symbol(const UserExpression& e) {
  assert(!e.isUninitialized() && e.isStore());
  const JuniorExpression symbol = e.cloneChildAtIndex(1);
  assert(symbol.isUserSymbol() || symbol.isUserFunction());
  return static_cast<const SymbolAbstract&>(symbol);
}

bool StoreHelper::PerformStore(Context* context, const UserExpression& e) {
  return StoreValueForSymbol(context, Value(e), Symbol(e));
}

bool StoreHelper::StoreValueForSymbol(Context* context,
                                      const UserExpression& value,
                                      const SymbolAbstract& symbol) {
  assert(!value.isUninitialized());
  assert(symbol.isUserSymbol() || symbol.isUserFunction());
  return context->setExpressionForSymbolAbstract(value, symbol);
}

}  // namespace Poincare
