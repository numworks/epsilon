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

static bool HandleUnits(Tree* e, ProjectionContext* projectionContext,
                        const Dimension& dimension);
static bool ApplyStrategy(Tree* e, const ProjectionContext& projectionContext,
                          bool insideReduction);
static bool ApplyProjectionStrategy(
    Tree* e, const ProjectionContext& projectionContext) {
  return ApplyStrategy(e, projectionContext, false);
}
static bool ApplyReductionStrategy(Tree* e,
                                   const ProjectionContext& projectionContext) {
  return ApplyStrategy(e, projectionContext, true);
}

/* Some patterns are not reduced by the systematic and the advanced reductions,
 * but if the expression contains one of these patterns at the end of reduction,
 * the expression is replaced by an undefined. For example, ln(0) is left
 * unreduced by the systematic and the advanced reductions to allow exp(x*ln(0))
 * to be correctly simplified. */
constexpr static const Tree* specialUndefinedPatterns[] = {KLn(0_e)};
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
#ifdef POINCARE_MAX_TREE_SIZE_FOR_SIMPLIFICATION
  if (e->maxDepth() > POINCARE_MAX_TREE_SIZE_FOR_SIMPLIFICATION) {
    // Prevent recursive simplification algorithms from overflowing the stack
    return false;
  }
#endif
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

  Dimension dim = ProjectAndReduce(e, projectionContext);
  if (beautify) {
    BeautifyReduced(e, projectionContext, dim);
  }

  if (isStore) {
    // Restore the store structure
    dataTree->child(1)->cloneTree();
    e->cloneNodeAtNode(dataTree);
  }
  return e;
}

Dimension ProjectAndReduce(Tree* e, ProjectionContext* projectionContext) {
  assert(!e->isStore());
  Dimension dim = ToSystem(e, projectionContext).dimension;
  ReduceSystem(e, projectionContext->m_advanceReduce,
               projectionContext->m_reductionTarget);
  // Non-approximated numbers or node may have appeared during reduction.
  ApplyReductionStrategy(e, *projectionContext);
  return dim;
}

bool BeautifyReduced(Tree* e, ProjectionContext* projectionContext,
                     const Dimension& dimension) {
  assert(!e->isStore());
  // TODO: Should this be recomputed here ?
  /* Empty list has every type (kinda).
   * Example: Input `{}=1` is simplified to `{}`.
   * projectionContext->m_dimension is Boolean but Dimension::Get(e) is Scalar
   */
  assert(e->isUndefined() || dimension == Dimension::Get(e) ||
         (e->isList() &&
          (e->numberOfChildren() == 0 || e->child(0)->isUndefined())));
  bool changed = HandleUnits(e, projectionContext, dimension);
  changed =
      Beautification::DeepBeautify(e, *projectionContext, dimension) || changed;
  return changed;
}

bool PrepareForProjection(Tree* e, ProjectionContext* projectionContext) {
  // Seed random nodes before anything is merged/duplicated
  int previousMaxRandomSeed = Random::GetMaxSeed(e);
  int maxRandomSeed = Random::SeedRandomNodes(e, previousMaxRandomSeed);
  bool changed = maxRandomSeed > previousMaxRandomSeed;
  // Replace UserFunctions before projecting local variables
  const SymbolicComputation symbolic = projectionContext->m_symbolic;
  if (symbolic != SymbolicComputation::KeepAllSymbols &&
      symbolic != SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    if (Projection::DeepReplaceUserNamed(
            e, projectionContext->m_context,
            SymbolicComputation::ReplaceDefinedFunctions)) {
      // Seed random nodes that may have appeared after replacing
      maxRandomSeed = Random::SeedRandomNodes(e, maxRandomSeed);
      changed = true;
    }
  }
  changed = Variables::ProjectLocalVariablesToId(e) || changed;
  // Replace user named symbols after having projected local variables
  if (symbolic != SymbolicComputation::ReplaceDefinedFunctions) {
    changed = Projection::DeepReplaceUserNamed(e, projectionContext->m_context,
                                               symbolic) ||
              changed;
  } else {
    assert(!Projection::DeepReplaceUserNamed(e, projectionContext->m_context,
                                             symbolic));
  }
  // No additional random nodes should have appeared
  assert(Random::SeedRandomNodes(e, maxRandomSeed) == maxRandomSeed);
  // Check dimension after having replaced symbols and functions
  if (!Dimension::DeepCheck(e)) {
    e->cloneTreeOverTree(KUndefUnhandledDimension);
    changed = true;
  }
  return changed;
}

