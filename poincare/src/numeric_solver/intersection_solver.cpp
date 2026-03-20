#include <poincare/numeric_solver/intersection_solver.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/numeric_solver/hone_functions.h>
#include <poincare/src/numeric_solver/solver_tests.h>

using namespace Poincare::Internal;

namespace Poincare::NumericSolver {

// ------- IntersectionSolver implementation -------

static Tree* FunctionDifferenceForIntersection(const Tree* f, const Tree* g,
                                               bool areAlongSameAxis) {
  /* TODO: simplify if we decide that functions should be simplified. Either
   * pass ProjectionContext(m_complexFormat, m_angleUnit,
   * ReductionTarget::SystemForAnalysis, UnitFormat::Metric, m_context) or
   * ensure expression is projected. */
  if (areAlongSameAxis) {
    return PatternMatching::Create(KAdd(KA, KMult(-1_e, KB)),
                                   {.KA = f, .KB = g});
  }
  // Build the g(f(a))-a expression
  Tree* difference = SharedTreeStack->pushAdd(2);
  // Creating g(f(a))
  Tree* gOfFOfa = g->cloneTree();
  Variables::Replace(gOfFOfa, 0, f, false, false);
  // Cloning -a
  PatternMatching::Create(KMult(-1_e, KVarX));
  /* If f is a dependency expression, add the dependencies of f to the result.
   * This is because if g is a constant function, f will not appear in
   * a->g(f(a)) and the dependencies of f could get lost. */
  if (f->isDep()) {
    Dependency::AddDependencies(difference, Dependency::Dependencies(f));
  }
  return difference;
}

IntersectionSolver::IntersectionSolver(double xStart, double xEnd,
                                       const PreparedFunction& f,
                                       const PreparedFunction& g,
                                       bool areAlongSameAxis,
                                       const SymbolContext& context)
    : RootSolver(xStart, xEnd, nullptr, context),
      m_f(f),
      m_g(g),
      m_alongSameAxis(areAlongSameAxis) {
  Tree* diff =
      FunctionDifferenceForIntersection(f.tree(), g.tree(), areAlongSameAxis);
  m_diff = PreparedFunction::Builder(diff);
  setEvaluator(m_diff.tree());
}

Solution<double> IntersectionSolver::next() {
  Solution<double> root = RootSolver::next();
  if (root.interest != Interest::Root) {
    assert(root.interest == Interest::None);
    return Solution<double>{};
  }

  double x = root.x;
  double y = Approximation::To<double>(
      m_f, x,
      Approximation::Parameters{
          .policy = {.optimize = true},
          .expressionParameters = Approximation::Prepared{}});
  // The secondImage is a "y" if areAlongSameAxis=true, or an "x" otherwise
  double secondImage = Approximation::To<double>(
      m_g, m_alongSameAxis ? x : y,
      Approximation::Parameters{
          .policy = {.optimize = true},
          .expressionParameters = Approximation::Prepared{}});
  if (!std::isfinite(y) || !std::isfinite(secondImage)) {
    /* Sometimes, with expressions m_f and m_g that take extreme values like x^x
     * or undef expressions in specific points like x^2/x, the root of the
     * difference yields an infinite or a nan value when m_f or m_g is
     * evaluated. It means the intersection was incorrectly computed, and the
     * search continues. */
    return next();
  }
  double comparedTo = m_alongSameAxis ? y : x;
  /* These thresholds are quite large because we cannot expect exact
   * approximation. They could be lowered again if we find too many false
   * intersections. */
  constexpr double k_relativeThreshold = 1e-4;
  constexpr double k_absoluteThreshold = 1e-4;
  if (!OMG::Float::RelativelyEqual(secondImage, comparedTo,
                                   k_relativeThreshold) &&
      !OMG::Float::RoughlyEqual(secondImage - comparedTo, 0.,
                                k_absoluteThreshold)) {
    // Intersection is not accurate enough, continue searching
    return next();
  }

  /* In the areAlongSameAxis case, take the middle of the two approximated "y"
   * values */
  return m_alongSameAxis
             ? Solution<double>{.x = x,
                                .y = (y + secondImage) / 2.,
                                .interest = Interest::Intersection}
             : Solution<double>{
                   .x = x, .y = y, .interest = Interest::Intersection};
}

}  // namespace Poincare::NumericSolver
