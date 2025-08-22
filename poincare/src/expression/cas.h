#pragma once

#include <poincare/context.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

namespace CAS {
bool Enabled();

bool NeverDisplayReductionOfInput(const Tree* input, const Context&);

bool ShouldOnlyDisplayApproximation(const Tree* input, const Tree* exactOutput,
                                    const Tree* approximateOutput,
                                    const Context&);
}  // namespace CAS

}  // namespace Poincare::Internal
