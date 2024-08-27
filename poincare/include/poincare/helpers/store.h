#ifndef POINCARE_HELPERS_STORE_H
#define POINCARE_HELPERS_STORE_H

#include <poincare/old/junior_expression.h>
#include <poincare/old/symbol_abstract.h>

namespace Poincare {

namespace StoreHelper {

const JuniorExpression Value(const UserExpression& e);
const SymbolAbstract Symbol(const UserExpression& e);
bool Store(Context* context, const UserExpression& e);
bool StoreValueForSymbol(Context* context, const UserExpression& value,
                         const SymbolAbstract& symbol);

}  // namespace StoreHelper

}  // namespace Poincare

#endif
