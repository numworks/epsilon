#include <poincare/helpers/store.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/symbol_store_interface.h>
#include <poincare/user_expression.h>

namespace Poincare {

UserExpression StoreHelper::Value(UserExpression e) {
  assert(!e.isUninitialized() && e.isStore());
  return e.cloneChildAtIndex(0);
}

UserExpression StoreHelper::Symbol(UserExpression e) {
  assert(!e.isUninitialized() && e.isStore());
  UserExpression symbol = e.cloneChildAtIndex(1);
  assert(symbol.isUserSymbol() || symbol.isUserFunction());
  return symbol;
}

UserExpression StoreHelper::SetExpressionToStore(UserExpression e,
                                                 UserExpression valueToStore) {
  assert(!e.isUninitialized() && e.isStore());
  Internal::Tree* newStoreExpression = e.tree()->cloneTree();
  newStoreExpression->child(0)->cloneTreeOverTree(valueToStore.tree());
  UserExpression result = UserExpression::Builder(newStoreExpression);
  return result;
}

}  // namespace Poincare
