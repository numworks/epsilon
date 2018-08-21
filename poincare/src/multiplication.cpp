#include <poincare/multiplication.h>
#include <poincare/addition.h>
#include <poincare/arithmetic.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
//#include <poincare/matrix.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
//#include <poincare/simplification_root.h>
#include <poincare/subtraction.h>
//#include <poincare/tangent.h>
#include <poincare/undefined.h>
#include <cmath>
#include <ion.h>
#include <assert.h>

namespace Poincare {

static inline const Expression Base(const Expression e) {
  if (e.type() == ExpressionNode::Type::Power) {
    return e.childAtIndex(0);
  }
  return e;
}

MultiplicationNode * MultiplicationNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<MultiplicationNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

ExpressionNode::Sign MultiplicationNode::sign() const {
  if (numberOfChildren() == 0) {
    return Sign::Unknown;
  }
  int sign = 1;
  for (int i = 0; i < numberOfChildren(); i++) {
    sign *= (int)childAtIndex(i)->sign();
  }
  return (Sign)sign;
}

int MultiplicationNode::polynomialDegree(char symbolName) const {
  int degree = 0;
  for (int i = 0; i < numberOfChildren(); i++) {
    int d = childAtIndex(i)->polynomialDegree(symbolName);
    if (d < 0) {
      return -1;
    }
    degree += d;
  }
  return degree;
}

int MultiplicationNode::getPolynomialCoefficients(char symbolName, Expression coefficients[]) const {
  return Multiplication(this).getPolynomialCoefficients(symbolName, coefficients);
}

template<typename T>
void MultiplicationNode::computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns) {
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

template<typename T>
MatrixComplex<T> MultiplicationNode::computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n) {
  if (m.numberOfColumns() != n.numberOfRows()) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> result;
  for (int i = 0; i < m.numberOfRows(); i++) {
    for (int j = 0; j < n.numberOfColumns(); j++) {
      std::complex<T> c(0.0);
      for (int k = 0; k < m.numberOfColumns(); k++) {
        c += m.complexAtIndex(i*m.numberOfColumns()+k)*n.complexAtIndex(k*n.numberOfColumns()+j);
      }
      result.addChildAtIndexInPlace(Complex<T>(c), i*n.numberOfColumns()+j, result.numberOfChildren());
    }
  }
  result.setDimensions(m.numberOfRows(), n.numberOfColumns());
  return result;
}

Expression MultiplicationNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  return Multiplication(this).setSign(s, context, angleUnit);
}

bool MultiplicationNode::needsParenthesesWithParent(const SerializationHelperInterface * parentNode) const {
  Type types[] = {Type::Division, Type::Power, Type::Factorial};
  return static_cast<const ExpressionNode *>(parentNode)->isOfType(types, 3);
}

LayoutRef MultiplicationNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  const char middleDotString[] = {Ion::Charset::MiddleDot, 0};
  return LayoutHelper::Infix(Multiplication(this), floatDisplayMode, numberOfSignificantDigits, middleDotString);
}

int MultiplicationNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  const char multiplicationString[] = {Ion::Charset::MultiplicationSign, 0};
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, multiplicationString);
}

Expression MultiplicationNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return Multiplication(this).shallowReduce(context, angleUnit);
}

Expression MultiplicationNode::shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const {
  return Multiplication(this).shallowBeautify(context, angleUnit);
}

Expression MultiplicationNode::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  return Multiplication(this).denominator(context, angleUnit);
}

/* Multiplication */

Expression Multiplication::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  assert(s == ExpressionNode::Sign::Positive);
  Expression result = *this;
  for (int i = 0; i < result.numberOfChildren(); i++) {
    if (childAtIndex(i).sign() == ExpressionNode::Sign::Negative) {
      result.replaceChildAtIndexInPlace(i, childAtIndex(i).setSign(s, context, angleUnit));
    }
  }
  return result.shallowReduce(context, angleUnit);
}

Expression Multiplication::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return privateShallowReduce(context, angleUnit, true, true);
}

