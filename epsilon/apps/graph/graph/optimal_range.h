#ifndef GRAPH_GRAPH_OPTIMAL_RANGE_H
#define GRAPH_GRAPH_OPTIMAL_RANGE_H

#include <poincare/context.h>
#include <poincare/range.h>
#include <shared/continuous_function_store.h>

namespace Graph {

Poincare::Range2D<float> OptimalRange(
    bool computeX, bool computeY, Poincare::Range2D<float> originalRange,
    const Shared::ContinuousFunctionStore* store, bool defaultRangeIsNormalized,
    const Poincare::Context& context);

}

#endif
