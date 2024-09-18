#ifndef POINCARE_EXPRESSION_DIVISION_H
#define POINCARE_EXPRESSION_DIVISION_H

#include <poincare/src/memory/tree_ref.h>

#include "projection.h"

namespace Poincare::Internal {

class Division {
 public:
  static bool BeautifyIntoDivision(Tree* e);
  static Tree* PushDenominatorAndComputeDegreeOfNumerator(const Tree* e,
                                                          const char* symbol,
                                                          int* numeratorDegree);

 private:
  // Get numerator, denominator, outerNumerator, and opposite (if needed)
  static void GetDivisionComponents(const Tree* e, TreeRef& numerator,
                                    TreeRef& denominator,
                                    TreeRef& outerNumerator,
                                    bool* needOpposite);
};

}  // namespace Poincare::Internal

#endif
