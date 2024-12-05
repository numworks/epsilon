#ifndef POINCARE_HELPERS_SYMBOL_H
#define POINCARE_HELPERS_SYMBOL_H

#include <poincare/old/junior_expression.h>

namespace Poincare {

namespace SymbolHelper {

const char* AnsMainAlias();
bool IsTheta(NewExpression e);
const char* GetName(NewExpression e);

}  // namespace SymbolHelper

}  // namespace Poincare

#endif
