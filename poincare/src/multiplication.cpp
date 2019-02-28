#include <poincare/multiplication.h>
#include <poincare/addition.h>
#include <poincare/arithmetic.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/subtraction.h>
#include <poincare/tangent.h>
#include <poincare/undefined.h>
#include <cmath>
#include <ion.h>
#include <assert.h>

namespace Poincare {

ExpressionNode::Sign MultiplicationNode::sign(Context * context) const {
  if (numberOfChildren() == 0) {
    return Sign::Unknown;
  }
  int sign = 1;
  for (ExpressionNode * c : children()) {
    sign *= (int)(c->sign(context));
  }
  if (sign == 0) {
    return ExpressionNode::sign(context);
  }
  return (Sign)sign;
}

int MultiplicationNode::polynomialDegree(Context & context, const char * symbolName) const {
  int degree = 0;
  for (ExpressionNode * c : children()) {
    int d = c->polynomialDegree(context, symbolName);
    if (d < 0) {
      return -1;
    }
    degree += d;
  }
  return degree;
}

int MultiplicationNode::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  return Multiplication(this).getPolynomialCoefficients(context, symbolName, coefficients);
}

template<typename T>
MatrixComplex<T> MultiplicationNode::computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat) {
  if (m.numberOfColumns() != n.numberOfRows()) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> result = MatrixComplex<T>::Builder();
  for (int i = 0; i < m.numberOfRows(); i++) {
    for (int j = 0; j < n.numberOfColumns(); j++) {
      std::complex<T> c(0.0);
      for (int k = 0; k < m.numberOfColumns(); k++) {
        c += m.complexAtIndex(i*m.numberOfColumns()+k)*n.complexAtIndex(k*n.numberOfColumns()+j);
      }
      result.addChildAtIndexInPlace(Complex<T>::Builder(c), i*n.numberOfColumns()+j, result.numberOfChildren());
    }
  }
  result.setDimensions(m.numberOfRows(), n.numberOfColumns());
  return result;
}

Expression MultiplicationNode::setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  assert(s == ExpressionNode::Sign::Positive);
  return Multiplication(this).setSign(s, context, complexFormat, angleUnit, target);
}

bool MultiplicationNode::childNeedsParenthesis(const TreeNode * child) const {
  if ((static_cast<const ExpressionNode *>(child)->isNumber() && Number(static_cast<const NumberNode *>(child)).sign() == Sign::Negative)
        || static_cast<const ExpressionNode *>(child)->type() == ExpressionNode::Type::Opposite)
  {
    if (child == childAtIndex(0)) {
      return false;
    }
    return true;
  }
  Type types[] = {Type::Subtraction, Type::Addition};
  return static_cast<const ExpressionNode *>(child)->isOfType(types, 2);
}

Layout MultiplicationNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  constexpr int stringMaxSize = CodePoint::MaxCodePointCharLength + 1;
  char string[stringMaxSize];
  SerializationHelper::CodePoint(string, stringMaxSize, UCodePointMiddleDot);
  return LayoutHelper::Infix(Multiplication(this), floatDisplayMode, numberOfSignificantDigits, string);
}

int MultiplicationNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  constexpr int stringMaxSize = CodePoint::MaxCodePointCharLength + 1;
  char string[stringMaxSize];
  SerializationHelper::CodePoint(string, stringMaxSize, UCodePointMultiplicationSign);
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, string);
}

Expression MultiplicationNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Multiplication(this).shallowReduce(context, complexFormat, angleUnit, target);
}

Expression MultiplicationNode::shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Multiplication(this).shallowBeautify(context, complexFormat, angleUnit, target);
}

Expression MultiplicationNode::denominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return Multiplication(this).denominator(context, complexFormat, angleUnit);
}

/* Multiplication */

template<typename T>
void Multiplication::computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns) {
  for (int i = 0; i < mNumberOfRows; i++) {
    for (int j = 0; j < nNumberOfColumns; j++) {
      T res = 0.0f;
      for (int k = 0; k < mNumberOfColumns; k++) {
        res+= m[i*mNumberOfColumns+k]*n[k*nNumberOfColumns+j];
      }
      result[i*nNumberOfColumns+j] = res;
    }
  }
}

Expression Multiplication::setSign(ExpressionNode::Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  assert(s == ExpressionNode::Sign::Positive);
  for (int i = 0; i < numberOfChildren(); i++) {
    if (childAtIndex(i).sign(context) == ExpressionNode::Sign::Negative) {
      replaceChildAtIndexInPlace(i, childAtIndex(i).setSign(s, context, complexFormat, angleUnit, target));
    }
  }
  return shallowReduce(*context, complexFormat, angleUnit, target);
}

