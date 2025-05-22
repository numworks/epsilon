#include "simplification.h"

#include <omg/float.h>

#include "advanced_reduction.h"
#include "beautification.h"
#include "cas.h"
#include "dependency.h"
#include "k_tree.h"
#include "list.h"
#include "random.h"
#include "sign.h"
#include "systematic_reduction.h"
#include "units/unit.h"
#include "variables.h"

namespace Poincare::Internal::Simplification {

static Tree* ApplySimplify(const Tree* dataTree,
                           ProjectionContext* projectionContext, bool beautify);

static bool HandleUnits(Tree* e, ProjectionContext* projectionContext);
static bool ApplyStrategy(Tree* e, const ProjectionContext& projectionContext,
                          bool reduceIfSuccess);

/* Some patterns are not reduced by the systematic and the advanced reductions,
 * but if the expression contains one of these patterns at the end of reduction,
 * the expression is replaced by an undefined. For example, ln(0) is left
 * unreduced by the systematic and the advanced reductions to allow exp(x*ln(0))
 * to be correctly simplified. */
static constexpr const Tree* specialUndefinedPatterns[] = {KLn(0_e)};
static bool ProcessSpecialUndefinedPatterns(Tree* e) {
  if (e->hasDescendantSatisfying([](const Tree* t) {
        for (const Tree* pattern : specialUndefinedPatterns) {
          if (t->treeIsIdenticalTo(pattern)) {
            return true;
          }
        }
        return false;
      })) {
    e->moveTreeOverTree(KOutOfDefinition->cloneTree());
    return true;
  }
  return false;
}

#if ASSERTIONS
template <typename T>
inline static bool AreConsistent(const Sign& sign, const T& value) {
  static_assert(std::is_arithmetic<T>());

  return std::isnan(value) ||
         (((value > 0 && sign.canBeStrictlyPositive()) ||
           (value < 0 && sign.canBeStrictlyNegative()) ||
           (value == 0 && sign.canBeNull())) &&
          (sign.canBeNonInteger() ||
           OMG::Float::RoughlyEqual<T>(value, std::round(value),
                                       100 * OMG::Float::EpsilonLax<T>())));
}
template <typename T>
inline static bool AreConsistent(const ComplexSign& sign,
                                 const std::complex<T>& value) {
  return std::isnan(value.real()) || std::isnan(value.imag()) ||
         (AreConsistent(sign.realSign(), value.real()) &&
          AreConsistent(sign.imagSign(), value.imag()));
}
#endif

bool Simplify(Tree* e, const ProjectionContext& projectionContext,
              bool beautify) {
  ExceptionTry {
#if ASSERTIONS
    size_t treesNumber = SharedTreeStack->numberOfTrees();
#endif
    ProjectionContext contextCopy = projectionContext;
    // Clone the tree, simplify and raise an exception for pool overflow.
    Tree* simplified = ApplySimplify(e, &contextCopy, beautify);
    // Prevent from leaking: simplification creates exactly one tree.
    assert(SharedTreeStack->numberOfTrees() == treesNumber + 1);
    e->moveTreeOverTree(simplified);
  }
  ExceptionCatch(type) {
    switch (type) {
      case ExceptionType::TreeStackOverflow:
      case ExceptionType::IntegerOverflow:
        return false;
      default:
        TreeStackCheckpoint::Raise(type);
    }
  }
  return true;
}

Tree* ApplySimplify(const Tree* dataTree, ProjectionContext* projectionContext,
                    bool beautify) {
  /* Store is an expression only for convenience. Only first child is to
   * be simplified. */
  bool isStore = dataTree->isStore();
  Tree* e;
  if (isStore) {
    // Store might not be properly handled in reduced expressions.
    assert(beautify);
    const Tree* firstChild = dataTree->child(0);
    if (firstChild->nextTree()->isUserFunction()) {
      if (CAS::Enabled()) {
        projectionContext->m_symbolic = SymbolicComputation::KeepAllSymbols;
      } else {
        return dataTree->cloneTree();
      }
    }
    /* Store is an expression only for convenience. Only first child is to
     * be simplified. */
    e = firstChild->cloneTree();
  } else {
    e = dataTree->cloneTree();
  }

  assert(!e->hasDescendantSatisfying(
      [](const Tree* tree) { return tree->isStore(); }));

  ProjectAndReduce(e, projectionContext);
  if (beautify) {
    BeautifyReduced(e, projectionContext);
  }

  if (isStore) {
    // Restore the store structure
    dataTree->child(1)->cloneTree();
    e->cloneNodeAtNode(dataTree);
  }
  return e;
}

void ProjectAndReduce(Tree* e, ProjectionContext* projectionContext) {
  assert(!e->isStore());
  ToSystem(e, projectionContext);
  ReduceSystem(e, projectionContext->m_advanceReduce,
               projectionContext->m_reductionTarget);
  // Non-approximated numbers or node may have appeared during reduction.
  ApplyStrategy(e, *projectionContext, true);
}

bool BeautifyReduced(Tree* e, ProjectionContext* projectionContext) {
  assert(!e->isStore());
  // TODO: Should this be recomputed here ?
  /* Empty list has every type (kinda).
   * Example: Input `{}=1` is simplified to `{}`.
   * projectionContext->m_dimension is Boolean but Dimension::Get(e) is Scalar
   */
  assert(e->isUndefined() ||
         projectionContext->m_dimension == Dimension::Get(e) ||
         (e->isList() &&
          (e->numberOfChildren() == 0 || e->child(0)->isUndefined())));
  bool changed = HandleUnits(e, projectionContext);
  changed = Beautification::DeepBeautify(e, *projectionContext) || changed;
  return changed;
}

bool PrepareForProjection(Tree* e, ProjectionContext* projectionContext) {
  // Seed random nodes before anything is merged/duplicated.
  int previousMaxRandomSeed = Random::GetMaxSeed(e);
  int maxRandomSeed = Random::SeedRandomNodes(e, previousMaxRandomSeed);
  bool changed = maxRandomSeed > previousMaxRandomSeed;
  // Replace functions and variable before dimension check
  changed = Variables::ProjectLocalVariablesToId(e) || changed;
  // Replace local variables before user named
  if (Projection::DeepReplaceUserNamed(e, projectionContext->m_context,
                                       projectionContext->m_symbolic)) {
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

bool ToSystem(Tree* e, ProjectionContext* projectionContext) {
  /* 1 - Prepare for projection */
  bool changed = PrepareForProjection(e, projectionContext);
  /* 2 - Update projection context */
  projectionContext->m_dimension = Dimension::Get(e);
  /* 3 - Apply projection strategy */
  changed = ApplyStrategy(e, *projectionContext, false) || changed;
  /* 4 - Project */
  changed = Projection::DeepSystemProject(e, *projectionContext) || changed;
  return changed;
}

#if ASSERTIONS
bool IsSystem(const Tree* e) {
  Tree* c = e->cloneTree();
  // Use ComplexFormat::Cartesian to avoid having PowReal interfering
  ProjectionContext ctx = {.m_complexFormat = ComplexFormat::Cartesian};
  bool changed = ToSystem(c, &ctx);
  c->removeTree();
  return !changed;
}
#endif

bool ReduceSystem(Tree* e, bool advanced, ReductionTarget reductionTarget) {
  bool changed = SystematicReduction::DeepReduce(e);
  assert(!SystematicReduction::DeepReduce(e));
#if POINCARE_LIST
  changed = List::BubbleUp(e, SystematicReduction::ShallowReduce) || changed;
#endif
  if (advanced) {
    changed = AdvancedReduction::Reduce(e) || changed;
  }
  // TODO: Also handle ReductionTarget::SystemForApproximation
  if (reductionTarget == ReductionTarget::SystemForAnalysis) {
    changed = AdvancedReduction::DeepExpandAlgebraic(e) || changed;
  }
  changed = ProcessSpecialUndefinedPatterns(e) || changed;
  return Dependency::DeepRemoveUselessDependencies(e) || changed;
}

bool HandleUnits(Tree* e, ProjectionContext* projectionContext) {
  bool changed = false;
#if POINCARE_UNIT
  if (!e->isUndefined() &&
      Units::Unit::ProjectToBestUnits(
          e, projectionContext->m_dimension, projectionContext->m_unitDisplay,
          projectionContext->m_angleUnit, projectionContext->m_unitFormat)) {
    if (e->isUndefined()) {
      return true;
    }
    // Re-apply strategy to make sure introduced integers are floats.
    if (projectionContext->m_strategy == Strategy::ApproximateToFloat) {
      ApplyStrategy(e, *projectionContext, true);
    }
    ReduceSystem(e, false, projectionContext->m_reductionTarget);
    changed = true;
  }
  if (projectionContext->m_dimension.isUnit() &&
      !projectionContext->m_dimension.isAngleUnit()) {
    // Only angle units are expected not to be approximated.
    projectionContext->m_strategy = Strategy::ApproximateToFloat;
    ApplyStrategy(e, *projectionContext, true);
  }
#endif
  return changed;
}

bool ApplyStrategy(Tree* e, const ProjectionContext& projectionContext,
                   bool reduceIfSuccess) {
  if (projectionContext.m_strategy != Strategy::ApproximateToFloat ||
      !Approximation::ApproximateAndReplaceEveryScalar<double>(
          e, Approximation::Context(projectionContext.m_angleUnit,
                                    projectionContext.m_complexFormat,
                                    projectionContext.m_context))) {
    return false;
  }
  if (reduceIfSuccess) {
    // NAries could be sorted again, some children may be merged.
    SystematicReduction::DeepReduce(e);
  }
  return true;
}

}  // namespace Poincare::Internal::Simplification
