#pragma once

#include <poincare/context.h>
#include <poincare/helpers/symbol.h>
#include <poincare/sign.h>

#include "context.h"
#include "k_tree.h"

namespace Poincare::Internal {

namespace Symbol {

using SymbolHelper::k_maxNameLength;
using SymbolHelper::k_maxNameLengthWithoutQuotationMarks;
using SymbolHelper::k_maxNameSize;

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

bool InvolvesCircularity(const Tree* e, const Poincare::Context& context);
};  // namespace Symbol

}  // namespace Poincare::Internal
