#include "equation_solver.h"

#include <poincare/equation_solver/equation_solver.h>
#include <poincare/helpers/polynomial.h>
#include <poincare/numeric_solver/solver.h>
#include <poincare/numeric_solver/zoom.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/expression/matrix.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_helpers.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/src/numeric_solver/roots.h>
#include <poincare/symbol_context.h>

namespace Poincare::EquationSolver {

void VariableArray::fillWithList(const Internal::Tree* list) {
  assert(list->isList() && list->numberOfChildren() <= capacity());
  clear();
  for (const Internal::Tree* variable : list->children()) {
    push(SymbolHelper::GetName(variable));
  }
}

}  // namespace Poincare::EquationSolver

namespace Poincare::Internal::EquationSolver {
using namespace Poincare::EquationSolver;

enum class UnknownSelectionStrategy : uint8_t {
  // Only includes symbols that are not defined by the user
  OnlyUndefinedSymbols,
  // Includes all symbols, even ones defined by the user
  AllSymbols,
  /* Choses whether or not to override defined variables in order to have only
   * one unknown at the end */
  MaxOneSymbol,
};

struct PreprocessingResult {
  Tree* reducedEquationList = nullptr;
  Error error = Error::NoError;
  EquationMetadata equationMetadata;
};

constexpr static char k_parameterPrefix = 't';

/* This is used by both ExactSolve and ApproximateSolve.
 * It computes the EquationMetadata, and reduces the equation list. */
static PreprocessingResult PreprocessEquationList(
    const Tree* equationList, ProjectionContext* projectionContext,
    UnknownSelectionStrategy selectionStrategy) {
  assert(equationList->isList());

  EquationMetadata equationMetadata;

  // Step 1. Retrieve user symbols and infer the list of unknowns
  const SymbolContext& symbolContext = projectionContext->m_context;
  Tree* userSymbols = Variables::GetUserSymbols(equationList, symbolContext);
  assert(userSymbols->isList());

  VariableArray undefinedVariables;
  /* The total number can be over the capacity of VariableArray. Either:
   * - The total number of unknowns (undefined + eventually defined) overloads
   * the capacity of VariableArray. In this case the solving will fail with the
   * error TooManyVariables.
   * - Only the number of defined variables overloads the capacity of
   * VariableArray. In this case the solving continues. The array just won't
   * hold the extra defined variables, which is not that bad since it's only
   * used to display to the user which already defined variables were used.
   */
  size_t nDefinedVariables = 0;
  size_t nUndefinedVariables = 0;

  for (Tree* userSymbol : userSymbols->children()) {
    const char* symbolName = Symbol::GetName(userSymbol);
    if (symbolContext.expressionForUserNamed(userSymbol)) {
      if (!equationMetadata.definedVariables.isFull()) {
        equationMetadata.definedVariables.push(symbolName);
      }
      nDefinedVariables++;
    } else {
      if (!undefinedVariables.isFull()) {
        undefinedVariables.push(symbolName);
      }
      nUndefinedVariables++;
    }
  }

  switch (selectionStrategy) {
    case UnknownSelectionStrategy::OnlyUndefinedSymbols:
      equationMetadata.overrideDefinedVariables = false;
      break;
    case UnknownSelectionStrategy::AllSymbols:
      equationMetadata.overrideDefinedVariables =
          equationMetadata.definedVariables.size() > 0;
      break;
    case UnknownSelectionStrategy::MaxOneSymbol:
      equationMetadata.overrideDefinedVariables =
          (undefinedVariables.size() == 0 &&
           equationMetadata.definedVariables.size() == 1);
      break;
    default:
      OMG::unreachable();
  }

  int expectedNumberOfUnknowns =
      nUndefinedVariables +
      (equationMetadata.overrideDefinedVariables ? nDefinedVariables : 0);

  Error error = Error::NoError;
  if (expectedNumberOfUnknowns > k_maxNumberOfExactSolutions ||
      (selectionStrategy == UnknownSelectionStrategy::MaxOneSymbol &&
       expectedNumberOfUnknowns > 1)) {
    error = Error::TooManyVariables;
  } else if ((equationList->numberOfChildren() > 1 ||
              expectedNumberOfUnknowns > 1) &&
             SharedExamMode->forbidSimultaneousEquationSolver()) {
    error = Error::DisabledInExamMode;
  }

  if (error != Error::NoError) {
    userSymbols->removeTree();
    return {.error = error, .equationMetadata = equationMetadata};
  }

  if (equationMetadata.overrideDefinedVariables) {
    equationMetadata.unknownVariables.fillWithList(userSymbols);
  } else {
    equationMetadata.unknownVariables = undefinedVariables;
  }

  assert(equationMetadata.unknownVariables.size() == expectedNumberOfUnknowns);
  userSymbols->removeTree();

  projectionContext->m_symbolic =
      equationMetadata.overrideDefinedVariables
          ? SymbolicComputation::ReplaceDefinedFunctions
          : SymbolicComputation::ReplaceDefinedSymbols;

  /* Step 2. Update complex format from equationList
   * NOTE: This must be done after deciding the symbolic computation mode */

  Projection::UpdateComplexFormatWithExpressionInput(equationList,
                                                     projectionContext);
  equationMetadata.complexFormat = projectionContext->m_complexFormat;

  // Step 3. Clone and simplify the equations

  Tree* reducedEquationList = equationList->cloneTree();

  // Step 3.1. Replace KEqual with KSub
  for (Tree* equation : reducedEquationList->children()) {
    Internal::PatternMatching::MatchReplace(equation, KEqual(KA, KB),
                                            KSub(KA, KB));
  }

  /* The symbols should be replaced in this order :
   *  - User functions (because they can hide UserSymbols and local symbols)
   *  - UserSymbols into unkowns (because they have a different complexSign)
   *  - Local symbols into variables (Done in ProjectAndReduce)
   *  - Reduction (Done in ProjectAndReduce). */

  // Step 3.2. Replace UserFunctions
  Projection::DeepReplaceUserNamed(
      reducedEquationList, symbolContext,
      SymbolicComputation::ReplaceDefinedFunctions);

  // Step 3.3. Replace unkowns

  for (int i = 0; i < equationMetadata.unknownVariables.size(); i++) {
    const char* variable = equationMetadata.unknownVariables[i];
    // TODO: Use a more precise complexSign when possible for better reduction.
    Variables::ReplaceSymbol(reducedEquationList, variable, i,
                             ComplexSign::Finite());
  }

  // Step 3.4. Project (replace local symbols) and reduce

  Simplification::ProjectAndReduce(reducedEquationList, projectionContext);
  if (projectionContext->m_dimension.isUnit()) {
    error = Error::EquationUndefined;
  } else if (!reducedEquationList->isList()) {
    error = Error::EquationUndefined;
  } else {
    for (const Tree* equation : reducedEquationList->children()) {
      if (equation->isUndefined()) {
        if (equation->isNonReal()) {
          error = Error::EquationNonReal;
          continue;  // Continue in case another equation is undefined
        }
        error = Error::EquationUndefined;
        break;
      }
    }
  }
  if (error != Error::NoError) {
    reducedEquationList->removeTree();
    return {.error = error, .equationMetadata = equationMetadata};
  }

  return {.reducedEquationList = reducedEquationList,
          .equationMetadata = equationMetadata};
}

/* Return list of linear coefficients for each variables and final constant. */
static Tree* GetLinearCoefficients(const Tree* equation,
                                   uint8_t numberOfVariables) {
  TreeRef result = List::PushEmpty();
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

static uint32_t TagParametersUsedAsVariables(VariableArray variables) {
  uint32_t tags = 0;
  constexpr size_t k_maxIndex = OMG::BitHelper::numberOfBitsIn(tags);
  constexpr size_t k_maxNumberOfDigits =
      OMG::Print::LengthOfUInt32(OMG::Base::Decimal, k_maxIndex);
  /* Only check local variables that may not have a global definition. The
   * others  will be checked for later. */
  for (int i = 0; i < variables.size(); i++) {
    // Set the k-th bit in tags if name == "t{k}" and 0th if name is "t"
    const char* variable = variables[i];
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

/* Return the userSymbol for the next additional parameter variable. */
static Tree* GetNextParameterSymbol(size_t* parameterIndex,
                                    uint32_t usedParameterIndices,
                                    const SymbolContext& symbolContext) {
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
    if (!symbolContext.expressionForUserNamed(symbol)) {
      return symbol;
    }
    // Skip already used parameter indices in global variables
    symbol->removeTree();
  }
  OMG::unreachable();
}

/* Checks if the equationList is a linear system, and if so, computes the
 * solutions. The equationMetada passed as arguments are contained in the
 * returned SolverResult.  */
static SolverResult SolveLinearSystem(const Tree* reducedEquationList,
                                      const EquationMetadata& equationMetadata,
                                      const SymbolContext& symbolContext) {
  SolutionMetadata solutionMetadata{
      .solvingMethod = SolvingMethod::LinearSystem,
      .solutionType = SolutionType::Exact,
      .degree = 1,
  };

  // Solve without dependencies
  Tree* equationListWithoutDep =
      SharedTreeStack->pushList(reducedEquationList->numberOfChildren());
  for (const Tree* equation : reducedEquationList->children()) {
    Dependency::SafeMain(equation)->cloneTree();
  }

  // n unknown variables and rows equations
  int n = equationMetadata.unknownVariables.size();
  uint8_t cols = n + 1;
  uint8_t rows = reducedEquationList->numberOfChildren();
  TreeRef matrix = SharedTreeStack->pushMatrix(0, 0);
  int m = reducedEquationList->numberOfChildren();

  // Create the matrix (A|b) for the equation Ax=b;
  for (const Tree* equation : equationListWithoutDep->children()) {
    Tree* coefficients = GetLinearCoefficients(equation, n);
    if (!coefficients) {
      matrix->removeTree();
      equationListWithoutDep->removeTree();
      return {
          .error = Error::NonLinearSystem,
          .equationMetadata = equationMetadata,
          .solutionMetadata = solutionMetadata,
      };
    }
    assert(coefficients->numberOfChildren() == cols);
    // Invert constant because Ax=b is represented by Ax-b
    Tree* constant = coefficients->lastChild();
    PatternMatching::MatchReplaceReduce(constant, KA, KMult(-1_e, KA));
    coefficients->removeNode();
    Matrix::SetNumberOfColumns(matrix, cols);
    Matrix::SetNumberOfRows(matrix, Matrix::NumberOfRows(matrix) + 1);
  }
  assert(Matrix::NumberOfRows(matrix) == rows);
  // Compute the rank of (A|b)
  int rank = Matrix::CanonizeAndRank(matrix);
  if (rank == Matrix::k_failedToCanonizeRank) {
    matrix->removeTree();
    equationListWithoutDep->removeTree();
    return {
        .error = Error::EquationUndefined,
        .equationMetadata = equationMetadata,
        .solutionMetadata = solutionMetadata,
    };
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
      equationListWithoutDep->removeTree();
      return {.exactSolutionList = List::PushEmpty(),
              .equationMetadata = equationMetadata,
              .solutionMetadata = solutionMetadata};
    }
    coefficient = coefficient->nextTree();
  }

  size_t firstExtraVariableId = equationMetadata.unknownVariables.size();
  size_t numberOfExtraVariables = 0;

  if (rank != n || n <= 0) {
    solutionMetadata.solutionType = SolutionType::Formal;
#if POINCARE_NO_INFINITE_SYSTEMS
    (void)m;
    matrix->removeTree();
    equationListWithoutDep->removeTree();
    return {.exactSolutionList = List::PushEmpty(),
            .equationMetadata = equationMetadata,
            .solutionMetadata = solutionMetadata};
#else
    /* The system is insufficiently qualified: bind the value of n-rank
     * variables to parameters. */
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
      // Push a finite variable starting from firstExtraVariableId
      SharedTreeStack->pushVar(firstExtraVariableId + numberOfExtraVariables,
                               ComplexSign::Finite());
      numberOfExtraVariables++;
      rows = m + 1;
      Matrix::SetDimensions(matrix, ++m, n + 1);
      variable--;
    }

    /* forceCanonization = true so that canonization still happens even if
     * t.approximate() is NAN. If other children of ab had an undef
     * approximation, the resolution would already have failed at
     * reduction of the equation list. */
    rank = Matrix::CanonizeAndRank(matrix, true);
    if (rank == Matrix::k_failedToCanonizeRank) {
      matrix->removeTree();
      equationListWithoutDep->removeTree();
      return {.error = Error::EquationUndefined,
              .equationMetadata = equationMetadata,
              .solutionMetadata = solutionMetadata};
    }
#endif
  }
  assert(rank == n);

  /* The rank is equal to the number of variables: the system has n
   * solutions, and after canonization their values are the first n values on
   * the last column. */
  TreeRef equationListClone = reducedEquationList->cloneTree();
  Tree* child = matrix->child(0);
  for (uint8_t row = 0; row < rows; row++) {
    for (uint8_t col = 0; col < cols; col++) {
      if (row < n && col == cols - 1) {
        /* Replace the solution in the equation list to check later that it
         * respects the dependencies. */
        Variables::Replace(equationListClone, row, child);
        /* Reduce the solution. RowCanonize only applies systematic reduction,
         * so we need to apply advanced reduction.
         * TODO: Should rowCanonize advance reduce ? */
        assert(!Simplification::ReduceSystem(child, false));
        Simplification::ReduceSystem(child, true);
        // Continue to preserve TreeStack integrity
        child = child->nextTree();
      } else {
        child->removeTree();
      }
    }
  }
  matrix->moveNodeOverNode(SharedTreeStack->pushList(n));
  Dependency::DeepRemoveUselessDependencies(equationListClone);

  // Make sure the solution satisfies dependencies in equations
  assert(equationListClone->isList());
  for (const Tree* equation : equationListClone->children()) {
    if (equation->isUndefined()) {
      equationListClone->removeTree();
      matrix->removeTree();
      equationListWithoutDep->removeTree();
      return {.exactSolutionList = List::PushEmpty(),
              .equationMetadata = equationMetadata,
              .solutionMetadata = solutionMetadata};
    }
    if (equation->isDep()) {
      // Approximate if the equation is monovariable and different from 0=0
      Error error = (n == 1 && !Dependency::Main(equation)->isZero())
                        ? Error::RequireApproximateSolution
                        : Error::EquationUnhandled;
      equationListClone->removeTree();
      matrix->removeTree();
      equationListWithoutDep->removeTree();
      return {.error = error,
              .equationMetadata = equationMetadata,
              .solutionMetadata = solutionMetadata};
    }
  }
  equationListClone->removeTree();
  equationListWithoutDep->removeTree();

  // If the solution is formal, replace the extra variables with t, t1, t2, etc.
  if (numberOfExtraVariables > 0) {
    // Start at 0 ("t") instead of 1 ("t1") if there is only one variable
    size_t parameterIndex = (numberOfExtraVariables > 1) ? 1 : 0;
    uint32_t usedParameterIndices =
        TagParametersUsedAsVariables(equationMetadata.unknownVariables);
    size_t lastExtraVariableId = firstExtraVariableId + numberOfExtraVariables;
    for (int j = firstExtraVariableId; j < lastExtraVariableId; j++) {
      // Generate a unique identifier t? that does not collide with variables.
      TreeRef symbol = GetNextParameterSymbol(
          &parameterIndex, usedParameterIndices, symbolContext);
      Variables::Replace(matrix, j, symbol, false, false);
      symbol->removeTree();
    }
  }

  return {.exactSolutionList = matrix,
          .equationMetadata = equationMetadata,
          .solutionMetadata = solutionMetadata};
}

#if POINCARE_POLYNOMIAL_SOLVER
/* Checks if the equationList is a deg 2 or 3 polynomial, and if so, computes
 * the solutions. The equationMetada passed as arguments are contained in the
 * returned SolverResult. */
static SolverResult SolvePolynomial(const Tree* simplifiedEquationList,
                                    const EquationMetadata& equationMetadata) {
  assert(simplifiedEquationList->isList() &&
         simplifiedEquationList->numberOfChildren() == 1);

  SolutionMetadata solutionMetadata{
      .solvingMethod = SolvingMethod::PolynomialMonovariable,
      .solutionType = SolutionType::Exact,
  };

  Tree* equation = simplifiedEquationList->child(0)->cloneTree();
  // TODO: expansion should be done only once
  SystematicReduction::DeepReduce(equation);
  AdvancedReduction::DeepExpandAlgebraic(equation);
  Dependency::DeepRemoveUselessDependencies(equation);
  // Solve without dependencies
  Tree* equationWithoutDep = Dependency::SafeMain(equation)->cloneTree();
  Tree* polynomial = PolynomialParser::Parse(
      equationWithoutDep, Variables::Variable(0, ComplexSign::Finite()));
  if (!polynomial) {
    SharedTreeStack->dropBlocksFrom(equation);
    return {.error = Error::RequireApproximateSolution,
            .equationMetadata = equationMetadata,
            .solutionMetadata = solutionMetadata};
  }

  const Tree* coefficients[PolynomialHelpers::NumberOfCoefficients(
      PolynomialHelpers::k_maxSolvableDegree)] = {};
  int degree = Polynomial::Degree(polynomial);
  if (degree > PolynomialHelpers::k_maxSolvableDegree) {
    SharedTreeStack->dropBlocksFrom(equation);
    return {.error = Error::RequireApproximateSolution,
            .equationMetadata = equationMetadata,
            .solutionMetadata = solutionMetadata};
  }

  int numberOfTerms = Polynomial::NumberOfTerms(polynomial);
  const Tree* coefficient = Polynomial::LeadingCoefficient(polynomial);
  for (int i = 0; i < numberOfTerms; i++) {
    int exponent = Polynomial::ExponentAtIndex(polynomial, i);
    if (exponent < PolynomialHelpers::NumberOfCoefficients(
                       PolynomialHelpers::k_maxSolvableDegree)) {
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

  // The quadratic / cubic solver may have failed to find exact solutions
  if (solutionList->isUndefined()) {
    SharedTreeStack->dropBlocksFrom(equation);
    return {.error = Error::RequireApproximateSolution,
            .equationMetadata = equationMetadata,
            .solutionMetadata = solutionMetadata};
  }

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
        SharedTreeStack->dropBlocksFrom(equation);
        return {.error = Error::RequireApproximateSolution,
                .equationMetadata = equationMetadata,
                .solutionMetadata = solutionMetadata};
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
  solutionMetadata.degree = degree;
  return {.exactSolutionList = solutionList,
          .equationMetadata = equationMetadata,
          .solutionMetadata = solutionMetadata};
}
#endif

SolverResult ExactSolveAdaptive(const Tree* equationList,
                                ProjectionContext projectionContext) {
  // Try solving while using user variables
  SolverResult firstResult = ExactSolve(equationList, projectionContext, false);

  Error firstError = firstResult.error;
  size_t nDefinedVariables =
      firstResult.equationMetadata.definedVariables.size();

  /* Retry with overriden variables if:
   *     - There are variables to override (nDefinedVariables > 0)
   * AND -    There is no error but no solution was found
   *       OR The equation is undefined or non real or unhandled
   *
   * For every other errors, overriding the variables shouldn't change anything.
   */
  bool retryWithOverridenVariables =
      nDefinedVariables > 0 &&
      ((firstError == Error::NoError &&
        firstResult.exactSolutionList->numberOfChildren() == 0) ||
       firstError == Error::EquationUndefined ||
       firstError == Error::EquationNonReal ||
       firstError == Error::EquationUnhandled);

  if (!retryWithOverridenVariables) {
    return firstResult;
  }

  // Try solving while overriding user variables
  SolverResult secondResult = ExactSolve(equationList, projectionContext, true);
  Error secondError = secondResult.error;

  if (firstError == Error::NoError && secondError != Error::NoError &&
      secondError != Error::RequireApproximateSolution) {
    assert(!secondResult.exactSolutionList &&
           !secondResult.approximateSolutionList &&
           firstResult.exactSolutionList);
    // Use the first result empty solution list
    return firstResult;
  }

  if (firstResult.exactSolutionList) {
    firstResult.exactSolutionList->removeTree();
  }
  if (firstResult.approximateSolutionList) {
    firstResult.approximateSolutionList->removeTree();
  }

  return secondResult;
}

SolverResult ExactSolve(const Tree* equationList,
                        ProjectionContext projectionContext,
                        bool overrideDefinedVariables) {
  // Step 1. Analyze the equations
  PreprocessingResult preprocessingResult = PreprocessEquationList(
      equationList, &projectionContext,
      overrideDefinedVariables
          ? UnknownSelectionStrategy::AllSymbols
          : UnknownSelectionStrategy::OnlyUndefinedSymbols);

  Tree* reducedEquationList = preprocessingResult.reducedEquationList;
  EquationMetadata equationMetadata = preprocessingResult.equationMetadata;
  projectionContext.m_complexFormat =
      preprocessingResult.equationMetadata.complexFormat;

  if (preprocessingResult.error != Error::NoError) {
    /* If the analysis failed, return an empty solution list */
    assert(reducedEquationList == nullptr);
    return {
        .error = preprocessingResult.error,
        .equationMetadata = equationMetadata,
    };
  }

  // Step 2. Solve the equations

  // Step 2.1. Try with linear system solving
  SolverResult result = SolveLinearSystem(reducedEquationList, equationMetadata,
                                          projectionContext.m_context);

#if POINCARE_POLYNOMIAL_SOLVER
  // Step 2.2. Try with polynomial solving
  if (result.error == Error::NonLinearSystem &&
      equationMetadata.unknownVariables.size() <= 1 &&
      reducedEquationList->numberOfChildren() <= 1) {
    assert(!result.exactSolutionList && !result.approximateSolutionList);
    result = SolvePolynomial(reducedEquationList, equationMetadata);

    if (result.error == Error::NoError) {
      /* Remove non real solutions of a polynomial if the equation was
       * projected with a "Real" Complex format */
      Tree* solutionList = result.exactSolutionList;
      assert(solutionList->isList());
      if (projectionContext.m_complexFormat == ComplexFormat::Real) {
        for (int i = solutionList->numberOfChildren() - 1; i >= 0; i--) {
          if (!SignOfTreeOrApproximation(solutionList->child(i)).isReal()) {
            NAry::RemoveChildAtIndex(solutionList, i);
          }
        }
      }
    }
  }
#endif

  reducedEquationList->removeTree();

  // Step 3. Handle the result

  if (result.error == Error::NonLinearSystem ||
      result.error == Error::RequireApproximateSolution ||
      !result.exactSolutionList) {
    // TODO: Handle GeneralMonovariable solving.
    if (result.exactSolutionList) {
      result.exactSolutionList->removeTree();
    }
    // Reset solution metadata
    result.solutionMetadata = {};
    return result;
  }

  /* Approximate */

  if (result.solutionMetadata.solutionType != SolutionType::Formal) {
    Approximation::Context approxCtx(projectionContext.m_angleUnit,
                                     projectionContext.m_complexFormat,
                                     projectionContext.m_context);
    result.approximateSolutionList = Approximation::ToTree<double>(
        result.exactSolutionList,
        Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                  .prepare = true},
        approxCtx);
  }

  /* Beautify result */

  // Beautify exact solutions
  assert(result.exactSolutionList);
  Simplification::BeautifyReduced(result.exactSolutionList, &projectionContext);

  // Beautify approximate solutions
  if (result.approximateSolutionList) {
    // Advanced reduction is only needed to beautify in polar format
    projectionContext.m_advanceReduce =
        projectionContext.m_complexFormat == ComplexFormat::Polar;
    Simplification::BeautifyReduced(result.approximateSolutionList,
                                    &projectionContext);
  }

  return result;
}

template <typename T>
static Coordinate2D<T> evaluator(T t, const void* model) {
  const Tree* e = reinterpret_cast<const Tree*>(model);
  return Coordinate2D<T>(
      t, Approximation::To<T>(
             e, t, Approximation::Parameters{.isRootAndCanHaveRandom = true}));
}

SolverResult ApproximateSolve(const Tree* equationList,
                              ProjectionContext projectionContext,
                              Range1D<double> range,
                              size_t maxNumberOfSolutions) {
  assert(equationList->isList());

  if (equationList->numberOfChildren() > 1) {
    return {.error = Error::EquationUnhandled};
  }

  // Step 1. Analyze the equations
  PreprocessingResult preprocessingResult = PreprocessEquationList(
      equationList, &projectionContext, UnknownSelectionStrategy::MaxOneSymbol);

  Tree* reducedEquationList = preprocessingResult.reducedEquationList;
  EquationMetadata equationMetadata = preprocessingResult.equationMetadata;

  if (preprocessingResult.error != Error::NoError) {
    assert(reducedEquationList == nullptr);
    return {
        .error = preprocessingResult.error,
        .equationMetadata = equationMetadata,
    };
  }

  SolutionMetadata solutionMetadata{
      .solvingMethod = SolvingMethod::GeneralMonovariable,
      .solutionType = SolutionType::Approximate,
  };

  assert(reducedEquationList->isList() &&
         reducedEquationList->numberOfChildren() == 1);
  Tree* preparedEquation = reducedEquationList->child(0);

  assert(equationMetadata.unknownVariables.size() == 1);
  Approximation::PrepareFunctionForApproximation(
      preparedEquation, equationMetadata.unknownVariables[0],
      ComplexFormat::Real);

  // Step 2. Compute the solving range if not provided
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
    zoom.setMaxPointsOneSide(maxNumberOfSolutions, maxNumberOfSolutions / 2);
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
      solutionMetadata.incompleteSolutions = true;
    }
  }

  // Step 3. Compute the solutions
  assert(range.isValid());
  solutionMetadata.solvingRange = range;
  Solver<double> solver = Poincare::Solver<double>(range.min(), range.max());
  solver.stretch();

  TreeRef resultList = List::PushEmpty();

  for (int i = 0; i <= maxNumberOfSolutions; i++) {
    double root = solver.nextRoot(preparedEquation).x();
    if (root < range.min()) {
      i--;
      continue;
    } else if (root > range.max()) {
      root = NAN;
    }

    if (i == maxNumberOfSolutions) {
      solutionMetadata.incompleteSolutions = true;
    } else {
      if (std::isnan(root)) {
        break;
      }
      if (std::isfinite(root)) {
        NAry::AddChild(resultList, SharedTreeStack->pushFloat(root));
      }
    }
  }

  reducedEquationList->removeTree();
  // exactSolutionList is nullptr
  return {.approximateSolutionList = resultList,
          .equationMetadata = equationMetadata,
          .solutionMetadata = solutionMetadata};
}

}  // namespace Poincare::Internal::EquationSolver
