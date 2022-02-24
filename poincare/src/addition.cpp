#include <poincare/addition.h>
#include <poincare/complex_cartesian.h>
#include <poincare/derivative.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>
#include <assert.h>
#include <utility>

namespace Poincare {

int AdditionNode::polynomialDegree(Context * context, const char * symbolName) const {
  int degree = 0;
  for (ExpressionNode * e : children()) {
    int d = e->polynomialDegree(context, symbolName);
    if (d < 0) {
      return -1;
    }
    degree = d > degree ? d : degree;
  }
  return degree;
}

int AdditionNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const {
  return Addition(this).getPolynomialCoefficients(context, symbolName, coefficients, symbolicComputation);
}

// Layout

Layout AdditionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Infix(Addition(this), floatDisplayMode, numberOfSignificantDigits, "+");
}

int AdditionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "+");
}

// Simplication

Expression AdditionNode::shallowReduce(ReductionContext reductionContext) {
  return Addition(this).shallowReduce(reductionContext);
}

Expression AdditionNode::shallowBeautify(ReductionContext * reductionContext) {
  return Addition(this).shallowBeautify(reductionContext);
}

// Derivation
bool AdditionNode::derivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  return Addition(this).derivate(reductionContext, symbol, symbolValue);
}

// Addition

ExpressionNode::IntegerStatus AdditionNode::integerStatus(Context * context) const {
  int nbOfChildren = numberOfChildren();
  for (int i = 0; i < nbOfChildren; i++) {
    if (childAtIndex(i)->integerStatus(context) != IntegerStatus::Integer) {
      return IntegerStatus::Unknown;
    }
  }
  return IntegerStatus::Integer;
}

const Number Addition::NumeralFactor(const Expression & e) {
  if (e.type() == ExpressionNode::Type::Multiplication && e.childAtIndex(0).isNumber()) {
    Number result = e.childAtIndex(0).convert<Number>();
    return result;
  }
  return Rational::Builder(1);
}

int Addition::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const {
  int deg = polynomialDegree(context, symbolName);
  if (deg < 0 || deg > Expression::k_maxPolynomialDegree) {
    return -1;
  }
  for (int k = 0; k < deg+1; k++) {
    coefficients[k] = Addition::Builder();
  }
  Expression intermediateCoefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  for (int i = 0; i < numberOfChildren(); i++) {
    int d = childAtIndex(i).getPolynomialCoefficients(context, symbolName, intermediateCoefficients, symbolicComputation);
    assert(d < Expression::k_maxNumberOfPolynomialCoefficients);
    for (int j = 0; j < d+1; j++) {
      static_cast<Addition&>(coefficients[j]).addChildAtIndexInPlace(intermediateCoefficients[j], coefficients[j].numberOfChildren(), coefficients[j].numberOfChildren());
    }
  }
  return deg;
}

Expression Addition::shallowBeautify(ExpressionNode::ReductionContext * reductionContext) {
  /* Beautifying AdditionNode essentially consists in adding Subtractions if
   * needed.
   * In practice, we want to turn "a+(-1)*b" into "a-b". Or, more precisely, any
   * "a+(-r)*b" into "a-r*b" where r is a positive Rational.
   * Note: the process will slightly differ if the negative product occurs on
   * the first term: we want to turn "Addition(Multiplication(-1,b))" into
   * "Opposite(b)".
   * Last but not least, special care must be taken when iterating over children
   * since we may remove some during the process. */

  /* Sort children in decreasing order:
   * 1+x+x^2 --> x^2+x+1
   * 1+R(2) --> R(2)+1 */
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, false, canBeInterrupted); }, reductionContext->context(), true);

  int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    // Try to make the child i positive if any negative numeral factor is found
    Expression subtractant = childAtIndex(i).makePositiveAnyNegativeNumeralFactor(*reductionContext);
    if (subtractant.isUninitialized())
    {
      // if subtractant is not initialized, it means the child i had no negative numeral factor
      // we ignore terms which are not like "(-r)*a"
      continue;
    }

    if (i == 0) {
      Opposite o = Opposite::Builder(subtractant);
      replaceChildAtIndexInPlace(i, o);
    } else {
      Expression leftSibling = childAtIndex(i-1);
      removeChildAtIndexInPlace(i-1);
      /* CAUTION: we removed a child. So we need to decrement i to make sure
       * the next iteration is actually on the next child. */
      i--;
      nbChildren--;
      Subtraction s = Subtraction::Builder(leftSibling, subtractant);
      /* We stole subtractant from this which replaced it by a ghost. We thus
       * need to put the subtraction at the previous index of subtractant, which
       * is still i because we updated i after removing a child. */
      replaceChildAtIndexInPlace(i, s);
    }
  }

  Expression result = squashUnaryHierarchyInPlace();
  return result;
}

