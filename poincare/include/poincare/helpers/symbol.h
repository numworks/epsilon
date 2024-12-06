#ifndef POINCARE_HELPERS_SYMBOL_H
#define POINCARE_HELPERS_SYMBOL_H

#include <poincare/old/junior_expression.h>

namespace Poincare {

namespace SymbolHelper {

const char* AnsMainAlias();
bool IsTheta(NewExpression e);
bool IsSymbol(NewExpression e, CodePoint c);
const char* GetName(NewExpression e);

}  // namespace SymbolHelper

}  // namespace Poincare

#endif
