#pragma once

#include <assert.h>
#include <poincare/numeric_solver/solution.h>

#include <algorithm>

#include "step.h"

namespace Poincare::NumericSolver {

template <typename TFloat>
class Solver {
 public:
  virtual ~Solver() = default;
  virtual Solution<TFloat> next() = 0;
  virtual void stretch(bool onlyEnd = false) = 0;
  virtual void setSearchStep(TFloat step) = 0;
  virtual void setGrowthSpeed(GrowthSpeed g) = 0;
};

template <typename TFloat, typename TPolicies>
class PolicySolver : public Solver<TFloat> {
 public:
  using XY = Coordinate2D<TFloat>;
  using SolutionT = Solution<TFloat>;

  using Bracketer = typename TPolicies::Bracketer;
  using DiscontinuityTester = typename TPolicies::DiscontinuityTester;
  using Evaluator = typename TPolicies::Evaluator;
  using NumericHelper = typename TPolicies::NumericHelper;
  using SolutionQueue = typename TPolicies::SolutionQueue;
  using Stepper = typename TPolicies::Stepper;

  explicit PolicySolver(TFloat xStart, TFloat xEnd, Evaluator evaluator,
                        Bracketer bracketer = {},
                        DiscontinuityTester discontinuityTester = {})
      : m_bracketer(std::move(bracketer)),
        m_discontinuityTester(discontinuityTester),
        m_evaluator(std::move(evaluator)),
        m_solutionQueue{},
        m_stepper(xStart, xEnd) {}

  ~PolicySolver() override = default;

  void stretch(bool onlyEnd = false) override { m_stepper.stretch(onlyEnd); }

  void setSearchStep(TFloat step) override { m_stepper.setSearchStep(step); }

  void setGrowthSpeed(GrowthSpeed g) override { m_stepper.setGrowthSpeed(g); }

  SolutionT next() override {
    if (m_solutionQueue.size() > 0) {
      return registerSolution(m_solutionQueue.popFirst(), true);
    }

    m_stepper.init(m_evaluator);

    while (m_stepper.step(m_evaluator)) {
      XY p1 = m_stepper.point(0), p2 = m_stepper.point(1),
         p3 = m_stepper.point(2);

      assert((p1.x() < p2.x() && p2.x() < p3.x()) ||
             (p3.x() < p2.x() && p2.x() < p1.x()));

      Interest interest = m_bracketer.test(p1, p2, p3, m_evaluator.data());

      if (interest == Interest::None &&
          m_discontinuityTester.isDiscontinuousBetween(p1, p3,
                                                       m_evaluator.data())) {
        interest = refineSearchAroundDiscontinuity(p1, p2, p3);
      }

      if (interest != Interest::None) {
        findAndPushSolution(p1, p3, interest);
        if (m_solutionQueue.size() > 0) {
          return registerSolution(m_solutionQueue.popFirst(), false);
        }
      }
    }

    return k_noSolution<TFloat>;
  }

 protected:
  virtual void findAndPushSolution(XY start, XY end, Interest interest) {
    assert(m_solutionQueue.size() == 0);
    assert(interest != Interest::ReachedDiscontinuity);

    bool disc = m_discontinuityTester.isDiscontinuousBetween(
        start, end, m_evaluator.data());
    XY xy = m_bracketer.hone(start.x(), end.x(), interest,
                             NumericHelper::Precision(start.x(), disc),
                             m_evaluator);

    /* Fake-root filter: discard roots that sit on a discontinuity edge. */
    if (disc && interest == Interest::Root &&
        std::fabs(xy.y()) > NumericHelper::NullTolerance(xy.x())) {
      return;
    }

    if (!std::isfinite(xy.x()) ||
        !m_stepper.template isAdmissibleSolution<NumericHelper>(xy.x())) {
      return;
    }

    TFloat x = xy.x();
    TFloat roundX = NumericHelper::MagicRound(x);
    TFloat fRoundX = m_evaluator(roundX);
    if (std::isnan(fRoundX) && interest != Interest::Discontinuity) {
      return;
    }
    TFloat fx = m_evaluator(x);
    if (std::isfinite(roundX) &&
        m_stepper.template isAdmissibleSolution<NumericHelper>(roundX)) {
      if (m_discontinuityTester.isDiscontinuousBetween(
              XY(x, fx), XY(roundX, fRoundX), m_evaluator.data())) {
        return;
      }
      if (fRoundX == fx ||
          (interest == Interest::Root && std::fabs(fRoundX) < std::fabs(fx)) ||
          (interest == Interest::Minimum && fRoundX < fx) ||
          (interest == Interest::Maximum && fRoundX > fx)) {
        xy.setX(roundX);
        if (xy.y() == fx) {
          xy.setY(fRoundX);
        }
      }
    }
    if (std::isfinite(xy.y()) || interest == Interest::Discontinuity) {
      m_solutionQueue.push(CoordToSolution(xy, interest));
    }
  }

