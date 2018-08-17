#include <poincare/multiplication.h>
#include <poincare/addition.h>
//#include <poincare/arithmetic.h>
//#include <poincare/division.h>
#include <poincare/layout_helper.h>
//#include <poincare/matrix.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
//#include <poincare/power.h>
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
    int degI = childAtIndex(i)->getPolynomialCoefficients(symbolName, intermediateCoefficients);
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

Expression MultiplicationNode::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
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
  Expression e = Multiplication(this);
  return e;
  //TODO
#if 0
  // Merge negative power: a*b^-1*c^(-Pi)*d = a*(b*c^Pi)^-1
  // WARNING: we do not want to change the expression but to create a new one.
  SimplificationRoot root(clone());
  Expression * e = ((Multiplication *)root.childAtIndex(0))->mergeNegativePower(context, angleUnit);
  Expression * result = nullptr;
  if (e->type() == Type::Power) {
    result = static_cast<Power *>(e)->denominator(context, angleUnit);
  } else {
    assert(e->type() == Type::Multiplication);
    for (int i = 0; i < e->numberOfChildren(); i++) {
      // a*b^(-1)*... -> a*.../b
      if (e->childAtIndex(i)->type() == Type::Power && e->childAtIndex(i)->childAtIndex(1)->type() == Type::Rational && static_cast<const Rational *>(e->childAtIndex(i)->childAtIndex(1))->isMinusOne()) {
        Power * p = static_cast<Power *>(e->editableOperand(i));
        result = p->editableOperand(0);
        p->detachOperand((result));
      }
    }
  }
  root.detachOperand(e);
  delete e;
  return result;
#endif
}

// MULTIPLICATION

Expression Multiplication::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  assert(s == ExpressionNode::Sign::Positive);
  Expression result = *this;
  for (int i = 0; i < numberOfChildren(); i++) {
    if (childAtIndex(i).sign() == ExpressionNode::Sign::Negative) {
      result.replaceChildAtIndexInPlace(i, childAtIndex(i).setSign(s, context, angleUnit));
    }
  }
  return result.shallowReduce(context, angleUnit);
}

Expression Multiplication::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return privateShallowReduce(context, angleUnit, true, true);
}

