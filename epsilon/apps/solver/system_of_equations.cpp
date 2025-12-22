#include "system_of_equations.h"

#include <apps/constant.h>
#include <apps/exam_mode_manager.h>
#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/print.h>
#include <poincare/cas.h>
#include <poincare/equation_solver/equation_solver_pool.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/pool_variable_context.h>
#include <poincare/range.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/symbol_context.h>
#include <poincare/user_expression.h>

using namespace Poincare;
using namespace Shared;

namespace Solver {

UserExpression equationList(const EquationStore* store) {
  OMG::StaticVector<UserExpression, EquationStore::k_maxNumberOfEquations>
      expressions;
  for (int i = 0; i < store->numberOfDefinedModels(); i++) {
    expressions.push(store->cloneExpressionOfEquationAtIndex(i));
  }
  return UserExpression::BuildListOfExpressions(expressions.span());
}

SystemOfEquations::Error SystemOfEquations::exactSolve(
    const Poincare::SymbolContext& symbolContext) {
  m_wasInterrupted = false;
  m_equationMetadata.unknownVariables.clear();
  m_equationMetadata.definedVariables.clear();

  UserExpression eqList = equationList(m_store);
  EquationSolver::SolverResult result = EquationSolver::ExactSolveAdaptive(
      eqList,
      {
          .m_complexFormat =
              GlobalPreferences::SharedGlobalPreferences()->complexFormat(),
          .m_angleUnit =
              GlobalPreferences::SharedGlobalPreferences()->angleUnit(),
          .m_context = symbolContext,
      });
  UserExpression exactSolutionList = result.exactSolutionList;
  UserExpression approximateSolutionList = result.approximateSolutionList;
  m_solutionMetadata = result.solutionMetadata;
  m_equationMetadata = result.equationMetadata;
  Error error = result.error;

  if (error == Error::NoError) {
    assert(!exactSolutionList.isUninitialized() && exactSolutionList.isList());
    m_numberOfSolutions = 0;  // Reset number of solutions

    bool hasApproximateSolutions = !approximateSolutionList.isUninitialized();
    assert(!hasApproximateSolutions ||
           (approximateSolutionList.isList() &&
            approximateSolutionList.numberOfChildren() ==
                exactSolutionList.numberOfChildren()));

    size_t nSolutions = exactSolutionList.numberOfChildren();
    for (size_t i = 0; i < nSolutions; i++) {
      UserExpression approximate = UserExpression();
      if (hasApproximateSolutions) {
        approximate = approximateSolutionList.cloneChildAtIndex(i);
      }
      UserExpression exact = exactSolutionList.cloneChildAtIndex(i);
      registerExactSolution(exact, approximate, symbolContext);
    }
  } else {
    assert(exactSolutionList.isUninitialized() &&
           approximateSolutionList.isUninitialized());
  }
  return error;
}

void SystemOfEquations::setApproximateSolvingRange(
    Poincare::Range1D<double> approximateSolvingRange) {
  m_isUsingAutoSolvingRange = false;
  m_approximateSolvingRange = approximateSolvingRange;
}

void SystemOfEquations::cancelApproximateSolve() {
  m_wasInterrupted = true;
  m_numberOfSolutions = 0;
  // Ensure solution metadata is reset
  m_solutionMetadata = Poincare::EquationSolver::SolutionMetadata();
  m_solutionMetadata.solvingMethod =
      Poincare::EquationSolver::SolvingMethod::GeneralMonovariable;
  m_solutionMetadata.solutionType =
      Poincare::EquationSolver::SolutionType::Approximate;
}

void SystemOfEquations::approximateSolve(const SymbolContext& symbolContext) {
  assert(m_store->numberOfDefinedModels() == 1);
  m_wasInterrupted = false;
  UserExpression eqList = equationList(m_store);

  EquationSolver::SolverResult result = EquationSolver::ApproximateSolve(
      eqList,
      {.m_complexFormat =
           GlobalPreferences::SharedGlobalPreferences()->complexFormat(),
       .m_angleUnit = GlobalPreferences::SharedGlobalPreferences()->angleUnit(),
       .m_reductionTarget = ReductionTarget::SystemForApproximation,
       .m_context = symbolContext},
      m_isUsingAutoSolvingRange ? m_memoizedAutoSolvingRange
                                : m_approximateSolvingRange,
      k_maxNumberOfApproximateSolutions);

  m_solutionMetadata = result.solutionMetadata;
  m_equationMetadata = result.equationMetadata;

  // Store the range used to solve
  m_approximateSolvingRange = m_solutionMetadata.solvingRange;
  if (m_isUsingAutoSolvingRange) {
    m_memoizedAutoSolvingRange = m_approximateSolvingRange;
  }

  assert(!result.approximateSolutionList.isUninitialized() &&
         result.approximateSolutionList.isList() &&
         result.exactSolutionList.isUninitialized());
  // Update member variables for LinearSystem
  m_numberOfSolutions = result.approximateSolutionList.numberOfChildren();
  // Copy solutions
  for (size_t i = 0; i < m_numberOfSolutions; i++) {
    double solution = result.approximateSolutionList.cloneChildAtIndex(i)
                          .approximateToRealScalar<double>();
    m_solutions[i] =
        Solution(Poincare::Layout(), Poincare::Layout(), solution, false);
  }
}

void SystemOfEquations::tidy(PoolObject* treePoolCursor) {
  for (int i = 0; i < k_maxNumberOfSolutions; i++) {
    if (treePoolCursor == nullptr ||
        m_solutions[i].exactLayout().isDownstreamOf(treePoolCursor) ||
        m_solutions[i].approximateLayout().isDownstreamOf(treePoolCursor)) {
      m_solutions[i] = Solution();
    }
  }
}

SystemOfEquations::Error SystemOfEquations::registerExactSolution(
    UserExpression exact, UserExpression approximate,
    const SymbolContext& symbolContext) {
  assert(m_solutionMetadata.solutionType != SolutionType::Approximate);
  assert(!exact.isUninitialized());

  EquationStore* store = m_store;

  bool forbidExactSolution = ExamModeManager::ExamMode().forbidExactResults();

  int nEquations = store->numberOfDefinedModels();
  int i = 0;
  while (i < nEquations && !forbidExactSolution) {
    OMG::ExpiringPointer<Equation> equation =
        store->modelForRecord(store->definedRecordAtIndex(i));
    if (CAS::NeverDisplayReductionOfInput(equation->expressionClone(),
                                          symbolContext)) {
      forbidExactSolution = true;
    }
    i++;
  }

#if ASSERTIONS
  if (m_solutionMetadata.solutionType != SolutionType::Formal) {
    // Solution should not have any defined symbols.
    UserExpression e = exact.clone();
    e.replaceSymbols(symbolContext, SymbolicComputation::ReplaceDefinedSymbols);
    assert(e.isIdenticalTo(exact));
  }
#endif

  forbidExactSolution =
      forbidExactSolution || CAS::ShouldOnlyDisplayApproximation(
                                 exact, exact, approximate, symbolContext);

  if (forbidExactSolution && approximate.isUninitialized()) {
    // Re-reduce exact solution but approximate during reduction.
    ProjectionContext projCtx = {
        .m_complexFormat = m_equationMetadata.complexFormat,
        .m_angleUnit =
            GlobalPreferences::SharedGlobalPreferences()->angleUnit(),
        .m_strategy = Strategy::ApproximateToFloat,
        .m_unitFormat =
            GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
        // Any remaining symbol at this point should be an unknown parameter.
        .m_symbolic = SymbolicComputation::KeepAllSymbols,
        .m_context = symbolContext,
        .m_advanceReduce = false};
    bool failure = false;
    approximate = exact.cloneAndSimplify(projCtx, &failure);
  }

  if (!approximate.isUninitialized()) {
    if (approximate.isNonReal()) {
      return Error::EquationNonReal;
    }
    if (approximate.isUndefined()) {
      return Error::EquationUndefined;
    }
  }

  Layout exactLayout, approximateLayout;
  if (!forbidExactSolution) {
    assert(!exact.isUninitialized());
    exactLayout = PoincareHelpers::CreateLayout(exact, symbolContext);
  }
  if (!approximate.isUninitialized()) {
    approximateLayout =
        PoincareHelpers::CreateLayout(approximate, symbolContext);
  }

  assert(!approximateLayout.isUninitialized() ||
         !exactLayout.isUninitialized());

  bool exactAndApproximateAreEqual = false;
  if (!approximateLayout.isUninitialized() && !exactLayout.isUninitialized()) {
    char exactBuffer[::Constant::MaxSerializedExpressionSize];
    char approximateBuffer[::Constant::MaxSerializedExpressionSize];
    [[maybe_unused]] size_t exactSerializationLength =
        exactLayout.serialize(exactBuffer);
    assert(exactSerializationLength <= ::Constant::MaxSerializedExpressionSize);
    [[maybe_unused]] size_t approximateLength =
        approximateLayout.serialize(approximateBuffer);
    assert(approximateLength <= ::Constant::MaxSerializedExpressionSize);
    if (strcmp(exactBuffer, approximateBuffer) == 0) {
      exactLayout = Layout();
    } else if (Poincare::IsCalculationOutputStrictEquality(
                   exact, approximate,
                   GlobalPreferences::SharedGlobalPreferences()
                       ->numberOfSignificantDigits())) {
      exactAndApproximateAreEqual = true;
    }
  }

  assert(m_numberOfSolutions < k_maxNumberOfSolutions - 1);
  m_solutions[m_numberOfSolutions++] = Solution(
      exactLayout, approximateLayout, NAN, exactAndApproximateAreEqual);

  return Error::NoError;
}

}  // namespace Solver
