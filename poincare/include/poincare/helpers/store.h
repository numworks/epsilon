#ifndef POINCARE_HELPERS_STORE_H
#define POINCARE_HELPERS_STORE_H

#include <poincare/old/junior_expression.h>

namespace Poincare {

namespace StoreHelper {

const JuniorExpression Value(const UserExpression& e);
const JuniorSymbolAbstract Symbol(const UserExpression& e);
bool PerformStore(Context* context, const UserExpression& e);
bool StoreValueForSymbol(Context* context, const UserExpression& value,
                         const JuniorSymbolAbstract& symbol);

}  // namespace StoreHelper

}  // namespace Poincare

#endif
