#include <poincare/addition.h>
#include <poincare/complex_cartesian.h>
#include <poincare/derivative.h>
#include <poincare/layout_helper.h>
#include <poincare/list.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>
#include <poincare/simplification_helper.h>
#include <assert.h>
#include <utility>

namespace Poincare {

ExpressionNode::Sign AdditionNode::sign(Context * context) const {
  if (numberOfChildren() < 1) {
    return Sign::Unknown;
  }
  // If all children have same sign, addition has this sign too.
  ExpressionNode::Sign additionSign = childAtIndex(0)->sign(context);
  for (int i = 1; i < numberOfChildren(); i++) {
    if (childAtIndex(i)->sign(context) != additionSign) {
      return Sign::Unknown;
    }
  }
  return additionSign;
}

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

int AdditionNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  return Addition(this).getPolynomialCoefficients(context, symbolName, coefficients);
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
bool AdditionNode::derivate(ReductionContext reductionContext, Symbol symbol, Expression symbolValue) {
  return Addition(this).derivate(reductionContext, symbol, symbolValue);
}

// Addition

const Number Addition::NumeralFactor(const Expression & e) {
  if (e.type() == ExpressionNode::Type::Multiplication && e.childAtIndex(0).isNumber()) {
    Number result = e.childAtIndex(0).convert<Number>();
    return result;
  }
  return Rational::Builder(1);
}

int Addition::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  int deg = polynomialDegree(context, symbolName);
  if (deg < 0 || deg > Expression::k_maxPolynomialDegree) {
    return -1;
  }
  for (int k = 0; k < deg+1; k++) {
    coefficients[k] = Addition::Builder();
  }
  Expression intermediateCoefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  for (int i = 0; i < numberOfChildren(); i++) {
    int d = childAtIndex(i).getPolynomialCoefficients(context, symbolName, intermediateCoefficients);
    assert(d < Expression::k_maxNumberOfPolynomialCoefficients);
    for (int j = 0; j < d+1; j++) {
      static_cast<Addition&>(coefficients[j]).addChildAtIndexInPlace(intermediateCoefficients[j], coefficients[j].numberOfChildren(), coefficients[j].numberOfChildren());
    }
  }
  // No coefficient should be left as an empty addition.
  assert(coefficients[deg].numberOfChildren() > 0);
  return deg;
}