ToSystemOutput ToSystem(Tree* e, ProjectionContext* projectionContext) {
  /* 1 - Prepare for projection */
  bool changed = PrepareForProjection(e, projectionContext);
  /* 2 - Compute expression's dimension */
  Dimension dim = Dimension::Get(e);
  /* 3 - Apply projection strategy */
  changed = ApplyProjectionStrategy(e, *projectionContext) || changed;
  /* 4 - Project */
  changed =
      Projection::DeepSystemProject(e, *projectionContext, dim) || changed;
  return {changed, dim};
}

#if ASSERTIONS
bool IsSystem(const Tree* e) {
  Tree* c = e->cloneTree();
  // Use ComplexFormat::Cartesian to avoid having PowReal interfering
  ProjectionContext ctx = {.m_complexFormat = ComplexFormat::Cartesian};
  bool changed = ToSystem(c, &ctx).changed;
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
    changed = AdvancedReduction::Reduce(e, reductionTarget) || changed;
  }
  if (reductionTarget == ReductionTarget::SystemForAnalysis) {
    changed = AdvancedReduction::DeepExpandAlgebraic(e) || changed;
  }
  changed = ProcessSpecialUndefinedPatterns(e) || changed;
  return Dependency::DeepRemoveUselessDependencies(e) || changed;
}

bool HandleUnits(Tree* e, ProjectionContext* projectionContext,
                 const Dimension& dimension) {
  bool changed = false;
#if POINCARE_UNIT
  if (!e->isUndefined() &&
      Units::Unit::ProjectToBestUnits(
          e, dimension, projectionContext->m_unitDisplay,
          projectionContext->m_angleUnit, projectionContext->m_unitFormat)) {
    if (e->isUndefined()) {
      return true;
    }
    // Re-apply strategy to make sure introduced integers are floats.
    if (projectionContext->m_strategy == Strategy::ApproximateToFloat) {
      ApplyReductionStrategy(e, *projectionContext);
    }
    ReduceSystem(e, false, projectionContext->m_reductionTarget);
    changed = true;
  }
  if (dimension.isUnit() && !dimension.isAngleUnit()) {
    // Only angle units are expected not to be approximated.
    projectionContext->m_strategy = Strategy::ApproximateToFloat;
    ApplyReductionStrategy(e, *projectionContext);
  }
#endif
  return changed;
}

bool ApplyStrategy(Tree* e, const ProjectionContext& projectionContext,
                   bool insideReduction) {
  bool changed = false;

  switch (projectionContext.m_strategy) {
    case Strategy::Default:
      break;
    case Strategy::ApproximateToFloat:
      changed = Approximation::ApproximateAndReplaceEveryScalar<double>(
          e, Approximation::Context(projectionContext.m_angleUnit,
                                    projectionContext.m_complexFormat,
                                    projectionContext.m_context));
      if (changed && insideReduction) {
        // NAries could be sorted again, some children may be merged.
        SystematicReduction::DeepReduce(e);
      }
      break;
#if POINCARE_NO_ADVANCED_REDUCTION
    case Strategy::PseudoAdvanceReduce:
      changed = insideReduction && AdvancedReduction::PseudoReduce(e);
      break;
#endif
    default:
      OMG::unreachable();
  }

  return changed;
}

}  // namespace Poincare::Internal::Simplification
