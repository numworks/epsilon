#include <poincare/expression.h>
#include <poincare/helpers/store.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/variable_store.h>

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

bool StoreHelper::PerformStore(VariableStore* variableStore,
                               const UserExpression& e) {
  return StoreValueForSymbol(variableStore, Value(e), Symbol(e));
}

bool StoreHelper::StoreValueForSymbol(VariableStore* variableStore,
                                      const UserExpression& value,
                                      const UserExpression& symbol) {
  assert(!value.isUninitialized());
  assert(symbol.isUserSymbol() || symbol.isUserFunction());
  return variableStore->setExpressionForUserNamed(value, symbol);
}

}  // namespace Poincare
