#ifndef POINCARE_CAS_H
#define POINCARE_CAS_H

#include <poincare/expression.h>

namespace Poincare {

class CAS {
 public:
  static bool Enabled();

  static bool NeverDisplayReductionOfInput(UserExpression input, Context*);
  static bool ShouldOnlyDisplayApproximation(UserExpression input,
                                             UserExpression exactOutput,
                                             UserExpression approximateOutput,
                                             Context*);
};

}  // namespace Poincare

#endif
