#ifndef POINCARE_EXPRESSION_TRIGONOMETRY_H
#define POINCARE_EXPRESSION_TRIGONOMETRY_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Trigonometry final {
 public:
  static bool IsDirect(const Tree *node);
  static bool IsInverse(const Tree *node);
  // Given n, return the exact expression of sin(n*π/12).
  static const Tree *ExactFormula(uint8_t n, bool isSin, bool *isOpposed);
  static bool SimplifyTrig(Tree *u);
  EDITION_REF_WRAP(SimplifyTrig);
  static bool SimplifyTrigDiff(Tree *u);
  EDITION_REF_WRAP(SimplifyTrigDiff);
  static bool ContractTrigonometric(Tree *node);
  EDITION_REF_WRAP(ContractTrigonometric);
  static bool ExpandTrigonometric(Tree *node);
  EDITION_REF_WRAP(ExpandTrigonometric);

 private:
  static bool SimplifyTrigSecondElement(Tree *u, bool *isOpposed);
  EDITION_REF_WRAP_1(SimplifyTrigSecondElement, bool *);
};

}  // namespace PoincareJ

#endif
