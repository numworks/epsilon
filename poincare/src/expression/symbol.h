#ifndef POINCARE_EXPRESSION_SYMBOL_H
#define POINCARE_EXPRESSION_SYMBOL_H

#include "context.h"
#include "k_tree.h"

namespace Poincare::Internal {

class ComplexSign;

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

  constexpr static CodePoint k_cartesianSymbol = 'x';
  constexpr static CodePoint k_parametricSymbol = 't';
  constexpr static CodePoint k_polarSymbol = UCodePointGreekSmallLetterTheta;
  constexpr static CodePoint k_radiusSymbol = 'r';
  constexpr static CodePoint k_ordinateSymbol = 'y';
  constexpr static CodePoint k_sequenceSymbol = 'n';

  constexpr static KTree k_systemSymbol = "\x01"_e;

  static uint8_t Length(const Tree* e) {
    assert(e->isUserNamed());
    return e->nodeValue(0) - 1;
  }
  static char* CopyName(const Tree* e, char* buffer, size_t bufferSize);
  static const char* GetName(const Tree* e);

  static ComplexSign GetComplexSign(const Tree* e);
};

}  // namespace Poincare::Internal

#endif
