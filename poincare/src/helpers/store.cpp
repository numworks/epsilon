#include <poincare/expression.h>
#include <poincare/helpers/store.h>
#include <poincare/src/expression/dimension.h>

namespace Poincare {

const Expression StoreHelper::Value(const UserExpression& e) {
  assert(!e.isUninitialized() && e.isStore());
  return e.cloneChildAtIndex(0);
}

const UserExpression StoreHelper::Symbol(const UserExpression& e) {
  assert(!e.isUninitialized() && e.isStore());
  const UserExpression symbol = e.cloneChildAtIndex(1);
  assert(symbol.isUserSymbol() || symbol.isUserFunction());
  return symbol;
}

bool StoreHelper::PerformStore(Context* context, const UserExpression& e) {
  return StoreValueForSymbol(context, Value(e), Symbol(e));
}

bool StoreHelper::StoreValueForSymbol(Context* context,
                                      const UserExpression& value,
                                      const UserExpression& symbol) {
  assert(!value.isUninitialized());
  assert(symbol.isUserSymbol() || symbol.isUserFunction());
  return context->setExpressionForUserNamed(value, symbol);
}

}  // namespace Poincare
