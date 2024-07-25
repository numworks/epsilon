#ifndef POINCARE_EXPRESSION_DIVISION_H
#define POINCARE_EXPRESSION_DIVISION_H

#include <poincare/src/memory/tree_ref.h>

#include "projection.h"

namespace Poincare::Internal {

class Division {
 public:
  static bool BeautifyIntoDivision(Tree* e);
  static void GetNumeratorAndDenominator(const Tree* e, TreeRef& numerator,
                                         TreeRef& denominator);
  static bool IsFractionOfPolynomials(const Tree* e, const char* symbol,
                                      ProjectionContext projectionContext);

 private:
  static void GetDivisionComponents(const Tree* e, TreeRef& numerator,
                                    TreeRef& denominator, bool* needOpposite,
                                    bool* needI);
};

}  // namespace Poincare::Internal

#endif
