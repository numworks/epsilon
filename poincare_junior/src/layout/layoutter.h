#ifndef POINCARE_JUNIOR_LAYOUTTER_H
#define POINCARE_JUNIOR_LAYOUTTER_H

#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Layoutter {
 public:
  static Tree* LayoutExpression(Tree* expression, bool linearMode = false);

 private:
  void LayoutText(EditionReference& layoutParent, const char* text);
  void LayoutBuiltin(EditionReference& layoutParent, Tree* expression);
  void LayoutIntegerHandler(EditionReference& layoutParent,
                            IntegerHandler handler, int decimalOffset = 0);
  void LayoutInfixOperator(EditionReference& layoutParent, Tree* expression,
                           CodePoint op);
  void LayoutMatrix(EditionReference& layoutParent, Tree* expression);
  void LayoutUnit(EditionReference& layoutParent, Tree* expression);
  void LayoutPowerOrDivision(EditionReference& layoutParent, Tree* expression);
  void LayoutExpression(EditionReference& layoutParent, Tree* expression,
                        bool allowParentheses = true);

  bool m_linearMode;
};
}  // namespace PoincareJ

#endif
