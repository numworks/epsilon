#include "system_of_equations.h"

#include <apps/constant.h>
#include <apps/exam_mode_manager.h>
#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/print.h>
#include <poincare/cas.h>
#include <poincare/equation_solver/equation_solver_pool.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/helpers/layout.h>
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

#include "app.h"

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
      eqList, {.user = {.complexFormat = SharedGlobalPrefs().complexFormat(),
                        .angleUnit = SharedGlobalPrefs().angleUnit(),
                        .context = symbolContext}});
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
      {.user = {.complexFormat = SharedGlobalPrefs().complexFormat(),
                .angleUnit = SharedGlobalPrefs().angleUnit(),
                .context = symbolContext},
       .reductionPolicy = {.target = ReductionTarget::System}},
      m_isUsingAutoSolvingRange ? m_memoizedAutoSolvingRange
                                : m_approximateSolvingRange,
      k_maxNumberOfApproximateSolutions);

  m_solutionMetadata = result.solutionMetadata;
  m_equationMetadata = result.equationMetadata;

  if (result.error != Error::NoError) {
    /* An error occurred that was not there during the exact solve. We can't
     * solve this equation with approximation, so we return no solution and set
     * the auto range to the widest range instead of [NAN, NAN]. */
    if (m_isUsingAutoSolvingRange) {
      m_approximateSolvingRange =
          Poincare::Range1D<double>(-INFINITY, INFINITY);
      m_memoizedAutoSolvingRange = m_approximateSolvingRange;
    }
    m_numberOfSolutions = 0;
    return;
  }

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
    double solution =
        result.approximateSolutionList.cloneChildAtIndex(i)
            .approximateToRealScalar<double>(
                UserParameters{.angleUnit = SharedGlobalPrefs().angleUnit()},
                ApproximationPolicy{.optimize = true});
    m_solutions[i] =
        Solution(Poincare::Layout(), Poincare::Layout(), solution, false, true);
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
    const UserExpression& exact, const UserExpression& approximate,
    const SymbolContext& symbolContext) {
  assert(m_solutionMetadata.solutionType != SolutionType::Approximate);
  assert(!exact.isUninitialized());
  Poincare::AngleUnit angleUnit =
      GlobalPreferences::SharedGlobalPreferences()->angleUnit();
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

  // Solution should not have any defined symbols.
  assert(m_solutionMetadata.solutionType == SolutionType::Formal ||
         exact.isIdenticalTo(exact.cloneAndReplaceSymbols(
             symbolContext, SymbolicComputation::ReplaceDefinedSymbols)));

  forbidExactSolution =
      forbidExactSolution || CAS::ShouldOnlyDisplayApproximation(
                                 exact, exact, approximate, symbolContext);

  UserExpression exactApproximateClone;
  bool shouldUseExactForApproximate =
      forbidExactSolution && approximate.isUninitialized();
  if (shouldUseExactForApproximate) {
    // Approximate exact reduction preserving variables and symbols
    exactApproximateClone = exact.cloneAndApproximate(
        UserParameters{.complexFormat = m_equationMetadata.complexFormat,
                       .angleUnit = angleUnit,
                       .unitFormat = SharedGlobalPrefs().unitFormat(),
                       .context = symbolContext},
        ApproximationPolicy{.onlyReplaceNumerical = true});
  }

  const UserExpression& approximateToUse =
      shouldUseExactForApproximate ? exactApproximateClone : approximate;

  if (!approximateToUse.isUninitialized()) {
    if (approximateToUse.isNonReal()) {
      return Error::EquationNonReal;
    }
    if (approximateToUse.isUndefined()) {
      return Error::EquationUndefined;
    }
  }

  Layout exactLayout, approximateLayout;
  if (!forbidExactSolution) {
    assert(!exact.isUninitialized());
    exactLayout =
        PoincareHelpers::CreateLayout(exact, symbolContext, App::k_useTenPower);
  }
  if (!approximateToUse.isUninitialized()) {
    approximateLayout = PoincareHelpers::CreateLayout(
        approximateToUse, symbolContext, App::k_useTenPower);
  }

  assert(!approximateLayout.isUninitialized() ||
         !exactLayout.isUninitialized());

  uint8_t numberOfSignificantDigits =
      GlobalPreferences::SharedGlobalPreferences()->numberOfSignificantDigits();

  bool comparisonSignIsEqual = false;
  bool comparisonSignIsVisible = true;
  if (!approximateLayout.isUninitialized() && !exactLayout.isUninitialized()) {
    char exactBuffer[::Constant::MaxSerializedExpressionSize];
    char approximateBuffer[::Constant::MaxSerializedExpressionSize];
    size_t exactSerializationLength = exactLayout.serialize(exactBuffer);
    [[maybe_unused]] size_t approximateLength =
        approximateLayout.serialize(approximateBuffer);
    /* The approximate expression should always be small enough to fit in a
     * standard Escher buffer with size Constant::MaxSerializedExpressionSize.
     * The exact expression however can potentially overflow the buffer size.
     */
    assert(approximateLength <= ::Constant::MaxSerializedExpressionSize);
    if (exactSerializationLength == LayoutHelpers::k_bufferOverflow) {
      exactLayout = Layout();
    } else {
      assert(exactSerializationLength <=
             ::Constant::MaxSerializedExpressionSize);
      if (strcmp(exactBuffer, approximateBuffer) == 0) {
        comparisonSignIsEqual = true;
        comparisonSignIsVisible = false;
        exactLayout = Layout();
      }
    }
    if (!exactLayout.isUninitialized() &&
        Poincare::IsCalculationOutputStrictEquality(
            exact, approximateToUse, numberOfSignificantDigits,
            {.user = {.complexFormat = m_equationMetadata.complexFormat,
                      .angleUnit = angleUnit}})) {
      comparisonSignIsEqual = true;
      comparisonSignIsVisible = true;
    }
  } else if (approximateLayout.isUninitialized() &&
             !shouldUseExactForApproximate) {
    comparisonSignIsEqual = true;
    comparisonSignIsVisible = false;
  } else if (forbidExactSolution && !shouldUseExactForApproximate) {
    assert(exactLayout.isUninitialized() && !exact.isUninitialized());
    // Use hidden exact result for comparison symbol
    comparisonSignIsEqual = Poincare::IsCalculationOutputStrictEquality(
        exact, approximateToUse, numberOfSignificantDigits,
        {.user = {.complexFormat = m_equationMetadata.complexFormat,
                  .angleUnit = angleUnit}});
    comparisonSignIsVisible = !comparisonSignIsEqual;
  }
  assert(m_numberOfSolutions < k_maxNumberOfSolutions - 1);
  m_solutions[m_numberOfSolutions++] =
      Solution(exactLayout, approximateLayout, NAN, comparisonSignIsEqual,
               comparisonSignIsVisible);

  return Error::NoError;
}

}  // namespace Solver
