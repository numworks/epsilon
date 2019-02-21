#include <poincare/addition.h>
#include <poincare/complex_cartesian.h>
#include <poincare/multiplication.h>
#include <poincare/subtraction.h>
#include <poincare/power.h>
#include <poincare/opposite.h>
#include <poincare/undefined.h>
//#include <poincare/matrix.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

int AdditionNode::polynomialDegree(Context & context, const char * symbolName) const {
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

int AdditionNode::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  return Addition(this).getPolynomialCoefficients(context, symbolName, coefficients);
}

// Layout
bool AdditionNode::childNeedsParenthesis(const TreeNode * child) const {
  if (((static_cast<const ExpressionNode *>(child)->isNumber()
          && Number(static_cast<const NumberNode *>(child)).sign() == Sign::Negative)
        || static_cast<const ExpressionNode *>(child)->type() == Type::Opposite)
      && child != childAtIndex(0))
  {
    return true;
  }
  return false;
}

Layout AdditionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Infix(Addition(this), floatDisplayMode, numberOfSignificantDigits, "+");
}

int AdditionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "+");
}

// Simplication

Expression AdditionNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Addition(this).shallowReduce(context, complexFormat, angleUnit, target);
}

Expression AdditionNode::shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Addition(this).shallowBeautify(context, complexFormat, angleUnit, target);
}

// Addition

const Number Addition::NumeralFactor(const Expression & e) {
  if (e.type() == ExpressionNode::Type::Multiplication && e.childAtIndex(0).isNumber()) {
    Number result = e.childAtIndex(0).convert<Number>();
    return result;
  }
  return Rational::Builder(1);
}

int Addition::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
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
  return deg;
}

Addition Addition::Builder() {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(AdditionNode));
  AdditionNode * node = new (bufferNode) AdditionNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node);
  return static_cast<Addition &>(h);
}

Addition Addition::Builder(Expression * children, size_t numberOfChildren) {
  Addition a = Addition::Builder();
  for (size_t i = 0; i < numberOfChildren; i++) {
    a.addChildAtIndexInPlace(children[i], i, i);
  }
  return a;
}

Expression Addition::shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  /* Beautifying AdditionNode essentially consists in adding Subtractions if
   * needed.
   * In practice, we want to turn "a+(-1)*b" into "a-b". Or, more precisely, any
   * "a+(-r)*b" into "a-r*b" where r is a positive Rational.
   * Note: the process will slightly differ if the negative product occurs on
   * the first term: we want to turn "Addition(Multiplication::Builder(-1,b))" into
   * "Opposite(b)".
   * Last but not least, special care must be taken when iterating over children
   * since we may remove some during the process. */

  /* Sort children in decreasing order:
   * 1+x+x^2 --> x^2+x+1
   * 1+R(2) --> R(2)+1 */
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, false, canBeInterrupted); }, true);

  for (int i = 0; i < numberOfChildren(); i++) {
    // Try to make the child i positive if any negative numeral factor is found
    Expression subtractant = childAtIndex(i).makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, target);
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

Expression Addition::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }

  /* Step 1: AdditionNode is associative, so let's start by merging children
   * which are additions. */
  int i = 0;
  while (i < numberOfChildren()) {
    if (childAtIndex(i).type() == ExpressionNode::Type::Addition) {
      mergeChildrenAtIndexInPlace(childAtIndex(i), i);
      continue;
    }
    i++;
  }

  // Step 2: Sort the children
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, true, canBeInterrupted); }, true);

