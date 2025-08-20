#include "symbol_store.h"

#include <poincare/expression.h>
#include <poincare/helpers/store.h>
#include <poincare/src/layout/parser.h>

namespace PoincareTest {

void store(const char* storeExpression,
           Poincare::VariableStore& variableStore) {
  Poincare::UserExpression storeUserExpression =
      Poincare::UserExpression::Parse(storeExpression, variableStore);
  variableStore.setExpressionForUserNamed(
      Poincare::StoreHelper::Value(storeUserExpression),
      Poincare::StoreHelper::Symbol(storeUserExpression));
}

// TODO: create VariableStore class for poincare tests

}  // namespace PoincareTest
