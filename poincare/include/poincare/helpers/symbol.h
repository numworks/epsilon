#ifndef POINCARE_HELPERS_SYMBOL_H
#define POINCARE_HELPERS_SYMBOL_H

#include <poincare/old/junior_expression.h>

namespace Poincare {

namespace SymbolHelper {
/* A symbol  can have a max length of 7 chars, or 9 if it's
 * surrounded by quotation marks.
 * This makes it so a 9 chars name (with quotation marks), can be
 * turned into a 7 char name in the result cells of the solver (by
 * removing the quotation marks). */
constexpr static size_t k_maxNameLengthWithoutQuotationMarks = 7;
constexpr static size_t k_maxNameLength =
    k_maxNameLengthWithoutQuotationMarks + 2;
constexpr static size_t k_maxNameSize = k_maxNameLength + 1;

constexpr static bool NameHasQuotationMarks(const char* name, size_t length) {
  return length > 2 && name[0] == '"' && name[length - 1] == '"';
}
constexpr static bool NameLengthIsValid(const char* name, size_t length) {
  return length <= k_maxNameLengthWithoutQuotationMarks ||
         (NameHasQuotationMarks(name, length) && length <= k_maxNameLength);
}
size_t NameWithoutQuotationMarks(char* buffer, size_t bufferSize,
                                 const char* name, size_t nameLength);

const char* AnsMainAlias();
bool IsTheta(NewExpression e);
bool IsSymbol(NewExpression e, CodePoint c);
const char* GetName(NewExpression e);

}  // namespace SymbolHelper

}  // namespace Poincare

#endif
