#include "system_of_equations.h"

#include <apps/constant.h>
#include <apps/global_preferences.h>
#include <apps/shared/expression_display_permissions.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/matrix.h>
#include <poincare/polynomial.h>
#include <poincare/print_int.h>
#include <poincare/symbol.h>
#include <poincare/zoom.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/list.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/n_ary.h>

#include "app.h"
#include "poincare/empty_context.h"
#include "poincare/variable_context.h"

using namespace Shared;

namespace Solver {

SystemOfEquations::Error SystemOfEquations::exactSolve(
    Poincare::Context *context) {
  PoincareJ::Solver::Context pcjContext;
  Error error = Error::NoError;

  // Copy equations on the EditionPool
  PoincareJ::Tree *equations = PoincareJ::List::PushEmpty();
  int nEquations = m_store->numberOfDefinedModels();
  for (int i = 0; i < nEquations; i++) {
    ExpiringPointer<Equation> equation =
        m_store->modelForRecord(m_store->definedRecordAtIndex(i));
    Poincare::Expression equationExpression = equation->expressionModelClone();
    PoincareJ::NAry::AddChild(
        equations,
        PoincareJ::Expression::FromPoincareExpression(equationExpression));
  }

  PoincareJ::Tree *result =
      PoincareJ::Solver::ExactSolve(equations, &pcjContext, &error);

  if (error == Error::NoError) {
    assert(result);
    // Update member variables for LinearSystem
    m_type = Type::LinearSystem;
    m_degree = 1;
    m_hasMoreSolutions = false;
    m_numberOfSolutions = result->numberOfChildren();
    m_numberOfSolvingVariables = m_numberOfSolutions;
    m_overrideUserVariables = pcjContext.overrideUserVariables;
    m_numberOfUserVariables = pcjContext.numberOfUserVariables;
    // Copy user variables
    memcpy(m_userVariables, pcjContext.userVariables, sizeof(char[6][10]));
    // Copy solutions
    for (int i = 0; const PoincareJ::Tree *solution : result->children()) {
      Poincare::Expression exact =
          PoincareJ::Expression::ToPoincareExpression(solution);
      Poincare::Layout exactLayout =
          PoincareHelpers::CreateLayout(exact, context);
      m_solutions[i++] = Solution(exactLayout, Poincare::Layout(), NAN, true);
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
  equations->removeTree();
  return error;
}

#if 0
const Poincare::Expression
SystemOfEquations::ContextWithoutT::protectedExpressionForSymbolAbstract(
    const Poincare::SymbolAbstract &symbol, bool clone,
    ContextWithParent *lastDescendantContext) {
  if (symbol.type() == Poincare::ExpressionNode::Type::Symbol &&
      static_cast<const Poincare::Symbol &>(symbol).name()[0] == 't') {
    return Poincare::Expression();
  }
  return ContextWithParent::protectedExpressionForSymbolAbstract(
      symbol, clone, lastDescendantContext);
}

SystemOfEquations::Error SystemOfEquations::exactSolve(
    Poincare::Context *context) {
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
static Poincare::Coordinate2D<T> evaluator(T t, const void *model,
                                           Poincare::Context *context) {
  void **modelArray = reinterpret_cast<void **>(const_cast<void *>(model));
  const Poincare::Expression *e =
      reinterpret_cast<const Poincare::Expression *>(modelArray[0]);
  const char *variable = reinterpret_cast<const char *>(modelArray[1]);
  return Poincare::Coordinate2D<T>(
      t, e->approximateToScalarWithValueForSymbol<T>(
             variable, t,
             Poincare::ApproximationContext(
                 context,
                 Poincare::Preferences::SharedPreferences()->complexFormat(),
                 Poincare::Preferences::SharedPreferences()->angleUnit())));
}

void SystemOfEquations::setApproximateSolvingRange(
    Poincare::Range1D<double> approximateSolvingRange) {
  m_autoApproximateSolvingRange = false;
  m_hasMoreSolutions = false;
  m_approximateSolvingRange = approximateSolvingRange;
}

void SystemOfEquations::autoComputeApproximateSolvingRange(
    Poincare::Context *context) {
  // TODO: Handle approximate solve
  assert(false);
#if 0
  Poincare::Expression equationStandardForm =
      equationStandardFormForApproximateSolve(context);
  constexpr static float k_maxFloatForAutoApproximateSolvingRange = 1e15f;
  Poincare::Zoom zoom(NAN, NAN, InteractiveCurveViewRange::NormalYXRatio(),
                      context, k_maxFloatForAutoApproximateSolvingRange);
  // Use the intersection between the definition domain of f and the bounds
  zoom.setBounds(-k_maxFloatForAutoApproximateSolvingRange,
                 k_maxFloatForAutoApproximateSolvingRange);
  zoom.setMaxPointsOneSide(k_maxNumberOfApproximateSolutions,
                           k_maxNumberOfApproximateSolutions / 2);
  void *model[2] = {static_cast<void *>(&equationStandardForm),
                    static_cast<void *>(m_variables[0])};
  bool finiteNumberOfSolutions = true;
  bool didFitRoots =
      zoom.fitRoots(evaluator<float>, static_cast<void *>(model), false,
                    evaluator<double>, &finiteNumberOfSolutions);
  /* When there are more than k_maxNumberOfApproximateSolutions on one side of
   * 0, the zoom is setting the interval to have a maximum of 5 solutions left
   * of 0 and 5 solutions right of zero. This means that sometimes, for a
   * function like `piecewise(1, x<0; cos(x), x >= 0)`, only 5 solutions will be
   * displayed. We still want to notify the user that more solutions exist. */
  m_hasMoreSolutions = !finiteNumberOfSolutions;
  zoom.fitBounds(evaluator<float>, static_cast<void *>(model), false);
  Poincare::Range1D<float> finalRange = *(zoom.range(false, false).x());
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
      Poincare::Range1D<double>(static_cast<double>(finalRange.min()),
                                static_cast<double>(finalRange.max()));
#endif
}

void SystemOfEquations::approximateSolve(Poincare::Context *context) {
  // TODO: Handle approximate solve
  assert(false);
#if 0
  assert(m_type == Type::GeneralMonovariable);
  assert(m_numberOfSolvingVariables == 1);

  Poincare::Expression undevelopedExpression =
      equationStandardFormForApproximateSolve(context);
  m_numberOfSolutions = 0;

  assert(m_approximateSolvingRange.isValid());
  double rangeMin = m_approximateSolvingRange.min();
  double rangeMax = m_approximateSolvingRange.max();
  Poincare::Solver<double> solver =
      PoincareHelpers::Solver(rangeMin, rangeMax, m_variables[0], context);
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
#endif
}

void SystemOfEquations::tidy(Poincare::TreeNode *treePoolCursor) {
  for (int i = 0; i < k_maxNumberOfSolutions; i++) {
    if (treePoolCursor == nullptr ||
        m_solutions[i].exactLayout().isDownstreamOf(treePoolCursor) ||
        m_solutions[i].approximateLayout().isDownstreamOf(treePoolCursor)) {
      m_solutions[i] = Solution();
    }
  }
}

#if 0
Poincare::Expression SystemOfEquations::equationStandardFormForApproximateSolve(
    Poincare::Context *context) {
  return m_store->modelForRecord(m_store->definedRecordAtIndex(0))
      ->standardForm(context, m_overrideUserVariables,
                     Poincare::ReductionTarget::SystemForApproximation);
}

SystemOfEquations::Error SystemOfEquations::privateExactSolve(
    Poincare::Context *context) {
  m_numberOfSolutions = 0;
  Poincare::Expression
      simplifiedEquations[EquationStore::k_maxNumberOfEquations];
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
    Poincare::Context *context, Poincare::Expression *simplifiedEquations) {
  m_numberOfSolvingVariables = 0;
  m_numberOfUserVariables = 0;
  m_variables[0][0] = 0;
  m_userVariables[0][0] = 0;
  m_complexFormat = Poincare::Preferences::SharedPreferences()->complexFormat();

  bool forbidSimultaneousEquation = Poincare::Preferences::SharedPreferences()
                                        ->examMode()
                                        .forbidSimultaneousEquationSolver();

  EquationStore *store = m_store;
  int nEquations = store->numberOfDefinedModels();
  if (forbidSimultaneousEquation && nEquations > 1) {
    return Error::DisabledInExamMode;
  }
  for (int i = 0; i < nEquations; i++) {
    ExpiringPointer<Equation> equation =
        store->modelForRecord(store->definedRecordAtIndex(i));
    Poincare::Expression equationsWithUserVariables = equation->standardForm(
        context, true, Poincare::ReductionTarget::SystemForAnalysis);

    // Gather user variables
    int nVariables = equationsWithUserVariables.getVariables(
        context,
        [](const char *s, Poincare::Context *c) {
          return c->expressionTypeForIdentifier(s, strlen(s)) ==
                 Poincare::Context::SymbolAbstractType::Symbol;
        },
        &m_userVariables[0][0], Poincare::SymbolAbstractNode::k_maxNameSize,
        m_numberOfUserVariables);
    /* Don't abort if there are more the k_maxNumberOfVariables defined user
     * variables. */
    m_numberOfUserVariables =
        nVariables >= 0 ? nVariables
                        : Poincare::Expression::k_maxNumberOfVariables;

    simplifiedEquations[i] =
        m_overrideUserVariables
            ? equationsWithUserVariables
            : equation->standardForm(
                  context, false, Poincare::ReductionTarget::SystemForAnalysis);
    if (simplifiedEquations[i].isUninitialized() ||
        simplifiedEquations[i].type() ==
            Poincare::ExpressionNode::Type::Undefined ||
        simplifiedEquations[i].recursivelyMatches(
            Poincare::Expression::IsMatrix, context,
            m_overrideUserVariables
                ? Poincare::SymbolicComputation::
                      ReplaceDefinedFunctionsWithDefinitions
                : Poincare::SymbolicComputation::
                      ReplaceAllDefinedSymbolsWithDefinition)) {
      return Error::EquationUndefined;
    } else if (simplifiedEquations[i].type() ==
               Poincare::ExpressionNode::Type::Nonreal) {
      return Error::EquationNonreal;
    }

    m_complexFormat =
        Poincare::Preferences::UpdatedComplexFormatWithExpressionInput(
            m_complexFormat, simplifiedEquations[i], nullptr);

    // Gather solving variables
    int nbSolvingVariables = simplifiedEquations[i].getVariables(
        context, [](const char *, Poincare::Context *) { return true; },
        &m_variables[0][0], Poincare::SymbolAbstractNode::k_maxNameSize,
        m_numberOfSolvingVariables);
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
    Poincare::Context *context, Poincare::Expression *simplifiedEquations) {
  Poincare::Preferences::AngleUnit angleUnit =
      Poincare::Preferences::SharedPreferences()->angleUnit();
  Poincare::Preferences::UnitFormat unitFormat =
      GlobalPreferences::SharedGlobalPreferences()->unitFormat();
  Poincare::SymbolicComputation symbolicComputation =
      m_overrideUserVariables ? Poincare::SymbolicComputation::
                                    ReplaceDefinedFunctionsWithDefinitions
                              : Poincare::SymbolicComputation::
                                    ReplaceAllDefinedSymbolsWithDefinition;
  Poincare::Expression
      coefficients[EquationStore::k_maxNumberOfEquations]
                  [Poincare::Expression::k_maxNumberOfVariables];
  Poincare::Expression constants[EquationStore::k_maxNumberOfEquations];
  const int numberOfOriginalEquations = m_store->numberOfDefinedModels();
  int m = numberOfOriginalEquations;
  for (int i = 0; i < m; i++) {
    bool isLinear = simplifiedEquations[i].getLinearCoefficients(
        &m_variables[0][0], Poincare::SymbolAbstractNode::k_maxNameSize,
        coefficients[i], &constants[i], context, m_complexFormat, angleUnit,
        unitFormat, symbolicComputation);
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
  Poincare::Matrix ab = Poincare::Matrix::Builder();
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

  assert(
      !ab.recursivelyMatches(Poincare::Expression::IsUninitialized, context));

  // Compute the rank of (A|b)
  int rank = ab.rank(context);
  if (rank == -1) {
    return Error::EquationUndefined;
  }

  for (int row = 0; row < m; row++) {
    if (ab.matrixChild(row, n).isNull(context) ==
        Poincare::TrinaryBoolean::True) {
      continue;
    }
    bool allCoefficientsNull = true;
    for (int col = 0; allCoefficientsNull && col < n; col++) {
      if (ab.matrixChild(row, col).isNull(context) !=
          Poincare::TrinaryBoolean::True) {
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
          if (ab.matrixChild(row, col).isNull(context) !=
              Poincare::TrinaryBoolean::True) {
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
        ab.addChildAtIndexInPlace(
            Poincare::Rational::Builder(i == variable ? 1 : 0), abChildren,
            abChildren);
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
              : 1 + Poincare::PrintInt::Left(parameterIndex, parameterName + 1,
                                             parameterNameSize - 2);
      parameterIndex++;
      assert(parameterNameLength >= 1 &&
             parameterNameLength < parameterNameSize);
      parameterName[parameterNameLength] = 0;
      ab.addChildAtIndexInPlace(
          Poincare::Symbol::Builder(parameterName, parameterNameLength),
          abChildren, abChildren);
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
  Poincare::VariableContext solutionContexts[k_maxNumberOfSolutions];
  for (int i = 0; i < n; i++) {
    solutionContexts[i] = Poincare::VariableContext(
        variable(i), i == 0 ? context : &solutionContexts[i - 1]);
    solutionContexts[i].setExpressionForSymbolAbstract(
        ab.matrixChild(i, n),
        Poincare::Symbol::Builder(variable(i), strlen(variable(i))));
  }
  Poincare::ReductionContext reductionContextWithSolutions(
      &solutionContexts[n - 1], m_complexFormat,
      Poincare::Preferences::SharedPreferences()->angleUnit(),
      GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
      Poincare::ReductionTarget::SystemForAnalysis);
  for (int i = 0; i < numberOfOriginalEquations; i++) {
    if (simplifiedEquations[i].type() !=
        Poincare::ExpressionNode::Type::Dependency) {
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
    Poincare::Context *context, Poincare::Expression *simplifiedEquations) {
  assert(m_numberOfSolvingVariables == 1 &&
         m_store->numberOfDefinedModels() == 1);
  Poincare::Preferences::AngleUnit angleUnit =
      Poincare::Preferences::SharedPreferences()->angleUnit();
  Poincare::Preferences::UnitFormat unitFormat =
      GlobalPreferences::SharedGlobalPreferences()->unitFormat();
  Poincare::SymbolicComputation symbolicComputation =
      m_overrideUserVariables ? Poincare::SymbolicComputation::
                                    ReplaceDefinedFunctionsWithDefinitions
                              : Poincare::SymbolicComputation::
                                    ReplaceAllDefinedSymbolsWithDefinition;
  Poincare::Expression
      coefficients[Poincare::Expression::k_maxNumberOfPolynomialCoefficients];
  m_degree = simplifiedEquations[0].getPolynomialReducedCoefficients(
      m_variables[0], coefficients, context, m_complexFormat, angleUnit,
      unitFormat, symbolicComputation);
  if (m_degree < 2 || m_degree > 3) {
    return Error::RequireApproximateSolution;
  }

  m_type = Type::PolynomialMonovariable;
  Poincare::ReductionContext reductionContext(context, m_complexFormat,
                                              angleUnit, unitFormat,
                                              Poincare::ReductionTarget::User);
  Poincare::Expression delta;
  Poincare::Expression x[3];
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
    if (simplifiedEquations[0].type() ==
        Poincare::ExpressionNode::Type::Dependency) {
      Poincare::VariableContext contextWithSolution(variable(0), context);
      contextWithSolution.setExpressionForSymbolAbstract(
          x[i], Poincare::Symbol::Builder(variable(0), strlen(variable(0))));
      Poincare::ReductionContext reductionContextWithSolution =
          reductionContext;
      reductionContextWithSolution.setContext(&contextWithSolution);
      if (simplifiedEquations[0]
              .cloneAndReduce(reductionContextWithSolution)
              .isUndefined()) {
        continue;
      }
    }

    Error error = registerSolution(x[i], context, type);
    // Ignore EquationNonreal error on solutions since delta may still be real.
    if (error != Error::NoError && error != Error::EquationNonreal) {
      return error;
    }
  }
  // Account for delta
  return registerSolution(delta, context, type);
}

static void simplifyAndApproximateSolution(
    Poincare::Expression e, Poincare::Expression *exact,
    Poincare::Expression *approximate, bool approximateDuringReduction,
    Poincare::Context *context,
    Poincare::Preferences::ComplexFormat complexFormat,
    Poincare::Preferences::AngleUnit angleUnit,
    Poincare::Preferences::UnitFormat unitFormat,
    Poincare::SymbolicComputation symbolicComputation) {
  Poincare::ReductionContext reductionContext = Poincare::ReductionContext(
      context, complexFormat, angleUnit, unitFormat,
      Poincare::ReductionTarget::User, symbolicComputation,
      Poincare::UnitConversion::Default);
  e.cloneAndSimplifyAndApproximate(exact, approximate, reductionContext,
                                   approximateDuringReduction);
  if (exact->type() == Poincare::ExpressionNode::Type::Dependency) {
    /* e has been reduced under ReductionTarget::SystemForAnalysis in
     * Equation::Model::standardForm and has gone through Matrix::rank,
     * which discarded dependencies. Reducing here under
     * ReductionTarget::User may have created new dependencies. For example,
     * "i" had been preserved up to now and has been reduced to a
     * ComplexCartesian here, which may have triggered further reduction and
     * the creation of a dependency.
     * We remove that dependency in order to create layouts. */
    *exact = exact->childAtIndex(0);
  }
}

SystemOfEquations::Error SystemOfEquations::registerSolution(
    Poincare::Expression e, Poincare::Context *context, SolutionType type) {
  Poincare::Preferences::AngleUnit angleUnit =
      Poincare::Preferences::SharedPreferences()->angleUnit();
  Poincare::Expression exact, approximate;

  bool forbidExactSolution = Poincare::Preferences::SharedPreferences()
                                 ->examMode()
                                 .forbidExactResults();
  EquationStore *store = m_store;
  int nEquations = store->numberOfDefinedModels();
  int i = 0;
  while (i < nEquations && !forbidExactSolution) {
    ExpiringPointer<Equation> equation =
        store->modelForRecord(store->definedRecordAtIndex(i));
    if (ExpressionDisplayPermissions::NeverDisplayReductionOfInput(
            equation->expressionClone(), context)) {
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
    Poincare::Preferences::UnitFormat unitFormat =
        GlobalPreferences::SharedGlobalPreferences()->unitFormat();
    Poincare::SymbolicComputation symbolicComputation =
        m_overrideUserVariables ? Poincare::SymbolicComputation::
                                      ReplaceDefinedFunctionsWithDefinitions
                                : Poincare::SymbolicComputation::
                                      ReplaceAllDefinedSymbolsWithDefinition;
    simplifyAndApproximateSolution(
        e, &exact, &approximate, approximateDuringReduction, context,
        m_complexFormat, angleUnit, unitFormat, symbolicComputation);
    displayExactSolution =
        approximateDuringReduction ||
        (!forbidExactSolution &&
         !ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
             e, exact, approximate, context));
    displayApproximateSolution = type != SolutionType::Formal;
    if (!displayApproximateSolution && !displayExactSolution) {
      /* Happens if the formal solution has no permission to be displayed.
       * Re-reduce but force approximating during redution. */
      exact = Poincare::Expression();
      approximate = Poincare::Expression();
      simplifyAndApproximateSolution(e, &exact, &approximate, true, context,
                                     m_complexFormat, angleUnit, unitFormat,
                                     symbolicComputation);
      displayExactSolution = true;
    }
  }
  if (approximate.type() == Poincare::ExpressionNode::Type::Nonreal) {
    return Error::EquationNonreal;
  }
  if (type != SolutionType::Formal &&
      approximate.type() == Poincare::ExpressionNode::Type::Undefined) {
    return Error::EquationUndefined;
  }

  Poincare::Layout exactLayout, approximateLayout;
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
    exactLayout.serializeForParsing(exactBuffer,
                                    ::Constant::MaxSerializedExpressionSize);
    approximateLayout.serializeForParsing(
        approximateBuffer, ::Constant::MaxSerializedExpressionSize);
    if (strcmp(exactBuffer, approximateBuffer) == 0) {
      exactLayout = Poincare::Layout();
    } else if (Poincare::Expression::ExactAndApproximateExpressionsAreEqual(
                   exact, approximate)) {
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
    m_solutions[m_numberOfSolutions++] =
        Solution(Poincare::Layout(), Poincare::Layout(), f, false);
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

void SystemOfEquations::tagVariableIfParameter(const char *variable,
                                               uint32_t *tags) const {
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
