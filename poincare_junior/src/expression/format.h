#ifndef POINCARE_JUNIOR_FORMAT_H
#define POINCARE_JUNIOR_FORMAT_H

#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Format {
 public:
  static Tree* FormatExpression(Tree* expression);

 private:
  static void ConvertTextToLayout(EditionReference layoutParent,
                                  const char* text);
  static void ConvertBuiltinToLayout(EditionReference layoutParent,
                                     Tree* expression);
  static void ConvertIntegerHandlerToLayout(EditionReference layoutParent,
                                            IntegerHandler handler,
                                            int decimalOffset = 0);
  static void ConvertInfixOperatorToLayout(EditionReference layoutParent,
                                           Tree* expression, CodePoint op);
  static void ConvertMatrixToLayout(EditionReference layoutParent,
                                    Tree* expression);
  static void ConvertUnitToLayout(EditionReference layoutParent,
                                  Tree* expression);
  static void ConvertPowerOrDivisionToLayout(EditionReference layoutParent,
                                             Tree* expression);
  static void ConvertExpressionToLayout(EditionReference layoutParent,
                                        Tree* expression,
                                        bool allowParentheses = true);
};
}  // namespace PoincareJ

#endif