Expression Multiplication::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  return privateShallowReduce(context, complexFormat, angleUnit, target, true, true);
}

Expression Multiplication::shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  /* Beautifying a Multiplication consists in several possible operations:
   * - Add Opposite ((-3)*x -> -(3*x), useful when printing fractions)
   * - Adding parenthesis if needed (a*(b+c) is not a*b+c)
   * - Creating a Division if there's either a term with a power of -1 (a.b^(-1)
   *   shall become a/b) or a non-integer rational term (3/2*a -> (3*a)/2). */

  // Step 1: Turn -n*A into -(n*A)
  Expression noNegativeNumeral = makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, target);
  // If one negative numeral factor was made positive, we turn the expression in an Opposite
  if (!noNegativeNumeral.isUninitialized()) {
    Opposite o = Opposite::Builder();
    noNegativeNumeral.replaceWithInPlace(o);
    o.replaceChildAtIndexInPlace(0, noNegativeNumeral);
    return o;
  }

  /* Step 2: Merge negative powers: a*b^(-1)*c^(-pi)*d = a*(b*c^pi)^(-1)
   * This also turns 2/3*a into 2*a*3^(-1) */
  Expression thisExp = mergeNegativePower(context, complexFormat, angleUnit);
  if (thisExp.type() == ExpressionNode::Type::Power) {
    return thisExp.shallowBeautify(context, complexFormat, angleUnit, target);
  }
  assert(thisExp.type() == ExpressionNode::Type::Multiplication);

  // Step 3: Add Parenthesis if needed
  for (int i = 0; i < thisExp.numberOfChildren(); i++) {
    const Expression o = thisExp.childAtIndex(i);
    if (o.type() == ExpressionNode::Type::Addition) {
      Parenthesis p = Parenthesis::Builder(o);
      thisExp.replaceChildAtIndexInPlace(i, p);
    }
  }

  // Step 4: Create a Division if needed
  for (int i = 0; i < numberOfChildren(); i++) {
    Expression childI = thisExp.childAtIndex(i);
    if (!(childI.type() == ExpressionNode::Type::Power && childI.childAtIndex(1).type() == ExpressionNode::Type::Rational && childI.childAtIndex(1).convert<Rational>().isMinusOne())) {
      continue;
    }

    // Let's remove the denominator-to-be from this
    Expression denominatorOperand = childI.childAtIndex(0);
    removeChildInPlace(childI, childI.numberOfChildren());

    Expression numeratorOperand = shallowReduce(context, complexFormat, angleUnit, target);
    // Delete unnecessary parentheses on numerator
    if (numeratorOperand.type() == ExpressionNode::Type::Parenthesis) {
      Expression numeratorChild0 = numeratorOperand.childAtIndex(0);
      numeratorOperand.replaceWithInPlace(numeratorChild0);
      numeratorOperand = numeratorChild0;
    }
    Expression originalParent = numeratorOperand.parent();
    Division d = Division::Builder();
    numeratorOperand.replaceWithInPlace(d);
    d.replaceChildAtIndexInPlace(0, numeratorOperand);
    d.replaceChildAtIndexInPlace(1, denominatorOperand);
    return d.shallowBeautify(context, complexFormat, angleUnit, target);
  }
  return thisExp;
}

int Multiplication::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  int deg = polynomialDegree(context, symbolName);
  if (deg < 0 || deg > Expression::k_maxPolynomialDegree) {
    return -1;
  }
  // Initialization of coefficients
  for (int i = 1; i <= deg; i++) {
    coefficients[i] = Rational::Builder(0);
  }
  coefficients[0] = Rational::Builder(1);

  Expression intermediateCoefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Let's note result = a(0)+a(1)*X+a(2)*X^2+a(3)*x^3+..
  for (int i = 0; i < numberOfChildren(); i++) {
    // childAtIndex(i) = b(0)+b(1)*X+b(2)*X^2+b(3)*x^3+...
    int degI = childAtIndex(i).getPolynomialCoefficients(context, symbolName, intermediateCoefficients);
    assert(degI <= Expression::k_maxPolynomialDegree);
    for (int j = deg; j > 0; j--) {
      // new coefficients[j] = b(0)*a(j)+b(1)*a(j-1)+b(2)*a(j-2)+...
      Addition a = Addition::Builder();
      int jbis = j > degI ? degI : j;
      for (int l = 0; l <= jbis ; l++) {
        // Always copy the a and b coefficients are they are used multiple times
        a.addChildAtIndexInPlace(Multiplication::Builder(intermediateCoefficients[l].clone(), coefficients[j-l].clone()), a.numberOfChildren(), a.numberOfChildren());
      }
      /* a(j) and b(j) are used only to compute coefficient at rank >= j, we
       * can delete them as we compute new coefficient by decreasing ranks. */
      coefficients[j] = a;
    }
    // new coefficients[0] = a(0)*b(0)
    coefficients[0] = Multiplication::Builder(coefficients[0], intermediateCoefficients[0]);
  }
  return deg;
}

