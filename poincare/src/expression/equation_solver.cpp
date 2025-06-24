#include "equation_solver.h"

#include <poincare/preferences.h>
#include <poincare/solver/roots.h>
#include <poincare/solver/solver.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_helpers.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/src/solver/zoom.h>

#include "advanced_reduction.h"
#include "approximation.h"
#include "dependency.h"
#include "list.h"
#include "matrix.h"
#include "polynomial.h"
#include "sign.h"
#include "simplification.h"
#include "symbol.h"
#include "systematic_reduction.h"
#include "variables.h"

namespace Poincare::Internal {

void VariableArray::append(const char* variable) {
  assert(m_numberOfVariables < k_maxNumberOfVariables);
  assert(strlen(variable) < Symbol::k_maxNameLength);
  memcpy(m_variables[m_numberOfVariables], variable, strlen(variable) + 1);
  m_numberOfVariables++;
}

void VariableArray::fillWithList(const Tree* list) {
  assert((list->isList() || list->isSet()) &&
         list->numberOfChildren() <= k_maxNumberOfVariables);
  clear();
  for (const Tree* variable : list->children()) {
    append(Symbol::GetName(variable));
  }
}

EquationSolver::SolverResult EquationSolver::ExactSolve(
    const Tree* equationsSet, ProjectionContext projectionContext) {
  // Try solving while using user variables
  projectionContext.m_symbolic = SymbolicComputation::ReplaceDefinedSymbols;
  SolverResult firstResult = PrivateExactSolve(equationsSet, projectionContext);
  if (firstResult.metadata.error == Error::RequireApproximateSolution ||
      (firstResult.metadata.error == Error::NoError &&
       firstResult.solutionList->numberOfChildren() > 0)) {
    return firstResult;
  }
  assert((firstResult.solutionList == nullptr) ||
         (firstResult.solutionList->numberOfChildren() == 0));
  if (firstResult.solutionList) {
    firstResult.solutionList->removeTree();
  }

  // Try solving while overriding user variables
  projectionContext.m_symbolic = SymbolicComputation::ReplaceDefinedFunctions;
  SolverResult secondResult =
      PrivateExactSolve(equationsSet, projectionContext);
  if (firstResult.metadata.error == Error::NoError &&
      secondResult.metadata.error != Error::NoError &&
      secondResult.metadata.error != Error::RequireApproximateSolution) {
    assert(secondResult.solutionList == nullptr);
    /* The system becomes invalid when overriding the user variables: the
     * first solution was better. Restore inital empty set */
    firstResult.solutionList = SharedTreeStack->pushSet(0);
    return firstResult;
  }

  return secondResult;
}

EquationSolver::SolverResult EquationSolver::PrivateExactSolve(
    const Tree* equationsSet, ProjectionContext projectionContext) {
  // Update context from projectionContext
  assert(projectionContext.m_symbolic ==
             SymbolicComputation::ReplaceDefinedFunctions ||
         projectionContext.m_symbolic ==
             SymbolicComputation::ReplaceDefinedSymbols);

  SolutionMetadata metadata;
  metadata.overrideDefinedVariables =
      (projectionContext.m_symbolic ==
       SymbolicComputation::ReplaceDefinedFunctions);
  Projection::UpdateComplexFormatWithExpressionInput(equationsSet,
                                                     &projectionContext);
  metadata.complexFormat = projectionContext.m_complexFormat;

  // Retrieve user symbols before simplification and variable replacement
  Tree* userSymbols =
      Variables::GetUserSymbols(equationsSet, projectionContext.m_context);
  uint8_t numberOfUnknowns = userSymbols->numberOfChildren();
  /* When ReplaceDefinedSymbols, when a symbol in the expression is defined by
   * the user, remove it from possible unknowns */
  if (projectionContext.m_context && userSymbols->numberOfChildren() > 0) {
    Tree* userSymbol = userSymbols->child(0);
    uint8_t index = 0;
    while (index < numberOfUnknowns) {
      if (projectionContext.m_context->expressionForUserNamed(userSymbol)) {
        metadata.definedVariables.append(Symbol::GetName(userSymbol));
        if (projectionContext.m_symbolic ==
            SymbolicComputation::ReplaceDefinedSymbols) {
          userSymbol->removeTree();
          --numberOfUnknowns;
          continue;
        }
      }
      userSymbol = userSymbol->nextTree();
      ++index;
    }
    NAry::SetNumberOfChildren(userSymbols, numberOfUnknowns);
  }

  if ((equationsSet->numberOfChildren() > 1 || numberOfUnknowns > 1) &&
      Preferences::SharedPreferences()
          ->examMode()
          .forbidSimultaneousEquationSolver()) {
    metadata.error = Error::DisabledInExamMode;
  } else if (numberOfUnknowns > k_maxNumberOfExactSolutions) {
    metadata.error = Error::TooManyVariables;
  }
  if (metadata.error != Error::NoError) {
    userSymbols->removeTree();
    return {nullptr, metadata};
  }

  /* Clone and simplify the equations */
  Tree* reducedEquationSet = equationsSet->cloneTree();
  /* The symbols should be replaced in this order :
   *  - User functions (because they can hide UserSymbols and local symbols)
   *  - UserSymbols into Variables (because they have a different complexSign)
   *  - Local symbols into variables (Done in ProjectAndReduce)
   *  - Reduction (Done in ProjectAndReduce). */
  // Replace UserFunctions
  Projection::DeepReplaceUserNamed(
      reducedEquationSet, projectionContext.m_context,
      SymbolicComputation::ReplaceDefinedFunctions);
  // Replace UserSymbols
  int i = 0;
  for (const Tree* variable : userSymbols->children()) {
    // TODO: Use a more precise complexSign when possible for better reduction.
    Variables::ReplaceSymbol(reducedEquationSet, variable, i++,
                             ComplexSign::Finite());
  }
  // Project (replace local symbols) and reduce
  metadata.error = ProjectAndReduce(reducedEquationSet, projectionContext);
  if (metadata.error != Error::NoError) {
    reducedEquationSet->removeTree();
    userSymbols->removeTree();
    return {nullptr, metadata};
  }

  /* Find equation's results */
  TreeRef result;
  assert(metadata.error == Error::NoError);
  result = SolveLinearSystem(reducedEquationSet, numberOfUnknowns, &metadata);
#if POINCARE_POLYNOMIAL_SOLVER
  if (metadata.error == Error::NonLinearSystem && numberOfUnknowns <= 1 &&
      equationsSet->numberOfChildren() <= 1) {
    assert(result.isUninitialized());
    result = SolvePolynomial(reducedEquationSet, numberOfUnknowns, &metadata);
    if (metadata.error != Error::RequireApproximateSolution) {
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
  }
  if (metadata.error == Error::RequireApproximateSolution) {
    metadata.solvingMethod = SolvingMethod::GeneralMonovariable;
    // TODO: Handle GeneralMonovariable solving.
    assert(result.isUninitialized());
    reducedEquationSet->removeTree();
    userSymbols->removeTree();
    return {nullptr, metadata};
  }
#else
  if (metadata.error == Error::NonLinearSystem ||
      metadata.error == Error::RequireApproximateSolution) {
    metadata.solvingMethod = SolvingMethod::GeneralMonovariable;
  }
#endif
  reducedEquationSet->removeTree();

  /* Replace variables back to UserSymbols */
  if (!result.isUninitialized()) {
    metadata.unknownVariables.fillWithList(userSymbols);
    for (const Tree* symbol : userSymbols->children()) {
      Variables::LeaveScopeWithReplacement(result, symbol, false, false);
    }
    assert(numberOfUnknowns == metadata.unknownVariables.numberOfVariables());
    if (numberOfUnknowns > 0) {
      // Start at 0 ("t") instead of 1 ("t1") if there is only one variable
      size_t parameterIndex = (numberOfUnknowns > 1) ? 1 : 0;
      uint32_t usedParameterIndices =
          TagParametersUsedAsVariables(metadata.unknownVariables);

      for (int j = 0; j < numberOfUnknowns; j++) {
        // Generate a unique identifier t? that does not collide with variables.
        TreeRef symbol = GetNextParameterSymbol(
            &parameterIndex, usedParameterIndices, projectionContext.m_context);
        Variables::LeaveScopeWithReplacement(result, symbol, false, false);
        symbol->removeTree();
      }
    }
  }
  userSymbols->removeTree();

  /* Beautify result */
  if (!result.isUninitialized()) {
    Simplification::BeautifyReduced(result, &projectionContext);
  }

  return {result, metadata};
}

template <typename T>
static Coordinate2D<T> evaluator(T t, const void* model) {
  const Tree* e = reinterpret_cast<const Tree*>(model);
  return Coordinate2D<T>(
      t, Approximation::To<T>(
             e, t, Approximation::Parameters{.isRootAndCanHaveRandom = true}));
}

EquationSolver::SolverResult EquationSolver::ApproximateSolve(
    const Tree* equation, ProjectionContext projectionContext,
    Range1D<double> range) {
  // TODO: Restore "overrideDefinedVariables" when needed
  SolutionMetadata metadata{
      .solvingMethod = SolvingMethod::GeneralMonovariable,
      .solutionType = SolutionType::Approximate,
  };

  Tree* preparedEquation = PrepareEquationForApproximateSolve(
      equation, projectionContext, &metadata);

  if (range.isNan()) {
    /* Interval search is done in float to gain some time, since precision does
     * not matter as much as for the actual solve. The computed interval is
     * stretched and converted to double because the actual solver works on
     * double. */
    constexpr float k_maxFloatForAutoApproximateSolvingRange = 1e15f;
    // TODO: factor with InteractiveCurveViewRange::NormalYXRatio();
    constexpr float k_yxRatio = 3.06f / 5.76f;
    Zoom zoom(NAN, NAN, k_yxRatio, k_maxFloatForAutoApproximateSolvingRange);
    // Use the intersection between the definition domain of f and the bounds
    zoom.setBounds(-k_maxFloatForAutoApproximateSolvingRange,
                   k_maxFloatForAutoApproximateSolvingRange);
    zoom.setMaxPointsOneSide(k_maxNumberOfApproximateSolutions,
                             k_maxNumberOfApproximateSolutions / 2);
    const void* model = static_cast<const void*>(preparedEquation);
    bool finiteNumberOfSolutions = true;
    bool didFitRoots =
        zoom.fitRoots(evaluator<float>, model, false, evaluator<double>,
                      &finiteNumberOfSolutions);
    zoom.fitBounds(evaluator<float>, model, false);
    Range1D<float> finalRange = *(zoom.range(false, false).x());
    if (didFitRoots) {
      /* The range was computed from the solution found with a solver in float.
       * We need to strech the range in case it does not cover the solution
       * found with a solver in double. */
      constexpr static float k_securityMarginCoef = 1 / 10.0;
      float securityMargin =
          std::max(std::abs(finalRange.max()), std::abs(finalRange.min())) *
          k_securityMarginCoef;
      finalRange.stretchEachBoundBy(securityMargin,
                                    k_maxFloatForAutoApproximateSolvingRange);
    }

    range = Range1D<double>(finalRange.min(), finalRange.max());
    if (!finiteNumberOfSolutions) {
      /* When there are more than k_maxNumberOfApproximateSolutions on one side
       * of 0, the zoom is setting the interval to have a maximum of 5 solutions
       * left of 0 and 5 solutions right of zero. This means that sometimes, for
       * a function like `piecewise(1, x<0; cos(x), x >= 0)`, only 5 solutions
       * will be displayed. We still want to notify the user that more solutions
       * exist.
       */
      metadata.incompleteSolutions = true;
    }
  }

  assert(range.isValid());
  metadata.solvingRange = range;
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
      metadata.incompleteSolutions = true;
    } else {
      if (std::isnan(root)) {
        break;
      }
      if (std::isfinite(root)) {
        NAry::AddChild(resultList, SharedTreeStack->pushFloat(root));
      }
    }
  }

