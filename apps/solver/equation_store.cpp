#include <apps/constant.h>
#include <apps/global_preferences.h>
#include <apps/shared/expression_display_permissions.h>
#include <apps/shared/poincare_helpers.h>
#include <apps/solver/equation_store.h>
#include <poincare/matrix.h>
#include <poincare/polynomial.h>

using namespace Poincare;
using namespace Shared;

namespace Solver {

EquationStore::Error EquationStore::exactSolve(Context *context) {
  m_overrideUserVariables = false;
  Error firstError = privateExactSolve(context);
  if (firstError == Error::NoError && m_numberOfSolutions > 0 &&
      m_numberOfSolutions < INT_MAX) {
    return firstError;
  }
  size_t firstNumberOfSolutions = m_numberOfSolutions;

  m_overrideUserVariables = true;
  Error secondError = privateExactSolve(context);
  if (firstError == Error::NoError && secondError != Error::NoError &&
      secondError != Error::RequireApproximateSolution) {
    /* The system becomes invalid when overriding the user variables: the first
     * solution was better. */
    m_numberOfSolutions = firstNumberOfSolutions;
    return firstError;
  }
  return secondError;
}

void EquationStore::approximateSolve(Context *context) {
  assert(m_type == Type::GeneralMonovariable);
  assert(m_numberOfResolutionVariables == 1);

  m_hasMoreApproximateSolutions = false;
  Expression undevelopedExpression =
      modelForRecord(definedRecordAtIndex(0))
          ->standardForm(context, m_overrideUserVariables,
                         ReductionTarget::SystemForApproximation);
  m_numberOfSolutions = 0;

  assert(m_approximateResolutionMinimum <= m_approximateResolutionMaximum);
  Poincare::Solver<double> solver = PoincareHelpers::Solver(
      m_approximateResolutionMinimum, m_approximateResolutionMaximum,
      m_variables[0], context);
  solver.stretch();

  for (size_t i = 0; i <= k_maxNumberOfApproximateSolutions; i++) {
    double root = solver.nextRoot(undevelopedExpression).x1();
    if (root < m_approximateResolutionMinimum) {
      i--;
      continue;
    } else if (root > m_approximateResolutionMaximum) {
      root = NAN;
    }

    if (i == k_maxNumberOfApproximateSolutions) {
      m_hasMoreApproximateSolutions = true;
    } else {
      if (std::isnan(root)) {
        break;
      }
      registerSolution(root);
    }
  }
}

void EquationStore::tidyDownstreamPoolFrom(char *treePoolCursor) {
  ExpressionModelStore::tidyDownstreamPoolFrom(treePoolCursor);
  for (int i = 0; i < k_maxNumberOfExactSolutions; i++) {
    if (treePoolCursor == nullptr ||
        m_solutions[i].exactLayout().isDownstreamOf(treePoolCursor)) {
      m_solutions[i] = Solution();
    }
  }
}

Ion::Storage::Record::ErrorStatus EquationStore::addEmptyModel() {
  char name[3];
  static_assert(k_maxNumberOfEquations < 9,
                "Equation name record might not fit");
  const char *const extensions[1] = {Ion::Storage::eqExtension};
  name[0] = 'e';
  Ion::Storage::FileSystem::sharedFileSystem->firstAvailableNameFromPrefix(
      name, 1, 3, extensions, 1, k_maxNumberOfEquations);
  return Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      name, Ion::Storage::eqExtension, nullptr, 0);
}

Shared::ExpressionModelHandle *EquationStore::setMemoizedModelAtIndex(
    int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  m_equations[cacheIndex] = Equation(record);
  return &m_equations[cacheIndex];
}

ExpressionModelHandle *EquationStore::memoizedModelAtIndex(
    int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  return &m_equations[cacheIndex];
}

EquationStore::Error EquationStore::privateExactSolve(Context *context) {
  m_numberOfSolutions = 0;
  Expression simplifiedEquations[k_maxNumberOfEquations];
  Error error = simplifyAndFindVariables(context, simplifiedEquations);
  if (error != Error::NoError) {
    return error;
  }
  error = solveLinearSystem(context, simplifiedEquations);
  if (error != Error::NonLinearSystem || m_numberOfResolutionVariables > 1 ||
      numberOfDefinedModels() > 1) {
    return error;
  }
  error = solvePolynomial(context, simplifiedEquations);
  if (error == Error::RequireApproximateSolution) {
    m_type = Type::GeneralMonovariable;
  }
  assert(error != Error::NoError || m_type == Type::PolynomialMonovariable);
  return error;
}