Expression Multiplication::denominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  // Merge negative power: a*b^-1*c^(-Pi)*d = a*(b*c^Pi)^-1
  // WARNING: we do not want to change the expression but to create a new one.
  Multiplication thisClone = clone().convert<Multiplication>();
  Expression e = thisClone.mergeNegativePower(context, complexFormat, angleUnit);
  if (e.type() == ExpressionNode::Type::Power) {
    return e.denominator(context, complexFormat, angleUnit);
  } else {
    assert(e.type() == ExpressionNode::Type::Multiplication);
    for (int i = 0; i < e.numberOfChildren(); i++) {
      // a*b^(-1)*... -> a*.../b
      if (e.childAtIndex(i).type() == ExpressionNode::Type::Power
          && e.childAtIndex(i).childAtIndex(1).type() == ExpressionNode::Type::Rational
          && e.childAtIndex(i).childAtIndex(1).convert<Rational>().isMinusOne())
      {
        return e.childAtIndex(i).childAtIndex(0);
      }
    }
  }
  return Expression();
}

Expression Multiplication::privateShallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target, bool shouldExpand, bool canBeInterrupted) {
  {
    Expression e = Expression::defaultShallowReduce();;
    if (e.isUndefined()) {
      return e;
    }
  }

  /* Step 1: MultiplicationNode is associative, so let's start by merging children
   * which also are multiplications themselves. */
  mergeMultiplicationChildrenInPlace();

  // Step 2: Sort the children
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, true, canBeInterrupted); }, true);

#if MATRIX_EXACT_REDUCING
#if 0 // OLD CODE
  /* Step 2bis: get rid of matrix */
  int n = 1;
  int m = 1;
  /* All children have been simplified so if any child contains a matrix, it
   * is at the root node of the child. Moreover, thanks to the simplification
   * order, all matrix children (if any) are the last children. */
  Expression * lastOperand = childAtIndex(numberOfChildren()-1);
  if (lastOperand->type() == ExpressionNode::Type::Matrix) {
    Matrix * resultMatrix = static_cast<Matrix *>(lastOperand);
    // Use the last matrix child as the final matrix
    n = resultMatrix->numberOfRows();
    m = resultMatrix->numberOfColumns();
    /* Scan accross the multiplication children to find any other matrix:
     * (the last child is the result matrix so we start at
     * numberOfChildren()-2)*/
    int k = numberOfChildren()-2;
    while (k >= 0 && childAtIndex(k)->type() == ExpressionNode::Type::Matrix) {
      Matrix * currentMatrix = static_cast<Matrix *>(childAtIndex(k));
      int on = currentMatrix->numberOfRows();
      int om = currentMatrix->numberOfColumns();
      if (om != n) {
        return replaceWith(new Undefined::Builder(), true);
      }
      // Create the matrix resulting of the multiplication of the current matrix and the result matrix
     /*                        resultMatrix
      *                          i2= 0..m
      *                         +-+-+-+-+-+
      *                         | | | | | |
      *                         +-+-+-+-+-+
      *                  j=0..n | | | | | |
      *                         +-+-+-+-+-+
      *                         | | | | | |
      *                         +-+-+-+-+-+
      *        currentMatrix
      *           j=0..om
      *         +---+---+---+   +-+-+-+-+-+
      *         |   |   |   |   | | | | | |
      *         +---+---+---+   +-+-+-+-+-+
      *i1=0..on |   |   |   |   | |e| | | |
      *         +---+---+---+   +-+-+-+-+-+
      *         |   |   |   |   | | | | | |
      *         +---+---+---+   +-+-+-+-+-+
      * */
      Expression ** newMatrixOperands = new Expression * [on*m];
      for (int e = 0; e < on*m; e++) {
        newMatrixOperands[e] = new Addition::Builder();
        int i2 = e%m;
        int i1 = e/m;
        for (int j = 0; j < n; j++) {
          Expression * mult = new Multiplication::Builder(currentMatrix->childAtIndex(j+om*i1), resultMatrix->childAtIndex(j*m+i2), true);
          static_cast<Addition *>(newMatrixOperands[e])->addOperand(mult);
          mult->shallowReduce(context, complexFormat, angleUnit, target);
        }
        Reduce(&newMatrixOperands[e], context, complexFormat, angleUnit, false);
      }
      n = on;
      removeOperand(currentMatrix, true);
      resultMatrix = static_cast<Matrix *>(resultMatrix->replaceWith(new Matrix(newMatrixOperands, n, m, false), true));
      k--;
    }
    removeOperand(resultMatrix, false);
    // Distribute the remaining multiplication on matrix children
    for (int i = 0; i < n*m; i++) {
      Multiplication * m = static_cast<Multiplication *>(clone());
      Expression * entryI = resultMatrix->childAtIndex(i);
      resultMatrix->replaceOperand(entryI, m, false);
      m->addOperand(entryI);
      m->shallowReduce(context, complexFormat, angleUnit, target);
    }
    return replaceWith(resultMatrix, true)->shallowReduce(context, complexFormat, angleUnit, target);
  }
