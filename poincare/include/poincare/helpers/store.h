#ifndef POINCARE_HELPERS_STORE_H
#define POINCARE_HELPERS_STORE_H

#include <poincare/expression.h>

namespace Poincare {

namespace StoreHelper {

const Expression Value(const UserExpression& e);
const UserExpression Symbol(const UserExpression& e);
bool PerformStore(Context* context, const UserExpression& e);
bool StoreValueForSymbol(Context* context, const UserExpression& value,
                         const UserExpression& symbol);

}  // namespace StoreHelper

}  // namespace Poincare

#endif