Expression Addition::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }

  // Step 0: Let's remove the addition if it has a single child
  assert(numberOfChildren() > 0);
  if (numberOfChildren() == 1) {
    return squashUnaryHierarchyInPlace();
  }

  /* Step 1: Addition is associative, so let's start by merging children which
   * are additions.
   * TODO If the parent Expression is an Addition, one should perhaps
   * return now and let the parent do the reduction.
   */
  mergeSameTypeChildrenInPlace();

  const int childrenCount = numberOfChildren();
  assert(childrenCount > 1);

  /* Step 2: Handle the units. All children should have the same unit, otherwise
   * the result is not homogeneous. */
  {
    Expression unit;
    if (childAtIndex(0).removeUnit(&unit).isUndefined()) {
      return replaceWithUndefinedInPlace();
    }
    const bool hasUnit = !unit.isUninitialized();
    for (int i = 1; i < childrenCount; i++) {
      Expression otherUnit;
      Expression childI = childAtIndex(i).removeUnit(&otherUnit);
      if (childI.isUndefined()
          || hasUnit == otherUnit.isUninitialized()
          || (hasUnit && !unit.isIdenticalTo(otherUnit)))
      {
        return replaceWithUndefinedInPlace();
      }
    }
    if (hasUnit) {
      Expression addition = shallowReduce(reductionContext);
      Multiplication result = Multiplication::Builder(unit);
      result.mergeSameTypeChildrenInPlace();
      addition.replaceWithInPlace(result);
      result.addChildAtIndexInPlace(addition, 0, 1);
      return std::move(result);
    }
  }

  // Step 3: Sort the children
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, true, canBeInterrupted); }, reductionContext.context(), true);

  /* Step 4: Handle matrices. We return undef for a scalar added to a matrix.
   * Thanks to the simplification order, all matrix children (if any) are the
   * last children. */
  {
    Expression lastChild = childAtIndex(childrenCount - 1);
    if (lastChild.deepIsMatrix(reductionContext.context())) {
      if (!childAtIndex(0).deepIsMatrix(reductionContext.context())) {
        /* If there is a matrix in the children, the last child is a matrix. If
         * there is a a scalar, the first child is a scalar. We forbid the
         * addition of a matrix and a scalar. */
        return replaceWithUndefinedInPlace();
      }
      if (lastChild.type() != ExpressionNode::Type::Matrix) {
        /* All children are matrices that are not of type Matrix (for instance a
         * ConfidenceInterval that cannot be reduced). We cannot reduce the
         * addition more. */
        return *this;
      }
      // Create the addition matrix (in place of the last child)
      Matrix resultMatrix = static_cast<Matrix &>(lastChild);
      int n = resultMatrix.numberOfRows();
      int m = resultMatrix.numberOfColumns();
      // Scan to add the other children, which are  matrices
      for (int i = childrenCount - 2; i >= 0; i--) {
        if (childAtIndex(i).type() != ExpressionNode::Type::Matrix) {
          break;
        }
        Matrix currentMatrix = childAtIndex(i).convert<Matrix>();
        int currentN = currentMatrix.numberOfRows();
        int currentM = currentMatrix.numberOfColumns();
        if (currentN != n || currentM != m) {
          // Addition of matrices of different dimensions -> undef
          return replaceWithUndefinedInPlace();
        }
        // Dispatch the current matrix children in the created addition matrix
        for (int j = 0; j < n*m; j++) {
          Expression resultEntryJ = resultMatrix.childAtIndex(j);
          Expression currentEntryJ = currentMatrix.childAtIndex(j);
          if (resultEntryJ.type() == ExpressionNode::Type::Addition) {
            static_cast<Addition &>(resultEntryJ).addChildAtIndexInPlace(currentEntryJ, resultEntryJ.numberOfChildren(), resultEntryJ.numberOfChildren());
          } else {
            Addition a = Addition::Builder(resultEntryJ, currentEntryJ);
            resultMatrix.replaceChildAtIndexInPlace(j, a);
          }
        }
        removeChildInPlace(currentMatrix, currentMatrix.numberOfChildren());
      }
      for (int j = 0; j < n*m; j++) {
        resultMatrix.childAtIndex(j).shallowReduce(reductionContext);
      }
      return squashUnaryHierarchyInPlace();
    }
  }

  /* Step 5: Factorize like terms. Thanks to the simplification order, those are
   * next to each other at this point. */
  int i = 0;
  while (i < numberOfChildren()-1) {
    Expression e1 = childAtIndex(i);
    Expression e2 = childAtIndex(i+1);
    if (e1.isNumber() && e2.isNumber()) {
      Number r1 = static_cast<Number&>(e1);
      Number r2 = static_cast<Number&>(e2);
      Number a = Number::Addition(r1, r2);
      replaceChildAtIndexInPlace(i, a);
      removeChildAtIndexInPlace(i+1);
      continue;
    }
    if (TermsHaveIdenticalNonNumeralFactors(e1, e2, reductionContext.context())) {
      factorizeChildrenAtIndexesInPlace(i, i+1, reductionContext);
      continue;
    }
    i++;
  }

  /* Step 6: Let's remove any zero. It's important to do this after having
   * factorized because factorization can lead to new zeroes. For example
   * pi+(-1)*pi. We don't remove the last zero if it's the only child left
   * though. */
  i = 0;
  while (i < numberOfChildren()) {
    Expression e = childAtIndex(i);
    if (e.type() == ExpressionNode::Type::Rational && static_cast<Rational&>(e).isZero() && numberOfChildren() > 1) {
      removeChildAtIndexInPlace(i);
      continue;
    }
    i++;
  }

  // Step 7: Let's remove the addition altogether if it has a single child
  Expression result = squashUnaryHierarchyInPlace();
  if (result != *this) {
    return result;
  }

  /* Step 8: Let's bubble up the complex operator if possible
   * 3 cases:
   * - All children are real, we do nothing (allChildrenAreReal == 1)
   * - One of the child is non-real and not a ComplexCartesian: it means a
   *   complex expression could not be resolved as a ComplexCartesian, we cannot
   *   do anything about it now (allChildrenAreReal == -1)
   * - All children are either real or ComplexCartesian (allChildrenAreReal == 0)
   *   We can bubble up ComplexCartesian nodes. */
  if (allChildrenAreReal(reductionContext.context()) == 0) {
    /* We turn (a+ib)+(c+id) into (a+c)+i(c+d)*/
    Addition imag = Addition::Builder(); // we store all imaginary parts in 'imag'
    Addition real = *this; // we store all real parts in 'real'
    i = numberOfChildren() - 1;
    while (i >= 0) {
      Expression c = childAtIndex(i);
      if (c.type() == ExpressionNode::Type::ComplexCartesian) {
        real.replaceChildAtIndexInPlace(i, c.childAtIndex(0));
        imag.addChildAtIndexInPlace(c.childAtIndex(1), imag.numberOfChildren(), imag.numberOfChildren());
      } else {
        // the Addition is sorted so ComplexCartesian nodes are the last ones
        break;
      }
      i--;
    }
    ComplexCartesian newComplexCartesian = ComplexCartesian::Builder();
    replaceWithInPlace(newComplexCartesian);
    newComplexCartesian.replaceChildAtIndexInPlace(0, real);
    newComplexCartesian.replaceChildAtIndexInPlace(1, imag);
    real.shallowReduce(reductionContext);
    imag.shallowReduce(reductionContext);
    return newComplexCartesian.shallowReduce(reductionContext);
  }

  /* Step 9: Let's put everything under a common denominator.
   * This step is done only for ReductionTarget::User if the parent expression
   * is not an addition. */
  Expression p = result.parent();
  if (reductionContext.target() == ExpressionNode::ReductionTarget::User && result == *this && (p.isUninitialized() || p.type() != ExpressionNode::Type::Addition)) {
    // squashUnaryHierarchy didn't do anything: we're not an unary hierarchy
     result = factorizeOnCommonDenominator(reductionContext);
  }
  return result;
}

