#include <poincare/numeric_solver_jr/expression_solver.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/properties.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/numeric_solver/solver_tests.h>
#include <poincare/src/numeric_solver_jr/hone_functions.h>

using namespace Poincare::Internal;

namespace Poincare::NumericSolver {

ExpressionSolver::ExpressionSolver(double xStart, double xEnd, const Tree* e,
                                   const SymbolContext& context,
                                   Solver<double>::BracketTest test,
                                   Solver<double>::HoneResult hone)
    : Solver<double>(xStart, xEnd, nullptr, e, test, hone,
                     SolverTests<double>::IsDiscontinuousInBracket),
      m_context(context) {
  // We could accept e=nullptr and recompute the evaluator later if needed
  assert(e);

  if (e->hasDescendantSatisfying(
          [](const Tree* e) { return e->isRandomized(); })) {
    this->m_evaluator = [](double, const void*) {
      return static_cast<double>(NAN);
    };
    return;
  }

  /* TODO_PCJ: Either ensure expression is projected, or pass approximation
   * context(ComplexFormat, AngleUnit) and replace defined symbols. */
  this->m_evaluator = [](double x, const void* aux) {
    const Tree* e = reinterpret_cast<const Tree*>(aux);
    return Approximation::To<double>(
        e, x, Approximation::Parameters{.optimize = true});
  };
}

ExpressionSolver ExpressionSolver::BuildMinimumSolver(
    double xStart, double xEnd, const Tree* e, const SymbolContext& context) {
  return ExpressionSolver(xStart, xEnd, e, context,
                          SolverTests<double>::MinimumInBracket,
                          HoneFunctions::SafeBrentMinimum);
}

ExpressionSolver ExpressionSolver::BuildMaximumSolver(
    double xStart, double xEnd, const Tree* e, const SymbolContext& context) {
  return ExpressionSolver(xStart, xEnd, e, context,
                          SolverTests<double>::MaximumInBracket,
                          HoneFunctions::SafeBrentMaximum);
}

ExpressionSolver ExpressionSolver::BuildDiscontinuitySolver(
    double xStart, double xEnd, Tree* e, const SymbolContext& context) {
  return ExpressionSolver(xStart, xEnd, e, context,
                          SolverTests<double>::DiscontinuityInBracket,
                          // Hone is handled differently
                          HoneFunctions::DummyHone);
}

// ------- RootSolver implementation -------

RootSolver::RootSolver(double xStart, double xEnd, const Tree* e,
                       const SymbolContext& context)
    : ExpressionSolver(xStart, xEnd, e, context,
                       SolverTests<double>::EvenOrOddRootInBracket,
                       HoneFunctions::CompositeBrentForRoot) {}

Solution<double> RootSolver::next() {
  return SolutionFromDetailedRoot(nextDetailedRoot());
}

RootSolver::DetailedRoot RootSolver::nextDetailedRoot() {
  DetailedRoot nanRoot = k_nanRoot;
  const Tree* e = getTree();
  if (e->hasDescendantSatisfying(
          [](const Tree* e) { return e->isRandomized(); })) {
    return nanRoot;
  }

  switch (e->type()) {
    case Type::Mult:
      /* x*y = 0 => x = 0 or y = 0 */
      return registerDetailedRoot(nextRootInMultiplication());

    case Type::Add:
    case Type::Sub:
      return registerDetailedRoot(nextRootInAddition());
    case Type::Pow:
    case Type::PowReal:
    case Type::Root:
    case Type::Div:
      /* f(x,y) = 0 => x = 0 */
      return registerDetailedRoot(nextRootSearchingInChild(0));

    case Type::Abs:
    case Type::ATan:
    case Type::SinH:
    case Type::Opposite:
    case Type::Sqrt: {
      /* f(x) = 0 <=> x = 0 */
      RootSolver childSolver =
          RootSolver(m_xStart, m_xEnd, e->child(0), m_context);
      return registerDetailedRoot(childSolver.nextDetailedRoot());
    }

    case Type::Dep:
      return registerDetailedRoot(nextRootInDependency());

    default:
      if (!GetComplexProperties(e).canBeNull()) {
        return nanRoot;
      }

      // Use classic next function
      Solution<double> res = ExpressionSolver::next();
      if (res.interest != Interest::None) {
        res.interest = Interest::Root;
      }
      return {.x = res.x, .nullDescendant = e};
  }
}

RootSolver::DetailedRoot RootSolver::nextRootSearchingInChild(
    int childIndex) const {
  const Tree* e = getTree();
  const Tree* child = e->child(childIndex);

  RootSolver childSolver = RootSolver(m_xStart, m_xEnd, child, m_context);

  DetailedRoot root;
  while (std::isfinite(
      (root = childSolver.nextDetailedRoot()).x)) {  // assignment in condition
    /* Check the result in case another term is undefined,
     * e.g. (x+1)*ln(x) for x =- 1.
     *
     * Sometimes, xRoot is not precise enough and e can approximate to undef
     * even if it's not. e.g. f(x)=sqrt(cos(x)), when searching roots of cos(x),
     * sometimes we find cos(xRoot) = -0.0..01, so sqrt(cos(xRoot)) = undef.
     * To avoid this problem, clone e and replace cos(xRoot) by 0. */
    Tree* ebis = e->cloneTree();
    ebis->child(childIndex)->cloneTreeOverTree(0_e);
    /* This comparison relies on the fact that it is false for a NAN
     * approximation. */
    double value = Approximation::To<double>(
        ebis, root.x,
        Approximation::Parameters{.optimize = true});  // m_unknown
    ebis->removeTree();
    if (std::fabs(value) < NullTolerance(root.x)) {
      return {.x = root.x, .nullDescendant = root.nullDescendant};
    }
  }
  return k_nanRoot;
}

RootSolver::DetailedRoot RootSolver::nextRootSearchingInChildren(
    ExpressionTestAuxiliary test) const {
  const Tree* e = getTree();
  DetailedRoot root = k_nanRoot;
  int n = e->numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (test(e->child(i), m_context)) {
      DetailedRoot rootChild = nextRootSearchingInChild(i);
      if (std::isfinite(rootChild.x) &&
          (!std::isfinite(root.x) ||
           std::fabs(m_xStart - rootChild.x) < std::fabs(m_xStart - root.x))) {
        root = rootChild;
      }
    }
  }
  return root;
}

