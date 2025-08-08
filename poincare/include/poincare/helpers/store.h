#ifndef POINCARE_HELPERS_STORE_H
#define POINCARE_HELPERS_STORE_H

#include <poincare/expression.h>
#include <poincare/variable_store.h>

namespace Poincare {

namespace StoreHelper {

const UserExpression Value(const UserExpression& e);
const UserExpression Symbol(const UserExpression& e);
bool PerformStore(VariableStore* variableStore, const UserExpression& e);
bool StoreValueForSymbol(VariableStore* variableStore,
                         const UserExpression& value,
                         const UserExpression& symbol);

}  // namespace StoreHelper

}  // namespace Poincare

#endif
