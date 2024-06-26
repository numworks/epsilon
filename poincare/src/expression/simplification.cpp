#include "simplification.h"

#include "advanced_simplification.h"
#include "beautification.h"
#include "dependency.h"
#include "list.h"
#include "systematic_reduction.h"
#include "variables.h"

namespace Poincare::Internal {

bool ShouldApproximateOnSimplify(Dimension dimension) {
  // Only angle units are expected not to be approximated.
  return (dimension.isUnit() && !dimension.isAngleUnit());
}

bool RelaxProjectionContext(void* context) {
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);
  if (projectionContext->m_strategy == Strategy::ApproximateToFloat) {
    // Nothing more can be done.
    return false;
  }
  projectionContext->m_strategy = Strategy::ApproximateToFloat;
  return true;
}

bool Simplification::SimplifyWithAdaptiveStrategy(
    Tree* e, ProjectionContext* projectionContext) {
  assert(projectionContext);
  // Clone the tree, and use an adaptive strategy to handle pool overflow.
  SharedTreeStack->executeAndReplaceTree(
      [](void* context, const void* data) {
        Tree* e = static_cast<const Tree*>(data)->cloneTree();
        if (e->isStore()) {
          // Store is an expression only for convenience
          e = e->child(0);
        }
        // Copy ProjectionContext to avoid altering the original
        ProjectionContext projectionContext =
            *static_cast<ProjectionContext*>(context);
        ToSystem(e, &projectionContext);
        SimplifySystem(e, true);
        // TODO: Should be in SimplifySystem but projectionContext is needed.
        TryApproximationStrategyAgain(e, projectionContext);
        Beautification::DeepBeautify(e, projectionContext);
      },
      projectionContext, e, RelaxProjectionContext);
  /* TODO: Due to projection/beautification cycles and multiple intermediary
   * steps, keeping track of a changed status is unreliable. We could compare
   * CRC instead. */
  return true;
}

bool Simplification::PrepareForProjection(
    Tree* e, ProjectionContext* projectionContext) {
  // Seed random nodes before anything is merged/duplicated.
  int maxRandomSeed = Random::SeedRandomNodes(e, 0);
  bool changed = maxRandomSeed > 0;
  // Replace functions and variable before dimension check
  changed = Variables::ProjectLocalVariablesToId(e) || changed;
  // Replace local variables before user named
  if (Projection::DeepReplaceUserNamed(e, *projectionContext)) {
    // Seed random nodes that may have appeared after replacing.
    maxRandomSeed = Random::SeedRandomNodes(e, maxRandomSeed);
    changed = true;
  }
  if (!Dimension::DeepCheck(e)) {
    e->cloneTreeOverTree(KUndefUnhandledDimension);
    changed = true;
  }
  return changed;
}

bool Simplification::ToSystem(Tree* e, ProjectionContext* projectionContext) {
  /* 1 - Prepare for projection */
  bool changed = PrepareForProjection(e, projectionContext);
  /* 2 - Update strategy depending on dimension */
  projectionContext->m_dimension = Dimension::Get(e);
  if (ShouldApproximateOnSimplify(projectionContext->m_dimension)) {
    projectionContext->m_strategy = Strategy::ApproximateToFloat;
  }
  /* 3 - Project */
  changed = Projection::DeepSystemProject(e, *projectionContext) || changed;
  /* 4 - Handle Units */
  return Units::Unit::ProjectToBestUnits(e, projectionContext->m_dimension,
                                         projectionContext->m_unitDisplay,
                                         projectionContext->m_unitFormat) ||
         changed;
}

bool Simplification::SimplifySystem(Tree* e, bool advanced) {
  bool changed = SystematicReduction::DeepSystematicReduce(e);
  changed = List::BubbleUp(e, SystematicReduction::ShallowSystematicReduce) ||
            changed;
  if (advanced) {
    changed = AdvancedSimplification::AdvancedReduce(e) || changed;
  }
  return Dependency::DeepRemoveUselessDependencies(e) || changed;
}

bool Simplification::TryApproximationStrategyAgain(
    Tree* e, ProjectionContext projectionContext) {
  // Approximate again in case exact numbers appeared during simplification.
  if (projectionContext.m_strategy != Strategy::ApproximateToFloat ||
      !Approximation::ApproximateAndReplaceEveryScalar(e)) {
    return false;
  }
  // NAries could be sorted again, some children may be merged.
  SystematicReduction::DeepSystematicReduce(e);
  return true;
}

}  // namespace Poincare::Internal