  preparedEquation->removeTree();
  return {resultList, metadata};
}

EquationSolver::Error EquationSolver::ProjectAndReduce(
    Tree* equationsSet, ProjectionContext projectionContext) {
  assert(projectionContext.m_advanceReduce);
  Simplification::ProjectAndReduce(equationsSet, &projectionContext);
  if (projectionContext.m_dimension.isUnit()) {
    return Error::EquationUndefined;
  }
  if (!equationsSet->isList()) {
    return Error::EquationUndefined;
  }
  Error error = Error::NoError;
  for (const Tree* equation : equationsSet->children()) {
    if (!equation->isUndefined()) {
      continue;
    }
    if (!equation->isNonReal()) {
      return Error::EquationUndefined;
    }
    error = Error::EquationNonReal;
  }
  return error;
}

Tree* EquationSolver::SolveLinearSystem(const Tree* reducedEquationSet,
                                        uint8_t n, SolutionMetadata* metadata) {
  metadata->solvingMethod = SolvingMethod::LinearSystem;
  metadata->degree = 1;

  // Solve without dependencies
  Tree* equationSetWithoutDep =
      SharedTreeStack->pushSet(reducedEquationSet->numberOfChildren());
  for (const Tree* equation : reducedEquationSet->children()) {
    Dependency::SafeMain(equation)->cloneTree();
  }

  // n unknown variables and rows equations
  uint8_t cols = n + 1;
  uint8_t rows = reducedEquationSet->numberOfChildren();
  TreeRef matrix = SharedTreeStack->pushMatrix(0, 0);
  int m = reducedEquationSet->numberOfChildren();

  // Create the matrix (A|b) for the equation Ax=b;
  for (const Tree* equation : equationSetWithoutDep->children()) {
    Tree* coefficients = GetLinearCoefficients(equation, n);
    if (!coefficients) {
      metadata->error = Error::NonLinearSystem;
      matrix->removeTree();
      equationSetWithoutDep->removeTree();
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
    metadata->error = Error::EquationUndefined;
    matrix->removeTree();
    equationSetWithoutDep->removeTree();
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
      /* Row describes an equation of the form '0=b', the system has no
       * solution. */
      matrix->removeTree();
      equationSetWithoutDep->removeTree();
      assert(metadata->error == Error::NoError);
      return SharedTreeStack->pushSet(0);
    }
    coefficient = coefficient->nextTree();
  }

  if (rank != n || n <= 0) {
#if POINCARE_NO_INFINITE_SYSTEMS
    (void)m;
    metadata->solutionType = SolutionType::Formal;
    matrix->removeTree();
    equationSetWithoutDep->removeTree();
    assert(*error == Error::NoError);
    return SharedTreeStack->pushSet(0);
#else
    /* The system is insufficiently qualified: bind the value of n-rank
     * variables to parameters. */
    metadata->solutionType = SolutionType::Formal;
    int variable = n - 1;
    int row = m - 1;
    int firstVariableInRow = -1;
    int numberOfUnknowns = metadata->unknownVariables.numberOfVariables();
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
      // Push a finite variable starting from numberOfUnknowns
      SharedTreeStack->pushVar(numberOfUnknowns++, ComplexSign::Finite());
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
      metadata->error = Error::EquationUndefined;
      matrix->removeTree();
      equationSetWithoutDep->removeTree();
      return nullptr;
    }
#endif
  } else {
    metadata->solutionType = SolutionType::Exact;
  }
  assert(rank == n);

  /* The rank is equal to the number of variables: the system has n
   * solutions, and after canonization their values are the first n values on
   * the last column. */
  TreeRef equationSetClone = reducedEquationSet->cloneTree();
  Tree* child = matrix->child(0);
  for (uint8_t row = 0; row < rows; row++) {
    for (uint8_t col = 0; col < cols; col++) {
      if (row < n && col == cols - 1) {
        /* Replace the solution in the equation set to check later that it
         * respects the dependencies. */
        Variables::Replace(equationSetClone, row, child);
        // Child has already been reduced.
        assert(!Simplification::ReduceSystem(child, false));
        // Continue anyway to preserve TreeStack integrity
        child = child->nextTree();
      } else {
        child->removeTree();
      }
    }
  }
  matrix->moveNodeOverNode(SharedTreeStack->pushSet(n));
  Dependency::DeepRemoveUselessDependencies(equationSetClone);

  // Make sure the solution satisfies dependencies in equations
  assert(equationSetClone->isList());
  for (const Tree* equation : equationSetClone->children()) {
    if (equation->isUndefined()) {
      equationSetClone->removeTree();
      matrix->removeTree();
      equationSetWithoutDep->removeTree();
      assert(metadata->error == Error::NoError);
      return SharedTreeStack->pushSet(0);
    }
    if (equation->isDep()) {
      // Approximate if the equation is monovariable and different from 0=0
      metadata->error = (n == 1 && !Dependency::Main(equation)->isZero())
                            ? Error::RequireApproximateSolution
                            : Error::EquationUnhandled;
      equationSetClone->removeTree();
      matrix->removeTree();
      equationSetWithoutDep->removeTree();
      return nullptr;
    }
  }
  equationSetClone->removeTree();

  equationSetWithoutDep->removeTree();
  assert(metadata->error == Error::NoError);
  return matrix;
}