#endif
#endif

  /* Step 3: Gather like terms. For example, turn pi^2*pi^3 into pi^5. Thanks to
   * the simplification order, such terms are guaranteed to be next to each
   * other. */
  int i = 0;
  while (i < numberOfChildren()-1) {
    Expression oi = childAtIndex(i);
    Expression oi1 = childAtIndex(i+1);
    if (oi.recursivelyMatches(Expression::IsRandom, context, true)) {
      // Do not factorize random or randint
      i++;
      continue;
    }
    if (TermsHaveIdenticalBase(oi, oi1)) {
      bool shouldFactorizeBase = true;
      if (TermHasNumeralBase(oi)) {
        /* Combining powers of a given rational isn't straightforward. Indeed,
         * there are two cases we want to deal with:
         *  - 2*2^(1/2) or 2*2^pi, we want to keep as-is
         *  - 2^(1/2)*2^(3/2) we want to combine. */
        shouldFactorizeBase = oi.type() == ExpressionNode::Type::Power && oi1.type() == ExpressionNode::Type::Power;
      }
      if (shouldFactorizeBase) {
        factorizeBase(i, i+1, context, complexFormat, angleUnit, target);
        continue;
      }
    } else if (TermHasNumeralBase(oi) && TermHasNumeralBase(oi1) && TermsHaveIdenticalExponent(oi, oi1)) {
      factorizeExponent(i, i+1, context, complexFormat, angleUnit, target);
      continue;
    }
    i++;
  }

  /* Step 4: We look for terms of form sin(x)^p*cos(x)^q with p, q rational of
   * opposite signs. We replace them by either:
   * - tan(x)^p*cos(x)^(p+q) if |p|<|q|
   * - tan(x)^(-q)*sin(x)^(p+q) otherwise */
  if (target == ExpressionNode::ReductionTarget::User) {
    for (int i = 0; i < numberOfChildren(); i++) {
      Expression o1 = childAtIndex(i);
      if (Base(o1).type() == ExpressionNode::Type::Sine && TermHasNumeralExponent(o1)) {
        const Expression x = Base(o1).childAtIndex(0);
        /* Thanks to the SimplificationOrder, Cosine-base factors are after
         * Sine-base factors */
        for (int j = i+1; j < numberOfChildren(); j++) {
          Expression o2 = childAtIndex(j);
          if (Base(o2).type() == ExpressionNode::Type::Cosine && TermHasNumeralExponent(o2) && Base(o2).childAtIndex(0).isIdenticalTo(x)) {
            factorizeSineAndCosine(i, j, context, complexFormat, angleUnit);
            break;
          }
        }
      }
    }
    /* Replacing sin/cos by tan factors may have mixed factors and factors are
     * guaranteed to be sorted (according ot SimplificationOrder) at the end of
     * shallowReduce */
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, true, canBeInterrupted); }, true);
  }

  /* Step 5: We remove rational children that appeared in the middle of sorted
   * children. It's important to do this after having factorized because
   * factorization can lead to new ones. Indeed:
   * pi^(-1)*pi-> 1
   * i*i -> -1
   * 2^(1/2)*2^(1/2) -> 2
   * sin(x)*cos(x) -> 1*tan(x)
   * Last, we remove the only rational child if it is one and not the only
   * child. */
  i = 1;
  while (i < numberOfChildren()) {
    Expression o = childAtIndex(i);
    if (o.type() == ExpressionNode::Type::Rational && static_cast<Rational &>(o).isOne()) {
      removeChildAtIndexInPlace(i);
      continue;
    }
    if (o.isNumber()) {
      if (childAtIndex(0).isNumber()) {
        Number o0 = childAtIndex(0).convert<Rational>();
        Number m = Number::Multiplication(o0, static_cast<Number &>(o));
        replaceChildAtIndexInPlace(0, m);
        removeChildAtIndexInPlace(i);
      } else {
        // Number child has to be first
        removeChildAtIndexInPlace(i);
        addChildAtIndexInPlace(o, 0, numberOfChildren());
      }
      continue;
    }
    i++;
  }

   /* Step 5: If the first child is zero, the multiplication result is zero. We
    * do this after merging the rational children, because the merge takes care
    * of turning 0*inf into undef. We still have to check that no other child
    * involves an inifity expression to avoid reducing 0*e^(inf) to 0.
    * If the first child is 1, we remove it if there are other children. */
  {
    const Expression c = childAtIndex(0);
    if (c.type() == ExpressionNode::Type::Rational && static_cast<const Rational &>(c).isZero()) {
      // Check that other children don't match inf
      bool infiniteFactor = false;
      for (int i = 1; i < numberOfChildren(); i++) {
        infiniteFactor = childAtIndex(i).recursivelyMatchesInfinity(context);
        if (infiniteFactor) {
          break;
        }
      }
      if (!infiniteFactor) {
        replaceWithInPlace(c);
        return c;
      }
    }
    if (c.type() == ExpressionNode::Type::Rational && static_cast<const Rational &>(c).isOne() && numberOfChildren() > 1) {
      removeChildAtIndexInPlace(0);
    }
  }

  /* Step 6: Expand multiplication over addition children if any. For example,
   * turn (a+b)*c into a*c + b*c. We do not want to do this step right now if
   * the parent is a multiplication or if the reduction is done bottom up to
   * avoid missing factorization such as (x+y)^(-1)*((a+b)*(x+y)).
   * Note: This step must be done after Step 4, otherwise we wouldn't be able to
   * reduce expressions such as (x+y)^(-1)*(x+y)(a+b).
   * If there is a random somewhere, do not expand. */
  Expression p = parent();
  bool hasRandom = recursivelyMatches(Expression::IsRandom, context, true);
  if (shouldExpand
      && (p.isUninitialized() || p.type() != ExpressionNode::Type::Multiplication)
      && !hasRandom)
  {
    for (int i = 0; i < numberOfChildren(); i++) {
      if (childAtIndex(i).type() == ExpressionNode::Type::Addition) {
        return distributeOnOperandAtIndex(i, context, complexFormat, angleUnit, target);
      }
    }
  }

  // Step 7: Let's remove the multiplication altogether if it has one child
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
   *   We can bubble up ComplexCartesian nodes.
   * Do not simplify if there are randoms !*/
  if (!hasRandom && allChildrenAreReal(context) == 0) {
    int nbChildren = numberOfChildren();
    int i = nbChildren-1;
    // Children are sorted so ComplexCartesian nodes are at the end
    assert(childAtIndex(i).type() == ExpressionNode::Type::ComplexCartesian);
    // First, we merge all ComplexCartesian children into one
    ComplexCartesian child = childAtIndex(i).convert<ComplexCartesian>();
    removeChildAtIndexInPlace(i);
    i--;
    while (i >= 0) {
      Expression e = childAtIndex(i);
      if (e.type() != ExpressionNode::Type::ComplexCartesian) {
        // the Multiplication is sorted so ComplexCartesian nodes are the last ones
        break;
      }
      child = child.multiply(static_cast<ComplexCartesian &>(e), context, complexFormat, angleUnit, target);
      removeChildAtIndexInPlace(i);
      i--;
    }
    // The real children are both factors of the real and the imaginary multiplication
    Multiplication real = *this;
    Multiplication imag = clone().convert<Multiplication>();
    real.addChildAtIndexInPlace(child.real(), real.numberOfChildren(), real.numberOfChildren());
    imag.addChildAtIndexInPlace(child.imag(), real.numberOfChildren(), real.numberOfChildren());
    ComplexCartesian newComplexCartesian = ComplexCartesian::Builder();
    replaceWithInPlace(newComplexCartesian);
    newComplexCartesian.replaceChildAtIndexInPlace(0, real);
    newComplexCartesian.replaceChildAtIndexInPlace(1, imag);
    real.shallowReduce(context, complexFormat, angleUnit, target);
    imag.shallowReduce(context, complexFormat, angleUnit, target);
    return newComplexCartesian.shallowReduce();
  }

  return result;
}