Expression Multiplication::privateShallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool shouldExpand, bool canBeInterrupted) const {
  return Expression::defaultShallowReduce(context, angleUnit);;
  //TODO
#if 0
  Expression * e = Expression::defaultShallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  /* Step 1: MultiplicationNode is associative, so let's start by merging children
   * which also are multiplications themselves. */
  mergeMultiplicationOperands();

  /* Step 2: If any of the operand is zero, the multiplication result is zero */
  for (int i = 0; i < numberOfChildren(); i++) {
    const Expression * o = childAtIndex(i);
    if (o->type() == Type::Rational && static_cast<const Rational *>(o)->isZero()) {
      return replaceWith(RationalReference(0), true);
    }
  }

  // Step 3: Sort the operands
  sortOperands(SimplificationOrder, canBeInterrupted);

#if MATRIX_EXACT_REDUCING
  /* Step 3bis: get rid of matrix */
  int n = 1;
  int m = 1;
  /* All operands have been simplified so if any operand contains a matrix, it
   * is at the root node of the operand. Moreover, thanks to the simplification
   * order, all matrix operands (if any) are the last operands. */
  Expression * lastOperand = editableOperand(numberOfChildren()-1);
  if (lastOperand->type() == Type::Matrix) {
    Matrix * resultMatrix = static_cast<Matrix *>(lastOperand);
    // Use the last matrix operand as the final matrix
    n = resultMatrix->numberOfRows();
    m = resultMatrix->numberOfColumns();
    /* Scan accross the multiplication operands to find any other matrix:
     * (the last operand is the result matrix so we start at
     * numberOfChildren()-2)*/
    int k = numberOfChildren()-2;
    while (k >= 0 && childAtIndex(k)->type() == Type::Matrix) {
      Matrix * currentMatrix = static_cast<Matrix *>(editableOperand(k));
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
          Expression * mult = new Multiplication(currentMatrix->editableOperand(j+om*i1), resultMatrix->editableOperand(j*m+i2), true);
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
    // Distribute the remaining multiplication on matrix operands
    for (int i = 0; i < n*m; i++) {
      Multiplication * m = static_cast<Multiplication *>(clone());
      Expression * entryI = resultMatrix->editableOperand(i);
      resultMatrix->replaceOperand(entryI, m, false);
      m->addOperand(entryI);
      m->shallowReduce(context, angleUnit);
    }
    return replaceWith(resultMatrix, true)->shallowReduce(context, angleUnit);
  }
#endif

  /* Step 4: Gather like terms. For example, turn pi^2*pi^3 into pi^5. Thanks to
   * the simplification order, such terms are guaranteed to be next to each
   * other. */
  int i = 0;
  while (i < numberOfChildren()-1) {
    Expression * oi = editableOperand(i);
    Expression * oi1 = editableOperand(i+1);
    if (TermsHaveIdenticalBase(oi, oi1)) {
      bool shouldFactorizeBase = true;
      if (TermHasNumeralBase(oi)) {
        /* Combining powers of a given rational isn't straightforward. Indeed,
         * there are two cases we want to deal with:
         *  - 2*2^(1/2) or 2*2^pi, we want to keep as-is
         *  - 2^(1/2)*2^(3/2) we want to combine. */
        shouldFactorizeBase = oi->type() == Type::Power && oi1->type() == Type::Power;
      }
      if (shouldFactorizeBase) {
        factorizeBase(oi, oi1, context, angleUnit);
        continue;
      }
    } else if (TermHasNumeralBase(oi) && TermHasNumeralBase(oi1) && TermsHaveIdenticalExponent(oi, oi1)) {
      factorizeExponent(oi, oi1, context, angleUnit);
      continue;
    }
    i++;
  }

  /* Step 5: We look for terms of form sin(x)^p*cos(x)^q with p, q rational of
   *opposite signs. We replace them by either:
   * - tan(x)^p*cos(x)^(p+q) if |p|<|q|
   * - tan(x)^(-q)*sin(x)^(p+q) otherwise */
  for (int i = 0; i < numberOfChildren(); i++) {
    Expression * o1 = editableOperand(i);
    if (Base(o1)->type() == Type::Sine && TermHasNumeralExponent(o1)) {
      const Expression * x = Base(o1)->childAtIndex(0);
      /* Thanks to the SimplificationOrder, Cosine-base factors are after
       * Sine-base factors */
      for (int j = i+1; j < numberOfChildren(); j++) {
        Expression * o2 = editableOperand(j);
        if (Base(o2)->type() == Type::Cosine && TermHasNumeralExponent(o2) && Base(o2)->childAtIndex(0)->isIdenticalTo(x)) {
          factorizeSineAndCosine(o1, o2, context, angleUnit);
          break;
        }
      }
    }
  }
  /* Replacing sin/cos by tan factors may have mixed factors and factors are
   * guaranteed to be sorted (according ot SimplificationOrder) at the end of
   * shallowReduce */
  sortOperands(SimplificationOrder, true);

  /* Step 6: We remove rational operands that appeared in the middle of sorted
   * operands. It's important to do this after having factorized because
   * factorization can lead to new ones. Indeed:
   * pi^(-1)*pi-> 1
   * i*i -> -1
   * 2^(1/2)*2^(1/2) -> 2
   * sin(x)*cos(x) -> 1*tan(x)
   * Last, we remove the only rational operand if it is one and not the only
   * operand. */
  i = 1;
  while (i < numberOfChildren()) {
    Expression * o = editableOperand(i);
    if (o->type() == Type::Rational && static_cast<Rational *>(o)->isOne()) {
      removeOperand(o, true);
      continue;
    }
    if (o->type() == Type::Rational) {
      if (childAtIndex(0)->type() == Type::Rational) {
        Rational * o0 = static_cast<Rational *>(editableOperand(0));
        Rational m = Rational::Multiplication(*o0, *(static_cast<Rational *>(o)));
        replaceOperand(o0, new Rational(m), true);
        removeOperand(o, true);
      } else {
        removeOperand(o, false);
        addOperandAtIndex(o, 0);
      }
      continue;
    }
    i++;
  }
  if (childAtIndex(0)->type() == Type::Rational && static_cast<Rational *>(editableOperand(0))->isOne() && numberOfChildren() > 1) {
    removeOperand(editableOperand(0), true);
  }


  /* Step 7: Expand multiplication over addition operands if any. For example,
   * turn (a+b)*c into a*c + b*c. We do not want to do this step right now if
   * the parent is a multiplication to avoid missing factorization such as
   * (x+y)^(-1)*((a+b)*(x+y)).
   * Note: This step must be done after Step 4, otherwise we wouldn't be able to
   * reduce expressions such as (x+y)^(-1)*(x+y)(a+b). */
  if (shouldExpand && parent()->type() != Type::Multiplication) {
    for (int i=0; i<numberOfChildren(); i++) {
      if (childAtIndex(i)->type() == Type::Addition) {
        return distributeOnOperandAtIndex(i, context, angleUnit);
      }
    }
  }

  // Step 8: Let's remove the multiplication altogether if it has one operand
  Expression * result = squashUnaryHierarchy();

  return result;
#endif
}

bool Multiplication::HaveSameNonNumeralFactors(const Expression e1, const Expression e2) {
  assert(e1.numberOfChildren() > 0);
  assert(e2.numberOfChildren() > 0);
  int numberOfNonNumeralFactors1 = e1.childAtIndex(0).type() == ExpressionNode::Type::Rational ? e1.numberOfChildren()-1 : e1.numberOfChildren();
  int numberOfNonNumeralFactors2 = e2.childAtIndex(0).type() == ExpressionNode::Type::Rational ? e2.numberOfChildren()-1 : e2.numberOfChildren();
  if (numberOfNonNumeralFactors1 != numberOfNonNumeralFactors2) {
    return false;
  }
  int firstNonNumeralOperand1 = e1.childAtIndex(0).type() == ExpressionNode::Type::Rational ? 1 : 0;
  int firstNonNumeralOperand2 = e2.childAtIndex(0).type() == ExpressionNode::Type::Rational ? 1 : 0;
  for (int i = 0; i < numberOfNonNumeralFactors1; i++) {
    if (!(e1.childAtIndex(firstNonNumeralOperand1+i).isIdenticalTo(e2.childAtIndex(firstNonNumeralOperand2+i)))) {
      return false;
    }
  }
  return true;
}

static inline const Expression Base(const Expression e) {
  if (e.type() == ExpressionNode::Type::Power) {
    return e.childAtIndex(0);
  }
  return e;
}

void Multiplication::mergeMultiplicationOperands() {
  // Multiplication is associative: a*(b*c)->a*b*c
  int i = 0;
  int initialNumberOfChildren = numberOfChildren();
  while (i < initialNumberOfChildren) {
    Expression c = childAtIndex(i);
    if (c.type() == ExpressionNode::Type::Multiplication) {
      mergeChildrenAtIndexInPlace(c, numberOfChildren()); // TODO: ensure that matrix operands are not swapped to implement MATRIX_EXACT_REDUCING
      continue;
    }
    i++;
  }
}

void Multiplication::factorizeSineAndCosine(Expression e1, Expression e2, Context & context, Preferences::AngleUnit angleUnit) {
  // TODO Warning!! e1 and e2 are copies, so their parent is not this !!
#if 0
  assert(e1->parent() == this && o2->parent() == this);
  /* This function turn sin(x)^p * cos(x)^q into either:
   * - tan(x)^p*cos(x)^(p+q) if |p|<|q|
   * - tan(x)^(-q)*sin(x)^(p+q) otherwise */
  const Expression * x = Base(o1)->childAtIndex(0);
  Rational p = o1->type() == Type::Power ? *(static_cast<Rational *>(o1->editableOperand(1))) : Rational(1);
  Rational q = o2->type() == Type::Power ? *(static_cast<Rational *>(o2->editableOperand(1))) : Rational(1);
  /* If p and q have the same sign, we cannot replace them by a tangent */
  if ((int)p.sign()*(int)q.sign() > 0) {
    return;
  }
  Rational sumPQ = Rational::Addition(p, q);
  Rational absP = p;
  absP.setSign(Sign::Positive);
  Rational absQ = q;
  absQ.setSign(Sign::Positive);
  Expression * tan = new Tangent(x, true);
  if (Rational::NaturalOrder(absP, absQ) < 0) {
    if (o1->type() == Type::Power) {
      o1->replaceOperand(o1->childAtIndex(0), tan, true);
    } else {
      replaceOperand(o1, tan, true);
      o1 = tan;
    }
    o1->shallowReduce(context, angleUnit);
    if (o2->type() == Type::Power) {
      o2->replaceOperand(o2->childAtIndex(1), new Rational(sumPQ), true);
    } else {
      Expression * newO2 = new Power(o2, new Rational(sumPQ), false);
      replaceOperand(o2, newO2, false);
      o2 = newO2;
    }
    o2->shallowReduce(context, angleUnit);
  } else {
    if (o2->type() == Type::Power) {
      o2->replaceOperand(o2->childAtIndex(1), new Rational(Rational::Multiplication(q, Rational(-1))), true);
      o2->replaceOperand(o2->childAtIndex(0), tan, true);
    } else {
      Expression * newO2 = new Power(tan, new Rational(-1), false);
      replaceOperand(o2, newO2, true);
      o2 = newO2;
    }
    o2->shallowReduce(context, angleUnit);
    if (o1->type() == Type::Power) {
      o1->replaceOperand(o1->childAtIndex(1), new Rational(sumPQ), true);
    } else {
      Expression * newO1 = new Power(o1, new Rational(sumPQ), false);
      replaceOperand(o1, newO1, false);
      o1 = newO1;
    }
    o1->shallowReduce(context, angleUnit);
  }
#endif
}

void Multiplication::factorizeBase(Expression e1, Expression e2, Context & context, Preferences::AngleUnit angleUnit) {
#if 0
  /* This function factorizes two operands which have a common base. For example
   * if this is Multiplication(pi^2, pi^3), then pi^2 and pi^3 could be merged
   * and this turned into Multiplication(pi^5). */
  assert(TermsHaveIdenticalBase(e1, e2));

  // Step 1: Find the new exponent
  Expression * s = new Addition(CreateExponent(e1), CreateExponent(e2), false);

  // Step 2: Get rid of one of the operands
  removeOperand(e2, true);

  // Step 3: Use the new exponent
  Power * p = nullptr;
  if (e1->type() == Type::Power) {
    // If e1 is a power, replace the initial exponent with the new one
    e1->replaceOperand(e1->childAtIndex(1), s, true);
    p = static_cast<Power *>(e1);
  } else {
    // Otherwise, create a new Power node
    p = new Power(e1, s, false);
    replaceOperand(e1, p, false);
  }

  // Step 4: Reduce the new power
  s->shallowReduce(context, angleUnit); // pi^2*pi^3 -> pi^(2+3) -> pi^5
  Expression * reducedP = p->shallowReduce(context, angleUnit); // pi^2*pi^-2 -> pi^0 -> 1
  /* Step 5: Reducing the new power might have turned it into a multiplication,
   * ie: 12^(1/2) -> 2*3^(1/2). In that case, we need to merge the multiplication
   * node with this. */
  if (reducedP->type() == Type::Multiplication) {
    mergeMultiplicationOperands();
  }
#endif
}

void Multiplication::factorizeExponent(Expression e1, Expression e2, Context & context, Preferences::AngleUnit angleUnit) {
#if 0
  /* This function factorizes operands which share a common exponent. For
   * example, it turns Multiplication(2^x,3^x) into Multiplication(6^x). */
  assert(e1->parent() == this && e2->parent() == this);

  const Expression * base1 = e1->childAtIndex(0)->clone();
  const Expression * base2 = e2->childAtIndex(0);
  e2->detachOperand(base2);
  Expression * m = new Multiplication(base1, base2, false);
  removeOperand(e2, true);
  e1->replaceOperand(e1->childAtIndex(0), m, true);

  m->shallowReduce(context, angleUnit); // 2^x*3^x -> (2*3)^x -> 6^x
  Expression * reducedE1 = e1->shallowReduce(context, angleUnit); // 2^x*(1/2)^x -> (2*1/2)^x -> 1
  /* Reducing the new power might have turned it into a multiplication,
   * ie: 12^(1/2) -> 2*3^(1/2). In that case, we need to merge the multiplication
   * node with this. */
  if (reducedE1->type() == Type::Multiplication) {
    mergeMultiplicationOperands();
  }
#endif
}

Expression Multiplication::distributeOnOperandAtIndex(int i, Context & context, Preferences::AngleUnit angleUnit) {
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
    Expression reducedM = m.shallowReduce(context, angleUnit);
    a.addChildAtIndexInPlace(m, a.numberOfChildren(), a.numberOfChildren());
  }
  return a.shallowReduce(context, angleUnit); // Order terms, put under a common denominator if needed
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
  return Base(e).type() == ExpressionNode::Type::Rational;
}

