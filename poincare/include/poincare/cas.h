#ifndef POINCARE_CAS_H
#define POINCARE_CAS_H

#include <poincare/old/context.h>
#include <poincare/src/memory/tree.h>

namespace Poincare {

class CAS {
 public:
  static bool Enabled();

  static bool NeverDisplayReductionOfInput(const Internal::Tree* input,
                                           Context*);
  static bool ShouldOnlyDisplayApproximation(
      const Internal::Tree* input, const Internal::Tree* exactOutput,
      const Internal::Tree* approximateOutput, Context*);
};

}  // namespace Poincare

#endif