void Multiplication::mergeMultiplicationChildrenInPlace() {
  // Multiplication is associative: a*(b*c)->a*b*c
  int i = 0;
  while (i < numberOfChildren()) {
    Expression c = childAtIndex(i);
    if (c.type() == ExpressionNode::Type::Multiplication) {
      mergeChildrenAtIndexInPlace(c, numberOfChildren()); // TODO: ensure that matrix children are not swapped to implement MATRIX_EXACT_REDUCING
      continue;
    }
    i++;
  }
}

void Multiplication::factorizeBase(int i, int j, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  /* This function factorizes two children which have a common base. For example
   * if this is Multiplication::Builder(pi^2, pi^3), then pi^2 and pi^3 could be merged
   * and this turned into Multiplication::Builder(pi^5). */

  Expression e = childAtIndex(j);
  // Step 1: Get rid of the child j
  removeChildAtIndexInPlace(j);
  // Step 2: Merge child j in child i by factorizing base
  mergeInChildByFactorizingBase(i, e, context, complexFormat, angleUnit, target);
}

void Multiplication::mergeInChildByFactorizingBase(int i, Expression e, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  /* This function replace the child at index i by its factorization with e. e
   * and childAtIndex(i) are supposed to have a common base. */

  // Step 1: Find the new exponent
  Expression s = Addition::Builder(CreateExponent(childAtIndex(i)), CreateExponent(e)); // pi^2*pi^3 -> pi^(2+3) -> pi^5
  // Step 2: Create the new Power
  Expression p = Power::Builder(Base(childAtIndex(i)), s); // pi^2*pi^-2 -> pi^0 -> 1
  s.shallowReduce(context, complexFormat, angleUnit, target);
  // Step 3: Replace one of the child
  replaceChildAtIndexInPlace(i, p);
  p = p.shallowReduce(context, complexFormat, angleUnit, target);
  /* Step 4: Reducing the new power might have turned it into a multiplication,
   * ie: 12^(1/2) -> 2*3^(1/2). In that case, we need to merge the multiplication
   * node with this. */
  if (p.type() == ExpressionNode::Type::Multiplication) {
    mergeMultiplicationChildrenInPlace();
  }
}

