#ifndef POINCARE_EXPRESSION_DIVISION_H
#define POINCARE_EXPRESSION_DIVISION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Division {
 public:
  static bool BeautifyIntoDivision(Tree* e);
  static Tree* PushDenominatorAndComputeDegreeOfNumerator(const Tree* e,
                                                          const char* symbol,
                                                          int* numeratorDegree);

 private:
  /* Get numerator, denominator, outerNumerator, and opposite (if needed). If
   * hasBeautifiedIntegers is given, integers will be beautified, and it will be
   * set to true if one changed. */
  static void GetDivisionComponents(const Tree* e, TreeRef& numerator,
                                    TreeRef& denominator,
                                    TreeRef& outerNumerator,
                                    bool* needOpposite);
};

}  // namespace Poincare::Internal

#endif
