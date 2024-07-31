#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare/src/memory/tree.h>

#include "projection.h"
#include "stdbool.h"

namespace Poincare::Internal {

class Simplification {
 public:
  static bool SimplifyWithAdaptiveStrategy(
      Tree* e, ProjectionContext* projectionContext);
  TREE_REF_WRAP_1(SimplifyWithAdaptiveStrategy, ProjectionContext*);

  static void ProjectAndReduceWithAdaptiveStrategy(
      Tree* e, ProjectionContext* projectionContext, bool advanced);

  static void ApplySimplify(const Tree* dataTree,
                            ProjectionContext* projectionContext);
  static void ApplyProjectAndReduce(const Tree* dataTree,
                                    ProjectionContext* projectionContext,
                                    bool advanced);

  // Simplification steps
  static void ProjectAndReduce(Tree* e, ProjectionContext* projectionContext,
                               bool advanced);
  static void BeautifyReduced(Tree* e, ProjectionContext* projectionContext);
  static bool PrepareForProjection(Tree* e,
                                   ProjectionContext* projectionContext);
  static bool ToSystem(Tree* e, ProjectionContext* projectionContext);
  TREE_REF_WRAP_1(ToSystem, ProjectionContext*);
#if ASSERTIONS
  static bool IsSystem(const Tree* e);
#endif
  static bool ReduceSystem(Tree* e, bool advanced);

 private:
  static bool HandleUnits(Tree* e, ProjectionContext* projectionContext);
  static bool ApplyStrategy(Tree* e, Strategy strategy, bool reduceIfSuccess);
};

}  // namespace Poincare::Internal

#endif
