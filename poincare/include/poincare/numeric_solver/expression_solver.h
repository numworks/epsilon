#pragma once

#include <poincare/symbol_context.h>
#include <poincare/tree.h>

#include "policies/bracket.h"
#include "policies/discontinuity.h"
#include "policies/evaluation.h"
#include "policies/numeric_helper.h"
#include "policies/queue.h"
#include "policies/step.h"
#include "solver.h"

namespace Poincare::NumericSolver {

template <typename TBracketer, typename TDiscontinuityTester>
struct ExpressionSolverPolicies {
  using Bracketer = TBracketer;
  using DiscontinuityTester = TDiscontinuityTester;

  using Evaluator = ExpressionEvaluator<double>;
  using NumericHelper = DefaultNumericHelper<double>;
  /* NOTE: The same abscissa can yield up to three solutions when searching for
   * discontinuities. */
  using SolutionQueue = StaticSolutionQueue<double, 3>;
  using Stepper = AdaptiveLogStepper<double>;
};

template <typename TBracketer, typename TDiscontinuityTester>
class ExpressionSolver
    : public PolicySolver<
          double, ExpressionSolverPolicies<TBracketer, TDiscontinuityTester>> {
  using Base =
      PolicySolver<double,
                   ExpressionSolverPolicies<TBracketer, TDiscontinuityTester>>;

 public:
  ExpressionSolver(double xStart, double xEnd, const Internal::Tree* e)
      : Base(xStart, xEnd, ExpressionEvaluator<double>(e)) {}
};

using ExtremumSolver =
    ExpressionSolver<ExtremumBracketer, ExpressionDiscontinuityTester<double>>;

class RootSolver
    : public ExpressionSolver<RootBracketer,
                              ExpressionDiscontinuityTester<double>> {
  using Base =
      ExpressionSolver<RootBracketer, ExpressionDiscontinuityTester<double>>;

 public:
  using Base::ExpressionSolver;

  Solution<double> next() override;

 private:
  struct DetailedRoot {
    double x;
    const Internal::Tree* nullDescendant = nullptr;
  };
  constexpr static DetailedRoot k_nanRoot = {.x = static_cast<double>(NAN),
                                             .nullDescendant = nullptr};

  const Internal::Tree* getTree() const { return m_evaluator.tree(); }

  DetailedRoot nextDetailedRoot();
  DetailedRoot nextRootSearchingInChild(int childIndex) const;
  using ExpressionTestAuxiliary = bool (*)(const Internal::Tree*);
  DetailedRoot nextRootSearchingInChildren(ExpressionTestAuxiliary test) const;
  DetailedRoot nextRootInMultiplication() const;
  DetailedRoot nextRootInAddition() const;
  DetailedRoot nextRootInDependency() const;

  DetailedRoot registerDetailedRoot(DetailedRoot root) {
    registerSolution(SolutionFromDetailedRoot(root));
    return root;
  }

  static Solution<double> SolutionFromDetailedRoot(DetailedRoot root) {
    return std::isfinite(root.x) ? Solution<double>{root.x, 0., Interest::Root}
                                 : k_noSolution<double>;
  }
};

class DiscontinuitySolver
    : public ExpressionSolver<DiscontinuityBracketer,
                              NullDiscontinuityTester<double>> {
  using Base =
      ExpressionSolver<DiscontinuityBracketer, NullDiscontinuityTester<double>>;

 public:
  using Base::ExpressionSolver;

  void findAndPushSolution(Coordinate2D<double> start, Coordinate2D<double> end,
                           Interest interest) override;
};

}  // namespace Poincare::NumericSolver
