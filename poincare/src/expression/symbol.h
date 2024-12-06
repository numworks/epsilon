#ifndef POINCARE_EXPRESSION_SYMBOL_H
#define POINCARE_EXPRESSION_SYMBOL_H

#include <poincare/old/context.h>
#include <poincare/sign.h>

#include "context.h"
#include "k_tree.h"

namespace Poincare::Internal {

class Symbol final {
 public:
  /* A symbol abstract can have a max length of 7 chars, or 9 if it's
   * surrounded by quotation marks.
   * This makes it so a 9 chars name (with quotation marks), can be
   * turned into a 7 char name in the result cells of the solver (by
   * removing the quotation marks). */
  constexpr static size_t k_maxNameLengthWithoutQuotationMarks = 7;
  constexpr static size_t k_maxNameLength =
      k_maxNameLengthWithoutQuotationMarks + 2;
  constexpr static size_t k_maxNameSize = k_maxNameLength + 1;

  constexpr static int k_maxSymbolReplacementsCount = 10;

  static uint8_t Length(const Tree* e) {
    assert(e->isUserNamed());
    return e->nodeValue(0) - 1;
  }
  static char* CopyName(const Tree* e, char* buffer, size_t bufferSize);
  static const char* GetName(const Tree* e);

  static ComplexSign GetComplexSign(const Tree* e);

  static bool InvolvesCircularity(const Tree* e, Poincare::Context* context);
};

}  // namespace Poincare::Internal

#endif