bool Multiplication::TermHasNumeralExponent(const Expression e) {
  if (e.type() != ExpressionNode::Type::Power) {
    return true;
  }
  if (e.childAtIndex(1).type() == ExpressionNode::Type::Rational) {
    return true;
  }
  return false;
}

Expression Multiplication::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const {
  return *this;
  // TODO
#if 0
  /* Beautifying a Multiplication consists in several possible operations:
   * - Add Opposite ((-3)*x -> -(3*x), useful when printing fractions)
   * - Adding parenthesis if needed (a*(b+c) is not a*b+c)
   * - Creating a Division if there's either a term with a power of -1 (a.b^(-1)
   *   shall become a/b) or a non-integer rational term (3/2*a -> (3*a)/2). */

  // Step 1: Turn -n*A into -(n*A)
  if (childAtIndex(0)->type() == Type::Rational && childAtIndex(0)->sign() == Sign::Negative) {
    if (static_cast<const Rational *>(childAtIndex(0))->isMinusOne()) {
      removeOperand(editableOperand(0), true);
    } else {
      editableOperand(0)->setSign(Sign::Positive, context, angleUnit);
    }
    Expression * e = squashUnaryHierarchy();
    Opposite * o = new Opposite(e, true);
    e->replaceWith(o, true);
    o->editableOperand(0)->shallowBeautify(context, angleUnit);
    return o;
  }

  /* Step 2: Merge negative powers: a*b^(-1)*c^(-pi)*d = a*(b*c^pi)^(-1)
   * This also turns 2/3*a into 2*a*3^(-1) */
  Expression * e = mergeNegativePower(context, angleUnit);
  if (e->type() == Type::Power) {
    return e->shallowBeautify(context, angleUnit);
  }
  assert(e == this);

  // Step 3: Add Parenthesis if needed
  for (int i = 0; i < numberOfChildren(); i++) {
    const Expression * o = childAtIndex(i);
    if (o->type() == Type::Addition ) {
      Parenthesis * p = new Parenthesis(o, false);
      replaceOperand(o, p, false);
    }
  }

  // Step 4: Create a Division if needed
  for (int i = 0; i < numberOfChildren(); i++) {
    if (!(childAtIndex(i)->type() == Type::Power && childAtIndex(i)->childAtIndex(1)->type() == Type::Rational && static_cast<const Rational *>(childAtIndex(i)->childAtIndex(1))->isMinusOne())) {
      continue;
    }

    // Let's remove the denominator-to-be from this
    Power * p = static_cast<Power *>(editableOperand(i));
    Expression * denominatorOperand = p->editableOperand(0);
    p->detachOperand(denominatorOperand);
    removeOperand(p, true);

    Expression * numeratorOperand = shallowReduce(context, angleUnit);
    // Delete parenthesis unnecessary on numerator
    if (numeratorOperand->type() == Type::Parenthesis) {
      numeratorOperand = numeratorOperand->replaceWith(numeratorOperand->editableOperand(0), true);
    }
    Expression * originalParent = numeratorOperand->parent();
    Division * d = new Division(numeratorOperand, denominatorOperand, false);
    originalParent->replaceOperand(numeratorOperand, d, false);
    return d->shallowBeautify(context, angleUnit);
  }

  return this;
#endif
}

