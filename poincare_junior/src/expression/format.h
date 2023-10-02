#ifndef POINCARE_JUNIOR_FORMAT_H
#define POINCARE_JUNIOR_FORMAT_H

#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Format {
 public:
  static Tree* FormatExpression(Tree* expression);

 private:
  static void FormatText(EditionReference layoutParent, const char* text);
  static void FormatBuiltin(EditionReference layoutParent, Tree* expression);
  static void FormatIntegerHandler(EditionReference layoutParent,
                                   IntegerHandler handler,
                                   int decimalOffset = 0);
  static void FormatInfixOperator(EditionReference layoutParent,
                                  Tree* expression, CodePoint op);
  static void FormatMatrix(EditionReference layoutParent, Tree* expression);
  static void FormatUnit(EditionReference layoutParent, Tree* expression);
  static void FormatPowerOrDivision(EditionReference layoutParent,
                                    Tree* expression);
  static void FormatExpression(EditionReference layoutParent, Tree* expression,
                               bool allowParentheses = true);
};
}  // namespace PoincareJ

#endif
