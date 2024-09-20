#include "system_of_equations.h"

#include <apps/constant.h>
#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/print.h>
#include <poincare/cas.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/numeric/zoom.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/matrix.h>
#include <poincare/old/polynomial.h>
#include <poincare/old/symbol.h>
#include <poincare/old/variable_context.h>
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
      set, &m_solverContext, {.m_context = context}, &error);

  if (error == Error::NoError) {
    assert(result);
    // Update member variables for LinearSystem
    m_type = m_solverContext.type;
    m_degree = m_solverContext.degree;
    m_hasMoreSolutions = m_solverContext.hasMoreSolutions;
    m_numberOfSolutions = result->numberOfChildren();
    // Copy solutions
    for (int i = 0; const Internal::Tree* solution : result->children()) {
      Poincare::Expression exact = Poincare::UserExpression::Builder(solution);
      Poincare::Expression reduced = exact.cloneAndReduce({});
      Poincare::Layout exactLayout =
          PoincareHelpers::CreateLayout(exact, context);
      Poincare::Layout approximatedLayout;
      bool areStriclyEqual = false;
      if (!m_hasMoreSolutions) {
        /* TODO: even if some variables depend on t, others may be worth to
         * approximate, alternatively approx everything with ReplaceScalars */
        ApproximationContext ctx(context);
        Poincare::Expression approximated =
            reduced.approximateToTree<double>(ctx);
        if (approximated.tree()->isNonReal()) {
          return Error::EquationNonReal;
        }
        Internal::ProjectionContext projCtx;  // TODO: pass arguments
        areStriclyEqual =
            Poincare::ExactAndApproximateExpressionsAreStrictlyEqual(
                reduced, approximated, &projCtx);
        approximatedLayout =
            PoincareHelpers::CreateLayout(approximated, context);
        if (exactLayout.isIdenticalTo(approximatedLayout)) {
          approximatedLayout = Poincare::Layout();
        }
      }
      m_solutions[i++] =
          Solution(exactLayout, approximatedLayout, NAN, areStriclyEqual);
    }
    result->removeTree();
  }
#if 0
  else if (error == Error::RequireApproximateSolution) {
    m_type = Type::GeneralMonovariable;
    m_approximateResolutionMinimum = -k_defaultApproximateSearchRange;
    m_approximateResolutionMaximum = k_defaultApproximateSearchRange;
  }
#endif
  set->removeTree();
  return error;
}

const UserExpression
SystemOfEquations::ContextWithoutT::protectedExpressionForSymbolAbstract(
    const SymbolAbstract& symbol, bool clone,
    ContextWithParent* lastDescendantContext) {
  if (symbol.isUserSymbol() &&
      static_cast<const Symbol&>(symbol).name()[0] == 't') {
    return UserExpression();
  }
  return ContextWithParent::protectedExpressionForSymbolAbstract(
      symbol, clone, lastDescendantContext);
}

#if 0
SystemOfEquations::Error SystemOfEquations::exactSolve(Context* context) {
  m_overrideUserVariables = false;
  Error firstError = privateExactSolve(context);
  if (firstError == Error::RequireApproximateSolution ||
      (firstError == Error::NoError && m_numberOfSolutions > 0)) {
    return firstError;
  }

  m_overrideUserVariables = true;
  Error secondError = privateExactSolve(context);
  if (firstError == Error::NoError && secondError != Error::NoError &&
      secondError != Error::RequireApproximateSolution) {
    /* The system becomes invalid when overriding the user variables: the first
     * solution was better. */
    m_numberOfSolutions = 0;
    return firstError;
  }
  return secondError;
}
#endif

template <typename T>
static Coordinate2D<T> evaluator(T t, const void* model, Context* context) {
  void** modelArray = reinterpret_cast<void**>(const_cast<void*>(model));
  const SystemFunction* e =
      reinterpret_cast<const SystemFunction*>(modelArray[0]);
  const char* variable = reinterpret_cast<const char*>(modelArray[1]);
  return Coordinate2D<T>(
      t, e->approximateToScalarWithValueForSymbol<T>(
             variable, t,
             ApproximationContext(
                 context, Preferences::SharedPreferences()->complexFormat(),
                 Preferences::SharedPreferences()->angleUnit())));
}