RootSolver::DetailedRoot RootSolver::nextRootInMultiplication() const {
  assert(getTree()->isMult());
  return nextRootSearchingInChildren(
      [](const Tree*, const SymbolContext&) { return true; });
}

RootSolver::DetailedRoot RootSolver::nextRootInAddition() const {
  /* Special case for expressions of the form "f(x)^a+g(x)", with:
   * - f(x) and g(x) sharing a root x0
   * - f(x) being defined only on one side of x0
   * - 0 < a
   * Since the expression does not change sign around x0, the usual numerical
   * schemes won't work. We instead look for the zeroes of f, and check whether
   * they are zeroes of the whole expression. */
  ExpressionTestAuxiliary test = [](const Tree* e,
                                    const SymbolContext& symbolContext) {
    /* TODO_PCJ: Either ensure expression is projected, or pass
     * approximation context(ComplexFormat, AngleUnit) and replace defined
     * symbols. */
    return e->hasDescendantSatisfying([](const Tree* e) {
      double exponent = k_NAN;
      if (e->type() == Type::Sqrt) {
        exponent = 0.5;
      } else if (e->type() == Type::Pow || e->type() == Type::PowReal) {
        exponent = Approximation::To<double>(
            e->child(1), Approximation::Parameters{.optimize = true});
      } else if (e->type() == Type::Root) {
        exponent =
            1. / Approximation::To<double>(
                     e->child(1), Approximation::Parameters{.optimize = true});
      }
      if (std::isnan(exponent)) {
        return false;
      }
      return k_zero < exponent;
    });
  };
  DetailedRoot childrenRoot = nextRootSearchingInChildren(test);

  // Compare result with classic next function
  RootSolver solver = *this;
  DetailedRoot root = {.x = (&solver)->ExpressionSolver::next().x,
                       .nullDescendant = getTree()};
  if (!std::isfinite(root.x) ||
      std::fabs(childrenRoot.x - m_xStart) < std::fabs(root.x - m_xStart)) {
    root = childrenRoot;
  }
  return root;
}

RootSolver::DetailedRoot RootSolver::nextRootInDependency() const {
  const Tree* e = getTree();
  assert(e->isDep());
  const Tree* main = Dependency::Main(e);
  const Tree* depsList = Dependency::Dependencies(e);
  RootSolver depSolver = RootSolver(m_xStart, m_xEnd, main, m_context);

  DetailedRoot root;
  // Find root in main and then check that the dependencies are not undefined
  while (std::isfinite((root = depSolver.nextDetailedRoot()).x)) {
    /* Replace nullDescendant by 0 in deps list. This ensures that approximation
     * errors do not invalidate the dependencies check.
     *
     * For example, when solving `sqrt(0.5x^2-1)=0`, the expression has a
     * dependency on `RealPos(0.5x^2-1)`. One of the found root found is
     * slightly below `sqrt(2)`, which makes the dependency undefined. By
     * replacing `0.5x^2-1` by `0`, we ensure that this dependency check doesn't
     * fail.
     */
    Tree* depsListClone = depsList->cloneTree();
    depsListClone->deepReplaceWith(root.nullDescendant, 0_e);
    bool definedDeps = true;
    for (const Tree* childDep : depsListClone->children()) {
      double value = Approximation::To<double>(
          childDep, root.x, Approximation::Parameters{.optimize = true});
      if (std::isnan(value)) {
        definedDeps = false;
        break;
      }
    }
    depsListClone->removeTree();
    if (definedDeps) {
      break;
    }
  }

  return {.x = root.x, .nullDescendant = root.nullDescendant};
}

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
                                       const Tree* f, const Tree* g,
                                       bool areAlongSameAxis,
                                       const SymbolContext& context)
    : RootSolver(xStart, xEnd,
                 FunctionDifferenceForIntersection(f, g, areAlongSameAxis),
                 context),
      m_alongSameAxis(areAlongSameAxis) {}

Solution<double> IntersectionSolver::next() {
  Solution<double> root = RootSolver::next();
  if (root.interest != Interest::Root) {
    assert(root.interest == Interest::None);
    return Solution<double>{};
  }

  double x = root.x;
  double y = Approximation::To<double>(
      m_f, x, Approximation::Parameters{.optimize = true});
  // The secondImage is a "y" if areAlongSameAxis=true, or an "x" otherwise
  double secondImage =
      Approximation::To<double>(m_g, m_alongSameAxis ? x : y,
                                Approximation::Parameters{.optimize = true});
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
             ? Solution(x, (y + secondImage) / 2., Interest::Intersection)
             : Solution(x, y, Interest::Intersection);
}
}  // namespace Poincare::NumericSolver