bool Addition::derivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  for (int i = 0; i < numberOfChildren(); i++) {
    replaceChildAtIndexInPlace(i, Derivative::Builder(childAtIndex(i), symbol.clone().convert<Symbol>(), symbolValue.clone()));
  }
  return true;
}

int Addition::NumberOfNonNumeralFactors(const Expression & e) {
  if (e.type() != ExpressionNode::Type::Multiplication) {
    return 1; // Or (e->type() != Type::Rational);
  }
  int result = e.numberOfChildren();
  if (e.childAtIndex(0).isNumber()) {
    result--;
  }
  return result;
}

const Expression Addition::FirstNonNumeralFactor(const Expression & e) {
  if (e.type() != ExpressionNode::Type::Multiplication) {
    return e;
  }
  if (e.childAtIndex(0).isNumber()) {
    return e.numberOfChildren() > 1 ? e.childAtIndex(1) : Expression();
  }
  return e.childAtIndex(0);
}

bool Addition::TermsHaveIdenticalNonNumeralFactors(const Expression & e1, const Expression & e2, Context * context) {
  /* Return true if two expressions differ only by a rational factor. For
   * example, 2*pi and pi do, 2*pi and 2*ln(2) don't. We do not want to
   * factorize random(). */

  int numberOfNonNumeralFactorsInE1 = NumberOfNonNumeralFactors(e1);
  int numberOfNonNumeralFactorsInE2 = NumberOfNonNumeralFactors(e2);

  if (numberOfNonNumeralFactorsInE1 != numberOfNonNumeralFactorsInE2) {
    return false;
  }

  int numberOfNonNumeralFactors = numberOfNonNumeralFactorsInE1;
  if (numberOfNonNumeralFactors == 1) {
    Expression nonNumeralFactor = FirstNonNumeralFactor(e1);
    if (nonNumeralFactor.recursivelyMatches(Expression::IsRandom, context)) {
      return false;
    }
    return FirstNonNumeralFactor(e1).isIdenticalTo(FirstNonNumeralFactor(e2));
  } else {
    assert(numberOfNonNumeralFactors > 1);
    return Multiplication::HaveSameNonNumeralFactors(e1, e2);
  }
}

