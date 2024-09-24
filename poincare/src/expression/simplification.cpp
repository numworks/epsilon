#include "simplification.h"

#include <omg/float.h>
#include <poincare/cas.h>

#include "advanced_reduction.h"
#include "beautification.h"
#include "dependency.h"
#include "k_tree.h"
#include "list.h"
#include "random.h"
#include "sign.h"
#include "systematic_reduction.h"
#include "units/unit.h"
#include "variables.h"

namespace Poincare::Internal {

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
  SharedTreeStack->executeAndReplaceTree(ApplySimplify, e, projectionContext,
                                         RelaxProjectionContext, true);
  /* TODO: Due to projection/beautification cycles and multiple intermediary
   * steps, keeping track of a changed status is unreliable. We could compare
   * CRC instead. */
  return true;
}

void Simplification::ProjectAndReduceWithAdaptiveStrategy(
    Tree* e, ProjectionContext* projectionContext, bool advanced) {
  SharedTreeStack->executeAndReplaceTree(ApplyProjectAndReduce, e,
                                         projectionContext,
                                         RelaxProjectionContext, advanced);
}

void Simplification::ApplySimplify(const Tree* dataTree,
                                   ProjectionContext* projectionContext,
                                   bool advanced) {
  /* Store is an expression only for convenience. Only first child is to
   * be simplified. */
  bool isStore = dataTree->isStore();
  Tree* e;
  if (isStore) {
    const Tree* firstChild = dataTree->child(0);
    if (firstChild->nextTree()->isUserFunction()) {
      if (CAS::Enabled()) {
        projectionContext->m_symbolic =
            SymbolicComputation::DoNotReplaceAnySymbol;
      } else {
        dataTree->cloneTree();
        return;
      }
    }
    /* Store is an expression only for convenience. Only first child is to
     * be simplified. */
    e = firstChild->cloneTree();
  } else {
    e = dataTree->cloneTree();
  }

  ProjectAndReduce(e, projectionContext, advanced);
  BeautifyReduced(e, projectionContext);

  if (isStore) {
    // Restore the store structure
    dataTree->child(1)->cloneTree();
    e->cloneNodeAtNode(dataTree);
  }
}

void Simplification::ApplyProjectAndReduce(const Tree* dataTree,
                                           ProjectionContext* projectionContext,
                                           bool advanced) {
  assert(!dataTree->isStore());
  Tree* treeClone = dataTree->cloneTree();
  ProjectAndReduce(treeClone, projectionContext, advanced);
}

void Simplification::ProjectAndReduce(Tree* e,
                                      ProjectionContext* projectionContext,
                                      bool advanced) {
  assert(!e->isStore());
  ToSystem(e, projectionContext);
  ReduceSystem(e, advanced);
  // Non-approximated numbers or node may have appeared during reduction.
  ApplyStrategy(e, projectionContext->m_strategy, true);
}

void Simplification::BeautifyReduced(Tree* e,
                                     ProjectionContext* projectionContext) {
  assert(!e->isStore());
  HandleUnits(e, projectionContext);
  Beautification::DeepBeautify(e, *projectionContext);
}

bool Simplification::PrepareForProjection(
    Tree* e, ProjectionContext* projectionContext) {
  // Seed random nodes before anything is merged/duplicated.
  int maxRandomSeed = Random::SeedRandomNodes(e, 0);
  bool changed = maxRandomSeed > 0;
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

bool Simplification::ToSystem(Tree* e, ProjectionContext* projectionContext) {
  /* 1 - Prepare for projection */
  bool changed = PrepareForProjection(e, projectionContext);
  /* 2 - Update projection context */
  projectionContext->m_dimension = Dimension::Get(e);
  /* 3 - Apply projection strategy */
  changed = ApplyStrategy(e, projectionContext->m_strategy, false) || changed;
  /* 4 - Project */
  changed = Projection::DeepSystemProject(e, *projectionContext) || changed;
  return changed;
}

#if ASSERTIONS
bool Simplification::IsSystem(const Tree* e) {
  Tree* c = e->cloneTree();
  // Use ComplexFormat::Cartesian to avoid having PowReal interfering
  ProjectionContext ctx = {.m_complexFormat = ComplexFormat::Cartesian};
  bool changed = ToSystem(c, &ctx);
  c->removeTree();
  return !changed;
}
#endif

bool Simplification::ReduceSystem(Tree* e, bool advanced) {
  bool changed = SystematicReduction::DeepReduce(e);
  assert(!SystematicReduction::DeepReduce(e));
  changed = List::BubbleUp(e, SystematicReduction::ShallowReduce) || changed;
  if (advanced) {
    changed = AdvancedReduction::Reduce(e) || changed;
  }
  bool result = Dependency::DeepRemoveUselessDependencies(e) || changed;

#if ASSERTIONS
  /* The following block verifies that the ComplexSign logic is correct, by
    checking that the ComplexSign of the tree is consistent with the tree
    approximation. */
  if (Dimension::IsNonListScalar(e)) {
    /* FIXME: The tree approximation is computed in two steps: first calling
     * RootTreeToTree to have a proper management of Approximation::s_context,
     * then by applying ToComplex on the resulting tree. It would be better to
     * have a single function that approximates a tree to a scalar value while
     * also providing a safe approximation context. */
    Tree* approximatedTree = Approximation::RootTreeToTree<double>(e);
    std::complex<double> value =
        Approximation::ToComplex<double>(approximatedTree);
    assert(AreConsistent(GetComplexSign(e), value));
    approximatedTree->removeTree();
  }
#endif

  return result;
}

bool Simplification::HandleUnits(Tree* e,
                                 ProjectionContext* projectionContext) {
  bool changed = false;
  if (!e->isUndefined() &&
      Units::Unit::ProjectToBestUnits(e, projectionContext->m_dimension,
                                      projectionContext->m_unitDisplay,
                                      projectionContext->m_angleUnit)) {
    ReduceSystem(e, false);
    changed = true;
  }
  if (projectionContext->m_dimension.isUnit() &&
      !projectionContext->m_dimension.isAngleUnit()) {
    // Only angle units are expected not to be approximated.
    projectionContext->m_strategy = Strategy::ApproximateToFloat;
    ApplyStrategy(e, projectionContext->m_strategy, true);
  }
  return changed;
}

bool Simplification::ApplyStrategy(Tree* e, Strategy strategy,
                                   bool reduceIfSuccess) {
  if (strategy != Strategy::ApproximateToFloat ||
      !Approximation::ApproximateAndReplaceEveryScalar(e)) {
    return false;
  }
  if (reduceIfSuccess) {
    // NAries could be sorted again, some children may be merged.
    SystematicReduction::DeepReduce(e);
  }
  return true;
}

}  // namespace Poincare::Internal