void Multiplication::factorizeExponent(int i, int j, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  /* This function factorizes children which share a common exponent. For
   * example, it turns Multiplication::Builder(2^x,3^x) into Multiplication::Builder(6^x). */

  // Step 1: Find the new base
  Expression m = Multiplication::Builder(Base(childAtIndex(i)), Base(childAtIndex(j))); // 2^x*3^x -> (2*3)^x -> 6^x
  // Step 2: Get rid of one of the children
  removeChildAtIndexInPlace(j);
  // Step 3: Replace the other child
  childAtIndex(i).replaceChildAtIndexInPlace(0, m);
  // Step 4: Reduce expressions
  m.shallowReduce(context, complexFormat, angleUnit, target);
  Expression p = childAtIndex(i).shallowReduce(context, complexFormat, angleUnit, target); // 2^x*(1/2)^x -> (2*1/2)^x -> 1
  /* Step 5: Reducing the new power might have turned it into a multiplication,
   * ie: 12^(1/2) -> 2*3^(1/2). In that case, we need to merge the multiplication
   * node with this. */
  if (p.type() == ExpressionNode::Type::Multiplication) {
    mergeMultiplicationChildrenInPlace();
  }
}

Expression Multiplication::distributeOnOperandAtIndex(int i, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  /* This method creates a*...*b*y... + a*...*c*y... + ... from
   * a*...*(b+c+...)*y... */
  assert(i >= 0 && i < numberOfChildren());
  assert(childAtIndex(i).type() == ExpressionNode::Type::Addition);

  Addition a = Addition::Builder();
  Expression childI = childAtIndex(i);
  int numberOfAdditionTerms = childI.numberOfChildren();
  for (int j = 0; j < numberOfAdditionTerms; j++) {
    Multiplication m = clone().convert<Multiplication>();
    m.replaceChildAtIndexInPlace(i, childI.childAtIndex(j));
    // Reduce m: pi^(-1)*(pi + x) -> pi^(-1)*pi + pi^(-1)*x -> 1 + pi^(-1)*x
    a.addChildAtIndexInPlace(m, a.numberOfChildren(), a.numberOfChildren());
    m.shallowReduce(context, complexFormat, angleUnit, target);
  }
  replaceWithInPlace(a);
  return a.shallowReduce(context, complexFormat, angleUnit, target); // Order terms, put under a common denominator if needed
}

