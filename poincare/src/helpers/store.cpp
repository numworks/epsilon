#include <poincare/helpers/store.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/user_expression.h>
#include <poincare/variable_store.h>

namespace Poincare {

const UserExpression StoreHelper::Value(const UserExpression& e) {
  assert(!e.isUninitialized() && e.isStore());
  return e.cloneChildAtIndex(0);
}

const UserExpression StoreHelper::Symbol(const UserExpression& e) {
  assert(!e.isUninitialized() && e.isStore());
  const UserExpression symbol = e.cloneChildAtIndex(1);
  assert(symbol.isUserSymbol() || symbol.isUserFunction());
  return symbol;
}

}  // namespace Poincare
