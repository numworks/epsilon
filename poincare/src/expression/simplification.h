#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare/src/memory/tree.h>

#include "projection.h"

namespace Poincare::Internal {

class Simplification {
 public:
  static bool Simplify(Tree* e, const ProjectionContext& projectionContext,
                       bool beautify = true);

  // Simplification steps
  static void ProjectAndReduce(Tree* e, ProjectionContext* projectionContext);
  static bool BeautifyReduced(Tree* e, ProjectionContext* projectionContext);
  static bool PrepareForProjection(Tree* e,
                                   ProjectionContext* projectionContext);
  static bool ToSystem(Tree* e, ProjectionContext* projectionContext);
#if ASSERTIONS
  static bool IsSystem(const Tree* e);
#endif
  static bool ReduceSystem(Tree* e, bool advanced,
                           bool expandAlgebraic = false);

 private:
  static Tree* ApplySimplify(const Tree* dataTree,
                             ProjectionContext* projectionContext,
                             bool beautify);

  static bool HandleUnits(Tree* e, ProjectionContext* projectionContext);
  static bool ApplyStrategy(Tree* e, const ProjectionContext& projectionContext,
                            bool reduceIfSuccess);
};

}  // namespace Poincare::Internal

#endif
