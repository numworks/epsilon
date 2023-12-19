#include "solver.h"

#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/expression/set.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/expression/variables.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/storage_context.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

Tree* Solver::ExactSolve(const Tree* equationsSet, Context* context,
                         Error* error) {
  context->overrideUserVariables = false;
  Tree* result = PrivateExactSolve(equationsSet, context, error);
  if (*error == Error::RequireApproximateSolution ||
      (*error == Error::NoError && result->numberOfChildren() > 0)) {
    return result;
  }
  assert((result == nullptr) || (result->numberOfChildren() == 0));

  Error secondError = Error::NoError;
  context->overrideUserVariables = true;
  result = PrivateExactSolve(equationsSet, context, &secondError);
  if (*error != Error::NoError || secondError == Error::NoError ||
      secondError == Error::RequireApproximateSolution) {
    *error = secondError;
  } else {
    assert(!result);
    context->overrideUserVariables = false;
    if (*error == Error::NoError) {
      /* The system becomes invalid when overriding the user variables: the
       * first solution was better. Restore inital empty set */
      result = SharedEditionPool->push<BlockType::Set>(0);
    }
  }
  return result;
}

Tree* Solver::PrivateExactSolve(const Tree* equationsSet, Context* context,
                                Error* error) {
  Tree* simplifiedEquationSet = equationsSet->clone();
  if (!context->overrideUserVariables) {
    // Collect replaced user variables in context.
    EditionReference userVariables =
        PolynomialParser::GetVariables(simplifiedEquationSet);
    // Replace user variables before SimplifyAndFindVariables
    StorageContext::DeepReplaceIdentifiersWithTrees(simplifiedEquationSet);
    // Find replaced variables using set difference
    userVariables = Set::Difference(
        userVariables, PolynomialParser::GetVariables(simplifiedEquationSet));
    // Update context
    context->numberOfUserVariables = userVariables->numberOfChildren();
    Tree* userVariable = userVariables->firstChild();
    for (int i = 0; i < context->numberOfUserVariables; i++) {
      Symbol::GetName(userVariable, context->userVariables[i], 10);
      context->userVariables[i][Symbol::Length(userVariable)] = 0;
      userVariable->removeTree();
    }
    userVariables->removeNode();
  }
  Tree* variables =
      SimplifyAndFindVariables(simplifiedEquationSet, *context, error);
  uint8_t numberOfVariables = variables->numberOfChildren();
  SwapTrees(&simplifiedEquationSet, &variables);
  Variables::ProjectToId(simplifiedEquationSet, variables);
  EditionReference result;
  if (*error == Error::NoError) {
    result = SolveLinearSystem(simplifiedEquationSet, numberOfVariables,
                               *context, error);
    if (*error == Error::NonLinearSystem &&
        variables->numberOfChildren() <= 1 &&
        equationsSet->numberOfChildren() <= 1) {
      assert(result.isUninitialized());
      result = SolvePolynomial(simplifiedEquationSet, numberOfVariables,
                               *context, error);
      if (*error == Error::RequireApproximateSolution) {
        // TODO: Handle GeneralMonovariable solving.
        assert(result.isUninitialized());
      }
    }
  }
  if (!result.isUninitialized()) {
    Variables::BeautifyToName(result, variables);
  }
  simplifiedEquationSet->removeTree();
  variables->removeTree();
  return result;
}

Tree* Solver::SimplifyAndFindVariables(Tree* equationsSet, Context context,
                                       Error* error) {
  assert(*error == Error::NoError);
  /* TODO:
   * - Implement a number of variable limit (Error::TooManyVariables).
   * - Catch Undefined and Nonreal simplified equations (Error::EquationNonreal
   *   and Error::EquationUndefined).
   * - Pass ProjectionContext and project equality into subtraction.
   */
  Tree* variables = Variables::GetUserSymbols(equationsSet);
  SwapTrees(&equationsSet, &variables);
  for (Tree* equation : equationsSet->children()) {
    if (!Dimension::DeepCheckDimensions(equation) ||
        !Dimension::GetDimension(equation).isScalar()) {
      *error = Error::EquationUndefined;
      break;
    }
    Projection::DeepSystemProjection(equation);
    Simplification::DeepSystematicReduce(equation);
    Simplification::AdvancedReduction(equation);
  }
  SwapTrees(&variables, &equationsSet);
  return variables;
}

Tree* Solver::SolveLinearSystem(const Tree* simplifiedEquationSet, uint8_t n,
                                Context context, Error* error) {
  context.exactResults = true;
  // n unknown variables and rows equations
  uint8_t cols = n + 1;
  uint8_t rows = simplifiedEquationSet->numberOfChildren();
  Tree* matrix = SharedEditionPool->push<BlockType::Matrix>(0, 0);
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
  const Tree* coefficient = matrix->nextNode();
  for (uint8_t row = 0; row < rows; row++) {
    bool allCoefficientsNull = true;
    for (uint8_t col = 0; col < n; col++) {
      if (allCoefficientsNull && !Sign::GetSign(coefficient).isZero()) {
        allCoefficientsNull = false;
      }
      coefficient = coefficient->nextTree();
    }
    if (allCoefficientsNull && !Sign::GetSign(coefficient).isZero()) {
      /* Row j describes an equation of the form '0=b', the system has no
       * solution. */
      matrix->removeTree();
      *error = Error::NoError;
      return SharedEditionPool->push<BlockType::Set>(0);
    }
    coefficient = coefficient->nextTree();
  }
  if (rank == n && n > 0) {
    /* The rank is equal to the number of variables: the system has n
     * solutions, and after canonization their values are the first n values on
     * the last column. */
    uint8_t variableId = 0;
    Tree* child = matrix->nextNode();
    for (uint8_t row = 0; row < rows; row++) {
      for (uint8_t col = 0; col < cols; col++) {
        if (row < n && col == cols - 1) {
          if (*error == Error::NoError) {
            *error = RegisterSolution(child, variableId++, context);
            // Continue anyway to preserve EditionPool integrity
          }
          child = child->nextTree();
        } else {
          child->removeTree();
        }
      }
    }
    matrix->moveNodeOverNode(SharedEditionPool->push<BlockType::Set>(n));
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
  EditionReference result = SharedEditionPool->push<BlockType::List>(0);
  EditionReference tree = equation->clone();
  for (uint8_t i = 0; i < numberOfVariables; i++) {
    // TODO: PolynomialParser::Parse may need to handle more block types.
    Tree* polynomial = PolynomialParser::Parse(tree, Variables::Variable(i));
    if (!polynomial->isPolynomial()) {
      // tree did not depend on variable. Continue.
      tree = polynomial;
      NAry::AddChild(result, SharedEditionPool->push(BlockType::Zero));
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
    tree = nullConstant ? SharedEditionPool->push(BlockType::Zero)
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
  solution->moveTreeBeforeNode(
      SharedEditionPool->push<BlockType::Variable>(variableId));
  solution->moveNodeBeforeNode(SharedEditionPool->push<BlockType::Addition>(2));
  Simplification::DeepSystematicReduce(solution);
  Simplification::AdvancedReduction(solution);
  Beautification::DeepBeautify(solution);
  return Error::NoError;
}

}  // namespace PoincareJ
