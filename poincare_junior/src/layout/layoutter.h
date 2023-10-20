#ifndef POINCARE_JUNIOR_LAYOUTTER_H
#define POINCARE_JUNIOR_LAYOUTTER_H

#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Layoutter {
 public:
  static Tree* LayoutExpression(Tree* expression, bool linearMode = false);

 private:
  Layoutter(bool linearMode) : m_linearMode(linearMode) {}
  void layoutText(EditionReference& layoutParent, const char* text);
  void layoutBuiltin(EditionReference& layoutParent, Tree* expression);
  void layoutIntegerHandler(EditionReference& layoutParent,
                            IntegerHandler handler, int decimalOffset = 0);
  void layoutInfixOperator(EditionReference& layoutParent, Tree* expression,
                           CodePoint op);
  void layoutMatrix(EditionReference& layoutParent, Tree* expression);
  void layoutUnit(EditionReference& layoutParent, Tree* expression);
  void layoutPowerOrDivision(EditionReference& layoutParent, Tree* expression);
  void layoutExpression(EditionReference& layoutParent, Tree* expression,
                        int parentPriority);
  bool m_linearMode;
};
}  // namespace PoincareJ

#endif