void SystemOfEquations::setApproximateSolvingRange(
    Poincare::Range1D<double> approximateSolvingRange) {
  m_autoApproximateSolvingRange = false;
  m_hasMoreSolutions = false;
  m_solverContext.hasMoreSolutions = false;
  m_approximateSolvingRange = approximateSolvingRange;
}

void SystemOfEquations::autoComputeApproximateSolvingRange(Context* context) {
  // TODO: factor with approximateSolve to avoid preparing the equation twice
  Internal::Tree* set = equationSet(m_store);
  assert(set->numberOfChildren() == 1);
  Internal::Tree* equation = set->child(0);
  Internal::Tree* variables = Internal::Variables::GetUserSymbols(equation);
  assert(variables->numberOfChildren() == 1);
  m_solverContext.variables.append(
      Internal::Symbol::GetName(variables->child(0)));
  variables->removeTree();
  Internal::ProjectionContext ctx;
  Internal::Simplification::ToSystem(equation, &ctx);
  Internal::Approximation::PrepareFunctionForApproximation(
      equation, m_solverContext.variables.variable(0),
      Preferences::ComplexFormat::Real);

  m_approximateSolvingRange =
      Poincare::Internal::EquationSolver::AutomaticInterval(equation,
                                                            &m_solverContext);
  m_autoApproximateSolvingRange = true;
  m_hasMoreSolutions = m_solverContext.hasMoreSolutions;
}

#if 0
void SystemOfEquations::autoComputeApproximateSolvingRange(Context* context) {
  SystemExpression equationStandardForm =
      equationStandardFormForApproximateSolve(context);
  constexpr static float k_maxFloatForAutoApproximateSolvingRange = 1e15f;
  Zoom zoom(NAN, NAN, InteractiveCurveViewRange::NormalYXRatio(), context,
            k_maxFloatForAutoApproximateSolvingRange);
  // Use the intersection between the definition domain of f and the bounds
  zoom.setBounds(-k_maxFloatForAutoApproximateSolvingRange,
                 k_maxFloatForAutoApproximateSolvingRange);
  zoom.setMaxPointsOneSide(k_maxNumberOfApproximateSolutions,
                           k_maxNumberOfApproximateSolutions / 2);
  void* model[2] = {static_cast<void*>(&equationStandardForm),
                    static_cast<void*>(m_variables[0])};
  bool finiteNumberOfSolutions = true;
  bool didFitRoots =
      zoom.fitRoots(evaluator<float>, static_cast<void*>(model), false,
                    evaluator<double>, &finiteNumberOfSolutions);
  /* When there are more than k_maxNumberOfApproximateSolutions on one side of
   * 0, the zoom is setting the interval to have a maximum of 5 solutions left
   * of 0 and 5 solutions right of zero. This means that sometimes, for a
   * function like `piecewise(1, x<0; cos(x), x >= 0)`, only 5 solutions will be
   * displayed. We still want to notify the user that more solutions exist. */
  m_hasMoreSolutions = !finiteNumberOfSolutions;
  zoom.fitBounds(evaluator<float>, static_cast<void*>(model), false);
  Range1D<float> finalRange = *(zoom.range(false, false).x());
  if (didFitRoots) {
    /* The range was computed from the solution found with a solver in float. We
     * need to strech the range in case it does not cover the solution found
     * with a solver in double. */
    constexpr static float k_securityMarginCoef = 1 / 10.0;
    float securityMargin =
        std::max(std::abs(finalRange.max()), std::abs(finalRange.min())) *
        k_securityMarginCoef;
    finalRange.stretchEachBoundBy(securityMargin,
                                  k_maxFloatForAutoApproximateSolvingRange);
  }
  m_autoApproximateSolvingRange = true;
  m_approximateSolvingRange =
      Range1D<double>(static_cast<double>(finalRange.min()),
                      static_cast<double>(finalRange.max()));
}
#endif