Expression Multiplication::mergeNegativePower(Context & context, Preferences::AngleUnit angleUnit) {
  Expression e = *this;
  return e;
  //TODO
#if 0
  Multiplication * m = new Multiplication();
  // Special case for rational p/q: if q != 1, q should be at denominator
  if (childAtIndex(0)->type() == Type::Rational && !static_cast<const Rational *>(childAtIndex(0))->denominator().isOne()) {
    Rational * r = static_cast<Rational *>(editableOperand(0));
    m->addOperand(new Rational(r->denominator()));
    if (r->numerator().isOne()) {
      removeOperand(r, true);
    } else {
      replaceOperand(r, new Rational(r->numerator()), true);
    }
  }
  int i = 0;
  while (i < numberOfChildren()) {
    if (childAtIndex(i)->type() == Type::Power && childAtIndex(i)->childAtIndex(1)->sign() == Sign::Negative) {
      Expression * e = editableOperand(i);
      e->editableOperand(1)->setSign(Sign::Positive, context, angleUnit);
      removeOperand(e, false);
      m->addOperand(e);
      e->shallowReduce(context, angleUnit);
    } else {
      i++;
    }
  }
  if (m->numberOfChildren() == 0) {
    delete m;
    return this;
  }
  Power * p = new Power(m, new Rational(-1), false);
  m->sortOperands(SimplificationOrder, true);
  m->squashUnaryHierarchy();
  addOperand(p);
  sortOperands(SimplificationOrder, true);
  return squashUnaryHierarchy();
#endif
}

