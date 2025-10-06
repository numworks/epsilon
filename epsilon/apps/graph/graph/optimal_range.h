#pragma once

#include <poincare/range.h>
#include <poincare/symbol_context.h>
#include <shared/continuous_function_store.h>

namespace Graph {

Poincare::Range2D<float> OptimalRange(
    bool computeX, bool computeY, Poincare::Range2D<float> originalRange,
    const Shared::ContinuousFunctionStore* store, bool enforceNormalizedRange,
    const Poincare::SymbolContext& symbolContext);

}
