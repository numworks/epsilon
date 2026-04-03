#pragma once

#include "policies/bracket.h"
#include "policies/discontinuity.h"
#include "policies/evaluation.h"
#include "policies/numeric_helper.h"
#include "policies/queue.h"
#include "policies/step.h"
#include "solver.h"

namespace Poincare::NumericSolver {

/* LambdaSolver: generic solver with caller-provided bracket test and hone
 * function pointers, and a caller-provided evaluation function.
 * Matches the old ZoomSolverPolicies; ZoomSolver<T> is now an alias for
 * LambdaSolver<T>. */
template <typename TFloat>
struct LambdaSolverPolicies {
  using Bracketer = LambdaBracketer<TFloat>;
  using DiscontinuityTester = NullDiscontinuityTester<TFloat>;
  using Evaluator = LambdaEvaluator<TFloat>;
  using NumericHelper = DefaultNumericHelper<TFloat>;
  using SolutionQueue = StaticSolutionQueue<TFloat, 3>;
  using Stepper = AdaptiveLogStepper<TFloat>;
};

template <typename TFloat>
using LambdaSolver = PolicySolver<TFloat, LambdaSolverPolicies<TFloat>>;

/* LambdaRootSolver: root finder with a caller-provided evaluation function.
 * Uses the standard RootBracketer (EvenOrOddRootInBracket + CompositeBrent);
 * no function pointers needed at construction. */
template <typename TFloat>
struct LambdaRootSolverPolicies {
  using Bracketer = RootBracketer;
  using DiscontinuityTester = NullDiscontinuityTester<TFloat>;
  using Evaluator = LambdaEvaluator<TFloat>;
  using NumericHelper = DefaultNumericHelper<TFloat>;
  using SolutionQueue = StaticSolutionQueue<TFloat, 3>;
  using Stepper = AdaptiveLogStepper<TFloat>;
};

template <typename TFloat>
using LambdaRootSolver = PolicySolver<TFloat, LambdaRootSolverPolicies<TFloat>>;

}  // namespace Poincare::NumericSolver