Expression Addition::factorizeOnCommonDenominator(ExpressionNode::ReductionContext reductionContext) {
  /* We want to turn (a/b+c/d+e/b) into (a*d+b*c+e*d)/(b*d), except if one of
   * the denominators contains random, in which case the factors with random
   * should stay appart. */

  /* a will store the random factors, which should not be put to the same
   * denominator. */
  Addition a = Addition::Builder();

  // Step 1: We want to compute the common denominator, b*d
  Multiplication commonDenominator = Multiplication::Builder();
  for (int i = 0; i < numberOfChildren(); i++) {
    Expression childI = childAtIndex(i);
    Expression currentDenominator = childI.denominator(reductionContext);
    if (!currentDenominator.isUninitialized()) {
      if (currentDenominator.recursivelyMatches(Expression::IsRandom, reductionContext.context())) {
        // Remove "random" factors
        removeChildInPlace(childI, childI.numberOfChildren());
        a.addChildAtIndexInPlace(childI, a.numberOfChildren(), a.numberOfChildren());
        i--;
        continue;
      }
      // Make commonDenominator = LeastCommonMultiple(commonDenominator, denominator);
      commonDenominator.addMissingFactors(currentDenominator, reductionContext);
    }
  }
  if (commonDenominator.numberOfChildren() == 0) {
    // If commonDenominator is empty this means that no child was a fraction.
    while (a.numberOfChildren() > 0) {
      // Put back the "random" children
      addChildAtIndexInPlace(a.childAtIndex(0), numberOfChildren(), numberOfChildren());
    }
    return *this;
  }

  /* Step 2: Create the numerator. We start with this being a/b+c/d+e/b and we
   * want to create numerator = a/b*b*d + c/d*b*d + e/b*b*d = a*d + c*b + e*d */
  assert(reductionContext.target() ==  ExpressionNode::ReductionTarget::User); // Else, before, the algorithm used User target -> put back ?
  Addition numerator = Addition::Builder();
  for (int i = 0; i < numberOfChildren(); i++) {
    Multiplication m = Multiplication::Builder(childAtIndex(i), commonDenominator.clone());
    numerator.addChildAtIndexInPlace(m, numerator.numberOfChildren(), numerator.numberOfChildren());
    m.privateShallowReduce(reductionContext, true, false);
  }

  // Step 3: Add the denominator
  Power inverseDenominator = Power::Builder(commonDenominator, Rational::Builder(-1));
  Multiplication result = Multiplication::Builder(numerator, inverseDenominator);

  /* To simplify the numerator and the denominator, we allow symbolic
   * computation: all unwanted symbols should have already disappeared by now,
   * and if we checked again for symbols we might find "parameter" symbols
   * disconnected from the parametered expression, which would be replaced with
   * undef.
   * Example: int((ℯ^(-x))-x^(0.5), x, 0, 3), when creating the common
   * denominator for the integrand. */
  ExpressionNode::ReductionContext contextWithSymbolicComputation = ExpressionNode::ReductionContext(
      reductionContext.context(),
      reductionContext.complexFormat(),
      reductionContext.angleUnit(),
      reductionContext.unitFormat(),
      reductionContext.target(),
      ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);

  // Step 4: Simplify the numerator
  numerator.shallowReduce(contextWithSymbolicComputation);

  // Step 5: Simplify the denominator (in case it's a rational number)
  inverseDenominator.deepReduce(contextWithSymbolicComputation);

  /* Step 6: We simplify the resulting multiplication forbidding any
   * distribution of multiplication on additions (to avoid an infinite loop).
   * Handle the removed random factors. */
  int aChildrenCount = a.numberOfChildren();
  if (aChildrenCount == 0) {
    replaceWithInPlace(result);
    return result.privateShallowReduce(reductionContext, false, true);
  }
  a.addChildAtIndexInPlace(result, aChildrenCount, aChildrenCount);
  result.privateShallowReduce(reductionContext, false, true);
  replaceWithInPlace(a);
  return std::move(a);
}

