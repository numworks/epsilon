#ifndef POINCARE_CAS_H
#define POINCARE_CAS_H

#include <poincare/expression.h>

#include "context.h"

namespace Poincare {

class CAS {
 public:
  static bool Enabled();

  static bool NeverDisplayReductionOfInput(const UserExpression& input,
                                           const Context&);
  static bool ShouldOnlyDisplayApproximation(
      const UserExpression& input, const UserExpression& exactOutput,
      const UserExpression& approximateOutput, const Context&);
};

}  // namespace Poincare

#endif
