#pragma once

#include <poincare/src/memory/tree.h>
#include <poincare/symbol_context.h>

namespace Poincare::Internal {

namespace CAS {
bool Enabled();

bool NeverDisplayReductionOfInput(const Tree* input, const SymbolContext&);

bool ShouldOnlyDisplayApproximation(const Tree* input, const Tree* exactOutput,
                                    const Tree* approximateOutput,
                                    const SymbolContext&);
}  // namespace CAS

}  // namespace Poincare::Internal