Expression Addition::shallowBeautify(ExpressionNode::ReductionContext * reductionContext) {
  /* Step 1 : Sort children in decreasing order of degree.
   * 1+x+x^3+y^2+x*y+sqrt(x) --> x^3+y^2+x*y+x+1+sqrt(x)
   * sqrt(2)+1 = 1+sqrt(2)
   *
   * First we consider the degree of symbols so that deg(x^2) > deg(x) > deg(1)
   * and deg(x*y) = deg(x^2). An expression without symbols has a degree of 0.
   * An expression which has an exponent that is not a number has a degree
   * of NAN (like x^y). It is put at the beginning of the addition.
   *
   * Second, if two terms of the addition have the same symbol degree,
   * we consider the exponent of their right-most term.
   * So deg(3*sqrt(2)) = 0.5, deg(1) = 0, deg(pi^3) = 3
   *
   * Third if two terms still have the same degre, we compare their bases.
   *
   * WARNING : This algorithm is not the cleanest possible but is a good
   * compromise between efficiency and exhaustivity.
   * It covers the main cases which are numbers (such as 2+3*sqrt(2))
   * and polynomials like (x + y + 1)^3 which will be sorted as :
   * x^3 + y^3 + 3*x*y^2 + 3*x^2*y + 3*x^2 + 3*y^2 + 6*x*y + 3*x + 3*y + 1
   *
   * What it does not handle, is expressions like x^sqrt(2) or x^pi,
   * since it does not approximate exponents that are not Numbers in Poincare.
   * These terms will just be put at the beginning of the addition.
   *
   * It also supposes that all terms are developped, so if we want to keep
   * factorized terms after the reduction, this won't work for sorting
   * additions like (x+1)^5 + x^2 + 1 (the degree of (x+1)^5 is not computed
   * yet in its factorized form, but it could be easily implemented if needed,
   * by implementing "degreeForSortingAddition" on additionNode.).
   *
   * The algorithm also does not sort 5*x+sqrt(2)*x as we might expect.
   * Since the two terms have the same symbol degree, they are sorted by
   * the degree of their right-most term, which is also equal.
   * So in the end it uses the simplificationOrder for sorting, which puts
   * sqrt(2)*x before 5*x. So 5*x+sqrt(2)*x = sqrt(2)*x+5*x
   * */

  sortChildrenInPlace(
      [](const ExpressionNode * e1, const ExpressionNode * e2) {
        /* Repeat twice, once for symbol degree, once for any degree */
        for (bool sortBySymbolDegree : {true, false}) {
          double e1Degree = e1->degreeForSortingAddition(sortBySymbolDegree);
          double e2Degree = e2->degreeForSortingAddition(sortBySymbolDegree);
          if (!std::isnan(e2Degree) && (std::isnan(e1Degree) || e1Degree > e2Degree)) {
            return -1;
          }
          if (!std::isnan(e1Degree) && (std::isnan(e2Degree) || e2Degree > e1Degree)) {
            return 1;
          }
        }
        // If they have same degree, sort children in decreasing order of base.
        return ExpressionNode::SimplificationOrder(e1, e2, false);
      },
      reductionContext->context());

   /* Step 2 : Add Subtractions if needed
   * We want to turn "a+(-1)*b" into "a-b". Or, more precisely, any
   * "a+(-r)*b" into "a-r*b" where r is a positive Rational.
   * Note: the process will slightly differ if the negative product occurs on
   * the first term: we want to turn "Addition(Multiplication(-1,b))" into
   * "Opposite(b)".
   * Special care must be taken when iterating over children since we may
   * remove some during the process. */
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
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
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

  // Step 2: Sort the children
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2) { return ExpressionNode::SimplificationOrder(e1, e2, true); }, reductionContext.context());

 // Step 3 : Distribute the addition over lists
  Expression distributed = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
  if (!distributed.isUninitialized()) {
    return distributed;
  }

  /* Step 4: Handle the units. All children should have the same unit, otherwise
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
      /* The Tree is now free of units
       * Recurse to run the reduction, then create the result
       * result = MUL( addition, unit1, unit2...) */
      Expression addition = shallowReduce(reductionContext);
      assert((addition.type() != ExpressionNode::Type::Nonreal && addition.type() != ExpressionNode::Type::Undefined));
      Multiplication result = Multiplication::Builder(unit);
      // In case `unit` was a multiplication of units, flatten
      result.mergeSameTypeChildrenInPlace();
      addition.replaceWithInPlace(result);
      result.addChildAtIndexInPlace(addition, 0, 1);
      return std::move(result);
    }
  }

  /* Step 5: Handle matrices. We return undef for a scalar added to a matrix.
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

  /* Step 6: Factorize like terms. Thanks to the simplification order, those are
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

  /* Step 7: Let's remove any zero. It's important to do this after having
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

  // Step 8: Let's remove the addition altogether if it has a single child
  Expression result = squashUnaryHierarchyInPlace();
  if (result != *this) {
    return result;
  }

  /* Step 9: Let's bubble up the complex operator if possible
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

  /* Step 10: Let's put everything under a common denominator.
   * This step is done only for ReductionTarget::User if the parent expression
   * is not an addition. */
  Expression p = result.parent();
  if (reductionContext.target() == ExpressionNode::ReductionTarget::User && result == *this && (p.isUninitialized() || p.type() != ExpressionNode::Type::Addition)) {
    // squashUnaryHierarchy didn't do anything: we're not an unary hierarchy
     result = factorizeOnCommonDenominator(reductionContext);
  }
  return result;
}

bool Addition::derivate(ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue) {
  for (int i = 0; i < numberOfChildren(); i++) {
    derivateChildAtIndexInPlace(i, reductionContext, symbol, symbolValue);
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
    m.privateShallowReduce(reductionContext, true);
  }

  // Step 3: Add the denominator
  Power inverseDenominator = Power::Builder(commonDenominator, Rational::Builder(-1));
  Multiplication result = Multiplication::Builder(numerator, inverseDenominator);

  /* To simplify the numerator and the denominator, we temporarily disable
   * symbolic computation: all unwanted symbols should have already disappeared
   * by now, and if we checked again for symbols we might find "parameter"
   * symbols disconnected from the parametered expression (which is no longer a
   * parent here), which would be replaced with undef.
   * Example: int((e^(-x))-x^(0.5), x, 0, 3), when creating the common
   * denominator for the integrand. */
  ExpressionNode::SymbolicComputation previousSymbolicComputation = reductionContext.symbolicComputation();
  reductionContext.setSymbolicComputation(ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol);

  // Step 4: Simplify the numerator
  numerator.shallowReduce(reductionContext);

  // Step 5: Simplify the denominator (in case it's a rational number)
  inverseDenominator.deepReduce(reductionContext);

  // Restore symbolicComputation status
  reductionContext.setSymbolicComputation(previousSymbolicComputation);

  /* Step 6: We simplify the resulting multiplication forbidding any
   * distribution of multiplication on additions (to avoid an infinite loop).
   * Handle the removed random factors. */
  int aChildrenCount = a.numberOfChildren();
  if (aChildrenCount == 0) {
    replaceWithInPlace(result);
    return result.privateShallowReduce(reductionContext, false);
  }
  a.addChildAtIndexInPlace(result, aChildrenCount, aChildrenCount);
  result.privateShallowReduce(reductionContext, false);
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

}
