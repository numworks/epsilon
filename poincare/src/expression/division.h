#ifndef POINCARE_EXPRESSION_DIVISION_H
#define POINCARE_EXPRESSION_DIVISION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Division {
 public:
  static bool BeautifyIntoDivision(Tree* e);

 private:
  static void GetDivisionComponents(const Tree* e, TreeRef& numerator,
                                    TreeRef& denominator, bool* needOpposite,
                                    bool* needI);
};

}  // namespace Poincare::Internal

#endif
