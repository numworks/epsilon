#pragma once

#include <poincare/src/memory/tree.h>

#include "projection.h"

namespace Poincare::Internal {

namespace Simplification {
bool Simplify(Tree* e, const ProjectionContext& projectionContext,
              bool beautify = true);

// Simplification steps
Dimension ProjectAndReduce(Tree* e, ProjectionContext* projectionContext);
bool BeautifyReduced(Tree* e, ProjectionContext* projectionContext,
                     const Dimension& dimension);
bool PrepareForProjection(Tree* e, ProjectionContext* projectionContext);

struct ToSystemOutput {
  bool changed;
  Dimension dimension;
};
ToSystemOutput ToSystem(Tree* e, ProjectionContext* projectionContext);
#if ASSERTIONS
bool IsSystem(const Tree* e);
#endif
bool ReduceSystem(Tree* e, bool advanced,
                  ReductionTarget reductionTarget = ReductionTarget::User);
}  // namespace Simplification

}  // namespace Poincare::Internal