void Addition::factorizeChildrenAtIndexesInPlace(int index1, int index2, ExpressionNode::ReductionContext reductionContext) {
  /* This function factorizes two children which only differ by a rational
   * factor. For example, if this is AdditionNode(2*pi, 3*pi), then 2*pi and 3*pi
   * could be merged, and this turned into AdditionNode(5*pi). */
  assert(index1 >= 0 && index1 < numberOfChildren());
  assert(index2 >= 0 && index2 < numberOfChildren());

  Expression e1 = childAtIndex(index1);
  Expression e2 = childAtIndex(index2);

  // Step 1: Compute the new numeral factor
  Number r = Number::Addition(NumeralFactor(e1), NumeralFactor(e2));

  // Step 2: Get rid of one of the children
  removeChildAtIndexInPlace(index2);

  // Step 3: Create a multiplication
  Multiplication m = Multiplication::Builder();
  if (e1.type() == ExpressionNode::Type::Multiplication) {
    m = static_cast<Multiplication&>(e1);
  } else {
    replaceChildAtIndexInPlace(index1, m);
    m.addChildAtIndexInPlace(e1, 0, 0);
  }

  // Step 4: Use the new rational factor
  assert(m.numberOfChildren() > 0);
  if (m.childAtIndex(0).isNumber()) {
    /* The children were ordered before, so ant rational child of m would be on
     * the first position */
    m.replaceChildAtIndexInPlace(0, r);
  } else {
    m.addChildAtIndexInPlace(r, 0, m.numberOfChildren());
  }

  // Step 5: Reduce the multiplication (in case the new rational factor is zero)
  m.shallowReduce(reductionContext);
}

template Complex<float> Poincare::AdditionNode::compute<float>(std::complex<float>, std::complex<float>, Preferences::ComplexFormat);
template Complex<double> Poincare::AdditionNode::compute<double>(std::complex<double>, std::complex<double>, Preferences::ComplexFormat);

template MatrixComplex<float> AdditionNode::computeOnMatrices<float>(const MatrixComplex<float>,const MatrixComplex<float>, Preferences::ComplexFormat complexFormat);
template MatrixComplex<double> AdditionNode::computeOnMatrices<double>(const MatrixComplex<double>,const MatrixComplex<double>, Preferences::ComplexFormat complexFormat);

template MatrixComplex<float> AdditionNode::computeOnComplexAndMatrix<float>(std::complex<float> const, const MatrixComplex<float>, Preferences::ComplexFormat complexFormat);
template MatrixComplex<double> AdditionNode::computeOnComplexAndMatrix<double>(std::complex<double> const, const MatrixComplex<double>, Preferences::ComplexFormat complexFormat);

}