void Multiplication::addMissingFactors(Expression factor, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  if (factor.type() == ExpressionNode::Type::Multiplication) {
    for (int j = 0; j < factor.numberOfChildren(); j++) {
      addMissingFactors(factor.childAtIndex(j), context, complexFormat, angleUnit);
    }
    return;
  }
  /* Special case when factor is a Rational: if 'this' has already a rational
   * child, we replace it by its LCM with factor ; otherwise, we simply add
   * factor as a child. */
  if (numberOfChildren() > 0 && childAtIndex(0).type() == ExpressionNode::Type::Rational && factor.type() == ExpressionNode::Type::Rational) {
    assert(static_cast<Rational &>(factor).integerDenominator().isOne());
    assert(childAtIndex(0).convert<Rational>().integerDenominator().isOne());
    Integer lcm = Arithmetic::LCM(static_cast<Rational &>(factor).unsignedIntegerNumerator(), childAtIndex(0).convert<Rational>().unsignedIntegerNumerator());
    if (lcm.isOverflow()) {
      addChildAtIndexInPlace(Rational::Builder(static_cast<Rational &>(factor).unsignedIntegerNumerator()), 1, numberOfChildren());
      return;
    }
    replaceChildAtIndexInPlace(0, Rational::Builder(lcm));
    return;
  }
  if (factor.type() != ExpressionNode::Type::Rational) {
    /* If factor is not a rational, we merge it with the child of identical
     * base if any. Otherwise, we add it as an new child. */
    for (int i = 0; i < numberOfChildren(); i++) {
      if (TermsHaveIdenticalBase(childAtIndex(i), factor)) {
        Expression sub = Subtraction::Builder(CreateExponent(childAtIndex(i)), CreateExponent(factor)).deepReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
        if (sub.sign(&context) == ExpressionNode::Sign::Negative) { // index[0] < index[1]
          sub = Opposite::Builder(sub);
          if (factor.type() == ExpressionNode::Type::Power) {
            factor.replaceChildAtIndexInPlace(1, sub);
          } else {
            factor = Power::Builder(factor, sub);
          }
          sub.shallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
          mergeInChildByFactorizingBase(i, factor, context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
        } else if (sub.sign(&context) == ExpressionNode::Sign::Unknown) {
          mergeInChildByFactorizingBase(i, factor, context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
        }
        return;
      }
    }
  }
  addChildAtIndexInPlace(factor.clone(), 0, numberOfChildren());
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, true, canBeInterrupted); }, true);
}

void Multiplication::factorizeSineAndCosine(int i, int j, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  /* This function turn sin(x)^p * cos(x)^q into either:
   * - tan(x)^p*cos(x)^(p+q) if |p|<|q|
   * - tan(x)^(-q)*sin(x)^(p+q) otherwise */
  const Expression x = Base(childAtIndex(i)).childAtIndex(0);
  // We check before that p and q were numbers
  Number p = CreateExponent(childAtIndex(i)).convert<Number>();
  Number q = CreateExponent(childAtIndex(j)).convert<Number>();
  // If p and q have the same sign, we cannot replace them by a tangent
  if ((int)p.sign()*(int)q.sign() > 0) {
    return;
  }
  Number sumPQ = Number::Addition(p, q);
  Number absP = p.clone().convert<Number>().setSign(ExpressionNode::Sign::Positive);
  Number absQ = q.clone().convert<Number>().setSign(ExpressionNode::Sign::Positive);
  Expression tan = Tangent::Builder(x.clone());
  if (Number::NaturalOrder(absP, absQ) < 0) {
    // Replace sin(x) by tan(x) or sin(x)^p by tan(x)^p
    if (p.isRationalOne()) {
      replaceChildAtIndexInPlace(i, tan);
    } else {
      replaceChildAtIndexInPlace(i, Power::Builder(tan, p));
    }
    childAtIndex(i).shallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
    // Replace cos(x)^q by cos(x)^(p+q)
    replaceChildAtIndexInPlace(j, Power::Builder(Base(childAtIndex(j)), sumPQ));
    childAtIndex(j).shallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
  } else {
    // Replace cos(x)^q by tan(x)^(-q)
    Expression newPower = Power::Builder(tan, Number::Multiplication(q, Rational::Builder(-1)));
    newPower.childAtIndex(1).shallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
    replaceChildAtIndexInPlace(j, newPower);
    newPower.shallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
    // Replace sin(x)^p by sin(x)^(p+q)
    replaceChildAtIndexInPlace(i, Power::Builder(Base(childAtIndex(i)), sumPQ));
    childAtIndex(i).shallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
  }
}

bool Multiplication::HaveSameNonNumeralFactors(const Expression & e1, const Expression & e2) {
  assert(e1.numberOfChildren() > 0);
  assert(e2.numberOfChildren() > 0);
  int numberOfNonNumeralFactors1 = e1.childAtIndex(0).isNumber() ? e1.numberOfChildren()-1 : e1.numberOfChildren();
  int numberOfNonNumeralFactors2 = e2.childAtIndex(0).isNumber() ? e2.numberOfChildren()-1 : e2.numberOfChildren();
  if (numberOfNonNumeralFactors1 != numberOfNonNumeralFactors2) {
    return false;
  }
  int firstNonNumeralOperand1 = e1.childAtIndex(0).isNumber() ? 1 : 0;
  int firstNonNumeralOperand2 = e2.childAtIndex(0).isNumber() ? 1 : 0;
  for (int i = 0; i < numberOfNonNumeralFactors1; i++) {
    Expression currentChild1 = e1.childAtIndex(firstNonNumeralOperand1+i);
    if (currentChild1.isRandom()
        || !(currentChild1.isIdenticalTo(e2.childAtIndex(firstNonNumeralOperand2+i))))
    {
      return false;
    }
  }
  return true;
}

