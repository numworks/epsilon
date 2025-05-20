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
  struct DivisionComponentsResult {
    TreeRef numerator;
    TreeRef denominator;
    TreeRef outerNumerator;
    bool needOpposite;
  };
  /* Split terms of e into different components that may be used to build an
   * equivalent representation : (numerator / denominator) * outerNumerator */
  static DivisionComponentsResult GetDivisionComponents(const Tree* e);
};

}  // namespace Poincare::Internal

#endif