void SystemOfEquations::approximateSolve(Context* context) {
  Internal::Tree* set = equationSet(m_store);
  assert(set->numberOfChildren() == 1);
  Internal::Tree* equation = set->child(0);
  Internal::Tree* variables = Internal::Variables::GetUserSymbols(equation);
  assert(variables->numberOfChildren() == 1);
  m_solverContext.variables.clear();
  m_solverContext.variables.append(
      Internal::Symbol::GetName(variables->child(0)));
  variables->removeTree();
  Internal::ProjectionContext ctx;
  Internal::Simplification::ToSystem(equation, &ctx);
  Internal::Approximation::PrepareFunctionForApproximation(
      equation, m_solverContext.variables.variable(0),
      Preferences::ComplexFormat::Real);
  Internal::Tree* result = Poincare::Internal::EquationSolver::ApproximateSolve(
      equation, m_approximateSolvingRange, &m_solverContext);

  assert(result && result->isList());
  // Update member variables for LinearSystem
  m_type = m_solverContext.type;
  m_degree = m_solverContext.degree;
  m_hasMoreSolutions = m_solverContext.hasMoreSolutions;
  m_numberOfSolutions = result->numberOfChildren();
  // Copy solutions
  for (int i = 0; const Internal::Tree* solution : result->children()) {
    m_solutions[i++] =
        Solution(Poincare::Layout(), Poincare::Layout(),
                 Poincare::Internal::FloatHelper::To(solution), false);
  }
  result->removeTree();
  set->removeTree();
}

#if 0
void SystemOfEquations::approximateSolve(Context* context) {
  assert(m_type == Type::GeneralMonovariable);
  assert(m_numberOfSolvingVariables == 1);

  SystemExpression undevelopedExpression =
      equationStandardFormForApproximateSolve(context);
  m_numberOfSolutions = 0;

  assert(m_approximateSolvingRange.isValid());
  double rangeMin = m_approximateSolvingRange.min();
  double rangeMax = m_approximateSolvingRange.max();
  Poincare::OSolver<double> solver =
      PoincareHelpers::OSolver(rangeMin, rangeMax, m_variables[0], context);
  solver.stretch();

  for (int i = 0; i <= k_maxNumberOfApproximateSolutions; i++) {
    double root = solver.nextRoot(undevelopedExpression).x();
    if (root < rangeMin) {
      i--;
      continue;
    } else if (root > rangeMax) {
      root = NAN;
    }

    if (i == k_maxNumberOfApproximateSolutions) {
      m_hasMoreSolutions = true;
    } else {
      if (std::isnan(root)) {
        break;
      }
      registerSolution(root);
    }
  }
}
#endif

void SystemOfEquations::tidy(PoolObject* treePoolCursor) {
  for (int i = 0; i < k_maxNumberOfSolutions; i++) {
    if (treePoolCursor == nullptr ||
        m_solutions[i].exactLayout().isDownstreamOf(treePoolCursor) ||
        m_solutions[i].approximateLayout().isDownstreamOf(treePoolCursor)) {
      m_solutions[i] = Solution();
    }
  }
}

SystemExpression SystemOfEquations::equationStandardFormForApproximateSolve(
    Context* context) {
  return m_store->modelForRecord(m_store->definedRecordAtIndex(0))
      ->standardForm(context, overrideUserVariables(),
                     ReductionTarget::SystemForApproximation);
}

#if 0
SystemOfEquations::Error SystemOfEquations::privateExactSolve(
    Context* context) {
  m_numberOfSolutions = 0;
  SystemExpression simplifiedEquations[EquationStore::k_maxNumberOfEquations];
  Error error = simplifyAndFindVariables(context, simplifiedEquations);
  if (error != Error::NoError) {
    return error;
  }
  error = solveLinearSystem(context, simplifiedEquations);
  if (error != Error::NonLinearSystem || m_numberOfSolvingVariables > 1 ||
      m_store->numberOfDefinedModels() > 1) {
    return error;
  }
  error = solvePolynomial(context, simplifiedEquations);
  if (error == Error::RequireApproximateSolution) {
    m_type = Type::GeneralMonovariable;
  }
  assert(error != Error::NoError || m_type == Type::PolynomialMonovariable);
  return error;
}

