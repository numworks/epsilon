#include <poincare/numeric_solver/expression_solver.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/properties.h>
#include <poincare/src/numeric_solver/hone_functions.h>
#include <poincare/src/numeric_solver/solver_tests.h>

using namespace Poincare::Internal;

namespace Poincare::NumericSolver {

// ------- DiscontinuitySolver implementation -------

void DiscontinuitySolver::findAndPushSolution(Coordinate2D<double> start,
                                              Coordinate2D<double> end,
                                              Interest interest) {
  assert(interest == Interest::ReachedDiscontinuity);
  assert(m_solutionQueue.size() == 0);

  double precision = NumericHelper::NullTolerance(start.x());

  Coordinate2D<double> left = start;
  Coordinate2D<double> middle;
  Coordinate2D<double> right = end;
  // Use ExpressionDiscontinuityTester explicitly: our policy is
  // NullDiscontinuityTester (to suppress the outer-loop refinement), but here
  // we need the real discontinuity check.
  if (!findMinimalIntervalContainingDiscontinuity<
          ExpressionDiscontinuityTester<double>>(left, middle, right,
                                                 precision)) {
    return;
  }

  middle.setX(NumericHelper::MagicRound(middle.x()));
  assert(std::isfinite(middle.x()));
  if (!m_stepper.template isAdmissibleSolution<NumericHelper>(middle.x())) {
    return;
  }

  // Update left and right so they differ from middle
  left.setX(std::min(left.x(), middle.x() - precision / 2.));
  right.setX(std::max(right.x(), middle.x() + precision / 2.));
  assert(middle.x() != left.x() && middle.x() != right.x());

  // Evaluate at all three points (left/right use "limit" via MagicRound)
  middle.setY(m_evaluator(middle.x()));
  left.setY(NumericHelper::MagicRound(m_evaluator(left.x())));
  right.setY(NumericHelper::MagicRound(m_evaluator(right.x())));

  bool leftIsDiscontinuous =
      ExpressionDiscontinuityTester<double>::IsDiscontinuousBetween(
          left, middle, m_evaluator.data());
  bool rightIsDiscontinuous =
      ExpressionDiscontinuityTester<double>::IsDiscontinuousBetween(
          middle, right, m_evaluator.data());
  // Note: If this assertion fails, escape and return no discontinuity.
  assert(leftIsDiscontinuous || rightIsDiscontinuous);

  // There should be at least one finite value
  assert((leftIsDiscontinuous && std::isfinite(left.y())) ||
         std::isfinite(middle.y()) ||
         (rightIsDiscontinuous && std::isfinite(right.y())));

  // Unreached discontinuities
  if (leftIsDiscontinuous && std::isfinite(left.y())) {
    left.setX(middle.x());
    m_solutionQueue.push(
        CoordToSolution(left, Interest::UnreachedDiscontinuity));
  }
  if (rightIsDiscontinuous && std::isfinite(right.y())) {
    right.setX(middle.x());
    m_solutionQueue.push(
        CoordToSolution(right, Interest::UnreachedDiscontinuity));
  }

  // Reached discontinuities
  if (std::isfinite(middle.y())) {
    bool leftIsReachedDiscontinuity =
        leftIsDiscontinuous && (!std::isfinite(left.y()) ||
                                std::abs(middle.y() - left.y()) >=
                                    NumericHelper::NullTolerance(middle.x()));
    bool rightIsReachedDiscontinuity =
        rightIsDiscontinuous && (!std::isfinite(right.y()) ||
                                 std::abs(middle.y() - right.y()) >=
                                     NumericHelper::NullTolerance(middle.x()));
    if (leftIsReachedDiscontinuity || rightIsReachedDiscontinuity) {
      m_solutionQueue.push(
          CoordToSolution(middle, Interest::ReachedDiscontinuity));
    }
  }
}

// ------- RootSolver implementation -------

Solution<double> RootSolver::next() {
  return SolutionFromDetailedRoot(nextDetailedRoot());
}

RootSolver::DetailedRoot RootSolver::nextDetailedRoot() {
  const Tree* e = getTree();
  if (e == nullptr || e->hasDescendantSatisfying(
                          [](const Tree* t) { return t->isRandomized(); })) {
    return k_nanRoot;
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
      RootSolver childSolver(m_stepper.start(), m_stepper.end(), e->child(0));
      return registerDetailedRoot(childSolver.nextDetailedRoot());
    }

    case Type::Dep:
      return registerDetailedRoot(nextRootInDependency());

    default:
      if (!GetComplexProperties(e).canBeNull()) {
        return k_nanRoot;
      }
      // Use classic numeric scan
      Solution<double> res = Base::next();
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

  RootSolver childSolver(m_stepper.start(), m_stepper.end(), child);

  DetailedRoot root;
  while (std::isfinite(
      (root = childSolver.nextDetailedRoot()).x)) {  // assignment in condition
    /* Check the result in case another term is undefined,
     * e.g. (x+1)*ln(x) for x = -1.
     *
     * Sometimes xRoot is not precise enough and e can approximate to undef
     * even if it's not. e.g. f(x)=sqrt(cos(x)), when searching roots of
     * cos(x), sometimes cos(xRoot) = -0.0..01, so sqrt(cos(xRoot)) = undef.
     * Avoid this by cloning e and replacing cos(xRoot) by 0. */
    Tree* ebis = e->cloneTree();
    ebis->child(childIndex)->cloneTreeOverTree(0_de);
    double value = Approximation::To<double>(
        ebis, root.x,
        Approximation::Parameters{
            .policy = {.optimize = true},
            .expressionParameters = Approximation::Prepared{}});
    ebis->removeTree();
    if (std::fabs(value) < NumericHelper::NullTolerance(root.x)) {
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
    if (test(e->child(i))) {
      DetailedRoot rootChild = nextRootSearchingInChild(i);
      if (std::isfinite(rootChild.x) &&
          (!std::isfinite(root.x) ||
           std::fabs(m_stepper.start() - rootChild.x) <
               std::fabs(m_stepper.start() - root.x))) {
        root = rootChild;
      }
    }
  }
  return root;
}

RootSolver::DetailedRoot RootSolver::nextRootInMultiplication() const {
  assert(getTree()->isMult());
  return nextRootSearchingInChildren([](const Tree*) { return true; });
}

RootSolver::DetailedRoot RootSolver::nextRootInAddition() const {
  /* Special case for expressions of the form "f(x)^a+g(x)", with:
   * - f(x) and g(x) sharing a root x0
   * - f(x) being defined only on one side of x0
   * - 0 < a
   * Since the expression does not change sign around x0, the usual numerical
   * schemes won't work. We instead look for the zeroes of f, and check whether
   * they are zeroes of the whole expression. */
  ExpressionTestAuxiliary test = [](const Tree* e) {
    return e->hasDescendantSatisfying([](const Tree* e) {
      double exponent = static_cast<double>(NAN);
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
      return static_cast<double>(0) < exponent;
    });
  };
  DetailedRoot childrenRoot = nextRootSearchingInChildren(test);

  // Compare result with classic numeric scan
  RootSolver solver = *this;
  DetailedRoot root = {.x = (&solver)->Base::next().x,
                       .nullDescendant = getTree()};
  if (!std::isfinite(root.x) || std::fabs(childrenRoot.x - m_stepper.start()) <
                                    std::fabs(root.x - m_stepper.start())) {
    root = childrenRoot;
  }
  return root;
}

RootSolver::DetailedRoot RootSolver::nextRootInDependency() const {
  const Tree* e = getTree();
  assert(e->isDep());
  const Tree* main = Dependency::Main(e);
  const Tree* depsList = Dependency::Dependencies(e);
  RootSolver depSolver(m_stepper.start(), m_stepper.end(), main);

  DetailedRoot root;
  // Find root in main and then check that the dependencies are not undefined
  while (std::isfinite((root = depSolver.nextDetailedRoot()).x)) {
    /* Replace nullDescendant by 0 in deps list. This ensures that
     * approximation errors do not invalidate the dependencies check.
     *
     * For example, when solving `sqrt(0.5x^2-1)=0`, one found root is
     * slightly below `sqrt(2)`, making the dependency undefined. By replacing
     * `0.5x^2-1` by `0`, this dependency check doesn't fail. */
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

}  // namespace Poincare::NumericSolver
