#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare/src/memory/tree.h>

#include "projection.h"

namespace Poincare::Internal {

namespace Simplification {
bool Simplify(Tree* e, const ProjectionContext& projectionContext,
              bool beautify = true);

// Simplification steps
void ProjectAndReduce(Tree* e, ProjectionContext* projectionContext);
bool BeautifyReduced(Tree* e, ProjectionContext* projectionContext);
bool PrepareForProjection(Tree* e, ProjectionContext* projectionContext);
bool ToSystem(Tree* e, ProjectionContext* projectionContext);
#if ASSERTIONS
bool IsSystem(const Tree* e);
#endif
bool ReduceSystem(Tree* e, bool advanced,
                  ReductionTarget reductionTarget = ReductionTarget::User);
}  // namespace Simplification

}  // namespace Poincare::Internal

#endif
