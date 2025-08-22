#pragma once

#include <poincare/expression.h>

#include "symbol.h"

namespace Poincare::SymbolHelper {

bool IsTheta(UserExpression e);
bool IsSymbol(Expression e, CodePoint c);
const char* GetName(Expression e);

// Builders
UserExpression BuildSymbol(const char* name, int length = -1);
UserExpression BuildSymbol(CodePoint name);
UserExpression BuildFunction(const char* name, UserExpression child);
UserExpression BuildSequence(const char* name, UserExpression child);

static inline UserExpression Ans() { return BuildSymbol(AnsMainAlias()); }
static inline UserExpression SystemSymbol() {
  return BuildSymbol(UCodePointUnknown);
}

}  // namespace Poincare::SymbolHelper
