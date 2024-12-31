#ifndef POINCARE_HELPERS_SYMBOL_H
#define POINCARE_HELPERS_SYMBOL_H

#include <poincare/expression.h>
#include <poincare/src/expression/symbol.h>

namespace Poincare {

namespace SymbolHelper {
using Internal::Symbol::k_maxNameLength;
using Internal::Symbol::k_maxNameLengthWithoutQuotationMarks;
using Internal::Symbol::k_maxNameSize;
using Internal::Symbol::NameHasQuotationMarks;
using Internal::Symbol::NameLengthIsValid;

size_t NameWithoutQuotationMarks(char* buffer, size_t bufferSize,
                                 const char* name, size_t nameLength);

const char* AnsMainAlias();
bool IsTheta(NewExpression e);
bool IsSymbol(NewExpression e, CodePoint c);
const char* GetName(NewExpression e);

// Builders
UserExpression BuildSymbol(const char* name, int length = -1);
UserExpression BuildSymbol(CodePoint name);
UserExpression BuildFunction(const char* name, Expression child);
UserExpression BuildSequence(const char* name, Expression child);

static inline UserExpression Ans() { return BuildSymbol(AnsMainAlias()); }
static inline UserExpression SystemSymbol() {
  return BuildSymbol(UCodePointUnknown);
}

}  // namespace SymbolHelper
}  // namespace Poincare

#endif