Expression Multiplication::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const {
  /* Beautifying a Multiplication consists in several possible operations:
   * - Add Opposite ((-3)*x -> -(3*x), useful when printing fractions)
   * - Adding parenthesis if needed (a*(b+c) is not a*b+c)
   * - Creating a Division if there's either a term with a power of -1 (a.b^(-1)
   *   shall become a/b) or a non-integer rational term (3/2*a -> (3*a)/2). */

  Expression thisCopy = *this;
  // Step 1: Turn -n*A into -(n*A)
  if (thisCopy.childAtIndex(0).isNumber() && thisCopy.childAtIndex(0).sign() == ExpressionNode::Sign::Negative) {
    if (thisCopy.childAtIndex(0).type() == ExpressionNode::Type::Rational && static_cast<Rational>(thisCopy.childAtIndex(0)).isMinusOne()) {
      thisCopy.removeChildAtIndexInPlace(0);
    } else {
      thisCopy.replaceChildAtIndexInPlace(0, childAtIndex(0).setSign(ExpressionNode::Sign::Positive, context, angleUnit));
    }
    return Opposite(static_cast<Multiplication>(thisCopy).squashUnaryHierarchy());
  }

  /* Step 2: Merge negative powers: a*b^(-1)*c^(-pi)*d = a*(b*c^pi)^(-1)
   * This also turns 2/3*a into 2*a*3^(-1) */
  thisCopy = static_cast<Multiplication>(thisCopy).mergeNegativePower(context, angleUnit);
  if (thisCopy.type() == ExpressionNode::Type::Power) {
    return thisCopy.shallowBeautify(context, angleUnit);
  }
  assert(thisCopy.type() == ExpressionNode::Type::Multiplication);

  // Step 3: Add Parenthesis if needed
  for (int i = 0; i < thisCopy.numberOfChildren(); i++) {
    const Expression o = thisCopy.childAtIndex(i);
    if (thisCopy.type() == ExpressionNode::Type::Addition ) {
      Parenthesis p(o);
      thisCopy.replaceChildAtIndexInPlace(i, p);
    }
  }

  // Step 4: Create a Division if needed
  for (int i = 0; i < thisCopy.numberOfChildren(); i++) {
    if (!(thisCopy.childAtIndex(i).type() == ExpressionNode::Type::Power && thisCopy.childAtIndex(i).childAtIndex(1).type() == ExpressionNode::Type::Rational && static_cast<Rational>(thisCopy.childAtIndex(i).childAtIndex(1)).isMinusOne())) {
      continue;
    }

    // Let's remove the denominator-to-be from this
    Expression denominatorOperand = thisCopy.childAtIndex(i).childAtIndex(0);
    thisCopy.removeChildAtIndexInPlace(i);
    Expression numeratorOperand = thisCopy.shallowReduce(context, angleUnit);
    // Delete parenthesis unnecessary on numerator
    if (numeratorOperand.type() == ExpressionNode::Type::Parenthesis) {
      numeratorOperand = numeratorOperand.childAtIndex(0);
    }
    return Division(numeratorOperand, denominatorOperand).shallowBeautify(context, angleUnit);
  }
  return thisCopy;
}

int Multiplication::getPolynomialCoefficients(char symbolName, Expression coefficients[]) const {
  int deg = polynomialDegree(symbolName);
  if (deg < 0 || deg > Expression::k_maxPolynomialDegree) {
    return -1;
  }
  // Initialization of coefficients
  for (int i = 1; i <= deg; i++) {
    coefficients[i] = Rational(0);
  }
  coefficients[0] = Rational(1);

  Expression intermediateCoefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Let's note result = a(0)+a(1)*X+a(2)*X^2+a(3)*x^3+..
  for (int i = 0; i < numberOfChildren(); i++) {
    // childAtIndex(i) = b(0)+b(1)*X+b(2)*X^2+b(3)*x^3+...
    int degI = childAtIndex(i).getPolynomialCoefficients(symbolName, intermediateCoefficients);
    assert(degI <= Expression::k_maxPolynomialDegree);
    for (int j = deg; j > 0; j--) {
      // new coefficients[j] = b(0)*a(j)+b(1)*a(j-1)+b(2)*a(j-2)+...
      Addition a;
      int jbis = j > degI ? degI : j;
      for (int l = 0; l <= jbis ; l++) {
        // Always copy the a and b coefficients are they are used multiple times
        a.addChildAtIndexInPlace(Multiplication(intermediateCoefficients[l], coefficients[j-l]), a.numberOfChildren(), a.numberOfChildren());
      }
      /* a(j) and b(j) are used only to compute coefficient at rank >= j, we
       * can delete them as we compute new coefficient by decreasing ranks. */
      // TODO remove if (j <= degI) { delete intermediateCoefficients[j]; };
      coefficients[j] = a;
    }
    // new coefficients[0] = a(0)*b(0)
    coefficients[0] = Multiplication(coefficients[0], intermediateCoefficients[0]);
  }
  return deg;
}