SystemOfEquations::Error SystemOfEquations::simplifyAndFindVariables(
    Context* context, SystemExpression* simplifiedEquations) {
  m_numberOfSolvingVariables = 0;
  m_numberOfUserVariables = 0;
  m_variables[0][0] = 0;
  m_userVariables[0][0] = 0;
  m_complexFormat = Preferences::SharedPreferences()->complexFormat();

  bool forbidSimultaneousEquation = Preferences::SharedPreferences()
                                        ->examMode()
                                        .forbidSimultaneousEquationSolver();

  EquationStore* store = m_store;
  int nEquations = store->numberOfDefinedModels();
  if (forbidSimultaneousEquation && nEquations > 1) {
    return Error::DisabledInExamMode;
  }
  for (int i = 0; i < nEquations; i++) {
    ExpiringPointer<Equation> equation =
        store->modelForRecord(store->definedRecordAtIndex(i));
    SystemExpression equationsWithUserVariables = equation->standardForm(
        context, true, ReductionTarget::SystemForAnalysis);

    // Gather user variables
    int nVariables = equationsWithUserVariables.getVariables(
        context,
        [](const char* s, Context* c) {
          return c->expressionTypeForIdentifier(s, strlen(s)) ==
                 Context::SymbolAbstractType::Symbol;
        },
        &m_userVariables[0][0], SymbolAbstractNode::k_maxNameSize,
        m_numberOfUserVariables);
    /* Don't abort if there are more the k_maxNumberOfVariables defined user
     * variables. */
    m_numberOfUserVariables =
        nVariables >= 0 ? nVariables : Expression::k_maxNumberOfVariables;

    simplifiedEquations[i] =
        m_overrideUserVariables
            ? equationsWithUserVariables
            : equation->standardForm(context, false,
                                     ReductionTarget::SystemForAnalysis);
    if (simplifiedEquations[i].isUninitialized() ||
        simplifiedEquations[i].isUndefined() ||
        simplifiedEquations[i].recursivelyMatches(
            NewExpression::IsMatrix, context,
            m_overrideUserVariables
                ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
                : SymbolicComputation::
                      ReplaceAllDefinedSymbolsWithDefinition)) {
      return Error::EquationUndefined;
    } else if (simplifiedEquations[i].isNonReal()) {
      return Error::EquationNonReal;
    }

    m_complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(
        m_complexFormat, simplifiedEquations[i], nullptr);

    // Gather solving variables
    int nbSolvingVariables = simplifiedEquations[i].getVariables(
        context, [](const char*, Context*) { return true; }, &m_variables[0][0],
        SymbolAbstractNode::k_maxNameSize, m_numberOfSolvingVariables);
    /* The equation has been parsed, so there should not be any variable with a
     * name that is too long. */
    // FIXME Special return values of getVariables should be named.
    assert(nbSolvingVariables != -2);
    if (nbSolvingVariables == -1) {
      return Error::TooManyVariables;
    }
    m_numberOfSolvingVariables = nbSolvingVariables;
  }

  if (forbidSimultaneousEquation && m_numberOfSolvingVariables > 1) {
    return Error::DisabledInExamMode;
  }
  return Error::NoError;
}