  /* TExplicitDiscTester defaults to the policy's DiscontinuityTester.
   * Pass a different type explicitly when the policy tester is not suitable
   * (e.g. DiscontinuitySolver uses NullDiscontinuityTester as its policy but
   * needs a real tester inside findAndPushSolution). */
  template <typename TExplicitDiscTester = DiscontinuityTester>
  bool findMinimalIntervalContainingDiscontinuity(XY& start, XY& middle,
                                                  XY& end,
                                                  TFloat minimalInterval,
                                                  TExplicitDiscTester disc) {
    if (std::isnan(middle.x())) {
      TFloat mx = (start.x() + end.x()) / static_cast<TFloat>(2);
      middle = XY(mx, m_evaluator(mx));
    }
    while (std::fabs(end.x() - start.x()) >= minimalInterval) {
      bool left =
          disc.isDiscontinuousBetween(start, middle, m_evaluator.data());
      bool right = disc.isDiscontinuousBetween(middle, end, m_evaluator.data());
      if (left == right) {
        return false;
      }
      if (left) {
        if (end.x() == middle.x()) {
          return false;
        }
        end = middle;
      } else {
        if (start.x() == middle.x()) {
          return false;
        }
        start = middle;
      }
      TFloat mx = (start.x() + end.x()) / static_cast<TFloat>(2);
      middle = XY(mx, m_evaluator(mx));
    }
    return true;
  }

  SolutionT registerSolution(SolutionT solution, bool wasQueued = false) {
    if (std::isnan(solution.x)) {
      return k_noSolution<TFloat>;
    }
    assert(
        (wasQueued && solution.x == m_stepper.start()) ||
        (!wasQueued &&
         m_stepper.template isAdmissibleSolution<NumericHelper>(solution.x)));
    solution.y = NumericHelper::CleanY(solution.y, solution.x);
    m_stepper.setStart(solution.x);
    return solution;
  }

  Bracketer m_bracketer;
  DiscontinuityTester m_discontinuityTester;
  Evaluator m_evaluator;
  SolutionQueue m_solutionQueue;
  Stepper m_stepper;

 private:
  Interest refineSearchAroundDiscontinuity(XY& p1, XY p2, XY& p3) {
    Interest interest = Interest::None;
    XY q1 = p1, q3 = p3;
    if (findMinimalIntervalContainingDiscontinuity(
            q1, p2, q3, Stepper::template MinimalStep<NumericHelper>(p2.x()),
            m_discontinuityTester)) {
      TFloat pq1x = (p1.x() + q1.x()) / static_cast<TFloat>(2);
      interest = m_bracketer.test(p1, XY(pq1x, m_evaluator(pq1x)), q1,
                                  m_evaluator.data());
      if (interest != Interest::None) {
        p3 = q1;
      } else {
        TFloat qp3x = (q3.x() + p3.x()) / static_cast<TFloat>(2);
        interest = m_bracketer.test(q3, XY(qp3x, m_evaluator(qp3x)), p3,
                                    m_evaluator.data());
        if (interest != Interest::None) {
          p1 = q3;
        }
      }
    }
    return interest;
  }
};
}  // namespace Poincare::NumericSolver
