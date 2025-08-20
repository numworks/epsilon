#include "symbol_store.h"

#include <poincare/expression.h>
#include <poincare/helpers/store.h>
#include <poincare/src/layout/parser.h>

namespace PoincareTest {

void store(const char* storeExpression,
           Poincare::VariableStore& variableStore) {
  Poincare::UserExpression s =
      Poincare::UserExpression::Parse(storeExpression, variableStore);
  // TODO: do not use Poincare::StoreHelper
  Poincare::StoreHelper::PerformStore(variableStore, s);
}

// TODO: create VariableStore class for poincare tests

}  // namespace PoincareTest