Expression Multiplication::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  // Merge negative power: a*b^-1*c^(-Pi)*d = a*(b*c^Pi)^-1
  // WARNING: we do not want to change the expression but to create a new one.
  Expression e = mergeNegativePower(context, angleUnit);
  if (e.type() == ExpressionNode::Type::Power) {
    return e.denominator(context, angleUnit);
  } else {
    assert(e.type() == ExpressionNode::Type::Multiplication);
    for (int i = 0; i < e.numberOfChildren(); i++) {
      // a*b^(-1)*... -> a*.../b
      if (e.childAtIndex(i).type() == ExpressionNode::Type::Power && e.childAtIndex(i).childAtIndex(1).type() == ExpressionNode::Type::Rational && static_cast<Rational>(e.childAtIndex(i).childAtIndex(1)).isMinusOne()) {
        return e.childAtIndex(i).childAtIndex(0);
      }
    }
  }
  return Expression();
}

Expression Multiplication::privateShallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool shouldExpand, bool canBeInterrupted) const {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);;
    if (e.isUndefinedOrAllocationFailure()) {
      return e;
    }
  }

  Multiplication thisCopy = *this;

  /* Step 1: MultiplicationNode is associative, so let's start by merging children
   * which also are multiplications themselves. */
  thisCopy.mergeMultiplicationChildrenInPlace();

  /* Step 2: If any of the child is zero, the multiplication result is zero */
  for (int i = 0; i < thisCopy.numberOfChildren(); i++) {
    const Expression o = thisCopy.childAtIndex(i);
    if (o.type() == ExpressionNode::Type::Rational && static_cast<Rational>(o).isZero()) {
      return Rational(0);
    }
  }

  // Step 3: Sort the children
  thisCopy.sortChildrenInPlace(ExpressionNode::SimplificationOrder, canBeInterrupted);

#if MATRIX_EXACT_REDUCING
#if 0 // OLD CODE
  /* Step 3bis: get rid of matrix */
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
        return replaceWith(new Undefined(), true);
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
        newMatrixOperands[e] = new Addition();
        int i2 = e%m;
        int i1 = e/m;
        for (int j = 0; j < n; j++) {
          Expression * mult = new Multiplication(currentMatrix->childAtIndex(j+om*i1), resultMatrix->childAtIndex(j*m+i2), true);
          static_cast<Addition *>(newMatrixOperands[e])->addOperand(mult);
          mult->shallowReduce(context, angleUnit);
        }
        Reduce(&newMatrixOperands[e], context, angleUnit, false);
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
      m->shallowReduce(context, angleUnit);
    }
    return replaceWith(resultMatrix, true)->shallowReduce(context, angleUnit);
  }