#if MATRIX_EXACT_REDUCING
#if 0
  // This code is very old
  /* Step 2bis: get rid of matrix */
  int n = 1;
  int m = 1;
  /* All children have been simplified so if any child contains a matrix, it
   * is at the root node of the child. Moreover, thanks to the simplification
   * order, all matrix children (if any) are the last children. */
  assert(thisCopy.numberOfChildren() > 0);
  Expression lastChild = thisCopy.childAtIndex(numberOfChildren()-1);
  if (lastChild.type() == Type::Matrix) {
    // Create in-place the matrix of addition M (in place of the last child)
    Matrix * resultMatrix = static_cast<Matrix *>(lastOperand);
    n = resultMatrix->numberOfRows();
    m = resultMatrix->numberOfColumns();
    removeOperand(resultMatrix, false);
    /* Scan (starting at the end) accross the addition children to find any
     * other matrix */
    int i = numberOfChildren()-1;
    while (i >= 0 && childAtIndex(i)->type() == Type::Matrix) {
      Matrix * currentMatrix = static_cast<Matrix *>(childAtIndex(i));
      int on = currentMatrix->numberOfRows();
      int om = currentMatrix->numberOfColumns();
      if (on != n || om != m) {
        return replaceWith(new Undefined::Builder(), true);
      }
      // Dispatch the current matrix children in the created additions matrix
      for (int j = 0; j < n*m; j++) {
        AdditionNode * a = new AdditionNode();
        Expression * resultMatrixEntryJ = resultMatrix->childAtIndex(j);
        resultMatrix->replaceOperand(resultMatrixEntryJ, a, false);
        a->addOperand(currentMatrix->childAtIndex(j));
        a->addOperand(resultMatrixEntryJ);
        a->shallowReduce(context, complexFormat, angleUnit);
      }
      currentMatrix->detachOperands();
      removeOperand(currentMatrix, true);
      i--;
    }
    // Distribute the remaining addition on matrix children
    for (int i = 0; i < n*m; i++) {
      AdditionNode * a = static_cast<AdditionNode *>(clone());
      Expression * entryI = resultMatrix->childAtIndex(i);
      resultMatrix->replaceOperand(entryI, a, false);
      a->addOperand(entryI);
      a->shallowReduce(context, complexFormat, angleUnit);
    }
    return replaceWith(resultMatrix, true)->shallowReduce(context, complexFormat, angleUnit);
  }
#endif
#endif

  /* Step 3: Factorize like terms. Thanks to the simplification order, those are
   * next to each other at this point. */
  i = 0;
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
    if (TermsHaveIdenticalNonNumeralFactors(e1, e2)) {
      factorizeChildrenAtIndexesInPlace(i, i+1, context, complexFormat, angleUnit, target);
      continue;
    }
    i++;
  }

  /* Step 4: Let's remove any zero. It's important to do this after having
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

  // Step 5: Let's remove the addition altogether if it has a single child
  Expression result = squashUnaryHierarchyInPlace();
  if (result != *this) {
    return result;
  }

  /* Step 6: Let's bubble up the complex operator if possible
   * 3 cases:
   * - All children are real, we do nothing (allChildrenAreReal == 1)
   * - One of the child is non-real and not a ComplexCartesian: it means a
   *   complex expression could not be resolved as a ComplexCartesian, we cannot
   *   do anything about it now (allChildrenAreReal == -1)
   * - All children are either real or ComplexCartesian (allChildrenAreReal == 0)
   *   We can bubble up ComplexCartesian nodes. */
  if (allChildrenAreReal(context) == 0) {
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
    real.shallowReduce(context, complexFormat, angleUnit, target);
    imag.shallowReduce(context, complexFormat, angleUnit, target);
    return newComplexCartesian.shallowReduce();
  }

  /* Step 7: Let's put everything under a common denominator.
   * This step is done only for ReductionTarget::User if the parent expression
   * is not an addition. */
  Expression p = result.parent();
  if (target == ExpressionNode::ReductionTarget::User && result == *this && (p.isUninitialized() || p.type() != ExpressionNode::Type::Addition)) {
    // squashUnaryHierarchy didn't do anything: we're not an unary hierarchy
     result = factorizeOnCommonDenominator(context, complexFormat, angleUnit);
  }
  return result;
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

