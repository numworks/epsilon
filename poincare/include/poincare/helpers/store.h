#pragma once

#include <poincare/user_expression.h>
#include <poincare/variable_store.h>

namespace Poincare {

namespace StoreHelper {

/* TODO: remove PerformStore and StoreValueForSymbol. Poincare can detect a
 * store expression ("a->b") and fetch the value and symbol from the expression.
 * But the store operation itself should not happen in Poincare */

const UserExpression Value(const UserExpression& e);
const UserExpression Symbol(const UserExpression& e);
bool PerformStore(VariableStore& variableStore, const UserExpression& e);
bool StoreValueForSymbol(VariableStore& variableStore,
                         const UserExpression& value,
                         const UserExpression& symbol);

}  // namespace StoreHelper

}  // namespace Poincare