#endif
#endif

  /* Step 4: Gather like terms. For example, turn pi^2*pi^3 into pi^5. Thanks to
   * the simplification order, such terms are guaranteed to be next to each
   * other. */
  int i = 0;
  while (i < thisCopy.numberOfChildren()-1) {
    Expression oi = thisCopy.childAtIndex(i);
    Expression oi1 = thisCopy.childAtIndex(i+1);
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
        thisCopy.factorizeBase(i, i+1, context, angleUnit);
        continue;
      }
    } else if (TermHasNumeralBase(oi) && TermHasNumeralBase(oi1) && TermsHaveIdenticalExponent(oi, oi1)) {
      thisCopy.factorizeExponent(i, i+1, context, angleUnit);
      continue;
    }
    i++;
  }

  /* Step 5: We look for terms of form sin(x)^p*cos(x)^q with p, q rational of
   *opposite signs. We replace them by either:
   * - tan(x)^p*cos(x)^(p+q) if |p|<|q|
   * - tan(x)^(-q)*sin(x)^(p+q) otherwise */
  for (int i = 0; i < thisCopy.numberOfChildren(); i++) {
    Expression o1 = thisCopy.childAtIndex(i);
    if (Base(o1).type() == ExpressionNode::Type::Sine && TermHasNumeralExponent(o1)) {
      const Expression x = Base(o1).childAtIndex(0);
      /* Thanks to the SimplificationOrder, Cosine-base factors are after
       * Sine-base factors */
      for (int j = i+1; j < thisCopy.numberOfChildren(); j++) {
        Expression o2 = thisCopy.childAtIndex(j);
        if (Base(o2).type() == ExpressionNode::Type::Cosine && TermHasNumeralExponent(o2) && Base(o2).childAtIndex(0).isIdenticalTo(x)) {
          thisCopy.factorizeSineAndCosine(i, j, context, angleUnit);
          break;
        }
      }
    }
  }
  /* Replacing sin/cos by tan factors may have mixed factors and factors are
   * guaranteed to be sorted (according ot SimplificationOrder) at the end of
   * shallowReduce */
  thisCopy.sortChildrenInPlace(ExpressionNode::SimplificationOrder, canBeInterrupted);

  /* Step 6: We remove rational children that appeared in the middle of sorted
   * children. It's important to do this after having factorized because
   * factorization can lead to new ones. Indeed:
   * pi^(-1)*pi-> 1
   * i*i -> -1
   * 2^(1/2)*2^(1/2) -> 2
   * sin(x)*cos(x) -> 1*tan(x)
   * Last, we remove the only rational child if it is one and not the only
   * child. */
  i = 1;
  while (i < thisCopy.numberOfChildren()) {
    Expression o = thisCopy.childAtIndex(i);
    if (o.type() == ExpressionNode::Type::Rational && static_cast<Rational>(o).isOne()) {
      thisCopy.removeChildAtIndexInPlace(i);
      continue;
    }
    if (o.isNumber()) {
      if (thisCopy.childAtIndex(0).isNumber()) {
        Number o0 = static_cast<Rational>(thisCopy.childAtIndex(0));
        Number m = Number::Multiplication(o0, static_cast<Number>(o));
        thisCopy.replaceChildAtIndexInPlace(0, m);
        thisCopy.removeChildAtIndexInPlace(i);
      } else {
        // Number child has to be first
        thisCopy.removeChildAtIndexInPlace(i);
        thisCopy.addChildAtIndexInPlace(o, 0, thisCopy.numberOfChildren());
      }
      continue;
    }
    i++;
  }
  if (thisCopy.childAtIndex(0).type() == ExpressionNode::Type::Rational && static_cast<Rational>(thisCopy.childAtIndex(0)).isOne() && thisCopy.numberOfChildren() > 1) {
    thisCopy.removeChildAtIndexInPlace(0);
  }

  /* Step 7: Expand multiplication over addition children if any. For example,
   * turn (a+b)*c into a*c + b*c. We do not want to do this step right now if
   * the parent is a multiplication to avoid missing factorization such as
   * (x+y)^(-1)*((a+b)*(x+y)).
   * Note: This step must be done after Step 4, otherwise we wouldn't be able to
   * reduce expressions such as (x+y)^(-1)*(x+y)(a+b). */
  if (shouldExpand) { // && parent()->type() != ExpressionNode::Type::Multiplication) { // TODO: Handle this top dow in deepReduce?
    for (int i=0; i<thisCopy.numberOfChildren(); i++) {
      if (thisCopy.childAtIndex(i).type() == ExpressionNode::Type::Addition) {
        return thisCopy.distributeOnOperandAtIndex(i, context, angleUnit);
      }
    }
  }

  // Step 8: Let's remove the multiplication altogether if it has one child
  Expression result = thisCopy.squashUnaryHierarchy();

  return result;
}

void Multiplication::mergeMultiplicationChildrenInPlace() {
  // Multiplication is associative: a*(b*c)->a*b*c
  int i = 0;
  int initialNumberOfChildren = numberOfChildren();
  while (i < initialNumberOfChildren) {
    Expression c = childAtIndex(i);
    if (c.type() == ExpressionNode::Type::Multiplication) {
      mergeChildrenAtIndexInPlace(c, numberOfChildren()); // TODO: ensure that matrix children are not swapped to implement MATRIX_EXACT_REDUCING
      continue;
    }
    i++;
  }
}