void Multiplication::addMissingFactors(Expression factor, Context & context, Preferences::AngleUnit angleUnit) {
  return;
  //TODO
#if 0
  if (factor->type() == Type::Multiplication) {
    for (int j = 0; j < factor->numberOfChildren(); j++) {
      addMissingFactors(factor->editableOperand(j), context, angleUnit);
    }
    return;
  }
  /* Special case when factor is a Rational: if 'this' has already a rational
   * operand, we replace it by its LCM with factor ; otherwise, we simply add
   * factor as an operand. */
  if (numberOfChildren() > 0 && childAtIndex(0)->type() == Type::Rational && factor->type() == Type::Rational) {
    Rational * f = static_cast<Rational *>(factor);
    Rational * o = static_cast<Rational *>(editableOperand(0));
    assert(f->denominator().isOne());
    assert(o->denominator().isOne());
    Integer i = f->numerator();
    Integer j = o->numerator();
    return replaceOperand(o, new Rational(Arithmetic::LCM(&i, &j)));
  }
  if (factor->type() != Type::Rational) {
    /* If factor is not a rational, we merge it with the operand of identical
     * base if any. Otherwise, we add it as an new operand. */
    for (int i = 0; i < numberOfChildren(); i++) {
      if (TermsHaveIdenticalBase(childAtIndex(i), factor)) {
        Expression * sub = new Subtraction(CreateExponent(editableOperand(i)), CreateExponent(factor), false);
        Reduce((Expression **)&sub, context, angleUnit);
        if (sub->sign() == Sign::Negative) { // index[0] < index[1]
          if (factor->type() == Type::Power) {
            factor->replaceOperand(factor->editableOperand(1), new Opposite(sub, true), true);
          } else {
            factor = new Power(factor, new Opposite(sub, true), false);
          }
          factor->editableOperand(1)->shallowReduce(context, angleUnit);
          factorizeBase(editableOperand(i), factor, context, angleUnit);
          editableOperand(i)->shallowReduce(context, angleUnit);
        } else if (sub->sign() == Sign::Unknown) {
          factorizeBase(editableOperand(i), factor, context, angleUnit);
          editableOperand(i)->shallowReduce(context, angleUnit);
        } else {}
        delete sub;
        /* Reducing the new operand i can lead to creating a new multiplication
         * (ie 2^(1+2*3^(1/2)) -> 2*2^(2*3^(1/2)). We thus have to get rid of
         * nested multiplication: */
        mergeMultiplicationOperands();
        return;
      }
    }
  }
  addOperand(factor->clone());
  sortOperands(SimplificationOrder, false);
#endif
}

template MatrixComplex<float> MultiplicationNode::computeOnComplexAndMatrix<float>(std::complex<float> const, const MatrixComplex<float>);
template MatrixComplex<double> MultiplicationNode::computeOnComplexAndMatrix<double>(std::complex<double> const, const MatrixComplex<double>);
template Complex<float> MultiplicationNode::compute<float>(const std::complex<float>, const std::complex<float>);
template Complex<double> MultiplicationNode::compute<double>(const std::complex<double>, const std::complex<double>);
template void MultiplicationNode::computeOnArrays<double>(double * m, double * n, double * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);

}
