#ifndef POINCARE_JUNIOR_LAYOUTTER_H
#define POINCARE_JUNIOR_LAYOUTTER_H

#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Layoutter {
 public:
  static Tree* LayoutExpression(Tree* expression);

 private:
  static void LayoutText(EditionReference& layoutParent, const char* text);
  static void LayoutBuiltin(EditionReference& layoutParent, Tree* expression);
  static void LayoutIntegerHandler(EditionReference& layoutParent,
                                   IntegerHandler handler,
                                   int decimalOffset = 0);
  static void LayoutInfixOperator(EditionReference& layoutParent,
                                  Tree* expression, CodePoint op);
  static void LayoutMatrix(EditionReference& layoutParent, Tree* expression);
  static void LayoutUnit(EditionReference& layoutParent, Tree* expression);
  static void LayoutPowerOrDivision(EditionReference& layoutParent,
                                    Tree* expression);
  static void LayoutExpression(EditionReference& layoutParent, Tree* expression,
                               bool allowParentheses = true);
};
}  // namespace PoincareJ

#endif
