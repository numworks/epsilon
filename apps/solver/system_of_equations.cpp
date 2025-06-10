#include "system_of_equations.h"

#include <apps/constant.h>
#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/print.h>
#include <poincare/cas.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/pool_variable_context.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/equation_solver.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "app.h"

using namespace Poincare;
using Poincare::Internal::EquationSolver;
using namespace Poincare::Internal::KTrees;
using namespace Shared;

namespace Solver {

Internal::Tree* equationSet(const EquationStore* store) {
  Internal::Tree* equationSet = Internal::List::PushEmpty();
  int nEquations = store->numberOfDefinedModels();
  for (int i = 0; i < nEquations; i++) {
    ExpiringPointer<Equation> equation =
        store->modelForRecord(store->definedRecordAtIndex(i));
    Poincare::Expression equationExpression = equation->expressionClone();
    Internal::Tree* equal = equationExpression.tree()->cloneTree();
    Internal::PatternMatching::MatchReplace(equal, KEqual(KA, KB),
                                            KSub(KA, KB));
    Internal::NAry::AddChild(equationSet, equal);
  }
  return equationSet;
}

SystemOfEquations::Error SystemOfEquations::exactSolve(
    Poincare::Context* context) {
  Error error = Error::NoError;

  m_solverContext.variables.clear();
  m_solverContext.userVariables.clear();

  Internal::Tree* set = equationSet(m_store);
  Internal::Tree* result = EquationSolver::ExactSolve(
      set, &m_solverContext,
      {
          .m_complexFormat =
              MathPreferences::SharedPreferences()->complexFormat(),
          .m_angleUnit = MathPreferences::SharedPreferences()->angleUnit(),
          .m_context = context,
      },
      &error);

  if (error == Error::NoError) {
    assert(result);
    m_numberOfSolutions = 0;
    assert(solutionStatus() != SolutionStatus::Interrupted);
    SolutionType solutionType = solutionStatus() == SolutionStatus::Incomplete
                                    ? SolutionType::Formal
                                    : SolutionType::Exact;
    for (const Internal::Tree* solution : result->children()) {
      registerSolution(UserExpression::Builder(solution), context,
                       solutionType);
    }
    result->removeTree();
  } else {
    assert(!result);
  }
  set->removeTree();
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
  m_autoApproximateSolvingRange = false;
  m_solverContext.solutionStatus = SolutionStatus::Complete;
  m_approximateSolvingRange = approximateSolvingRange;
}

void SystemOfEquations::cancelApproximateSolve(
    bool autoApproximate, Poincare::Range1D<double> range) {
  m_solverContext.solutionStatus = SolutionStatus::Interrupted;
  m_autoApproximateSolvingRange = autoApproximate;
  // Warning : A default range is given, but solutions have not been computed.
  m_approximateSolvingRange = range;
  m_numberOfSolutions = 0;
}

Internal::Tree* SystemOfEquations::prepareEquationForApproximateSolve(
    Context* context) {
  Internal::Tree* set = equationSet(m_store);
  assert(set->numberOfChildren() == 1);
  set->removeNode();
  Internal::Tree* equation = set;

  // Reduce and replace user variables if needed
  Internal::ProjectionContext ctx{
      .m_complexFormat = MathPreferences::SharedPreferences()->complexFormat(),
      .m_angleUnit = MathPreferences::SharedPreferences()->angleUnit(),
      .m_symbolic = (m_solverContext.overrideUserVariables
                         ? SymbolicComputation::ReplaceDefinedFunctions
                         : SymbolicComputation::ReplaceDefinedSymbols),
      .m_context = context};
  Internal::Projection::UpdateComplexFormatWithExpressionInput(equation, &ctx);
  m_solverContext.complexFormat = ctx.m_complexFormat;
  Internal::Simplification::ProjectAndReduce(equation, &ctx);

  // Find remaining variable
  Internal::Tree* variables = Internal::Variables::GetUserSymbols(equation);
  assert(variables->numberOfChildren() == 1);
  m_solverContext.variables.fillWithList(variables);
  variables->removeTree();

  Internal::Approximation::PrepareFunctionForApproximation(
      equation, m_solverContext.variables.variable(0),
      Preferences::ComplexFormat::Real);
  return equation;
}

void SystemOfEquations::autoComputeApproximateSolvingRange(Context* context) {
  Internal::Tree* preparedEquation =
      prepareEquationForApproximateSolve(context);
  m_approximateSolvingRange =
      Poincare::Internal::EquationSolver::AutomaticInterval(preparedEquation,
                                                            &m_solverContext);
  m_autoApproximateSolvingRange = true;
  preparedEquation->removeTree();
}

void SystemOfEquations::approximateSolve(Context* context) {
  Internal::Tree* preparedEquation =
      prepareEquationForApproximateSolve(context);
  Internal::Tree* result = Poincare::Internal::EquationSolver::ApproximateSolve(
      preparedEquation, m_approximateSolvingRange, &m_solverContext);

  assert(result && result->isList());
  // Update member variables for LinearSystem
  m_numberOfSolutions = result->numberOfChildren();
  // Copy solutions
  for (int i = 0; const Internal::Tree* solution : result->children()) {
    m_solutions[i++] =
        Solution(Poincare::Layout(), Poincare::Layout(),
                 Poincare::Internal::FloatHelper::To(solution), false);
  }
  result->removeTree();
  preparedEquation->removeTree();
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

// approximate is an optional parameter
static void simplifyAndApproximateSolution(
    UserExpression e, UserExpression* exact, UserExpression* approximate,
    bool approximateDuringReduction, Context* context,
    Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit,
    Preferences::UnitFormat unitFormat,
    SymbolicComputation symbolicComputation) {
  assert(exact);
  Internal::ProjectionContext projCtx = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = angleUnit,
      .m_strategy = approximateDuringReduction
                        ? Internal::Strategy::ApproximateToFloat
                        : Internal::Strategy::Default,
      .m_unitFormat = unitFormat,
      .m_symbolic = symbolicComputation,
      .m_context = context};
  if (approximate) {
    e.cloneAndSimplifyAndApproximate(exact, approximate, projCtx);
  } else {
    bool reductionFailure = false;
    *exact = e.cloneAndSimplify(projCtx, &reductionFailure);
  }
  assert(!exact->isUninitialized() &&
         (!approximate || !approximate->isUninitialized()));
  if (exact->isDep()) {
    /* Reduction may have created a dependency.
     * We remove that dependency in order to create layouts. */
    *exact = exact->cloneChildAtIndex(0);
  }
}

SystemOfEquations::Error SystemOfEquations::registerSolution(
    UserExpression e, Context* context, SolutionType type) {
  Preferences::AngleUnit angleUnit =
      MathPreferences::SharedPreferences()->angleUnit();
  UserExpression exact, approximate;

  bool forbidExactSolution =
      MathPreferences::SharedPreferences()->examMode().forbidExactResults();
  EquationStore* store = m_store;
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

  bool approximateDuringReduction =
      type == SolutionType::Formal && forbidExactSolution;

  bool displayExactSolution = false;
  bool displayApproximateSolution = false;
  if (type == SolutionType::Approximate) {
    approximate = e;
    displayApproximateSolution = true;
  } else {
    assert(type == SolutionType::Formal || type == SolutionType::Exact);
    UserExpression* approximatePointer =
        type == SolutionType::Formal ? nullptr : &approximate;
    Preferences::UnitFormat unitFormat =
        GlobalPreferences::SharedGlobalPreferences()->unitFormat();
    // Any remaining symbol at this point should be an unknown parameter.
    SymbolicComputation symbolicComputation =
        SymbolicComputation::KeepAllSymbols;
    assert(type == SolutionType::Formal || !e.clone().replaceSymbols(context));
    simplifyAndApproximateSolution(e, &exact, approximatePointer,
                                   approximateDuringReduction, context,
                                   m_solverContext.complexFormat, angleUnit,
                                   unitFormat, symbolicComputation);
    displayExactSolution =
        approximateDuringReduction ||
        (!forbidExactSolution &&
         !CAS::ShouldOnlyDisplayApproximation(e, exact, approximate, context));
    displayApproximateSolution = type != SolutionType::Formal;
    if (!displayApproximateSolution && !displayExactSolution) {
      /* Happens if the formal solution has no permission to be displayed.
       * Re-reduce but force approximating during reduction. */
      exact = UserExpression();
      approximate = UserExpression();
      simplifyAndApproximateSolution(e, &exact, approximatePointer, true,
                                     context, m_solverContext.complexFormat,
                                     angleUnit, unitFormat,
                                     symbolicComputation);
      displayExactSolution = true;
    }
  }
  if (!approximate.isUninitialized() && approximate.isNonReal()) {
    return Error::EquationNonReal;
  }
  if (type != SolutionType::Formal && approximate.isUndefined()) {
    return Error::EquationUndefined;
  }

  Layout exactLayout, approximateLayout;
  if (displayExactSolution) {
    assert(!exact.isUninitialized());
    exactLayout = PoincareHelpers::CreateLayout(exact, context);
  }
  if (displayApproximateSolution) {
    assert(!approximate.isUninitialized());
    approximateLayout = PoincareHelpers::CreateLayout(approximate, context);
  }
  assert(!approximateLayout.isUninitialized() ||
         !exactLayout.isUninitialized());

  bool exactAndApproximateAreEqual = false;
  if (!approximateLayout.isUninitialized() && !exactLayout.isUninitialized()) {
    char exactBuffer[::Constant::MaxSerializedExpressionSize];
    char approximateBuffer[::Constant::MaxSerializedExpressionSize];
    [[maybe_unused]] size_t exactSerializationLength = exactLayout.serialize(
        exactBuffer, ::Constant::MaxSerializedExpressionSize);
    assert(exactSerializationLength <= ::Constant::MaxSerializedExpressionSize);
    [[maybe_unused]] size_t approximateLength = approximateLayout.serialize(
        approximateBuffer, ::Constant::MaxSerializedExpressionSize);
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
