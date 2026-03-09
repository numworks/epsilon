#pragma once

#include <poincare/src/memory/tree.h>

#include "projection.h"

namespace Poincare::Internal {

namespace Simplification {
bool Simplify(Tree* e, const SimplificationParameters& simplificationParameters,
              bool beautify = true);

// Simplification steps
Dimension ProjectAndReduce(
    Tree* e, const SimplificationParameters& simplificationParameters);
bool BeautifyReduced(Tree* e,
                     const BeautificationParameters& beautificationParameters,
                     const Dimension& dimension);
bool PrepareForProjection(Tree* e,
                          const ProjectionParameters& projectionParameters);

struct ToSystemOutput {
  bool changed;
  Dimension dimension;
};
ToSystemOutput ToSystem(Tree* e,
                        const ProjectionParameters& projectionParameters);
bool ReduceSystem(Tree* e, const ReductionPolicy& reductionPolicy);

#if ASSERTIONS
bool IsUser(const Tree* e);
bool IsSystem(const Tree* e);
bool IsPreparedFunction(const Internal::Tree* e);
#endif

}  // namespace Simplification

}  // namespace Poincare::Internal
