#include "equation_solver.h"

#include <poincare/numeric/roots.h>
#include <poincare/numeric/solver.h>
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
#include "variables.h"

namespace Poincare::Internal {

Tree* EquationSolver::ExactSolve(const Tree* equationsSet, Context* context,
                                 ProjectionContext projectionContext,
                                 Error* error) {
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
      result = SharedTreeStack->pushSet(0);
    }
  }
  return result;
}

/* TODO:
 * - Implement a number of variable limit (Error::TooManyVariables).
 * - Catch Undefined and Nonreal simplified equations (Error::EquationNonreal
 *   and Error::EquationUndefined).
 * - Project equality into subtraction.  */
Tree* EquationSolver::PrivateExactSolve(const Tree* equationsSet,
                                        Context* context,
                                        ProjectionContext projectionContext,
                                        Error* error) {
  /* Clone and simplify the equations */
  Tree* reducedEquationSet = equationsSet->cloneTree();
  ProjectAndReduce(reducedEquationSet, projectionContext, error);
  if (*error != Error::NoError) {
    reducedEquationSet->removeTree();
    return nullptr;
  }

  /* Count and collect used and replaced UserSymbols */
  Tree* userSymbols = Variables::GetUserSymbols(reducedEquationSet);
  uint8_t numberOfVariables = userSymbols->numberOfChildren();
  Tree* replacedSymbols = Set::Difference(
      Variables::GetUserSymbols(equationsSet), userSymbols->cloneTree());
  if (replacedSymbols->numberOfChildren() > 0) {
    int i = 0;
    for (const Tree* variable : replacedSymbols->children()) {
      Symbol::CopyName(variable, context->userVariables[i++],
                       Symbol::k_maxNameSize);
    }
  }
  replacedSymbols->removeTree();

  /* Replace UserSymbols with variables for easier solution handling */
  SwapTreesPointers(&reducedEquationSet, &userSymbols);
  int i = 0;
  for (const Tree* variable : userSymbols->children()) {
    Variables::ReplaceSymbol(reducedEquationSet, variable, i++,
                             ComplexSign::Unknown());
  }

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
    }
  }
  reducedEquationSet->removeTree();

  /* Replace variables back to UserSymbols */
  if (!result.isUninitialized()) {
    int i = 0;
    for (const Tree* symbol : userSymbols->children()) {
      assert(i < 6);
      Symbol::CopyName(symbol, context->variables[i++],
                       Symbol::k_maxNameLength);
      Variables::LeaveScopeWithReplacement(result, symbol, false);
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
  assert(context->numberOfVariables == 1);

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
  if (equationsSet->isUndefined()) {
    *error = equationsSet->isNonReal() ? Error::EquationNonreal
                                       : Error::EquationUndefined;
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
  if (rank == -1) {
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
  if (rank == n && n > 0) {
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
  // TODO: Introduce temporary variables to formally solve the system.
  matrix->removeTree();
  *error = Error::TooManyVariables;
  return nullptr;
}

Tree* EquationSolver::GetLinearCoefficients(const Tree* equation,
                                            uint8_t numberOfVariables,
                                            Context* context) {
  TreeRef result = SharedTreeStack->pushList(0);
  TreeRef eq = equation->cloneTree();
  /* TODO: y*(1+x) is not handled by PolynomialParser. We expand everything as
   * temporary workaround. */
  AdvancedReduction::DeepExpand(eq);
  for (uint8_t i = 0; i < numberOfVariables; i++) {
    // TODO: PolynomialParser::Parse may need to handle more block types.
    // TODO: Use user settings for a RealUnkown sign ?
    Tree* polynomial = PolynomialParser::Parse(
        eq, Variables::Variable(i, ComplexSign::Unknown()));
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
  Tree* polynomial = PolynomialParser::Parse(
      equation, Variables::Variable(0, ComplexSign::Unknown()));
  if (!polynomial) {
    *error = Error::RequireApproximateSolution;
    SharedTreeStack->dropBlocksFrom(equation);
    return nullptr;
  }

  const Tree* coefficients[k_maxNumberOfPolynomialCoefficients] = {};
  int degree = Polynomial::Degree(polynomial);
  if (degree > k_maxPolynomialDegree) {
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
  TreeRef discriminant = Roots::QuadraticDiscriminant(
      coefficients[2], coefficients[1], coefficients[0]);
  TreeRef solutionList = Roots::Quadratic(coefficients[2], coefficients[1],
                                          coefficients[0], discriminant);
  polynomial->removeTree();
  for (Tree* solution : solutionList->children()) {
    // TODO_PCJ: restore dependencies handling here
    EnhanceSolution(solution, context);
  }
  if (Preferences::SharedPreferences()->complexFormat() ==
      ComplexFormat::Real) {
    for (int i = solutionList->numberOfChildren() - 1; i >= 0; i--) {
      Tree* solution = solutionList->child(i);
      ComplexSign sign = GetComplexSign(solution);
      // TODO: approximate if unknown
      if (!sign.isReal()) {
        NAry::RemoveChildAtIndex(solutionList, i);
      }
    }
  }
  NAry::AddChild(solutionList, discriminant);
  *error = Error::NoError;
  return solutionList;
}

EquationSolver::Error EquationSolver::EnhanceSolution(Tree* solution,
                                                      Context* context) {
  /* TODO:
   * - Handle exact results being forbidden.
   * - Pass more context.
   * - Handle Nonreal and Undefined solutions.
   * - Handle approximate display.
   */
  // TODO: Use user settings for a RealUnkown sign ?
  Simplification::ReduceSystem(solution, true);
  return Error::NoError;
}

}  // namespace Poincare::Internal