bool Addition::TermsHaveIdenticalNonNumeralFactors(const Expression & e1, const Expression & e2) {
  /* Return true if two expressions differ only by a rational factor. For
   * example, 2*pi and pi do, 2*pi and 2*ln(2) don't. */

  int numberOfNonNumeralFactorsInE1 = NumberOfNonNumeralFactors(e1);
  int numberOfNonNumeralFactorsInE2 = NumberOfNonNumeralFactors(e2);

  if (numberOfNonNumeralFactorsInE1 != numberOfNonNumeralFactorsInE2) {
    return false;
  }

  int numberOfNonNumeralFactors = numberOfNonNumeralFactorsInE1;
  if (numberOfNonNumeralFactors == 1) {
    return FirstNonNumeralFactor(e1).isIdenticalTo(FirstNonNumeralFactor(e2));
  } else {
    assert(numberOfNonNumeralFactors > 1);
    return Multiplication::HaveSameNonNumeralFactors(e1, e2);
  }
}

Expression Addition::factorizeOnCommonDenominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  // We want to turn (a/b+c/d+e/b) into (a*d+b*c+e*d)/(b*d)

  // Step 1: We want to compute the common denominator, b*d
  Multiplication commonDenominator = Multiplication::Builder();
  for (int i = 0; i < numberOfChildren(); i++) {
    Expression currentDenominator = childAtIndex(i).denominator(context, complexFormat, angleUnit);
    if (!currentDenominator.isUninitialized()) {
      // Make commonDenominator = LeastCommonMultiple(commonDenominator, denominator);
      commonDenominator.addMissingFactors(currentDenominator, context, complexFormat, angleUnit);
    }
  }
  if (commonDenominator.numberOfChildren() == 0) {
    // If commonDenominator is empty this means that no child was a fraction.
    return *this;
  }

  /* Step 2: Create the numerator. We start with this being a/b+c/d+e/b and we
   * want to create numerator = a/b*b*d + c/d*b*d + e/b*b*d = a*d + c*b + e*d */
  Addition numerator = Addition::Builder();
  for (int i = 0; i < numberOfChildren(); i++) {
    Multiplication m = Multiplication::Builder(childAtIndex(i), commonDenominator.clone());
    numerator.addChildAtIndexInPlace(m, numerator.numberOfChildren(), numerator.numberOfChildren());
    m.privateShallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User, true, false);
  }

  // Step 3: Add the denominator
  Power inverseDenominator = Power::Builder(commonDenominator, Rational::Builder(-1));
  Multiplication result = Multiplication::Builder(numerator, inverseDenominator);

  // Step 4: Simplify the numerator
  numerator.shallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);

  // Step 5: Simplify the denominator (in case it's a rational number)
  inverseDenominator.deepReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);

  /* Step 6: We simplify the resulting multiplication forbidding any
   * distribution of multiplication on additions (to avoid an infinite loop). */
  replaceWithInPlace(result);
  return result.privateShallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User, false, true);
}

void Addition::factorizeChildrenAtIndexesInPlace(int index1, int index2, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
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
  m.shallowReduce(context, complexFormat, angleUnit, target);
}

template Complex<float> Poincare::AdditionNode::compute<float>(std::complex<float>, std::complex<float>, Preferences::ComplexFormat);
template Complex<double> Poincare::AdditionNode::compute<double>(std::complex<double>, std::complex<double>, Preferences::ComplexFormat);

template MatrixComplex<float> AdditionNode::computeOnMatrices<float>(const MatrixComplex<float>,const MatrixComplex<float>, Preferences::ComplexFormat complexFormat);
template MatrixComplex<double> AdditionNode::computeOnMatrices<double>(const MatrixComplex<double>,const MatrixComplex<double>, Preferences::ComplexFormat complexFormat);

template MatrixComplex<float> AdditionNode::computeOnComplexAndMatrix<float>(std::complex<float> const, const MatrixComplex<float>, Preferences::ComplexFormat complexFormat);
template MatrixComplex<double> AdditionNode::computeOnComplexAndMatrix<double>(std::complex<double> const, const MatrixComplex<double>, Preferences::ComplexFormat complexFormat);

}