Tree* EquationSolver::GetLinearCoefficients(const Tree* equation,
                                            uint8_t numberOfVariables) {
  TreeRef result = SharedTreeStack->pushList(0);
  TreeRef eq = equation->cloneTree();
  /* TODO: y*(1+x) is not handled by PolynomialParser. We expand everything as
   * temporary workaround. */
  SystematicReduction::DeepReduce(eq);
  AdvancedReduction::DeepExpandAlgebraic(eq);
  Dependency::DeepRemoveUselessDependencies(eq);
  /* TODO If [eq] still has dependency, they will not be handled by Parse */
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
    if (Variables::HasVariables(polynomial) ||
        (i == numberOfVariables - 1 && Variables::HasVariables(eq))) {
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
                                      uint8_t n, SolutionMetadata* metadata) {
  assert(simplifiedEquationSet->isList() &&
         simplifiedEquationSet->numberOfChildren() == 1);
  assert(n == 1);
  Tree* equation = simplifiedEquationSet->child(0)->cloneTree();
  // TODO: expansion should be done only once
  SystematicReduction::DeepReduce(equation);
  AdvancedReduction::DeepExpandAlgebraic(equation);
  Dependency::DeepRemoveUselessDependencies(equation);
  // Solve without dependencies
  Tree* equationWithoutDep = Dependency::SafeMain(equation)->cloneTree();
  Tree* polynomial = PolynomialParser::Parse(
      equationWithoutDep, Variables::Variable(0, ComplexSign::Finite()));
  if (!polynomial) {
    metadata->error = Error::RequireApproximateSolution;
    SharedTreeStack->dropBlocksFrom(equation);
    return nullptr;
  }

  const Tree* coefficients[Polynomial::k_maxNumberOfPolynomialCoefficients] =
      {};
  int degree = Polynomial::Degree(polynomial);
  if (degree > Polynomial::k_maxPolynomialDegree) {
    metadata->error = Error::RequireApproximateSolution;
    SharedTreeStack->dropBlocksFrom(equation);
    return nullptr;
  }
  metadata->solvingMethod = SolvingMethod::PolynomialMonovariable;
  metadata->degree = degree;

  int numberOfTerms = Polynomial::NumberOfTerms(polynomial);
  const Tree* coefficient = Polynomial::LeadingCoefficient(polynomial);
  for (int i = 0; i < numberOfTerms; i++) {
    int exponent = Polynomial::ExponentAtIndex(polynomial, i);
    if (exponent < Polynomial::k_maxNumberOfPolynomialCoefficients) {
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

  // Enhance solutions and make sure they satisfy dependencies in the equation
  assert(solutionList->isList());
  size_t numberOfSolutions = solutionList->numberOfChildren();
  Tree* solution = solutionList->nextNode();  // solutionList could be empty
  TreeRef end = pushEndMarker(solutionList);
  while (solution != end) {
    if (equation->isDep()) {
      /* Replace the solution in the equation and check that all dependencies
       * are resolved.
       * For optimization, replace the main expression with zero
       * because we only want to check dependencies. */
      Tree* clonedEquation = SharedTreeStack->pushDep();
      (0_e)->cloneNode();
      Dependency::Dependencies(equation)->cloneTree();
      Variables::Replace(clonedEquation, 0, solution);
      Dependency::DeepRemoveUselessDependencies(clonedEquation);
      bool invalidSolution = clonedEquation->isUndefined();
      bool remainingDependency = clonedEquation->isDep();
      clonedEquation->removeTree();
      if (invalidSolution) {
        solution->removeTree();
        numberOfSolutions--;
        continue;
      }
      if (remainingDependency) {
        metadata->error = Error::RequireApproximateSolution;
        SharedTreeStack->dropBlocksFrom(equation);
        return nullptr;
      }
    }
    // solution has already been reduced.
    assert(!Simplification::ReduceSystem(solution, false));
    solution = solution->nextTree();
  }
  removeMarker(end);
  NAry::SetNumberOfChildren(solutionList, numberOfSolutions);
  equation->removeTree();

  NAry::AddChild(solutionList, discriminant);
  metadata->error = Error::NoError;
  return solutionList;
}

uint32_t EquationSolver::TagParametersUsedAsVariables(VariableArray variables) {
  uint32_t tags = 0;
  constexpr size_t k_maxIndex = OMG::BitHelper::numberOfBitsIn(tags);
  constexpr size_t k_maxNumberOfDigits =
      OMG::Print::LengthOfUInt32(OMG::Base::Decimal, k_maxIndex);
  /* Only check local variables that may not have a global definition. The
   * others  will be checked for later. */
  for (int i = 0; i < variables.numberOfVariables(); i++) {
    // Set the k-th bit in tags if name == "t{k}" and 0th if name is "t"
    const char* variable = variables.variable(i);
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

Tree* EquationSolver::GetNextParameterSymbol(size_t* parameterIndex,
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
    if (!context->expressionForUserNamed(symbol)) {
      return symbol;
    }
    // Skip already used parameter indices in global variables
    symbol->removeTree();
  }
  OMG::unreachable();
}

Tree* EquationSolver::PrepareEquationForApproximateSolve(
    const Tree* equation, ProjectionContext projectionContext,
    SolutionMetadata* metadata) {
  Tree* equationClone = equation->cloneTree();

  projectionContext.m_symbolic =
      (metadata->overrideDefinedVariables
           ? SymbolicComputation::ReplaceDefinedFunctions
           : SymbolicComputation::ReplaceDefinedSymbols);
  // Reduce and replace user variables if needed
  Projection::UpdateComplexFormatWithExpressionInput(equationClone,
                                                     &projectionContext);

  metadata->complexFormat = projectionContext.m_complexFormat;
  Simplification::ProjectAndReduce(equationClone, &projectionContext);

  // Find remaining variable
  Internal::Tree* variables = Variables::GetUserSymbols(equationClone);
  assert(variables->numberOfChildren() == 1);
  metadata->unknownVariables.fillWithList(variables);
  variables->removeTree();

  Approximation::PrepareFunctionForApproximation(
      equationClone, metadata->unknownVariables.variable(0),
      Preferences::ComplexFormat::Real);
  return equationClone;
}

}  // namespace Poincare::Internal
