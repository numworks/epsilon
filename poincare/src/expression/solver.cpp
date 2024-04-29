#include "solver.h"

#include <poincare/src/memory/exception_checkpoint.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_ref.h>

#include "advanced_simplification.h"
#include "beautification.h"
#include "matrix.h"
#include "polynomial.h"
#include "set.h"
#include "sign.h"
#include "simplification.h"
#include "symbol.h"
#include "variables.h"

namespace Poincare::Internal {

Tree* Solver::ExactSolve(const Tree* equationsSet, Context* context,
                         ProjectionContext projectionContext, Error* error) {
  context->overrideUserVariables = false;
  projectionContext.m_symbolic =
      SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  Tree* result =
      PrivateExactSolve(equationsSet, context, projectionContext, error);
  if (*error == Error::RequireApproximateSolution ||
      (*error == Error::NoError && result->numberOfChildren() > 0)) {
    return result;
  }
  assert((result == nullptr) || (result->numberOfChildren() == 0));

  Error secondError = Error::NoError;
  context->overrideUserVariables = true;
  projectionContext.m_symbolic = SymbolicComputation::DoNotReplaceAnySymbol;
  result =
      PrivateExactSolve(equationsSet, context, projectionContext, &secondError);
  if (*error != Error::NoError || secondError == Error::NoError ||
      secondError == Error::RequireApproximateSolution) {
    *error = secondError;
  } else {
    assert(!result);
    context->overrideUserVariables = false;
    if (*error == Error::NoError) {
      /* The system becomes invalid when overriding the user variables: the
       * first solution was better. Restore inital empty set */
      result = SharedTreeStack->push<Type::Set>(0);
    }
  }
  return result;
}

/* TODO:
 * - Implement a number of variable limit (Error::TooManyVariables).
 * - Catch Undefined and Nonreal simplified equations (Error::EquationNonreal
 *   and Error::EquationUndefined).
 * - Project equality into subtraction.  */
Tree* Solver::PrivateExactSolve(const Tree* equationsSet, Context* context,
                                ProjectionContext projectionContext,
                                Error* error) {
  /* Clone and simplify the equations */
  Tree* simplifiedEquationSet = equationsSet->clone();
  ProjectAndSimplify(simplifiedEquationSet, projectionContext, error);
  if (*error != Error::NoError) {
    simplifiedEquationSet->removeTree();
    return nullptr;
  }

  /* Count and collect used and replaced UserSymbols */
  Tree* userSymbols = Variables::GetUserSymbols(simplifiedEquationSet);
  uint8_t numberOfVariables = userSymbols->numberOfChildren();
  Tree* replacedSymbols = Set::Difference(
      Variables::GetUserSymbols(equationsSet), userSymbols->clone());
  if (replacedSymbols->numberOfChildren() > 0) {
    int i = 0;
    for (const Tree* variable : replacedSymbols->children()) {
      Symbol::CopyName(variable, context->userVariables[i++],
                       Symbol::k_maxNameSize);
    }
  }
  replacedSymbols->removeTree();

  /* Replace UserSymbols with variables for easier solution handling */
  SwapTreesPointers(&simplifiedEquationSet, &userSymbols);
  int i = 0;
  for (const Tree* variable : userSymbols->children()) {
    Variables::ReplaceSymbol(simplifiedEquationSet, variable, i++,
                             ComplexSign::Unknown());
  }

  /* Find equation's results */
  TreeRef result;
  assert(*error == Error::NoError);
  result = SolveLinearSystem(simplifiedEquationSet, numberOfVariables, *context,
                             error);
  if (*error == Error::NonLinearSystem && numberOfVariables <= 1 &&
      equationsSet->numberOfChildren() <= 1) {
    assert(result.isUninitialized());
    result = SolvePolynomial(simplifiedEquationSet, numberOfVariables, *context,
                             error);
    if (*error == Error::RequireApproximateSolution) {
      // TODO: Handle GeneralMonovariable solving.
      assert(result.isUninitialized());
    }
  }
  simplifiedEquationSet->removeTree();

  /* Replace variables back to UserSymbols */
  if (!result.isUninitialized()) {
    for (const Tree* variable : userSymbols->children()) {
      Variables::LeaveScopeWithReplacement(result, variable, false);
    }
  }
  userSymbols->removeTree();

  /* Beautify result */
  if (!result.isUninitialized()) {
    Beautify(result, projectionContext);
  }

  return result;
}

void Solver::ProjectAndSimplify(Tree* equationsSet,
                                ProjectionContext projectionContext,
                                Error* error) {
  assert(*error == Error::NoError);
  // TODO_PCJ: Factorize this with Simplification::Simplify
  Simplification::PrepareForProjection(equationsSet, projectionContext);
  Simplification::ExtractUnits(equationsSet, &projectionContext);
  if (projectionContext.m_dimension.isUnit()) {
    *error = Error::EquationUndefined;
    return;
  }
  Projection::DeepSystemProject(equationsSet, projectionContext);
  Simplification::SimplifyProjectedTree(equationsSet);
  Simplification::TryApproximationStrategyAgain(equationsSet,
                                                projectionContext);
  if (equationsSet->isUndefined()) {
    *error = equationsSet->isNonReal() ? Error::EquationNonreal
                                       : Error::EquationUndefined;
  }
}

void Solver::Beautify(Tree* equationsSet, ProjectionContext projectionContext) {
  Beautification::DeepBeautify(equationsSet, projectionContext);
}

Tree* Solver::SolveLinearSystem(const Tree* simplifiedEquationSet, uint8_t n,
                                Context context, Error* error) {
  context.exactResults = true;
  // n unknown variables and rows equations
  uint8_t cols = n + 1;
  uint8_t rows = simplifiedEquationSet->numberOfChildren();
  Tree* matrix = SharedTreeStack->push<Type::Matrix>(0, 0);
  // Create the matrix (A|b) for the equation Ax=b;
  for (const Tree* equation : simplifiedEquationSet->children()) {
    Tree* coefficients = GetLinearCoefficients(equation, n, context);
    if (!coefficients) {
      *error = Error::NonLinearSystem;
      matrix->removeTree();
      return nullptr;
    }
    assert(coefficients->numberOfChildren() == cols);
    coefficients->removeNode();
    Matrix::SetNumberOfColumns(matrix, cols);
    Matrix::SetNumberOfRows(matrix, Matrix::NumberOfRows(matrix) + 1);
  }
  assert(Matrix::NumberOfRows(matrix) == rows);
  // Compute the rank of (A|b)
  int rank = Matrix::CanonizeAndRank(matrix);
  if (rank == -1) {
    *error = Error::EquationUndefined;
    matrix->removeTree();
    return nullptr;
  }
  const Tree* coefficient = matrix->child(0);
  for (uint8_t row = 0; row < rows; row++) {
    bool allCoefficientsNull = true;
    for (uint8_t col = 0; col < n; col++) {
      if (allCoefficientsNull && !Sign::Get(coefficient).isZero()) {
        allCoefficientsNull = false;
      }
      coefficient = coefficient->nextTree();
    }
    if (allCoefficientsNull && !Sign::Get(coefficient).isZero()) {
      /* Row j describes an equation of the form '0=b', the system has no
       * solution. */
      matrix->removeTree();
      *error = Error::NoError;
      return SharedTreeStack->push<Type::Set>(0);
    }
    coefficient = coefficient->nextTree();
  }
  if (rank == n && n > 0) {
    /* The rank is equal to the number of variables: the system has n
     * solutions, and after canonization their values are the first n values on
     * the last column. */
    uint8_t variableId = 0;
    Tree* child = matrix->child(0);
    for (uint8_t row = 0; row < rows; row++) {
      for (uint8_t col = 0; col < cols; col++) {
        if (row < n && col == cols - 1) {
          if (*error == Error::NoError) {
            *error = RegisterSolution(child, variableId++, context);
            // Continue anyway to preserve TreeStack integrity
          }
          child = child->nextTree();
        } else {
          child->removeTree();
        }
      }
    }
    matrix->moveNodeOverNode(SharedTreeStack->push<Type::Set>(n));
    return matrix;
  }
  // TODO: Introduce temporary variables to formally solve the system.
  matrix->removeTree();
  *error = Error::TooManyVariables;
  return nullptr;
}

Tree* Solver::GetLinearCoefficients(const Tree* equation,
                                    uint8_t numberOfVariables,
                                    Context context) {
  TreeRef result = SharedTreeStack->push<Type::List>(0);
  TreeRef tree = equation->clone();
  /* TODO: y*(1+x) is not handled by PolynomialParser. We expand everything as
   * temporary workaround. */
  AdvancedSimplification::DeepExpand(tree);
  for (uint8_t i = 0; i < numberOfVariables; i++) {
    // TODO: PolynomialParser::Parse may need to handle more block types.
    // TODO: Use user settings for a RealUnkown sign ?
    Tree* polynomial = PolynomialParser::Parse(
        tree, Variables::Variable(i, ComplexSign::Unknown()));
    if (!polynomial->isPolynomial()) {
      // tree did not depend on variable. Continue.
      tree = polynomial;
      NAry::AddChild(result, SharedTreeStack->push(Type::Zero));
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
    // Update tree to follow [Coeff0] if it exists for next variables.
    tree = nullConstant ? SharedTreeStack->push(Type::Zero)
                        : polynomial->nextTree();
    if (PolynomialParser::ContainsVariable(polynomial) ||
        (i == numberOfVariables - 1 &&
         PolynomialParser::ContainsVariable(tree))) {
      /* The expression can be linear on all coefficients taken one by one but
       * non-linear (ex: xy = 2). We delete the results and return false if one
       * of the coefficients (or last constant term) contains a variable. */
      tree->removeTree();
      polynomial->removeTree();
      result->removeTree();
      return nullptr;
    }
    /* This will detach [Coeff1] into result, leaving tree alone and polynomial
     * properly pilfered. */
    NAry::AddChild(result, polynomial);
  }
  // Constant term is remaining [Coeff0].
  Tree* constant = tree->detachTree();
  NAry::AddChild(result, constant);
  return result;
}

Solver::Error Solver::RegisterSolution(Tree* solution, uint8_t variableId,
                                       Context context) {
  /* TODO:
   * - Implement equations. Here a x=2 solution will register as x-2.
   * - Handle exact results being forbidden.
   * - Pass more context.
   * - Handle Nonreal and Undefined solutions.
   * - Handle approximate display.
   */
  // TODO: Use user settings for a RealUnkown sign ?
  solution->moveTreeBeforeNode(
      SharedTreeStack->push<Type::Var>(variableId, ComplexSign::Unknown()));
  solution->moveNodeBeforeNode(SharedTreeStack->push<Type::Add>(2));
  Simplification::DeepSystematicReduce(solution);
  AdvancedSimplification::AdvancedReduce(solution);
  return Error::NoError;
}

}  // namespace Poincare::Internal
