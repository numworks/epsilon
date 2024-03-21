#ifndef POINCARE_EXPRESSION_SYMBOL_H
#define POINCARE_EXPRESSION_SYMBOL_H

#include "k_tree.h"

namespace PoincareJ {

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

  static uint8_t Length(const Tree* node) { return node->nodeValue(0); }
  static void GetName(const Tree* node, char* buffer, size_t bufferSize);
  static const char* NonNullTerminatedName(const Tree* node);
};

}  // namespace PoincareJ

#endif
