#include "equation_solver.h"

#include <poincare/numeric/roots.h>
#include <poincare/numeric/solver.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/src/numeric/zoom.h>

#include "advanced_reduction.h"
#include "approximation.h"
#include "float.h"
#include "list.h"
#include "matrix.h"
#include "polynomial.h"
#include "set.h"
#include "sign.h"
#include "simplification.h"
#include "symbol.h"
#include "systematic_reduction.h"
#include "variables.h"

namespace Poincare::Internal {

template <int N>
void VariableArray<N>::append(const char* variable) {
  assert(m_numberOfVariables < N);
  assert(strlen(variable) < Symbol::k_maxNameLength);
  memcpy(m_variables[m_numberOfVariables], variable, strlen(variable) + 1);
  m_numberOfVariables++;
}

template <int N>
void VariableArray<N>::fillWithList(const Tree* list) {
  assert((list->isList() || list->isSet()) && list->numberOfChildren() <= N);
  clear();
  for (const Tree* variable : list->children()) {
    append(Symbol::GetName(variable));
  }
}

Tree* EquationSolver::ExactSolve(const Tree* equationsSet, Context* context,
                                 ProjectionContext projectionContext,
                                 Error* error) {
  // TODO: Context is quite large and this copy could be optimized.
  Context firstContext = *context;
  projectionContext.m_symbolic =
      SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  Tree* result =
      PrivateExactSolve(equationsSet, &firstContext, projectionContext, error);
  if (*error == Error::RequireApproximateSolution ||
      (*error == Error::NoError && result->numberOfChildren() > 0)) {
    *context = firstContext;
    return result;
  }
  assert((result == nullptr) || (result->numberOfChildren() == 0));
  if (result) {
    result->removeTree();
  }

  Error secondError = Error::NoError;
  projectionContext.m_symbolic =
      SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions;
  result =
      PrivateExactSolve(equationsSet, context, projectionContext, &secondError);
  if (*error != Error::NoError || secondError == Error::NoError ||
      secondError == Error::RequireApproximateSolution) {
    *error = secondError;
  } else {
    assert(!result);
    *context = firstContext;
    if (*error == Error::NoError) {
      /* The system becomes invalid when overriding the user variables: the
       * first solution was better. Restore inital empty set */
      result = SharedTreeStack->pushSet(0);
    }
  }
  return result;
}

Tree* EquationSolver::PrivateExactSolve(const Tree* equationsSet,
                                        Context* context,
                                        ProjectionContext projectionContext,
                                        Error* error) {
  // Update context from projectionContext
  assert(projectionContext.m_symbolic ==
             SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions ||
         projectionContext.m_symbolic ==
             SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  context->overrideUserVariables =
      (projectionContext.m_symbolic ==
       SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
  Projection::UpdateComplexFormatWithExpressionInput(equationsSet,
                                                     &projectionContext);
  context->complexFormat = projectionContext.m_complexFormat;
  // Retrieve user symbols before simplification and variable replacement
  Tree* userSymbols = Variables::GetUserSymbols(equationsSet);
  for (const Tree* userSymbol : userSymbols->children()) {
    if (projectionContext.m_context->treeForSymbolIdentifier(userSymbol)) {
      context->userVariables.append(Symbol::GetName(userSymbol));
    }
  }
  userSymbols->removeTree();
  /* Clone and simplify the equations */
  Tree* reducedEquationSet = equationsSet->cloneTree();
  ProjectAndReduce(reducedEquationSet, projectionContext, error);
  if (*error != Error::NoError) {
    reducedEquationSet->removeTree();
    return nullptr;
  }

  /* Count and collect remaining UserSymbols */
  userSymbols = Variables::GetUserSymbols(reducedEquationSet);
  uint8_t numberOfVariables = userSymbols->numberOfChildren();

  if ((equationsSet->numberOfChildren() > 1 || numberOfVariables > 1) &&
      Preferences::SharedPreferences()
          ->examMode()
          .forbidSimultaneousEquationSolver()) {
    *error = Error::DisabledInExamMode;
  } else if (numberOfVariables > k_maxNumberOfExactSolutions) {
    *error = Error::TooManyVariables;
  }
  if (*error != Error::NoError) {
    userSymbols->removeTree();
    reducedEquationSet->removeTree();
    return nullptr;
  }

  /* Replace UserSymbols with variables for easier solution handling */
  SwapTreesPointers(&reducedEquationSet, &userSymbols);
  int i = 0;
  for (const Tree* variable : userSymbols->children()) {
    Variables::ReplaceSymbol(reducedEquationSet, variable, i++,
                             ComplexSign::Finite());
  }
  context->numberOfVariables = userSymbols->numberOfChildren();

  /* Find equation's results */
  TreeRef result;
  assert(*error == Error::NoError);
  result =
      SolveLinearSystem(reducedEquationSet, numberOfVariables, context, error);
  if (*error == Error::NonLinearSystem && numberOfVariables <= 1 &&
      equationsSet->numberOfChildren() <= 1) {
    assert(result.isUninitialized());
    result =
        SolvePolynomial(reducedEquationSet, numberOfVariables, context, error);
    if (*error == Error::RequireApproximateSolution) {
      context->type = Type::GeneralMonovariable;
      // TODO: Handle GeneralMonovariable solving.
      assert(result.isUninitialized());
      reducedEquationSet->removeTree();
      userSymbols->removeTree();
      return result;
    }
    /* Remove non real solutions of a polynomial if the equation was projected
     * with a "Real" Complex format */
    assert(result->isList());
    if (projectionContext.m_complexFormat == ComplexFormat::Real) {
      for (int i = result->numberOfChildren() - 1; i >= 0; i--) {
        if (!SignOfTreeOrApproximation(result->child(i)).isReal()) {
          NAry::RemoveChildAtIndex(result, i);
        }
      }
    }
  }
  reducedEquationSet->removeTree();

  /* Replace variables back to UserSymbols */
  if (!result.isUninitialized()) {
    context->variables.fillWithList(userSymbols);
    for (const Tree* symbol : userSymbols->children()) {
      Variables::LeaveScopeWithReplacement(result, symbol, false);
      // LeaveScopeWithReplacement can return dependencies
      Dependency::DeepRemoveUselessDependencies(result);
    }
    // Replace additional unknown parameter variables (t1, t2, ...)
    context->numberOfVariables -= userSymbols->numberOfChildren();
    if (context->numberOfVariables > 0) {
      // Start at 0 ("t") instead of 1 ("t1") if there is only one variable
      size_t parameterIndex = (context->numberOfVariables > 1) ? 1 : 0;
      uint32_t usedParameterIndices = TagParametersUsedAsVariables(context);

      for (int j = 0; j < context->numberOfVariables; j++) {
        // Generate a unique identifier t? that does not collide with variables.
        TreeRef symbol = getNextParameterSymbol(
            &parameterIndex, usedParameterIndices, projectionContext.m_context);
        Variables::LeaveScopeWithReplacement(result, symbol, false);
        symbol->removeTree();
      }
    }
  }
  userSymbols->removeTree();

  /* Beautify result */
  if (!result.isUninitialized()) {
    Simplification::BeautifyReduced(result, &projectionContext);
  }

  return result;
}

template <typename T>
static Coordinate2D<T> evaluator(T t, const void* model, Context* context) {
  const Tree* e = reinterpret_cast<const Tree*>(model);
  return Coordinate2D<T>(t, Approximation::RootPreparedToReal(e, t));
}

Range1D<double> EquationSolver::AutomaticInterval(const Tree* preparedEquation,
                                                  Context* context) {
  constexpr float k_maxFloatForAutoApproximateSolvingRange = 1e15f;
  // TODO: factor with InteractiveCurveViewRange::NormalYXRatio();
  constexpr float k_yxRatio = 3.06f / 5.76f;
  Zoom zoom(NAN, NAN, k_yxRatio, nullptr,
            k_maxFloatForAutoApproximateSolvingRange);
  // Use the intersection between the definition domain of f and the bounds
  zoom.setBounds(-k_maxFloatForAutoApproximateSolvingRange,
                 k_maxFloatForAutoApproximateSolvingRange);
  zoom.setMaxPointsOneSide(k_maxNumberOfApproximateSolutions,
                           k_maxNumberOfApproximateSolutions / 2);
  const void* model = static_cast<const void*>(preparedEquation);
  bool finiteNumberOfSolutions = true;
  bool didFitRoots = zoom.fitRoots(evaluator<float>, model, false,
                                   evaluator<double>, &finiteNumberOfSolutions);
  /* When there are more than k_maxNumberOfApproximateSolutions on one side of
   * 0, the zoom is setting the interval to have a maximum of 5 solutions left
   * of 0 and 5 solutions right of zero. This means that sometimes, for a
   * function like `piecewise(1, x<0; cos(x), x >= 0)`, only 5 solutions will be
   * displayed. We still want to notify the user that more solutions exist. */
  context->hasMoreSolutions = !finiteNumberOfSolutions;
  zoom.fitBounds(evaluator<float>, model, false);
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
  return {finalRange.min(), finalRange.max()};
}

Tree* EquationSolver::ApproximateSolve(const Tree* preparedEquation,
                                       Range1D<double> range,
                                       Context* context) {
  assert(context->type == Type::GeneralMonovariable);
  assert(context->variables.numberOfVariables() == 1);

  assert(range.isValid());
  Solver<double> solver =
      Poincare::Solver<double>(range.min(), range.max(), nullptr /*context*/);
  solver.stretch();

  TreeRef resultList = List::PushEmpty();

  for (int i = 0; i <= k_maxNumberOfApproximateSolutions; i++) {
    double root = solver.nextRoot(preparedEquation).x();
    if (root < range.min()) {
      i--;
      continue;
    } else if (root > range.max()) {
      root = NAN;
    }

    if (i == k_maxNumberOfApproximateSolutions) {
      context->hasMoreSolutions = true;
    } else {
      if (std::isnan(root)) {
        break;
      }
      if (std::isfinite(root)) {
        NAry::AddChild(resultList, SharedTreeStack->pushFloat(root));
      }
    }
  }
  return resultList;
}

void EquationSolver::ProjectAndReduce(Tree* equationsSet,
                                      ProjectionContext projectionContext,
                                      Error* error) {
  assert(*error == Error::NoError);
  Simplification::ProjectAndReduce(equationsSet, &projectionContext, true);
  if (projectionContext.m_dimension.isUnit()) {
    *error = Error::EquationUndefined;
    return;
  }
  if (!equationsSet->isList()) {
    *error = Error::EquationUndefined;
    return;
  }
  for (const Tree* equation : equationsSet->children()) {
    if (equation->isUndefined()) {
      *error = equation->isNonReal() && *error == Error::NoError
                   ? Error::EquationNonReal
                   : Error::EquationUndefined;
    }
  }
}

Tree* EquationSolver::SolveLinearSystem(const Tree* reducedEquationSet,
                                        uint8_t n, Context* context,
                                        Error* error) {
  context->exactResults = true;
  context->type = Type::LinearSystem;
  context->degree = 1;

  // n unknown variables and rows equations
  uint8_t cols = n + 1;
  uint8_t rows = reducedEquationSet->numberOfChildren();
  Tree* matrix = SharedTreeStack->pushMatrix(0, 0);
  int m = reducedEquationSet->numberOfChildren();

  // Create the matrix (A|b) for the equation Ax=b;
  for (const Tree* equation : reducedEquationSet->children()) {
    Tree* coefficients = GetLinearCoefficients(equation, n, context);
    if (!coefficients) {
      *error = Error::NonLinearSystem;
      matrix->removeTree();
      return nullptr;
    }
    assert(coefficients->numberOfChildren() == cols);
    // Invert constant because Ax=b is represented by Ax-b
    Tree* constant = coefficients->lastChild();
    PatternMatching::MatchReplaceSimplify(constant, KA, KMult(-1_e, KA));
    coefficients->removeNode();
    Matrix::SetNumberOfColumns(matrix, cols);
    Matrix::SetNumberOfRows(matrix, Matrix::NumberOfRows(matrix) + 1);
  }
  assert(Matrix::NumberOfRows(matrix) == rows);
  // Compute the rank of (A|b)
  int rank = Matrix::CanonizeAndRank(matrix);
  if (rank == Matrix::k_failedToCanonizeRank) {
    *error = Error::EquationUndefined;
    matrix->removeTree();
    return nullptr;
  }
  const Tree* coefficient = matrix->child(0);
  for (uint8_t row = 0; row < rows; row++) {
    bool allCoefficientsNull = true;
    for (uint8_t col = 0; col < n; col++) {
      if (allCoefficientsNull && !GetSign(coefficient).isNull()) {
        allCoefficientsNull = false;
      }
      coefficient = coefficient->nextTree();
    }
    if (allCoefficientsNull && !GetSign(coefficient).isNull()) {
      /* Row j describes an equation of the form '0=b', the system has no
       * solution. */
      matrix->removeTree();
      *error = Error::NoError;
      return SharedTreeStack->pushSet(0);
    }
    coefficient = coefficient->nextTree();
  }

  if (rank != n || n <= 0) {
    /* The system is insufficiently qualified: bind the value of n-rank
     * variables to parameters. */
    context->hasMoreSolutions = true;
    int variable = n - 1;
    int row = m - 1;
    int firstVariableInRow = -1;
    while (variable >= 0) {
      // Find the first variable with a non-null coefficient in the current row
      if (row >= 0) {
        for (int col = 0; firstVariableInRow < 0 && col < n; col++) {
          if (!Matrix::Child(matrix, row, col)->isZero()) {
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
        (i == variable ? 1_e : 0_e)->cloneTree();
      }
      // Push a finite variable starting from ??
      SharedTreeStack->pushVar(context->numberOfVariables++,
                               ComplexSign::Finite());
      rows = m + 1;
      Matrix::SetDimensions(matrix, ++m, n + 1);
      variable--;
    }

    /* forceCanonization = true so that canonization still happens even if
     * t.approximate() is NAN. If other children of ab had an undef
     * approximation, the resolution would already have failed at
     * reduction of the equation set. */
    rank = Matrix::CanonizeAndRank(matrix, true);
    if (rank == Matrix::k_failedToCanonizeRank) {
      *error = Error::EquationUndefined;
      matrix->removeTree();
      return nullptr;
    }
  } else {
    context->hasMoreSolutions = false;
  }
  assert(rank == n);

  // TODO: Make sure the solution satisfies dependencies in equations

  /* The rank is equal to the number of variables: the system has n
   * solutions, and after canonization their values are the first n values on
   * the last column. */
  Tree* child = matrix->child(0);
  for (uint8_t row = 0; row < rows; row++) {
    for (uint8_t col = 0; col < cols; col++) {
      if (row < n && col == cols - 1) {
        if (*error == Error::NoError) {
          *error = EnhanceSolution(child, context);
          // Continue anyway to preserve TreeStack integrity
        }
        child = child->nextTree();
      } else {
        child->removeTree();
      }
    }
  }
  matrix->moveNodeOverNode(SharedTreeStack->pushSet(n));
  return matrix;
}

Tree* EquationSolver::GetLinearCoefficients(const Tree* equation,
                                            uint8_t numberOfVariables,
                                            Context* context) {
  TreeRef result = SharedTreeStack->pushList(0);
  TreeRef eq = equation->cloneTree();
  /* TODO: y*(1+x) is not handled by PolynomialParser. We expand everything as
   * temporary workaround. */
  SystematicReduction::DeepReduce(eq);
  AdvancedReduction::DeepExpandAlgebraic(eq);
  Dependency::DeepRemoveUselessDependencies(eq);
  for (uint8_t i = 0; i < numberOfVariables; i++) {
    // TODO: PolynomialParser::Parse may need to handle more block types.
    // TODO: Use user settings for a RealUnkown sign ?
    Tree* polynomial = PolynomialParser::Parse(
        eq, Variables::Variable(i, ComplexSign::Finite()));
    if (!polynomial) {
      // equation is not polynomial
      SharedTreeStack->dropBlocksFrom(result);
      return nullptr;
    }
    if (!polynomial->isPolynomial()) {
      // eq did not depend on variable. Continue.
      eq = polynomial;
      NAry::AddChild(result, SharedTreeStack->pushZero());
      continue;
    }
    if (Polynomial::Degree(polynomial) != 1) {
      /* Degree is supposed to be 0 or 1. Otherwise, it means that equation
       * is 'undefined' due to the reduction of 0*inf for example.
       * (ie, x*y*inf = 0) */
      polynomial->removeTree();
      result->removeTree();
      return nullptr;
    }
    bool nullConstant = (Polynomial::NumberOfTerms(polynomial) == 1);
    /* The equation can be written: a_1*x+a_0 with a_1 and a_0 x-independent.
     * The equation supposed to be linear in all variables, so we can look for
     * the coefficients linked to the other variables in a_0. */
    // Pilfer polynomial result : [P][Variable][Coeff1][?Coeff0]
    polynomial->removeNode();  // Remove Node : [Variable][Coeff1][?Coeff0]
    polynomial->removeTree();  // Remove Variable : [Coeff1][?Coeff0]
    // Update eq to follow [Coeff0] if it exists for next variables.
    eq = nullConstant ? SharedTreeStack->pushZero() : polynomial->nextTree();
    if (PolynomialParser::ContainsVariable(polynomial) ||
        (i == numberOfVariables - 1 &&
         PolynomialParser::ContainsVariable(eq))) {
      /* The expression can be linear on all coefficients taken one by one but
       * non-linear (ex: xy = 2). We delete the results and return false if one
       * of the coefficients (or last constant term) contains a variable. */
      eq->removeTree();
      polynomial->removeTree();
      result->removeTree();
      return nullptr;
    }
    /* This will detach [Coeff1] into result, leaving eq alone and polynomial
     * properly pilfered. */
    NAry::AddChild(result, polynomial);
  }
  // Constant term is remaining [Coeff0].
  Tree* constant = eq->detachTree();
  NAry::AddChild(result, constant);
  return result;
}

Tree* EquationSolver::SolvePolynomial(const Tree* simplifiedEquationSet,
                                      uint8_t n, Context* context,
                                      Error* error) {
  constexpr static int k_maxPolynomialDegree = 3;
  constexpr static int k_maxNumberOfPolynomialCoefficients =
      k_maxPolynomialDegree + 1;

  assert(simplifiedEquationSet->isList() &&
         simplifiedEquationSet->numberOfChildren() == 1);
  assert(n == 1);
  Tree* equation = simplifiedEquationSet->child(0)->cloneTree();
  // TODO: expansion should be done only once
  SystematicReduction::DeepReduce(equation);
  AdvancedReduction::DeepExpandAlgebraic(equation);
  Dependency::DeepRemoveUselessDependencies(equation);
  Tree* polynomial = PolynomialParser::Parse(
      equation, Variables::Variable(0, ComplexSign::Finite()));
  if (!polynomial) {
    *error = Error::RequireApproximateSolution;
    SharedTreeStack->dropBlocksFrom(equation);
    return nullptr;
  }

  const Tree* coefficients[k_maxNumberOfPolynomialCoefficients] = {};
  int degree = Polynomial::Degree(polynomial);
  if (degree > k_maxPolynomialDegree) {
    *error = Error::RequireApproximateSolution;
    SharedTreeStack->dropBlocksFrom(equation);
    return nullptr;
  }
  context->type = Type::PolynomialMonovariable;
  context->degree = degree;

  int numberOfTerms = Polynomial::NumberOfTerms(polynomial);
  const Tree* coefficient = Polynomial::LeadingCoefficient(polynomial);
  for (int i = 0; i < numberOfTerms; i++) {
    int exponent = Polynomial::ExponentAtIndex(polynomial, i);
    if (exponent < k_maxNumberOfPolynomialCoefficients) {
      coefficients[exponent] = coefficient;
    }
    coefficient = coefficient->nextTree();
  }
  for (const Tree*& coef : coefficients) {
    if (coef == nullptr) {
      coef = 0_e;
    }
  }

  assert(degree == 2 || degree == 3);
  TreeRef discriminant =
      (degree == 2)
          ? Roots::QuadraticDiscriminant(coefficients[2], coefficients[1],
                                         coefficients[0])
          : Roots::CubicDiscriminant(coefficients[3], coefficients[2],
                                     coefficients[1], coefficients[0]);
  /* Exact solutions are computed, except for some of the cubic polynomials that
   * require Cardano's method, in which case approximate solutions are computed.
   * TODO: the "fastCardanoMethod" parameter needs to be exposed at upper
   * levels. */
  TreeRef solutionList =
      (degree == 2)
          ? Roots::Quadratic(coefficients[2], coefficients[1], coefficients[0],
                             discriminant)
          : Roots::Cubic(coefficients[3], coefficients[2], coefficients[1],
                         coefficients[0], discriminant, true);

  /* TODO: When all coefficients are real, the number of real solutions needs to
   * be checked in an assert (looking at the discrimant sign). The verification
   * function would be similar to Roots::ApproximateRootsOfRealCubic, but
   * without approximation. */

  polynomial->removeTree();
  for (Tree* solution : solutionList->children()) {
    // TODO_PCJ: restore dependencies handling here
    EnhanceSolution(solution, context);
  }
  NAry::AddChild(solutionList, discriminant);
  *error = Error::NoError;
  return solutionList;
}

EquationSolver::Error EquationSolver::EnhanceSolution(Tree* solution,
                                                      Context* context) {
  /* TODO_PCJ:
   * - Remove this function
   * - Pass more context
   */
  // TODO: Use user settings for a RealUnkown sign ?
  Simplification::ReduceSystem(solution, true);
  return Error::NoError;
}

uint32_t EquationSolver::TagParametersUsedAsVariables(const Context* context) {
  uint32_t tags = 0;
  constexpr size_t k_maxIndex = OMG::BitHelper::numberOfBitsIn(tags);
  constexpr size_t k_maxNumberOfDigits =
      OMG::Print::LengthOfUInt32(OMG::Base::Decimal, k_maxIndex);
  /* Only check local variables that may not have a global definition. The
   * others  will be checked for later. */
  for (size_t i = 0; i < context->variables.numberOfVariables(); i++) {
    // Set the k-th bit in tags if name == "t{k}" and 0th if name is "t"
    const char* variable = context->variables.variable(i);
    if (variable[0] != k_parameterPrefix) {
      continue;
    }
    if (variable[1] == '\0') {
      OMG::BitHelper::setBitAtIndex(tags, 0, true);
      continue;
    }
    size_t index =
        OMG::Print::ParseDecimalInt(&variable[1], k_maxNumberOfDigits);
    if (index > 0 && index < k_maxIndex) {
      OMG::BitHelper::setBitAtIndex(tags, index, true);
    }
  }
  return tags;
}

Tree* EquationSolver::getNextParameterSymbol(size_t* parameterIndex,
                                             uint32_t usedParameterIndices,
                                             Poincare::Context* context) {
  /* Equation had more solution and introduced new unknowns variables, name
   * them 't' + 2 digits + '\0' */
  constexpr size_t k_parameterNameSize = 1 + 2 + 1;
  constexpr size_t k_maxIndex =
      OMG::BitHelper::numberOfBitsIn(usedParameterIndices);
  char parameterName[k_parameterNameSize] = {k_parameterPrefix};
  while (*parameterIndex < k_maxIndex) {
    // Skip already used parameter indices in local variables
    while (OMG::BitHelper::bitAtIndex(usedParameterIndices, *parameterIndex)) {
      (*parameterIndex)++;
      assert(*parameterIndex < k_maxIndex);
    }
    size_t parameterNameLength =
        *parameterIndex == 0
            ? 1
            : 1 + OMG::Print::IntLeft(*parameterIndex, parameterName + 1,
                                      k_parameterNameSize - 2);
    (*parameterIndex)++;
    assert(parameterNameLength >= 1 &&
           parameterNameLength < k_parameterNameSize);
    parameterName[parameterNameLength] = 0;
    Tree* symbol =
        SharedTreeStack->pushUserSymbol(parameterName, parameterNameLength + 1);
    if (!context->treeForSymbolIdentifier(symbol)) {
      return symbol;
    }
    // Skip already used parameter indices in global variables
    symbol->removeTree();
  }
  OMG::unreachable();
}

}  // namespace Poincare::Internal