const Expression Multiplication::CreateExponent(Expression e) {
  Expression result = e.type() == ExpressionNode::Type::Power ? e.childAtIndex(1).clone() : Rational::Builder(1);
  return result;
}

bool Multiplication::TermsHaveIdenticalBase(const Expression & e1, const Expression & e2) {
  return Base(e1).isIdenticalTo(Base(e2));
}

bool Multiplication::TermsHaveIdenticalExponent(const Expression & e1, const Expression & e2) {
  /* Note: We will return false for e1=2 and e2=Pi, even though one could argue
   * that these have the same exponent whose value is 1. */
  return e1.type() == ExpressionNode::Type::Power && e2.type() == ExpressionNode::Type::Power && (e1.childAtIndex(1).isIdenticalTo(e2.childAtIndex(1)));
}

bool Multiplication::TermHasNumeralBase(const Expression & e) {
  return Base(e).isNumber();
}

bool Multiplication::TermHasNumeralExponent(const Expression & e) {
  if (e.type() != ExpressionNode::Type::Power) {
    return true;
  }
  if (e.childAtIndex(1).isNumber()) {
    return true;
  }
  return false;
}

Expression Multiplication::mergeNegativePower(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  /* mergeNegativePower groups all factors that are power of form a^(-b) together
   * for instance, a^(-1)*b^(-c)*c = c*(a*b^c)^(-1) */
  Multiplication m = Multiplication::Builder();
  // Special case for rational p/q: if q != 1, q should be at denominator
  if (childAtIndex(0).type() == ExpressionNode::Type::Rational && !childAtIndex(0).convert<Rational>().integerDenominator().isOne()) {
    Rational r = childAtIndex(0).convert<Rational>();
    m.addChildAtIndexInPlace(Rational::Builder(r.integerDenominator()), 0, m.numberOfChildren());
    if (r.signedIntegerNumerator().isOne()) {
      removeChildAtIndexInPlace(0);
    } else {
      replaceChildAtIndexInPlace(0, Rational::Builder(r.signedIntegerNumerator()));
    }
  }
  int i = 0;
  // Look for power of form a^(-b)
  while (i < numberOfChildren()) {
    if (childAtIndex(i).type() == ExpressionNode::Type::Power) {
      Expression p = childAtIndex(i);
      Expression positivePIndex = p.childAtIndex(1).makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
      if (!positivePIndex.isUninitialized()) {
        // Remove a^(-b) from the Multiplication
        removeChildAtIndexInPlace(i);
        // Add a^b to m
        m.addChildAtIndexInPlace(p, m.numberOfChildren(), m.numberOfChildren());
        if (p.childAtIndex(1).isRationalOne()) {
          p.replaceWithInPlace(p.childAtIndex(0));
        }
        // We do not increment i because we removed one child from the Multiplication
        continue;
      }
    }
    i++;
  }
  if (m.numberOfChildren() == 0) {
    return *this;
  }
  m.sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, true, canBeInterrupted); }, true);
  Power p = Power::Builder(m.squashUnaryHierarchyInPlace(), Rational::Builder(-1));
  addChildAtIndexInPlace(p, 0, numberOfChildren());
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, true, canBeInterrupted); }, true);
  return squashUnaryHierarchyInPlace();
}

const Expression Multiplication::Base(const Expression e) {
  if (e.type() == ExpressionNode::Type::Power) {
    return e.childAtIndex(0);
  }
  return e;
}

template MatrixComplex<float> MultiplicationNode::computeOnComplexAndMatrix<float>(std::complex<float> const, const MatrixComplex<float>, Preferences::ComplexFormat);
template MatrixComplex<double> MultiplicationNode::computeOnComplexAndMatrix<double>(std::complex<double> const, const MatrixComplex<double>, Preferences::ComplexFormat);
template Complex<float> MultiplicationNode::compute<float>(const std::complex<float>, const std::complex<float>, Preferences::ComplexFormat);
template Complex<double> MultiplicationNode::compute<double>(const std::complex<double>, const std::complex<double>, Preferences::ComplexFormat);
template void Multiplication::computeOnArrays<double>(double * m, double * n, double * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);

}