EquationStore::Error EquationStore::simplifyAndFindVariables(
    Context *context, Expression *simplifiedEquations) {
  m_numberOfResolutionVariables = 0;
  m_numberOfUserVariables = 0;
  m_variables[0][0] = '\0';
  m_userVariables[0][0] = '\0';
  m_complexFormat = Preferences::sharedPreferences->complexFormat();

  size_t nEquations = numberOfDefinedModels();
  for (size_t i = 0; i < nEquations; i++) {
    ExpiringPointer<Equation> equation =
        modelForRecord(definedRecordAtIndex(i));
    Expression equationsWithUserVariables = equation->standardForm(
        context, true, ReductionTarget::SystemForAnalysis);

    // Gather user variables
    int nVariables = equationsWithUserVariables.getVariables(
        context,
        [](const char *s, Context *c) {
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
        simplifiedEquations[i].type() == ExpressionNode::Type::Undefined ||
        simplifiedEquations[i].recursivelyMatches(
            Expression::IsMatrix, context,
            m_overrideUserVariables
                ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
                : SymbolicComputation::
                      ReplaceAllDefinedSymbolsWithDefinition)) {
      return Error::EquationUndefined;
    } else if (simplifiedEquations[i].type() == ExpressionNode::Type::Nonreal) {
      return Error::EquationNonreal;
    }

    m_complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(
        m_complexFormat, simplifiedEquations[i], nullptr);

    // Gather resolution variables
    m_numberOfResolutionVariables = simplifiedEquations[i].getVariables(
        context, [](const char *, Context *) { return true; },
        &m_variables[0][0], SymbolAbstractNode::k_maxNameSize,
        m_numberOfResolutionVariables);
    /* The equation has been parsed, so there should not be any variable with a
     * name that is too long. */
    // FIXME Special return values of getVariables should be named.
    assert(m_numberOfResolutionVariables != -2);
    if (m_numberOfResolutionVariables == -1) {
      return Error::TooManyVariables;
    }
  }
  return Error::NoError;
}

EquationStore::Error EquationStore::solveLinearSystem(
    Context *context, Expression *simplifiedEquations) {
  Preferences::AngleUnit angleUnit =
      Preferences::sharedPreferences->angleUnit();
  Preferences::UnitFormat unitFormat =
      GlobalPreferences::sharedGlobalPreferences->unitFormat();
  SymbolicComputation symbolicComputation =
      m_overrideUserVariables
          ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
          : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  Expression coefficients[k_maxNumberOfEquations]
                         [Expression::k_maxNumberOfVariables];
  Expression constants[k_maxNumberOfEquations];
  size_t m = numberOfDefinedModels();
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

  // n unknown variables and m equations
  size_t n = m_numberOfResolutionVariables;
  // Create the matrix (A|b) for the equation Ax=b;
  Matrix ab = Matrix::Builder();
  size_t abChildren = 0;
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      ab.addChildAtIndexInPlace(coefficients[i][j], abChildren, abChildren);
      ++abChildren;
    }
    ab.addChildAtIndexInPlace(constants[i], abChildren, abChildren);
    ++abChildren;
  }
  ab.setDimensions(m, n + 1);
  // Compute the rank of (A|b)
  int rank = ab.rank(context, m_complexFormat, angleUnit, unitFormat, true);
  if (rank == -1) {
    return Error::EquationUndefined;
  }
  // Initialize the number of solutions
  m_numberOfSolutions = INT_MAX;
  /* If the matrix has one null row except the last column, the system is
   * inconsistent (equivalent to 0 = x with x non-null */
  for (int j = m - 1; j >= 0; j--) {
    bool rowWithNullCoefficients = true;
    for (int i = 0; i < n; i++) {
      if (ab.matrixChild(j, i).isNull(context) != TrinaryBoolean::True) {
        rowWithNullCoefficients = false;
        break;
      }
    }
    if (rowWithNullCoefficients &&
        ab.matrixChild(j, n).isNull(context) != TrinaryBoolean::True) {
      // TODO: Handle ExpressionNode::NullStatus::Unknown
      m_numberOfSolutions = 0;
    }
  }
  if (m_numberOfSolutions > 0) {
    // if rank(A | b) < n, the system has an infinite number of solutions
    if (rank == n && n > 0) {
      /* Otherwise, the system has n solutions which correspond to the last
       * column */
      // Set m_numberOfSolutions to 0, as registerSolution will increment it.
      m_numberOfSolutions = 0;
      for (int i = 0; i < n; i++) {
        Error error = registerSolution(ab.matrixChild(i, n), context);
        if (error != Error::NoError) {
          return error;
        }
      }
    }
  }
  return Error::NoError;
}