void Multiplication::factorizeBase(int i, int j, Context & context, Preferences::AngleUnit angleUnit) {
  /* This function factorizes two children which have a common base. For example
   * if this is Multiplication(pi^2, pi^3), then pi^2 and pi^3 could be merged
   * and this turned into Multiplication(pi^5). */

  Expression e = childAtIndex(j);
  // Step 1: Get rid of the child j
  removeChildAtIndexInPlace(j);
  // Step 2: Merge child j in child i by factorizing base
  mergeInChildByFactorizingBase(i, e, context, angleUnit);
}

void Multiplication::mergeInChildByFactorizingBase(int i, Expression e, Context & context, Preferences::AngleUnit angleUnit) {
  /* This function replace the child at index i by its factorization with e.
   * e and childAtIndex(i) are supposed to have a command base. */

  // Step 1: Find the new exponent
  Expression s = Addition(CreateExponent(childAtIndex(i)), CreateExponent(e)).shallowReduce(context, angleUnit); // pi^2*pi^3 -> pi^(2+3) -> pi^5
  // Step 2: Create the new Power
  Expression p = Power(Base(childAtIndex(i)), s).shallowReduce(context, angleUnit); // pi^2*pi^-2 -> pi^0 -> 1
  // Step 3: Replace one of the child
  replaceChildAtIndexInPlace(i, p);
  /* Step 4: Reducing the new power might have turned it into a multiplication,
   * ie: 12^(1/2) -> 2*3^(1/2). In that case, we need to merge the multiplication
   * node with this. */
  if (p.type() == ExpressionNode::Type::Multiplication) {
    mergeMultiplicationChildrenInPlace();
  }
}

void Multiplication::factorizeExponent(int i, int j, Context & context, Preferences::AngleUnit angleUnit) {
  /* This function factorizes children which share a common exponent. For
   * example, it turns Multiplication(2^x,3^x) into Multiplication(6^x). */

  // Step 1: find the new base
  Expression m = Multiplication(Base(childAtIndex(i)), Base(childAtIndex(j))).shallowReduce(context, angleUnit); // 2^x*3^x -> (2*3)^x -> 6^x
  // Step 2: create the new power
  Expression p = Power(m, childAtIndex(i).childAtIndex(1)).shallowReduce(context, angleUnit); // 2^x*(1/2)^x -> (2*1/2)^x -> 1
  // Step 3: Replace one of the child
  replaceChildAtIndexInPlace(i, p);
  // Step 4: Get rid of the other child
  removeChildAtIndexInPlace(j);
  /* Step 5: reducing the new power might have turned it into a multiplication,
   * ie: 12^(1/2) -> 2*3^(1/2). In that case, we need to merge the multiplication
   * node with this. */
  if (p.type() == ExpressionNode::Type::Multiplication) {
    mergeMultiplicationChildrenInPlace();
  }
}

Expression Multiplication::distributeOnOperandAtIndex(int i, Context & context, Preferences::AngleUnit angleUnit) const {
  /* This method creates a*...*b*y... + a*...*c*y... + ... from
   * a*...*(b+c+...)*y... */
  assert(i >= 0 && i < numberOfChildren());
  assert(childAtIndex(i).type() == ExpressionNode::Type::Addition);

  Addition a;
  int numberOfAdditionTerms = childAtIndex(i).numberOfChildren();
  for (int j = 0; j < numberOfAdditionTerms; j++) {
    Multiplication m = *this;
    m.replaceChildAtIndexInPlace(i, childAtIndex(i).childAtIndex(j));
    // Reduce m: pi^(-1)*(pi + x) -> pi^(-1)*pi + pi^(-1)*x -> 1 + pi^(-1)*x
    a.addChildAtIndexInPlace(m.shallowReduce(context, angleUnit), a.numberOfChildren(), a.numberOfChildren());
  }
  return a.shallowReduce(context, angleUnit); // Order terms, put under a common denominator if needed
}