SystemOfEquations::Error SystemOfEquations::solveLinearSystem(
    Context* context, SystemExpression* simplifiedEquations) {
  Preferences::AngleUnit angleUnit =
      Preferences::SharedPreferences()->angleUnit();
  Preferences::UnitFormat unitFormat =
      GlobalPreferences::SharedGlobalPreferences()->unitFormat();
  SymbolicComputation symbolicComputation =
      m_overrideUserVariables
          ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
          : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  SystemExpression coefficients[EquationStore::k_maxNumberOfEquations]
                               [Expression::k_maxNumberOfVariables];
  SystemExpression constants[EquationStore::k_maxNumberOfEquations];
  const int numberOfOriginalEquations = m_store->numberOfDefinedModels();
  int m = numberOfOriginalEquations;
  for (int i = 0; i < m; i++) {
    bool isLinear = simplifiedEquations[i].getLinearCoefficients(
        &m_variables[0][0], SymbolAbstractNode::k_maxNameSize, coefficients[i],
        &constants[i], context, m_complexFormat, angleUnit, unitFormat,
        symbolicComputation);
    if (!isLinear) {
      return Error::NonLinearSystem;
    }
  }
  m_degree = 1;
  m_type = Type::LinearSystem;

  m_hasMoreSolutions = false;
  // n unknown variables and m equations
  int n = m_numberOfSolvingVariables;
  // Create the matrix (A|b) for the equation Ax=b;
  Matrix ab = Matrix::Builder();
  int abChildren = 0;
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      ab.addChildAtIndexInPlace(coefficients[i][j], abChildren, abChildren);
      ++abChildren;
    }
    ab.addChildAtIndexInPlace(constants[i], abChildren, abChildren);
    ++abChildren;
  }
  ab.setDimensions(m, n + 1);

  assert(!ab.recursivelyMatches(NewExpression::IsUninitialized, context));

  // Compute the rank of (A|b)
  int rank = ab.rank(context);
  if (rank == -1) {
    return Error::EquationUndefined;
  }

  for (int row = 0; row < m; row++) {
    if (ab.matrixChild(row, n).isNull(context) == OMG::Troolean::True) {
      continue;
    }
    bool allCoefficientsNull = true;
    for (int col = 0; allCoefficientsNull && col < n; col++) {
      if (ab.matrixChild(row, col).isNull(context) != OMG::Troolean::True) {
        allCoefficientsNull = false;
      }
    }
    if (allCoefficientsNull) {
      /* Row j describes an equation of the form '0=b', the system has no
       * solution. */
      m_numberOfSolutions = 0;
      return Error::NoError;
    }
  }

  /* Use a context without t to avoid replacing the t? parameters with a value
   * if the user stored something in them but they are not used by the
   * system.
   * It is declared here as it needs to be accessible when registering the
   * solutions at the end. */
  ContextWithoutT noTContext(context);

  if (rank != n || n <= 0) {
    /* The system is insufficiently qualified: bind the value of n-rank
     * variables to parameters. */
    m_hasMoreSolutions = true;

    context = &noTContext;

    // 't' + 2 digits + '\0'
    constexpr size_t parameterNameSize = 1 + 2 + 1;
    char parameterName[parameterNameSize] = {k_parameterPrefix};
    size_t parameterIndex = n - rank == 1 ? 0 : 1;
    uint32_t usedParameterIndices = tagParametersUsedAsVariables();

    int variable = n - 1;
    int row = m - 1;
    int firstVariableInRow = -1;
    while (variable >= 0) {
      // Find the first variable with a non-null coefficient in the current row
      if (row >= 0) {
        for (int col = 0; firstVariableInRow < 0 && col < n; col++) {
          if (ab.matrixChild(row, col).isNull(context) != OMG::Troolean::True) {
            firstVariableInRow = col;
          }
        }

        if (firstVariableInRow < 0 || firstVariableInRow == variable) {
          /* If firstVariableInRow < 0, the row is null and provides no
           * information. If variable is the first with a non-null coefficient,
           * the current row uniquely qualifies it, no need to bind a parameter
           * to it. */
          row--;
          if (firstVariableInRow == variable) {
            variable--;
          }
          firstVariableInRow = -1;
          continue;
        }
      }
      /* If row < 0, there are still unbound variables after scanning all the
       * row, so simply bind them all. */

      assert(firstVariableInRow < variable);
      /* No row uniquely qualifies the current variable, bind it to a parameter.
       * Add the row variable=parameter to increase the rank of the system. */
      for (int i = 0; i < n; i++) {
        ab.addChildAtIndexInPlace(Rational::Builder(i == variable ? 1 : 0),
                                  abChildren, abChildren);
        ++abChildren;
      }

      // Generate a unique identifier t? that does not collide with variables.
      while (OMG::BitHelper::bitAtIndex(usedParameterIndices, parameterIndex)) {
        parameterIndex++;
        assert(parameterIndex <
               OMG::BitHelper::numberOfBitsIn(usedParameterIndices));
      }
      size_t parameterNameLength =
          parameterIndex == 0
              ? 1
              : 1 + OMG::Print::IntLeft(parameterIndex, parameterName + 1,
                                        parameterNameSize - 2);
      parameterIndex++;
      assert(parameterNameLength >= 1 &&
             parameterNameLength < parameterNameSize);
      parameterName[parameterNameLength] = 0;
      ab.addChildAtIndexInPlace(
          Symbol::Builder(parameterName, parameterNameLength), abChildren,
          abChildren);
      ++abChildren;
      ab.setDimensions(++m, n + 1);
      variable--;
    }

    /* forceCanonization = true so that canonization still happens even if
     * t.approximate() is NAN. If other children of ab have an undef
     * approximation, the previous rank computation would already have returned
     * -1. */
    rank = ab.rank(context, true);
    if (rank == -1) {
      return Error::EquationUndefined;
    }
  }
  assert(rank == n);

  // System is fully qualified, register the solutions.
  m_numberOfSolutions = 0;

  // Make sure the solution satisfies dependencies in equations
  VariableContext solutionContexts[k_maxNumberOfSolutions];
  for (int i = 0; i < n; i++) {
    solutionContexts[i] = VariableContext(
        variable(i), i == 0 ? context : &solutionContexts[i - 1]);
    solutionContexts[i].setExpressionForSymbolAbstract(
        ab.matrixChild(i, n),
        Symbol::Builder(variable(i), strlen(variable(i))));
  }
  ReductionContext reductionContextWithSolutions(
      &solutionContexts[n - 1], m_complexFormat,
      Preferences::SharedPreferences()->angleUnit(),
      GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
      ReductionTarget::SystemForAnalysis);
  for (int i = 0; i < numberOfOriginalEquations; i++) {
    if (!simplifiedEquations[i].isDep()) {
      continue;
    }
    if (simplifiedEquations[i]
            .cloneAndReduce(reductionContextWithSolutions)
            .isUndefined()) {
      return Error::NoError;
    }
  }

  SolutionType solutionType =
      m_hasMoreSolutions ? SolutionType::Formal : SolutionType::Exact;
  for (int i = 0; i < n; i++) {
    Error error = registerSolution(ab.matrixChild(i, n), context, solutionType);
    if (error != Error::NoError) {
      return error;
    }
  }
  return Error::NoError;
}

