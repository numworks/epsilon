#include "system_of_equations.h"

#include <apps/constant.h>
#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/print.h>
#include <poincare/cas.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/pool_variable_context.h>
#include <poincare/range.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/equation_solver.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

using namespace Poincare;
using Poincare::Internal::EquationSolver;
using namespace Poincare::Internal::KTrees;
using namespace Shared;

namespace Solver {

Internal::Tree* equationList(const EquationStore* store) {
  Internal::Tree* equationList = Internal::List::PushEmpty();
  int nEquations = store->numberOfDefinedModels();
  for (int i = 0; i < nEquations; i++) {
    ExpiringPointer<Equation> equation =
        store->modelForRecord(store->definedRecordAtIndex(i));
    Poincare::Expression equationExpression = equation->expressionClone();
    Internal::NAry::AddChild(equationList,
                             equationExpression.tree()->cloneTree());
  }
  return equationList;
}

SystemOfEquations::Error SystemOfEquations::exactSolve(
    Poincare::Context* context) {
  m_wasInterrupted = false;
  m_equationMetadata.unknownVariables.clear();
  m_equationMetadata.definedVariables.clear();

  Internal::Tree* eqList = equationList(m_store);
  EquationSolver::SolverResult result = EquationSolver::ExactSolveAdaptive(
      eqList,
      {
          .m_complexFormat =
              MathPreferences::SharedPreferences()->complexFormat(),
          .m_angleUnit = MathPreferences::SharedPreferences()->angleUnit(),
          .m_context = context,
      });
  Internal::Tree* exactSolutionList = result.exactSolutionList;
  Internal::Tree* approximateSolutionList = result.approximateSolutionList;
  m_solutionMetadata = result.solutionMetadata;
  m_equationMetadata = result.equationMetadata;
  Error error = result.error;
  if (error == Error::NoError) {
    assert(exactSolutionList && exactSolutionList->isList());
    m_numberOfSolutions = 0;  // Reset number of solutions

    bool hasApproximateSolutions = approximateSolutionList;
    assert(!hasApproximateSolutions ||
           (approximateSolutionList->isList() &&
            approximateSolutionList->numberOfChildren() ==
                exactSolutionList->numberOfChildren()));

    size_t nSolutions = exactSolutionList->numberOfChildren();
    const Internal::Tree* currentApproximate =
        hasApproximateSolutions && nSolutions > 0
            ? approximateSolutionList->child(0)
            : nullptr;
    size_t i = 0;
    for (const Internal::Tree* exactSol : exactSolutionList->children()) {
      UserExpression approximate = UserExpression();
      if (currentApproximate) {
        approximate = UserExpression::Builder(currentApproximate);
        if (++i < nSolutions) {  // Do not go beyond the stack end
          currentApproximate = currentApproximate->nextTree();
        }
      }
      registerExactSolution(UserExpression::Builder(exactSol), approximate,
                            context);
    }
    if (approximateSolutionList) {
      assert(approximateSolutionList > exactSolutionList);
      approximateSolutionList->removeTree();
    }
    exactSolutionList->removeTree();
  } else {
    assert(!exactSolutionList && !approximateSolutionList);
  }
  eqList->removeTree();
  return error;
}

const Internal::Tree*
SystemOfEquations::ContextWithoutT::expressionForUserNamed(
    const Internal::Tree* symbol) {
  assert(symbol->isUserNamed());
  if (symbol->isUserSymbol() &&
      strcmp(Internal::Symbol::GetName(symbol), "t") == 0) {
    return UserExpression();
  }
  return ContextWithParent::expressionForUserNamed(symbol);
}

void SystemOfEquations::setApproximateSolvingRange(
    Poincare::Range1D<double> approximateSolvingRange) {
  m_isUsingAutoSolvingRange = false;
  m_approximateSolvingRange = approximateSolvingRange;
}

void SystemOfEquations::cancelApproximateSolve() {
  m_wasInterrupted = true;
  m_numberOfSolutions = 0;
}

void SystemOfEquations::approximateSolve(Context* context) {
  assert(m_store->numberOfDefinedModels() == 1);
  m_wasInterrupted = false;
  Internal::Tree* eqList = equationList(m_store);

  EquationSolver::SolverResult result =
      Poincare::Internal::EquationSolver::ApproximateSolve(
          eqList,
          {.m_complexFormat =
               MathPreferences::SharedPreferences()->complexFormat(),
           .m_angleUnit = MathPreferences::SharedPreferences()->angleUnit(),
           .m_context = context},
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

  assert(result.approximateSolutionList &&
         result.approximateSolutionList->isList() && !result.exactSolutionList);
  // Update member variables for LinearSystem
  m_numberOfSolutions = result.approximateSolutionList->numberOfChildren();
  // Copy solutions
  for (int i = 0; const Internal::Tree* solution :
                  result.approximateSolutionList->children()) {
    m_solutions[i++] =
        Solution(Poincare::Layout(), Poincare::Layout(),
                 Poincare::Internal::FloatHelper::To(solution), false);
  }
  result.approximateSolutionList->removeTree();
  eqList->removeTree();
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
    UserExpression exact, UserExpression approximate, Context* context) {
  assert(m_solutionMetadata.solutionType != SolutionType::Approximate);
  assert(!exact.isUninitialized());

  EquationStore* store = m_store;

  bool forbidExactSolution =
      MathPreferences::SharedPreferences()->examMode().forbidExactResults();

  int nEquations = store->numberOfDefinedModels();
  int i = 0;
  while (i < nEquations && !forbidExactSolution) {
    ExpiringPointer<Equation> equation =
        store->modelForRecord(store->definedRecordAtIndex(i));
    if (CAS::NeverDisplayReductionOfInput(equation->expressionClone(),
                                          context)) {
      forbidExactSolution = true;
    }
    i++;
  }

  assert(m_solutionMetadata.solutionType == SolutionType::Formal ||
         !exact.clone().replaceSymbols(context));

  forbidExactSolution =
      forbidExactSolution ||
      CAS::ShouldOnlyDisplayApproximation(exact, exact, approximate, context);

  if (forbidExactSolution && approximate.isUninitialized()) {
    // Re-reduce exact solution but approximate during reduction.
    Internal::ProjectionContext projCtx = {
        .m_complexFormat = m_equationMetadata.complexFormat,
        .m_angleUnit = MathPreferences::SharedPreferences()->angleUnit(),
        .m_strategy = Internal::Strategy::ApproximateToFloat,
        .m_unitFormat =
            GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
        // Any remaining symbol at this point should be an unknown parameter.
        .m_symbolic = SymbolicComputation::KeepAllSymbols,
        .m_context = context,
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
    exactLayout = PoincareHelpers::CreateLayout(exact, context);
  }
  if (!approximate.isUninitialized()) {
    approximateLayout = PoincareHelpers::CreateLayout(approximate, context);
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
    } else if (Poincare::ExactAndApproximateExpressionsAreStrictlyEqual(
                   exact, approximate)) {
      exactAndApproximateAreEqual = true;
    }
  }

  assert(m_numberOfSolutions < k_maxNumberOfSolutions - 1);
  m_solutions[m_numberOfSolutions++] = Solution(
      exactLayout, approximateLayout, NAN, exactAndApproximateAreEqual);

  return Error::NoError;
}

}  // namespace Solver
