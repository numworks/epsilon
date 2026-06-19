#include <poincare/numeric_solver/solver.h>
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
  if (!findMinimalIntervalContainingDiscontinuity(
          left, middle, right, precision,
          ExpressionDiscontinuityTester<double>{})) {
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
      ExpressionDiscontinuityTester<double>{}.isDiscontinuousBetween(
          left, middle, m_evaluator.data());
  bool rightIsDiscontinuous =
      ExpressionDiscontinuityTester<double>{}.isDiscontinuousBetween(
          middle, right, m_evaluator.data());
  // Note: If this assertion fails, escape and return no discontinuity.
  assert(leftIsDiscontinuous || rightIsDiscontinuous);

  /* If the function evaluates to NaN on both sides (e.g. f(x)=x^0*i, a
   * complex-valued function), no finite value exists to characterize the
   * discontinuity. Skip rather than assert. */
  if (!((leftIsDiscontinuous && std::isfinite(left.y())) ||
        std::isfinite(middle.y()) ||
        (rightIsDiscontinuous && std::isfinite(right.y())))) {
    return;
  }
  // There is at least one finite value

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

}  // namespace Poincare::NumericSolver
