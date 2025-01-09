#ifndef POINCARE_EXPRESSION_CAS_H
#define POINCARE_EXPRESSION_CAS_H

#include <poincare/old/context.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

namespace CAS {
bool Enabled();

bool NeverDisplayReductionOfInput(const Tree* input, Context*);

bool ShouldOnlyDisplayApproximation(const Tree* input, const Tree* exactOutput,
                                    const Tree* approximateOutput, Context*);
}  // namespace CAS

}  // namespace Poincare::Internal

#endif
