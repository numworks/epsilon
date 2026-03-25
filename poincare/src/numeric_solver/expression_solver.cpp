#include <poincare/numeric_solver/expression_solver.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/properties.h>
#include <poincare/src/numeric_solver/hone_functions.h>
#include <poincare/src/numeric_solver/solver_tests.h>

using namespace Poincare::Internal;

namespace Poincare::NumericSolver {

ExpressionSolver::ExpressionSolver(double xStart, double xEnd, const Tree* e,
                                   const SymbolContext& context,
                                   Solver<double>::BracketTest test,
                                   Solver<double>::HoneResult hone)
    : Solver<double>(xStart, xEnd, nullptr, e, test, hone,
                     SolverTests<double>::IsDiscontinuousInBracket),
      m_context(context) {
  setEvaluator(e);
}

void ExpressionSolver::setEvaluator(const Internal::Tree* e) {
  assert(e);
  m_auxiliary = e;
  if (e->hasDescendantSatisfying(
          [](const Tree* e) { return e->isRandomized(); })) {
    m_evaluator = [](double, const void*) { return static_cast<double>(NAN); };
    return;
  }

  m_evaluator = [](double x, const void* aux) {
    const Tree* e = reinterpret_cast<const Tree*>(aux);
    return Approximation::To<double>(
        e, x,
        Approximation::Parameters{
            .policy = {.optimize = true},
            .expressionParameters = Approximation::Prepared{}});
  };
}

// ------- Minimum/MaximumSolver implementation -------

ExtremumSolver::ExtremumSolver(double xStart, double xEnd, const Tree* e,
                               const SymbolContext& context)
    : ExpressionSolver(xStart, xEnd, e, context,
                       SolverTests<double>::ExtremumInBracket,
                       HoneFunctions::SafeBrentExtremum) {}

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
    ebis->child(childIndex)->cloneTreeOverTree(0_de);
    /* This comparison relies on the fact that it is false for a NAN
     * approximation. */
    double value = Approximation::To<double>(
        ebis, root.x,
        Approximation::Parameters{
            .policy = {.optimize = true},
            .expressionParameters = Approximation::Prepared{}});
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
            e->child(1),
            Approximation::Parameters{.policy = {.optimize = true}});
      } else if (e->type() == Type::Root) {
        exponent =
            1. / Approximation::To<double>(
                     e->child(1),
                     Approximation::Parameters{.policy = {.optimize = true}});
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
          childDep, root.x,
          Approximation::Parameters{.policy = {.optimize = true}});
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

// ------- DiscontinuitySolver implementation -------

DiscontinuitySolver::DiscontinuitySolver(double xStart, double xEnd,
                                         const Tree* e,
                                         const SymbolContext& context)
    : ExpressionSolver(xStart, xEnd, e, context,
                       SolverTests<double>::DiscontinuityInBracket,
                       // Hone is handled differently
                       HoneFunctions::DummyHone) {}

void DiscontinuitySolver::honeAndRoundSolution(Coordinate2D<double> start,
                                               Coordinate2D<double> end,
                                               Interest interest) {
  assert(interest == Interest::ReachedDiscontinuity);

  assert(m_solutionQueue.size() == 0);
  double precision = NullTolerance(start.x());

  // Find the smallest interval containing the discontinuity
  Coordinate2D<double> left = start;
  Coordinate2D<double> middle;
  Coordinate2D<double> right = end;
  if (!findMinimalIntervalContainingDiscontinuity(&left, &middle, &right,
                                                  precision)) {
    return;
  }

  // Round middle to find the discontinuity abscissa
  middle.setX(MagicRound(middle.x()));
  assert(std::isfinite(middle.x()));
  if (!validSolution(middle.x())) {
    return;
  }

  // Update left and right to make sure there are different from middle
  left.setX(middle.x() - precision / 2.);
  right.setX(middle.x() + precision / 2.);
  assert(middle.x() != left.x() && middle.x() != right.x());

  // Update ordinates
  middle.setY(evaluateAt(middle.x()));
  // We round because we want the "limit"
  left.setY(MagicRound(evaluateAt(left.x())));
  right.setY(MagicRound(evaluateAt(right.x())));
  bool leftIsDiscontinuous = isDiscontinuousBetween(left, middle);
  bool rightIsDiscontinuous = isDiscontinuousBetween(middle, right);
  if (!leftIsDiscontinuous && !rightIsDiscontinuous) {
    // No discontinuity
    return;
  }
  if (!leftIsDiscontinuous) {
    // No left discontinuity
    left = Coordinate2D<double>();
  }
  if (!rightIsDiscontinuous) {
    // No right discontinuity
    right = Coordinate2D<double>();
  }
  if (std::abs(right.y() - left.y()) < NullTolerance(middle.x())) {
    // Left and right are the same
    assert(leftIsDiscontinuous && rightIsDiscontinuous);
    // Keep only 1 point of interest
    left.setY(MagicRound((left.y() + right.y()) / 2.));
    right = Coordinate2D<double>();
  }

  // There should be at least one finite value
  assert(std::isfinite(left.y()) || std::isfinite(middle.y()) ||
         std::isfinite(right.y()));

  // Push solutions
  if (std::isfinite(left.y())) {
    left.setX(middle.x());
    m_solutionQueue.push(
        CoordToSolution(left, Interest::UnreachedDiscontinuity));
  }
  if (std::isfinite(middle.y())) {
    m_solutionQueue.push(
        CoordToSolution(middle, Interest::ReachedDiscontinuity));
  }
  if (std::isfinite(right.y())) {
    right.setX(middle.x());
    m_solutionQueue.push(
        CoordToSolution(right, Interest::UnreachedDiscontinuity));
  }
}

}  // namespace Poincare::NumericSolver