SystemOfEquations::Error SystemOfEquations::solvePolynomial(
    Context* context, SystemExpression* simplifiedEquations) {
  assert(m_numberOfSolvingVariables == 1 &&
         m_store->numberOfDefinedModels() == 1);
  Preferences::AngleUnit angleUnit =
      Preferences::SharedPreferences()->angleUnit();
  Preferences::UnitFormat unitFormat =
      GlobalPreferences::SharedGlobalPreferences()->unitFormat();
  SymbolicComputation symbolicComputation =
      m_overrideUserVariables
          ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
          : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  SystemExpression
      coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  m_degree = simplifiedEquations[0].getPolynomialReducedCoefficients(
      m_variables[0], coefficients, context, m_complexFormat, angleUnit,
      unitFormat, symbolicComputation);
  if (m_degree < 2 || m_degree > 3) {
    return Error::RequireApproximateSolution;
  }

  m_type = Type::PolynomialMonovariable;
  ReductionContext reductionContext(context, m_complexFormat, angleUnit,
                                    unitFormat, ReductionTarget::User);
  SystemExpression delta;
  SystemExpression x[3];
  bool solutionsAreApproximate = false;
  size_t numberOfSolutions = 0;
  if (m_degree == 2) {
    numberOfSolutions = Poincare::Polynomial::QuadraticPolynomialRoots(
        coefficients[2], coefficients[1], coefficients[0], x, x + 1, &delta,
        reductionContext, &solutionsAreApproximate);
  } else {
    assert(m_degree == 3);
    numberOfSolutions = Poincare::Polynomial::CubicPolynomialRoots(
        coefficients[3], coefficients[2], coefficients[1], coefficients[0], x,
        x + 1, x + 2, &delta, reductionContext, &solutionsAreApproximate);
  }
  SolutionType type =
      solutionsAreApproximate ? SolutionType::Approximate : SolutionType::Exact;

  for (size_t i = 0; i < numberOfSolutions; i++) {
    /* Since getPolynomialReducedCoefficients passes right through dependencies,
     * we need to handle them now. */
    if (simplifiedEquations[0].isDep()) {
      VariableContext contextWithSolution(variable(0), context);
      contextWithSolution.setExpressionForSymbolAbstract(
          x[i], Symbol::Builder(variable(0), strlen(variable(0))));
      ReductionContext reductionContextWithSolution = reductionContext;
      reductionContextWithSolution.setContext(&contextWithSolution);
      if (simplifiedEquations[0]
              .cloneAndReduce(reductionContextWithSolution)
              .isUndefined()) {
        continue;
      }
    }

    Error error = registerSolution(x[i], context, type);
    // Ignore EquationNonreal error on solutions since delta may still be real.
    if (error != Error::NoError && error != Error::EquationNonReal) {
      return error;
    }
  }
  // Account for delta
  return registerSolution(delta, context, type);
}

static void simplifyAndApproximateSolution(
    UserExpression e, UserExpression* exact, UserExpression* approximate,
    bool approximateDuringReduction, Context* context,
    Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit,
    Preferences::UnitFormat unitFormat,
    SymbolicComputation symbolicComputation) {
  ReductionContext reductionContext = ReductionContext(
      context, complexFormat, angleUnit, unitFormat, ReductionTarget::User,
      symbolicComputation, UnitConversion::Default);
  e.cloneAndSimplifyAndApproximate(exact, approximate, reductionContext,
                                   approximateDuringReduction);
  if (exact->isDep()) {
    /* e has been reduced under ReductionTarget::SystemForAnalysis in
     * Equation::Model::standardForm and has gone through Matrix::rank,
     * which discarded dependencies. Reducing here under
     * ReductionTarget::User may have created new dependencies. For example,
     * "i" had been preserved up to now and has been reduced to a
     * ComplexCartesian here, which may have triggered further reduction and
     * the creation of a dependency.
     * We remove that dependency in order to create layouts. */
    *exact = exact->cloneChildAtIndex(0);
  }
}

