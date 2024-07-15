#include "simplification.h"

#include "advanced_reduction.h"
#include "beautification.h"
#include "dependency.h"
#include "list.h"
#include "systematic_reduction.h"
#include "variables.h"

namespace Poincare::Internal {

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
        const Tree* dataTree = static_cast<const Tree*>(data);
        bool isStore = dataTree->isStore();
        /* Store is an expression only for convenience. Only first child is to
         * be simplified. */
        Tree* e =
            isStore ? dataTree->child(0)->cloneTree() : dataTree->cloneTree();
        // Copy ProjectionContext to avoid altering the original
        ProjectionContext projectionContext =
            *static_cast<ProjectionContext*>(context);
        ToSystem(e, &projectionContext);
        ReduceSystem(e, true);
        HandleUnits(e, &projectionContext);
        // TODO: Should be in ReduceSystem but projectionContext is needed.
        TryApproximationStrategyAgain(e, projectionContext);
        Beautification::DeepBeautify(e, projectionContext);
        if (isStore) {
          // Restore the store structure
          dataTree->child(1)->cloneTree();
          e->cloneNodeAtNode(dataTree);
        }
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
  /* 2 - Update projection context */
  projectionContext->m_dimension = Dimension::Get(e);
  /* 3 - Project */
  return Projection::DeepSystemProject(e, *projectionContext) || changed;
}

bool Simplification::ReduceSystem(Tree* e, bool advanced) {
  bool changed = SystematicReduction::DeepReduce(e);
  changed = List::BubbleUp(e, SystematicReduction::ShallowReduce) || changed;
  if (advanced) {
    changed = AdvancedReduction::Reduce(e) || changed;
  }
  return Dependency::DeepRemoveUselessDependencies(e) || changed;
}

bool Simplification::HandleUnits(Tree* e,
                                 ProjectionContext* projectionContext) {
  bool changed = false;
  if (!e->isUndefined() &&
      Units::Unit::ProjectToBestUnits(e, projectionContext->m_dimension,
                                      projectionContext->m_unitDisplay)) {
    ReduceSystem(e, false);
    changed = true;
  }
  if (projectionContext->m_dimension.isUnit() &&
      !projectionContext->m_dimension.isAngleUnit()) {
    // Only angle units are expected not to be approximated.
    projectionContext->m_strategy = Strategy::ApproximateToFloat;
  }
  return changed;
}

bool Simplification::TryApproximationStrategyAgain(
    Tree* e, ProjectionContext projectionContext) {
  // Approximate again in case exact numbers appeared during simplification.
  if (projectionContext.m_strategy != Strategy::ApproximateToFloat ||
      !Approximation::ApproximateAndReplaceEveryScalar(e)) {
    return false;
  }
  // NAries could be sorted again, some children may be merged.
  SystematicReduction::DeepReduce(e);
  return true;
}

}  // namespace Poincare::Internal
