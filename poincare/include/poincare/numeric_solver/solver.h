#pragma once
#include "policies/bracket.h"
#include "policies/discontinuity.h"
#include "policies/evaluation.h"
#include "policies/numeric_helper.h"
#include "policies/queue.h"
#include "policies/solver.h"
#include "policies/step.h"

namespace Poincare::NumericSolver {

/* WARNING: Each set of policies used corresponds to one instanciation of the
 * PolicySolver, taking space in the binary. */
namespace Policies {

/* NOTE: Used in Zoom */
struct SinglePrecisionStandardSolver {
  using Bracketer = LambdaBracketer<float>;
  using DiscontinuityTester = NullDiscontinuityTester<float>;
  using Evaluator = LambdaEvaluator<float>;
  using NumericHelper = DefaultNumericHelper<float>;
  using SolutionQueue = StaticSolutionQueue<float, 3>;
  using Stepper = AdaptiveLogStepper<float>;
};

/* NOTE: Use for finance and probability computations, and as a base for
 * ExpressionSolver. */
struct DoublePrecisionStandardSolver {
  using Bracketer = LambdaBracketer<double>;
  using DiscontinuityTester = LambdaDiscontinuityTester<double>;
  using Evaluator = LambdaEvaluator<double>;
  using NumericHelper = DefaultNumericHelper<double>;
  using SolutionQueue = StaticSolutionQueue<double, 3>;
  using Stepper = AdaptiveLogStepper<double>;
};

template <typename TFloat>
struct StandardPolicySelector;
template <>
struct StandardPolicySelector<float> {
  using Policy = SinglePrecisionStandardSolver;
};
template <>
struct StandardPolicySelector<double> {
  using Policy = DoublePrecisionStandardSolver;
};

/* NOTE: Used in scandium's solver */
struct DoublePrecisionBasicRootSolver {
  using Bracketer = RootBracketer;
  // TODO: Does it need to check discontinuities?
  using DiscontinuityTester = NullDiscontinuityTester<double>;
  using Evaluator = ExpressionEvaluator<double>;
  using NumericHelper = DefaultNumericHelper<double>;
  using SolutionQueue = StaticSolutionQueue<double, 1>;
  using Stepper = BasicStepper<double>;
};

}  // namespace Policies

// --- Solvers instanciation for anonymous functions ---

template <typename TFloat>
using LambdaSolver =
    PolicySolver<TFloat,
                 typename Policies::StandardPolicySelector<TFloat>::Policy>;

class LambdaRootSolver : public LambdaSolver<double> {
 public:
  LambdaRootSolver(double xMin, double xMax, LambdaEvaluator<double> f)
      : LambdaSolver<double>(xMin, xMax, f, RootBracketer{},
                             NullDiscontinuityTester<double>{}) {}
};

// --- Solver instanciations for expression trees (prepared functions) ---

/* NOTE: In theory, ExpressionSolver should hold an ExpressionEvaluator.
 * However, to reduce the flash memory footprint, we reuse the
 * LambdaSolver<double>: the ExpressionEvaluator passed to the constructor
 * trivially coerces to a LambdaEvaluator and resolution works as expected. */
class ExpressionSolver : public LambdaSolver<double> {
 public:
  ExpressionSolver(double xMin, double xMax, const Internal::Tree* e,
                   LambdaBracketer<double> bracketer,
                   LambdaDiscontinuityTester<double> disc =
                       ExpressionDiscontinuityTester<double>{})
      : LambdaSolver<double>(xMin, xMax, ExpressionEvaluator<double>{e},
                             bracketer, disc) {}
};

class ExtremumSolver : public ExpressionSolver {
 public:
  ExtremumSolver(double xMin, double xMax, const Internal::Tree* e)
      : ExpressionSolver(xMin, xMax, e, ExtremumBracketer{}) {}
};

class RootSolver : public ExpressionSolver {
 public:
  RootSolver(double xMin, double xMax, const Internal::Tree* e)
      : ExpressionSolver(xMin, xMax, e, RootBracketer{}) {}

  Solution<double> next() override;

 private:
  struct DetailedRoot {
    double x;
    const Internal::Tree* nullDescendant = nullptr;
  };
  constexpr static DetailedRoot k_nanRoot = {.x = static_cast<double>(NAN),
                                             .nullDescendant = nullptr};

  const Internal::Tree* getTree() const {
    return static_cast<const Internal::Tree*>(m_evaluator.data());
  }

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

class DiscontinuitySolver : public ExpressionSolver {
 public:
  DiscontinuitySolver(double xMin, double xMax, const Internal::Tree* e)
      : ExpressionSolver(xMin, xMax, e, DiscontinuityBracketer{},
                         NullDiscontinuityTester<double>{}) {}

  void findAndPushSolution(Coordinate2D<double> start, Coordinate2D<double> end,
                           Interest interest) override;
};

}  // namespace Poincare::NumericSolver
