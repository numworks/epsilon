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

static Tree* ApplySimplify(
    const Tree* input, const SimplificationParameters& simplificationParameters,
    bool beautify);

static bool HandleUnits(Tree* e, const UnitParameters& unitParameters,
                        const Dimension& dimension);

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
inline static bool AreConsistent(const Properties& properties, const T& value) {
  static_assert(std::is_arithmetic<T>());

  return std::isnan(value) ||
         (((value > 0 && properties.sign().canBeStrictlyPositive()) ||
           (value < 0 && properties.sign().canBeStrictlyNegative()) ||
           (value == 0 && properties.sign().canBeNull())) &&
          (properties.canBeNonInteger() ||
           OMG::Float::RoughlyEqual<T>(value, std::round(value),
                                       100 * OMG::Float::EpsilonLax<T>())));
}
template <typename T>
inline static bool AreConsistent(const ComplexProperties& properties,
                                 const std::complex<T>& value) {
  return std::isnan(value.real()) || std::isnan(value.imag()) ||
         (AreConsistent(properties.realSign(), value.real()) &&
          AreConsistent(properties.imagSign(), value.imag()));
}
#endif

bool Simplify(Tree* e, const SimplificationParameters& simplificationParameters,
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
    // Clone the tree, simplify and raise an exception for pool overflow.
    Tree* simplified = ApplySimplify(e, simplificationParameters, beautify);
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

Tree* ApplySimplify(const Tree* input,
                    const SimplificationParameters& simplificationParameters,
                    bool beautify) {
  assert(!input->hasDescendantSatisfying(
      [](const Tree* tree) { return tree->isStore(); }));
  Tree* e = input->cloneTree();
  Dimension dim = ProjectAndReduce(e, simplificationParameters);
  if (beautify) {
    BeautifyReduced(e, ToBeautificationParameters(simplificationParameters),
                    dim);
  }
  return e;
}

Dimension ProjectAndReduce(
    Tree* e, const SimplificationParameters& simplificationParameters) {
  assert(!e->isStore());
  ProjectionParameters projectionParameters =
      ToProjectionParameters(simplificationParameters);
  Dimension dim = ToSystem(e, projectionParameters).dimension;
  ReduceSystem(e, simplificationParameters.reductionPolicy);
  return dim;
}

bool BeautifyReduced(Tree* e,
                     const BeautificationParameters& beautificationParameters,
                     const Dimension& dimension) {
  assert(!e->isStore());
  // TODO: Should this be recomputed here ?
  /* Empty list has every type (kinda).
   * Example: Input `{}=1` is simplified to `{}`.
   * simplificationParameters->m_dimension is Boolean but Dimension::Get(e) is
   * Scalar
   */
  assert(e->isUndefined() || dimension == Dimension::Get(e) ||
         (e->isList() &&
          (e->numberOfChildren() == 0 || e->child(0)->isUndefined())));
  bool changed =
      HandleUnits(e, ToUnitParameters(beautificationParameters), dimension);
  changed =
      Beautification::DeepBeautify(e, beautificationParameters, dimension) ||
      changed;
  return changed;
}

bool PrepareForProjection(Tree* e,
                          const ProjectionParameters& projectionParameters) {
  // Seed random nodes before anything is merged/duplicated
  int previousMaxRandomSeed = Random::GetMaxSeed(e);
  int maxRandomSeed = Random::SeedRandomNodes(e, previousMaxRandomSeed);
  bool changed = maxRandomSeed > previousMaxRandomSeed;
  // Replace UserFunctions before projecting local variables
  const SymbolicComputation symbolic =
      projectionParameters.projectionPolicy.symbolic;
  if (symbolic != SymbolicComputation::KeepAllSymbols &&
      symbolic != SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    if (Projection::DeepReplaceUserSymbol(
            e, projectionParameters.user.context,
            SymbolicComputation::ReplaceDefinedFunctions)) {
      // Seed random nodes that may have appeared after replacing
      maxRandomSeed = Random::SeedRandomNodes(e, maxRandomSeed);
      changed = true;
    }
  }
  changed = Variables::ProjectLocalVariablesToId(e) || changed;
  // Replace user named symbols after having projected local variables
  if (symbolic != SymbolicComputation::ReplaceDefinedFunctions) {
    changed = Projection::DeepReplaceUserSymbol(
                  e, projectionParameters.user.context, symbolic) ||
              changed;
  } else {
    assert(!Projection::DeepReplaceUserSymbol(
        e, projectionParameters.user.context, symbolic));
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

ToSystemOutput ToSystem(Tree* e,
                        const ProjectionParameters& projectionParameters) {
  /* 1 - Prepare for projection */
  bool changed = PrepareForProjection(e, projectionParameters);
  /* 2 - Compute expression's dimension */
  Dimension dim = Dimension::Get(e);
  /* 3 - Project */
  changed =
      Projection::DeepSystemProject(e, projectionParameters, dim) || changed;
  return {changed, dim};
}

bool ReduceSystem(Tree* e, const ReductionPolicy& reductionPolicy) {
  bool changed = SystematicReduction::DeepReduce(e);
  COSTLY_ASSERT(!SystematicReduction::DeepReduce(e));
  if (reductionPolicy.advanced) {
    changed = AdvancedReduction::Reduce(e, reductionPolicy.target) || changed;
  }
  if (reductionPolicy.target == ReductionTarget::SystemForAnalysis) {
    changed = AdvancedReduction::DeepExpandAlgebraic(e) || changed;
  }
  changed = ProcessSpecialUndefinedPatterns(e) || changed;
  return Dependency::DeepRemoveUselessDependencies(e) || changed;
}

bool HandleUnits(Tree* e, const UnitParameters& unitParameters,
                 const Dimension& dimension) {
  bool changed = false;
#if POINCARE_UNIT
  if (!e->isUndefined() &&
      Units::Unit::ProjectToBestUnits(e, dimension, unitParameters)) {
    if (e->isUndefined()) {
      return true;
    }
    ReduceSystem(e, ReductionPolicy{.advanced = false});
    changed = true;
  }
  if (dimension.isUnit() && !dimension.isAngleUnit()) {
    // Only angle units are expected not to be approximated.
    changed =
        Approximation::Approximate(
            e, FloatPrecision::Double,
            Approximation::Parameters{
                .policy = {.onlyReplaceNumerical = true, .optimize = true}}) ||
        changed;
    /* Check that the expression after approximating scalars does not need to be
     * reduced again. We used to reduce the expression here. If we find out this
     * assertion breaks (when fuzzing), we can put it back. */
    COSTLY_ASSERT(!SystematicReduction::DeepReduce(e));
  }
#endif
  return changed;
}

#if ASSERTIONS
// A Tree is not a UserExpression if it contains some System nodes
bool IsUser(const Tree* e) {
  return !(e->hasDescendantSatisfying(
      [](const Tree* e) { return e->isTrig() || e->isLn(); }));
}

/* A Tree is a SystemExpression if it has already been projected (re-projecting
 * it returns the same Tree) */
bool IsSystem(const Tree* e) {
  Tree* c = e->cloneTree();
#if POINCARE_COMPLEX
  // Use ComplexFormat::Cartesian to avoid having PowReal interfering
  ProjectionParameters projectionParameters = {
      .user = {.complexFormat = ComplexFormat::Cartesian}};
#else
  /* Pow is used both in UserExpressions to represent all the types of powers
   * and in system for some specific powers (simple integral powers for
   * instance). This is an issue in ComplexFormat::Real since ToSystem replaces
   * all Pow nodes with PowReal nodes, ie Pow nodes are not allowed by IsSystem
   * while they are produced by the reduction.
   * TODO: introduce a PowUser node similar to LnUser to clarify the situation ?
   */
  ProjectionParameters projectionParameters = {};
  for (Tree* d : c->selfAndDescendants()) {
    if (d->isPow()) {
      d->cloneNodeOverNode(KPowReal);
    }
  }
#endif
  bool changed = ToSystem(c, projectionParameters).changed;
  c->removeTree();
  return !changed;
}

// A PreparedFunction contains either floats and / or variables, or is undef
bool IsPreparedFunction(const Internal::Tree* e) {
  return e->isUndef()
#if POINCARE_POINT
         || e->treeIsIdenticalTo(KPoint(KUndef, KUndef))
#endif
         || e->isComplexI() ||
         (e->hasDescendantSatisfying([](const Internal::Tree* e) {
           return e->isFloat() || e->isVar();
         }));
}

#endif

}  // namespace Poincare::Internal::Simplification
