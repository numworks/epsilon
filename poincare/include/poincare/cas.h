#pragma once

#include <poincare/user_expression.h>

#include "symbol_context.h"

namespace Poincare {

class CAS {
 public:
  static bool Enabled();

  static bool NeverDisplayReductionOfInput(const UserExpression& input,
                                           const SymbolContext&);
  static bool ShouldOnlyDisplayApproximation(
      const UserExpression& input, const UserExpression& exactOutput,
      const UserExpression& approximateOutput, const SymbolContext&);
};

}  // namespace Poincare