void Multiplication::addMissingFactors(Expression factor, Context & context, Preferences::AngleUnit angleUnit) {
  if (factor.type() == ExpressionNode::Type::Multiplication) {
    for (int j = 0; j < factor.numberOfChildren(); j++) {
      addMissingFactors(factor.childAtIndex(j), context, angleUnit);
    }
    return;
  }
  /* Special case when factor is a Rational: if 'this' has already a rational
   * child, we replace it by its LCM with factor ; otherwise, we simply add
   * factor as an child. */
  if (numberOfChildren() > 0 && childAtIndex(0).type() == ExpressionNode::Type::Rational && factor.type() == ExpressionNode::Type::Rational) {
    assert(static_cast<Rational>(factor).integerDenominator().isOne());
    assert(static_cast<Rational>(childAtIndex(0)).integerDenominator().isOne());
    replaceChildAtIndexInPlace(0, Rational(Arithmetic::LCM(static_cast<Rational>(factor).unsignedIntegerNumerator(), static_cast<Rational>(childAtIndex(0)).unsignedIntegerNumerator())));
    return;
  }
  if (factor.type() != ExpressionNode::Type::Rational) {
    /* If factor is not a rational, we merge it with the child of identical
     * base if any. Otherwise, we add it as an new child. */
    for (int i = 0; i < numberOfChildren(); i++) {
      if (TermsHaveIdenticalBase(childAtIndex(i), factor)) {
        Expression sub = Subtraction(CreateExponent(childAtIndex(i)), CreateExponent(factor)).deepReduce(context, angleUnit);
        if (sub.sign() == ExpressionNode::Sign::Negative) { // index[0] < index[1]
          sub = Opposite(sub).shallowReduce(context, angleUnit);
          if (factor.type() == ExpressionNode::Type::Power) {
            factor.replaceChildAtIndexInPlace(1, sub);
          } else {
            factor = Power(factor, sub);
          }
          factor = factor.shallowReduce(context, angleUnit);
          mergeInChildByFactorizingBase(i, factor, context, angleUnit);
          // TODO: we use to shallowReduce childAtIndex(i) here? Needed ?
        } else if (sub.sign() == ExpressionNode::Sign::Unknown) {
          // TODO: we use to shallowReduce childAtIndex(i) here? Needed ?
          mergeInChildByFactorizingBase(i, factor, context, angleUnit);
        } else {}
        /* Reducing the new child i can lead to creating a new multiplication
         * (ie 2^(1+2*3^(1/2)) -> 2*2^(2*3^(1/2)). We thus have to get rid of
         * nested multiplication: */
        //mergeMultiplicationOperands(); // TODO: required ???
        return;
      }
    }
  }
  addChildAtIndexInPlace(factor, 0, numberOfChildren());
  sortChildrenInPlace(ExpressionNode::SimplificationOrder, false);
}

void Multiplication::factorizeSineAndCosine(int i, int j, Context & context, Preferences::AngleUnit angleUnit) {
  /* This function turn sin(x)^p * cos(x)^q into either:
   * - tan(x)^p*cos(x)^(p+q) if |p|<|q|
   * - tan(x)^(-q)*sin(x)^(p+q) otherwise */
  const Expression x = Base(childAtIndex(i)).childAtIndex(0);
  // We check before that p and q were numbers
  Number p = static_cast<Number>(CreateExponent(childAtIndex(i)));
  Number q = static_cast<Number>(CreateExponent(childAtIndex(j)));
  /* If p and q have the same sign, we cannot replace them by a tangent */
  if ((int)p.sign()*(int)q.sign() > 0) {
    return;
  }
  Number sumPQ = Number::Addition(p, q);
  Number absP = p.setSign(ExpressionNode::Sign::Positive, context, angleUnit);
  Number absQ = q.setSign(ExpressionNode::Sign::Positive, context, angleUnit);
  // TODO: uncomment once Tangent is implemented
  /*Expression tan = Tangent(x);
  if (Number::NaturalOrder(absP, absQ) < 0) {
    // replace sin(x)^p by tan(x)^p
    replaceChildAtIndexInPlace(i, Power(tan, p).shallowReduce(context, angleUnit));
    // replace cos(x)^q by cos(x)^(p+q)
    replaceChildAtIndexInPlace(j, Power(Base(childAtIndex(j)), sumPQ).shallowReduce(context, angleUnit));
  } else {
    // replace cos(x)^q by tan(x)^(-q)
    replaceChildAtIndexInPlace(j, Power(tan, Number::Multiplication(q, Rational(-1)).shallowReduce(context, angleUnit)).shallowReduce(context, angleUnit));
    // replace sin(x)^p by sin(x)^(p+q)
    replaceChildAtIndexInPlace(i, Power(Base(childAtIndex(i)), sumPQ).shallowReduce(context, angleUnit));
  }*/
}