SystemOfEquations::Error SystemOfEquations::registerSolution(
    UserExpression e, Context* context, SolutionType type) {
  Preferences::AngleUnit angleUnit =
      Preferences::SharedPreferences()->angleUnit();
  UserExpression exact, approximate;

  bool forbidExactSolution =
      Preferences::SharedPreferences()->examMode().forbidExactResults();
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
    Preferences::UnitFormat unitFormat =
        GlobalPreferences::SharedGlobalPreferences()->unitFormat();
    SymbolicComputation symbolicComputation =
        m_solverContext.overrideUserVariables
            ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
            : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
    simplifyAndApproximateSolution(
        e, &exact, &approximate, approximateDuringReduction, context,
        m_complexFormat, angleUnit, unitFormat, symbolicComputation);
    displayExactSolution =
        approximateDuringReduction ||
        (!forbidExactSolution &&
         !CAS::ShouldOnlyDisplayApproximation(e, exact, approximate, context));
    displayApproximateSolution = type != SolutionType::Formal;
    if (!displayApproximateSolution && !displayExactSolution) {
      /* Happens if the formal solution has no permission to be displayed.
       * Re-reduce but force approximating during redution. */
      exact = UserExpression();
      approximate = UserExpression();
      simplifyAndApproximateSolution(e, &exact, &approximate, true, context,
                                     m_complexFormat, angleUnit, unitFormat,
                                     symbolicComputation);
      displayExactSolution = true;
    }
  }
  if (approximate.isNonReal()) {
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
    exactLayout.serialize(exactBuffer, ::Constant::MaxSerializedExpressionSize);
    approximateLayout.serialize(approximateBuffer,
                                ::Constant::MaxSerializedExpressionSize);
    Internal::ProjectionContext ctx;  // TODO: pass arguments
    if (strcmp(exactBuffer, approximateBuffer) == 0) {
      exactLayout = Layout();
    } else if (Poincare::ExactAndApproximateExpressionsAreStrictlyEqual(
                   exact, approximate, &ctx)) {
      exactAndApproximateAreEqual = true;
    }
  }

  assert(m_numberOfSolutions < k_maxNumberOfSolutions - 1);
  m_solutions[m_numberOfSolutions++] = Solution(
      exactLayout, approximateLayout, NAN, exactAndApproximateAreEqual);

  return Error::NoError;
}

void SystemOfEquations::registerSolution(double f) {
  if (std::isfinite(f)) {
    m_solutions[m_numberOfSolutions++] = Solution(Layout(), Layout(), f, false);
  }
}

uint32_t SystemOfEquations::tagParametersUsedAsVariables() const {
  uint32_t tags = 0;
  for (size_t i = 0; i < m_numberOfSolvingVariables; i++) {
    tagVariableIfParameter(variable(i), &tags);
  }
  for (size_t i = 0; i < m_numberOfUserVariables; i++) {
    tagVariableIfParameter(userVariable(i), &tags);
  }
  return tags;
}

void SystemOfEquations::tagVariableIfParameter(const char* variable,
                                               uint32_t* tags) const {
  if (variable[0] != k_parameterPrefix) {
    return;
  }
  if (variable[1] == '\0') {
    OMG::BitHelper::setBitAtIndex(*tags, 0, true);
    return;
  }
  size_t maxIndex = OMG::BitHelper::numberOfBitsIn(*tags);
  size_t maxNumberOfDigits =
      OMG::Print::LengthOfUInt32(OMG::Base::Decimal, maxIndex);
  size_t index = 0;
  for (size_t digit = 1; digit < 1 + maxNumberOfDigits &&
                         '0' <= variable[digit] && variable[digit] <= '9';
       digit++) {
    index = 10 * index + (variable[digit] - '0');
  }
  if (index > 0 && index < maxIndex) {
    OMG::BitHelper::setBitAtIndex(*tags, index, true);
  }
}
#endif

}  // namespace Solver