EquationStore::Error EquationStore::solvePolynomial(
    Context *context, Expression *simplifiedEquations) {
  assert(m_numberOfResolutionVariables == 1 && numberOfDefinedModels() == 1);
  Preferences::AngleUnit angleUnit =
      Preferences::sharedPreferences->angleUnit();
  Preferences::UnitFormat unitFormat =
      GlobalPreferences::sharedGlobalPreferences->unitFormat();
  SymbolicComputation symbolicComputation =
      m_overrideUserVariables
          ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
          : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  m_degree = simplifiedEquations[0].getPolynomialReducedCoefficients(
      m_variables[0], coefficients, context, m_complexFormat, angleUnit,
      unitFormat, symbolicComputation);
  if (m_degree < 2 || m_degree > 3) {
    return Error::RequireApproximateSolution;
  }

  m_type = Type::PolynomialMonovariable;
  ReductionContext reductionContext(context, m_complexFormat, angleUnit,
                                    unitFormat, ReductionTarget::User);
  Expression delta;
  Expression x[3];
  bool solutionsAreApproximate = false;
  size_t numberOfSolutions = 0;
  if (m_degree == 2) {
    numberOfSolutions = Poincare::Polynomial::QuadraticPolynomialRoots(
        coefficients[2], coefficients[1], coefficients[0], x, x + 1, &delta,
        reductionContext);
  } else {
    assert(m_degree == 3);
    numberOfSolutions = Poincare::Polynomial::CubicPolynomialRoots(
        coefficients[3], coefficients[2], coefficients[1], coefficients[0], x,
        x + 1, x + 2, &delta, reductionContext, &solutionsAreApproximate);
  }
  for (size_t i = 0; i < numberOfSolutions; i++) {
    Error error = registerSolution(x[i], context, solutionsAreApproximate);
    if (error != Error::NoError) {
      return error;
    }
  }
  // Account for delta
  return registerSolution(delta, context);
}

EquationStore::Error EquationStore::registerSolution(
    Expression e, Context *context, bool expressionIsApproximate) {
  Preferences::AngleUnit angleUnit =
      Preferences::sharedPreferences->angleUnit();
  Expression exact, approximate;

  if (expressionIsApproximate) {
    approximate = e;
  } else {
    Preferences::UnitFormat unitFormat =
        GlobalPreferences::sharedGlobalPreferences->unitFormat();
    SymbolicComputation symbolicComputation =
        m_overrideUserVariables
            ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
            : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
    e.cloneAndSimplifyAndApproximate(&exact, &approximate, context,
                                     m_complexFormat, angleUnit, unitFormat,
                                     symbolicComputation);
  }
  if (approximate.type() == ExpressionNode::Type::Nonreal) {
    return Error::NoError;
  }
  if (approximate.type() == ExpressionNode::Type::Undefined) {
    return Error::EquationUndefined;
  }

  Layout exactLayout = PoincareHelpers::CreateLayout(exact, context);
  Layout approximateLayout =
      PoincareHelpers::CreateLayout(approximate, context);

  // Compare approximate and exact layouts
  Solution::ApproximationType approximationType;
  if (expressionIsApproximate ||
      ExpressionDisplayPermissions::ShouldNeverDisplayExactOutput(exact,
                                                                  context)) {
    approximationType = Solution::ApproximationType::Identical;
  } else {
    char exactBuffer[::Constant::MaxSerializedExpressionSize];
    char approximateBuffer[::Constant::MaxSerializedExpressionSize];
    exactLayout.serializeForParsing(exactBuffer,
                                    ::Constant::MaxSerializedExpressionSize);
    approximateLayout.serializeForParsing(
        approximateBuffer, ::Constant::MaxSerializedExpressionSize);
    approximationType =
        strcmp(exactBuffer, approximateBuffer) == 0
            ? Solution::ApproximationType::Identical
        : Expression::ExactAndApproximateExpressionsAreEqual(
              exact, Expression::Parse(approximateBuffer, context))
            ? Solution::ApproximationType::Equal
            : Solution::ApproximationType::Approximate;
  }
  m_solutions[m_numberOfSolutions++] =
      Solution(approximationType == Solution::ApproximationType::Identical
                   ? approximateLayout
                   : exactLayout,
               approximateLayout,
               approximate.approximateToScalar<double>(context, m_complexFormat,
                                                       angleUnit),
               approximationType);
  return Error::NoError;
}

void EquationStore::registerSolution(double f) {
  if (std::isfinite(f)) {
    m_solutions[m_numberOfSolutions++] =
        Solution(Layout(), Layout(), f, Solution::ApproximationType::Identical);
  }
}

}  // namespace Solver
