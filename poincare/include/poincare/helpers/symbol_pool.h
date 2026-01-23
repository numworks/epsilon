#pragma once

#include <poincare/user_expression.h>

#include "symbol.h"

namespace Poincare::SymbolHelper {

bool IsTheta(UserExpression e);
bool IsUserVariable(Expression e, CodePoint c);
const char* GetName(Expression e);

// Builders
UserExpression BuildUserVariable(const char* name, int length = -1);
UserExpression BuildUserVariable(CodePoint name);
UserExpression BuildUserFunction(const char* name, UserExpression child);
UserExpression BuildUserSequence(const char* name, UserExpression child);

static inline UserExpression Ans() { return BuildUserVariable(AnsMainAlias()); }
static inline UserExpression SystemSymbol() {
  return BuildUserVariable(UCodePointUnknown);
}

}  // namespace Poincare::SymbolHelper
