#ifndef POINCARE_EXPRESSION_SYMBOL_H
#define POINCARE_EXPRESSION_SYMBOL_H

#include <poincare/old/context.h>
#include <poincare/sign.h>

#include "context.h"
#include "k_tree.h"

namespace Poincare::Internal {

namespace Symbol {
/* A symbol  can have a max length of 7 chars, or 9 if it's
 * surrounded by quotation marks.
 * This makes it so a 9 chars name (with quotation marks), can be
 * turned into a 7 char name in the result cells of the solver (by
 * removing the quotation marks). */
constexpr size_t k_maxNameLengthWithoutQuotationMarks = 7;
constexpr size_t k_maxNameLength = k_maxNameLengthWithoutQuotationMarks + 2;
constexpr size_t k_maxNameSize = k_maxNameLength + 1;

constexpr int k_maxSymbolReplacementsCount = 10;

constexpr bool NameHasQuotationMarks(const char* name, size_t length) {
  return length > 2 && name[0] == '"' && name[length - 1] == '"';
}
constexpr bool NameLengthIsValid(const char* name, size_t length) {
  return length <= k_maxNameLengthWithoutQuotationMarks ||
         (NameHasQuotationMarks(name, length) && length <= k_maxNameLength);
}

inline uint8_t Length(const Tree* e) {
  assert(e->isUserNamed());
  return e->nodeValue(0) - 1;
}
char* CopyName(const Tree* e, char* buffer, size_t bufferSize);
const char* GetName(const Tree* e);

ComplexSign GetComplexSign(const Tree* e);

bool InvolvesCircularity(const Tree* e, Poincare::Context* context);
};  // namespace Symbol

}  // namespace Poincare::Internal

#endif
