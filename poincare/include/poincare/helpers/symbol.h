#pragma once

#include <stddef.h>

namespace Poincare::Internal {
class Tree;
}

namespace Poincare::SymbolHelper {

/* A symbol  can have a max length of 7 chars, or 9 if it's
 * surrounded by quotation marks.
 * This makes it so a 9 chars name (with quotation marks), can be
 * turned into a 7 char name in the result cells of the solver (by
 * removing the quotation marks). */
constexpr size_t k_maxNameLengthWithoutQuotationMarks = 7;
constexpr size_t k_maxNameLength = k_maxNameLengthWithoutQuotationMarks + 2;
constexpr size_t k_maxNameSize = k_maxNameLength + 1;

size_t NameWithoutQuotationMarks(char* buffer, size_t bufferSize,
                                 const char* name, size_t nameLength);

const char* AnsMainAlias();
// TODO remove function exposing `const Tree*`
const char* GetName(const Internal::Tree* e);

}  // namespace Poincare::SymbolHelper