bool Multiplication::HaveSameNonNumeralFactors(const Expression e1, const Expression e2) {
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
    if (!(e1.childAtIndex(firstNonNumeralOperand1+i).isIdenticalTo(e2.childAtIndex(firstNonNumeralOperand2+i)))) {
      return false;
    }
  }
  return true;
}

const Expression Multiplication::CreateExponent(Expression e) {
  Expression result = e.type() == ExpressionNode::Type::Power ? e.childAtIndex(1) : Rational(1);
  return result;
}

bool Multiplication::TermsHaveIdenticalBase(const Expression e1, const Expression e2) {
  return Base(e1).isIdenticalTo(Base(e2));
}

bool Multiplication::TermsHaveIdenticalExponent(const Expression e1, const Expression e2) {
  /* Note: We will return false for e1=2 and e2=Pi, even though one could argue
   * that these have the same exponent whose value is 1. */
  return e1.type() == ExpressionNode::Type::Power && e2.type() == ExpressionNode::Type::Power && (e1.childAtIndex(1).isIdenticalTo(e2.childAtIndex(1)));
}

bool Multiplication::TermHasNumeralBase(const Expression e) {
  return Base(e).isNumber();
}

bool Multiplication::TermHasNumeralExponent(const Expression e) {
  if (e.type() != ExpressionNode::Type::Power) {
    return true;
  }
  if (e.childAtIndex(1).isNumber()) {
    return true;
  }
  return false;
}

Expression Multiplication::mergeNegativePower(Context & context, Preferences::AngleUnit angleUnit) const {
  Multiplication thisCopy = *this;
  Multiplication m;
  // Special case for rational p/q: if q != 1, q should be at denominator
  if (childAtIndex(0).type() == ExpressionNode::Type::Rational && !static_cast<const Rational>(childAtIndex(0)).integerDenominator().isOne()) {
    Rational r = static_cast<Rational>(childAtIndex(0));
    m.addChildAtIndexInPlace(Rational(r.integerDenominator()), 0, m.numberOfChildren());
    if (r.signedIntegerNumerator().isOne()) {
      thisCopy.removeChildAtIndexInPlace(0);
    } else {
      thisCopy.replaceChildAtIndexInPlace(0, Rational(r.signedIntegerNumerator()));
    }
  }
  int i = 0;
  while (i < thisCopy.numberOfChildren()) {
    if (thisCopy.childAtIndex(i).type() == ExpressionNode::Type::Power && thisCopy.childAtIndex(i).childAtIndex(1).sign() == ExpressionNode::Sign::Negative) {
     Expression e = thisCopy.childAtIndex(i);
     e.replaceChildAtIndexInPlace(1, e.childAtIndex(1).setSign(ExpressionNode::Sign::Positive, context, angleUnit));
     m.addChildAtIndexInPlace(e.shallowReduce(context, angleUnit), m.numberOfChildren(), m.numberOfChildren());
     thisCopy.removeChildAtIndexInPlace(i);
    } else {
      i++;
    }
  }
  if (m.numberOfChildren() == 0) {
    return thisCopy;
  }
  m.sortChildrenInPlace(ExpressionNode::SimplificationOrder, true);
  Power p(m.squashUnaryHierarchy(), Rational(-1));
  thisCopy.addChildAtIndexInPlace(p, 0, thisCopy.numberOfChildren());
  thisCopy.sortChildrenInPlace(ExpressionNode::SimplificationOrder, true);
  return thisCopy.squashUnaryHierarchy();
}

template MatrixComplex<float> MultiplicationNode::computeOnComplexAndMatrix<float>(std::complex<float> const, const MatrixComplex<float>);
template MatrixComplex<double> MultiplicationNode::computeOnComplexAndMatrix<double>(std::complex<double> const, const MatrixComplex<double>);
template Complex<float> MultiplicationNode::compute<float>(const std::complex<float>, const std::complex<float>);
template Complex<double> MultiplicationNode::compute<double>(const std::complex<double>, const std::complex<double>);
template void MultiplicationNode::computeOnArrays<double>(double * m, double * n, double * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);

}
