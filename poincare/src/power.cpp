#include <poincare/power.h>

#include <poincare/addition.h>
//#include <poincare/arithmetic.h>
//#include <poincare/binomial_coefficient.h>
//#include <poincare/cosine.h>
#include <poincare/division.h>
//#include <poincare/matrix.h>
//#include <poincare/matrix_inverse.h>
//#include <poincare/nth_root.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
//#include <poincare/simplification_root.h>
//#include <poincare/sine.h>
//#include <poincare/square_root.h>
#include <poincare/symbol.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>

#include <poincare/horizontal_layout_node.h>
#include <poincare/vertical_offset_layout_node.h>

#include <cmath>
#include <math.h>
#include <ion.h>

extern "C" {
#include <assert.h>
}

namespace Poincare {

PowerNode * PowerNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<PowerNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

ExpressionNode::Sign PowerNode::sign() const {
  if (Expression::shouldStopProcessing()) {
    return Sign::Unknown;
  }
  if (childAtIndex(0)->sign() == Sign::Positive && childAtIndex(1)->sign() != Sign::Unknown) {
    return Sign::Positive;
  }
  if (childAtIndex(0)->sign() == Sign::Negative && childAtIndex(1)->type() == Type::Rational) {
    RationalNode * r = static_cast<RationalNode *>(childAtIndex(1));
    if (r->denominator().isOne()) {
      NaturalIntegerPointer nip = r->numerator();
      if (Integer::Division(Integer(&nip), Integer(2)).remainder.isZero()) {
        return Sign::Positive;
      } else {
        return Sign::Negative;
      }
    }
  }
  return Sign::Unknown;
}

Expression PowerNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  return Power(this).setSign(s, context, angleUnit);
}

int PowerNode::polynomialDegree(char symbolName) const {
  int deg = Power(this).polynomialDegree(symbolName);
  if (deg == 0) {
    return deg;
  }
  int op0Deg = childAtIndex(0)->polynomialDegree(symbolName);
  if (op0Deg < 0) {
    return -1;
  }
  if (childAtIndex(1)->type() == Type::Rational) {
    RationalNode * r = static_cast<RationalNode *>(childAtIndex(1));
    if (!r->denominator().isOne() || r->sign() == Sign::Negative) {
      return -1;
    }
    NaturalIntegerPointer nip = r->numerator();
    Integer numeratorInt = Integer(&nip);
    if (Integer::NaturalOrder(numeratorInt, Integer(Integer::k_maxExtractableInteger)) > 0) {
      return -1;
    }
    op0Deg *= numeratorInt.extractedInt();
    return op0Deg;
  }
  return -1;
}

int PowerNode::getPolynomialCoefficients(char symbolName, Expression coefficients[]) const {
  return Power(this).getPolynomialCoefficients(symbolName, coefficients);
}

// Private

template<typename T>
Complex<T> PowerNode::compute(const std::complex<T> c, const std::complex<T> d) {
  /* Openbsd trigonometric functions are numerical implementation and thus are
   * approximative.
   * The error epsilon is ~1E-7 on float and ~1E-15 on double. In order to
   * avoid weird results as e(i*pi) = -1+6E-17*i, we compute the argument of
   * the result of c^d and if arg ~ 0 [Pi], we discard the residual imaginary
   * part and if arg ~ Pi/2 [Pi], we discard the residual real part. */
  std::complex<T> result = std::pow(c, d);
  return Complex<T>(ApproximationHelper::TruncateRealOrImaginaryPartAccordingToArgument(result));
}

// Layout

LayoutRef PowerNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  ExpressionNode * indiceOperand = childAtIndex(1);
  // Delete eventual parentheses of the indice in the pretty print
  if (indiceOperand->type() == Type::Parenthesis) {
    indiceOperand = indiceOperand->childAtIndex(0);
  }
  HorizontalLayoutRef result = HorizontalLayoutRef();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(VerticalOffsetLayoutRef(
        indiceOperand->createLayout(floatDisplayMode, numberOfSignificantDigits),
        VerticalOffsetLayoutNode::Type::Superscript),
      result.numberOfChildren(),
      result.numberOfChildren(),
      nullptr);
  return result;
}

// Serialize

bool PowerNode::needsParenthesesWithParent(const SerializationHelperInterface * e) const {
  Type types[] = {Type::Power, Type::Factorial};
  return static_cast<const ExpressionNode *>(e)->isOfType(types, 2);
}

// Simplify

Expression PowerNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return Power(this).shallowReduce(context, angleUnit);
}

Expression PowerNode::shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const {
  return Power(this).shallowBeautify(context, angleUnit);
}


int PowerNode::simplificationOrderGreaterType(const ExpressionNode * e, bool canBeInterrupted) const {
  int baseComparison = SimplificationOrder(childAtIndex(0), e, canBeInterrupted);
  if (baseComparison != 0) {
    return baseComparison;
  }
  Rational one(1);
  return SimplificationOrder(childAtIndex(1), one.node(), canBeInterrupted);
}

int PowerNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->numberOfChildren() > 0);
  int baseComparison = SimplificationOrder(childAtIndex(0), e->childAtIndex(0), canBeInterrupted);
  if (baseComparison != 0) {
    return baseComparison;
  }
  assert(e->numberOfChildren() > 1);
  return SimplificationOrder(childAtIndex(1), e->childAtIndex(1), canBeInterrupted);
}

Expression PowerNode::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  if (childAtIndex(1)->sign() == Sign::Negative) {
    Expression denominator = Power(this);
    Expression newExponent = denominator.childAtIndex(1).setSign(Sign::Positive, context, angleUnit);
    if (newExponent.type() == Type::Rational && static_cast<Rational *>(&newExponent)->isOne()) {
      Expression result = Power(this).childAtIndex(0);
      return result;
    }
    return denominator;
  }
  return Expression();
}


//TODO
#if 0
Expression * Power::simplifyPowerPower(Power * p, Expression * e, Context& context, Preferences::AngleUnit angleUnit) {
  Expression * p0 = p->editableOperand(0);
  Expression * p1 = p->editableOperand(1);
  p->detachOperands();
  Multiplication * m = new Multiplication(p1, e, false);
  replaceOperand(e, m, false);
  replaceOperand(p, p0, true);
  m->shallowReduce(context, angleUnit);
  return shallowReduce(context, angleUnit);
}

bool Power::parentIsALogarithmOfSameBase() const {
  if (parent()->type() == Type::Logarithm && parent()->childAtIndex(0) == this) {
    // parent = log(10^x)
    if (parent()->numberOfChildren() == 1) {
      if (childAtIndex(0)->type() == Type::Rational && static_cast<const Rational *>(childAtIndex(0))->isTen()) {
        return true;
      }
      return false;
    }
    // parent = log(x^y,x)
    if (childAtIndex(0)->isIdenticalTo(parent()->childAtIndex(1))) {
      return true;
    }
  }
  // parent = ln(e^x)
  if (parent()->type() == Type::NaperianLogarithm && parent()->childAtIndex(0) == this && childAtIndex(0)->type() == Type::Symbol && static_cast<const Symbol *>(childAtIndex(0))->name() == Ion::Charset::Exponential) {
    return true;
  }
  return false;
}

Expression * Power::simplifyPowerMultiplication(Multiplication * m, Expression * r, Context& context, Preferences::AngleUnit angleUnit) {
  for (int index = 0; index < m->numberOfChildren(); index++) {
    Expression * factor = m->editableOperand(index);
    Power * p = new Power(factor, r, true); // We copy r and factor to avoid inheritance issues
    m->replaceOperand(factor, p, true);
    p->shallowReduce(context, angleUnit);
  }
  detachOperand(m);
  return replaceWith(m, true)->shallowReduce(context, angleUnit); // delete r
}

Expression * Power::simplifyRationalRationalPower(Expression * result, Rational * a, Rational * b, Context& context, Preferences::AngleUnit angleUnit) {
  if (b->denominator().isOne()) {
    Rational r = Rational::Power(*a, b->numerator());
    return result->replaceWith(new Rational(r),true);
  }
  Expression * n = nullptr;
  Expression * d = nullptr;
  if (b->sign() == Sign::Negative) {
    b->setSign(Sign::Positive);
    n = CreateSimplifiedIntegerRationalPower(a->denominator(), b, false, context, angleUnit);
    d = CreateSimplifiedIntegerRationalPower(a->numerator(), b, true, context, angleUnit);
  } else {
    n = CreateSimplifiedIntegerRationalPower(a->numerator(), b, false, context, angleUnit);
    d = CreateSimplifiedIntegerRationalPower(a->denominator(), b, true, context, angleUnit);
  }
  Multiplication * m = new Multiplication(n, d, false);
  result->replaceWith(m, true);
  return m->shallowReduce(context, angleUnit);
}
#endif

//TODO
#if 0
Expression * Power::CreateSimplifiedIntegerRationalPower(Integer i, Rational * r, bool isDenominator, Context & context, Preferences::AngleUnit angleUnit) {
  assert(!i.isZero());
  assert(r->sign() == Sign::Positive);
  if (i.isOne()) {
    return RationalReference(1);
  }
  Integer absI = i;
  absI.setNegative(false);
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  Arithmetic::PrimeFactorization(&i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);

  if (coefficients[0].isMinusOne()) {
    /* We could not break i in prime factor (either it might take too many
     * factors or too much time). */
    r->setSign(isDenominator ? Sign::Negative : Sign::Positive);
    return new Power(new Rational(i), r->clone(), false);
  }

  Integer r1(1);
  Integer r2(1);
  int index = 0;
  while (!coefficients[index].isZero() && index < Arithmetic::k_maxNumberOfPrimeFactors) {
    Integer n = Integer::Multiplication(coefficients[index], r->numerator());
    IntegerDivision div = Integer::Division(n, r->denominator());
    r1 = Integer::Multiplication(r1, Integer::Power(factors[index], div.quotient));
    r2 = Integer::Multiplication(r2, Integer::Power(factors[index], div.remainder));
    index++;
  }
  Rational * p1 = new Rational(r2);
  Integer one = isDenominator ? Integer(-1) : Integer(1);
  Rational * p2 = new Rational(one, r->denominator());
  Power * p = new Power(p1, p2, false);
  if (r1.isEqualTo(Integer(1)) && !i.isNegative()) {
    return p;
  }
  Rational * r3 = isDenominator ? new Rational(Integer(1), r1) : new Rational(r1);
  Multiplication * m = new Multiplication(r3, p, false);
  if (r2.isOne()) {
    m->removeOperand(p);
  }
  if (i.isNegative()) {
    Expression * nthRootOfUnity = CreateNthRootOfUnity(*r);
    m->addOperand(nthRootOfUnity);
    nthRootOfUnity->shallowReduce(context, angleUnit);

  }
  m->sortOperands(SimplificationOrder, false);
  return m;
}
#endif

//TODO
#if 0
Expression * Power::CreateNthRootOfUnity(const Rational r) {
  const Symbol * exp = new Symbol(Ion::Charset::Exponential);
  const Symbol * iComplex = new Symbol(Ion::Charset::IComplex);
  const Symbol * pi = new Symbol(Ion::Charset::SmallPi);
  const Expression * multExpOperands[3] = {iComplex, pi, new Rational(r)};
  Multiplication * mExp = new Multiplication(multExpOperands, 3, false);
  mExp->sortOperands(SimplificationOrder, false);
  return new Power(exp, mExp, false);
#if 0
  const Symbol * iComplex = new Symbol(Ion::Charset::IComplex);
  const Symbol * pi = new Symbol(Ion::Charset::SmallPi);
  Expression * op = new Multiplication(pi, r->clone(), false);
  Cosine * cos = new Cosine(op, false);
  op = op->shallowReduce(context, angleUnit);
  Sine * sin = new Sine(op, true);
  Expression * m = new Multiplication(iComplex, sin, false);
  sin->shallowReduce(context, angleUnit);
  Expression * a = new Addition(cos, m, false);
  cos->shallowReduce(context, angleUnit);
  const Expression * multExpOperands[3] = {pi, r->clone()};
#endif
}

bool Power::TermIsARationalSquareRootOrRational(const Expression * e) {
  if (e->type() == Type::Rational) {
    return true;
  }
  if (e->type() == Type::Power && e->childAtIndex(0)->type() == Type::Rational && e->childAtIndex(1)->type() == Type::Rational && static_cast<const Rational *>(e->childAtIndex(1))->isHalf()) {
    return true;
  }
  if (e->type() == Type::Multiplication && e->numberOfChildren() == 2 && e->childAtIndex(0)->type() == Type::Rational && e->childAtIndex(1)->type() == Type::Power && e->childAtIndex(1)->childAtIndex(0)->type() == Type::Rational && e->childAtIndex(1)->childAtIndex(1)->type() == Type::Rational && static_cast<const Rational *>(e->childAtIndex(1)->childAtIndex(1))->isHalf()) {
  return true;
  }
  return false;
}

const Rational * Power::RadicandInExpression(const Expression * e) {
  if (e->type() == Type::Rational) {
    return nullptr;
  } else if (e->type() == Type::Power) {
    assert(e->type() == Type::Power);
    assert(e->childAtIndex(0)->type() == Type::Rational);
    return static_cast<const Rational *>(e->childAtIndex(0));
  } else {
    assert(e->type() == Type::Multiplication);
    assert(e->childAtIndex(1)->type() == Type::Power);
    assert(e->childAtIndex(1)->childAtIndex(0)->type() == Type::Rational);
    return static_cast<const Rational *>(e->childAtIndex(1)->childAtIndex(0));
  }
}

const Rational * Power::RationalFactorInExpression(const Expression * e) {
  if (e->type() == Type::Rational) {
    return static_cast<const Rational *>(e);
  } else if (e->type() == Type::Power) {
    return nullptr;
  } else {
    assert(e->type() == Type::Multiplication);
    assert(e->childAtIndex(0)->type() == Type::Rational);
    return static_cast<const Rational *>(e->childAtIndex(0));
  }
}

Expression * Power::removeSquareRootsFromDenominator(Context & context, Preferences::AngleUnit angleUnit) {
  Expression * result = nullptr;

  if (childAtIndex(0)->type() == Type::Rational && childAtIndex(1)->type() == Type::Rational && (static_cast<const Rational *>(childAtIndex(1))->isHalf() || static_cast<const Rational *>(childAtIndex(1))->isMinusHalf())) {
      /* We're considering a term of the form sqrt(p/q) (or 1/sqrt(p/q)), with
       * p and q integers.
       * We'll turn those into sqrt(p*q)/q (or sqrt(p*q)/p) . */
      Integer p = static_cast<const Rational *>(childAtIndex(0))->numerator();
      assert(!p.isZero()); // We eliminated case of form 0^(-1/2) at first step of shallowReduce
      Integer q = static_cast<const Rational *>(childAtIndex(0))->denominator();
      // We do nothing for terms of the form sqrt(p)
      if (!q.isOne() || static_cast<const Rational *>(childAtIndex(1))->isMinusHalf()) {
        Power * sqrt = new Power(new Rational(Integer::Multiplication(p, q)), new Rational(1, 2), false);
        if (static_cast<const Rational *>(childAtIndex(1))->isHalf()) {
          result = new Multiplication(new Rational(Integer(1), q), sqrt, false);
        } else {
          result = new Multiplication(new Rational(Integer(1), p), sqrt, false); // We use here the assertion that p != 0
        }
        sqrt->shallowReduce(context, angleUnit);
      }
  } else if (childAtIndex(1)->type() == Type::Rational && static_cast<const Rational *>(childAtIndex(1))->isMinusOne() && childAtIndex(0)->type() == Type::Addition && childAtIndex(0)->numberOfChildren() == 2 && TermIsARationalSquareRootOrRational(childAtIndex(0)->childAtIndex(0)) && TermIsARationalSquareRootOrRational(childAtIndex(0)->childAtIndex(1))) {
    /* We're considering a term of the form
     *
     * 1/(n1/d1*sqrt(p1/q1) + n2/d2*sqrt(p2/q2))
     *
     * and we want to turn it into
     *
     *  n1*q2*d1*d2^2*sqrt(p1*q1) - n2*q1*d2*d1^2*sqrt(p2*q2)
     * -------------------------------------------------------
     *          n1^2*d2^2*p1*q2 - n2^2*d1^2*p2*q1
     */
    const Rational * f1 = RationalFactorInExpression(childAtIndex(0)->childAtIndex(0));
    const Rational * f2 = RationalFactorInExpression(childAtIndex(0)->childAtIndex(1));
    const Rational * r1 = RadicandInExpression(childAtIndex(0)->childAtIndex(0));
    const Rational * r2 = RadicandInExpression(childAtIndex(0)->childAtIndex(1));
    Integer n1 = (f1 ? f1->numerator() : Integer(1));
    Integer d1 = (f1 ? f1->denominator() : Integer(1));
    Integer p1 = (r1 ? r1->numerator() : Integer(1));
    Integer q1 = (r1 ? r1->denominator() : Integer(1));
    Integer n2 = (f2 ? f2->numerator() : Integer(1));
    Integer d2 = (f2 ? f2->denominator() : Integer(1));
    Integer p2 = (r2 ? r2->numerator() : Integer(1));
    Integer q2 = (r2 ? r2->denominator() : Integer(1));

    // Compute the denominator = n1^2*d2^2*p1*q2 - n2^2*d1^2*p2*q1
    Integer denominator = Integer::Subtraction(
        Integer::Multiplication(
          Integer::Multiplication(
            Integer::Power(n1, Integer(2)),
            Integer::Power(d2, Integer(2))),
          Integer::Multiplication(p1, q2)),
        Integer::Multiplication(
          Integer::Multiplication(
            Integer::Power(n2, Integer(2)),
            Integer::Power(d1, Integer(2))),
          Integer::Multiplication(p2, q1)));

    // Compute the numerator
    Power * sqrt1 = new Power(new Rational(Integer::Multiplication(p1, q1)), new Rational(1, 2), false);
    Power * sqrt2 = new Power(new Rational(Integer::Multiplication(p2, q2)), new Rational(1, 2), false);
    Integer factor1 = Integer::Multiplication(
        Integer::Multiplication(n1, d1),
        Integer::Multiplication(Integer::Power(d2, Integer(2)), q2));
    Multiplication * m1 = new Multiplication(new Rational(factor1), sqrt1, false);
    Integer factor2 = Integer::Multiplication(
        Integer::Multiplication(n2, d2),
        Integer::Multiplication(Integer::Power(d1, Integer(2)), q1));
    Multiplication * m2 = new Multiplication(new Rational(factor2), sqrt2, false);
    Subtraction * numerator = nullptr;
    if (denominator.isNegative()) {
      numerator = new Subtraction(m2, m1, false);
      denominator.setNegative(false);
    } else {
      numerator = new Subtraction(m1, m2, false);
    }

    result = new Multiplication(numerator, new Rational(Integer(1), denominator), false);
    numerator->deepReduce(context, angleUnit);
  }

  if (result) {
    replaceWith(result, true);
    result = result->shallowReduce(context, angleUnit);
  }
  return result;
}

bool Power::isNthRootOfUnity() const {
  if (childAtIndex(0)->type() != Type::Symbol || static_cast<const Symbol *>(childAtIndex(0))->name() != Ion::Charset::Exponential) {
    return false;
  }
  if (childAtIndex(1)->type() != Type::Multiplication) {
    return false;
  }
  if (childAtIndex(1)->numberOfChildren() < 2 || childAtIndex(1)->numberOfChildren() > 3) {
    return false;
  }
  const Expression * i = childAtIndex(1)->childAtIndex(childAtIndex(1)->numberOfChildren()-1);
  if (i->type() != Type::Symbol || static_cast<const Symbol *>(i)->name() != Ion::Charset::IComplex) {
    return false;
  }
  const Expression * pi = childAtIndex(1)->childAtIndex(childAtIndex(1)->numberOfChildren()-2);
  if (pi->type() != Type::Symbol || static_cast<const Symbol *>(pi)->name() != Ion::Charset::SmallPi) {
    return false;
  }
  if (numberOfChildren() == 2) {
    return true;
  }
  if (childAtIndex(1)->childAtIndex(0)->type() == Type::Rational) {
    return true;
  }
  return false;
}

bool Power::RationalExponentShouldNotBeReduced(const Rational * b, const Rational * r) {
  if (r->isMinusOne()) {
    return false;
  }
  /* We check that the simplification does not involve too complex power of
   * integers (ie 3^999, 120232323232^50) that would take too much time to
   * compute:
   *  - we cap the exponent at k_maxExactPowerMatrix
   *  - we cap the resulting power at DBL_MAX
   * The complexity of computing a power of rational is mainly due to computing
   * the GCD of the resulting numerator and denominator. Euclide algorithm's
   * complexity is apportionned to the number of decimal digits in the smallest
   * integer. */
  Integer maxIntegerExponent = r->numerator();
  maxIntegerExponent.setNegative(false);
  if (Integer::NaturalOrder(maxIntegerExponent, Integer(k_maxExactPowerMatrix)) > 0) {
    return true;
  }
  double index = maxIntegerExponent.approximate<double>();
  double powerNumerator = std::pow(std::fabs(b->numerator().approximate<double>()), index);
  double powerDenominator = std::pow(std::fabs(b->denominator().approximate<double>()), index);
  if (std::isnan(powerNumerator) || std::isnan(powerDenominator) || std::isinf(powerNumerator) || std::isinf(powerDenominator)) {
    return true;
  }
  return false;
}

#endif


template<typename T> MatrixComplex<T> PowerNode::computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n) {
  return MatrixComplex<T>::Undefined();
}

template<typename T> MatrixComplex<T> PowerNode::computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> d) {
 if (m.numberOfRows() != m.numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  T power = Complex<T>(d).toScalar();
  if (std::isnan(power) || std::isinf(power) || power != (int)power || std::fabs(power) > k_maxApproximatePowerMatrix) {
    return MatrixComplex<T>::Undefined();
  }
  if (power < 0) {
    MatrixComplex<T> inverse = m.inverse();
    if (!inverse.isDefined()) {
      return MatrixComplex<T>::Undefined();
    }
    Complex<T> minusC = Complex<T>(-d);
    MatrixComplex<T> result = PowerNode::computeOnMatrixAndComplex(inverse, minusC);
    return result;
  }
  MatrixComplex<T> result = MatrixComplex<T>::createIdentity(m.numberOfRows());
  // TODO: implement a quick exponentiation
  for (int k = 0; k < (int)power; k++) {
    if (Expression::shouldStopProcessing()) {
      return MatrixComplex<T>::Undefined();
    }
    result = MultiplicationNode::computeOnMatrices<T>(result, m);
  }
  return result;
}

template<typename T> MatrixComplex<T> PowerNode::computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n) {
  return MatrixComplex<T>::Undefined();
}

// Power

Expression Power::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  assert(s == Sign::Positive);
  assert(childAtIndex(0).sign() == Sign::Negative);
  return Power(childAtIndex(0).setSign(ExpressionNode::Sign::Positive, context, angleUnit), childAtIndex(1));
}

int Power::getPolynomialCoefficients(char symbolName, Expression coefficients[]) const {
  int deg = polynomialDegree(symbolName);
  if (deg <= 0) {
    return Expression::getPolynomialCoefficients(symbolName, coefficients);
  }
  /* Here we only consider the case x^4 as privateGetPolynomialCoefficients is
   * supposed to be called after reducing the expression. */
  if (childAtIndex(0).type() == ExpressionNode::Type::Symbol && static_cast<const SymbolNode *>(childAtIndex(0).node())->name() == symbolName && childAtIndex(1).type() == ExpressionNode::Type::Rational) {
    RationalNode * r = static_cast<RationalNode *>(childAtIndex(1).node());
    if (!r->denominator().isOne() || r->sign() == ExpressionNode::Sign::Negative) {
      return -1;
    }
    NaturalIntegerPointer nip = r->numerator();
    Integer num = Integer(&nip);
    if (Integer::NaturalOrder(num, Integer(Integer::k_maxExtractableInteger)) > 0) {
      return -1;
    }
    int n = num.extractedInt();
    if (n <= k_maxPolynomialDegree) {
      for (int i = 0; i < n; i++) {
        coefficients[i] = Rational(0);
      }
      coefficients[n] = Rational(1);
      return n;
    }
  }
  return -1;
}

Expression Power::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression result = *this;
  return result;
  //TODO
#if 0
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  /* Step 0: get rid of matrix */
  if (childAtIndex(1)->type() == Type::Matrix) {
    return replaceWith(new Undefined(), true);
  }
  if (childAtIndex(0)->type() == Type::Matrix) {
    Matrix * mat = static_cast<Matrix *>(editableOperand(0));
    if (childAtIndex(1)->type() != Type::Rational || !static_cast<const Rational *>(childAtIndex(1))->denominator().isOne()) {
      return replaceWith(new Undefined(), true);
    }
    Integer exponent = static_cast<const Rational *>(childAtIndex(1))->numerator();
    if (mat->numberOfRows() != mat->numberOfColumns()) {
      return replaceWith(new Undefined(), true);
    }
    if (exponent.isNegative()) {
      editableOperand(1)->setSign(Sign::Positive, context, angleUnit);
      Expression * newMatrix = shallowReduce(context, angleUnit);
      Expression * parent = newMatrix->parent();
      MatrixInverse * inv = new MatrixInverse(newMatrix, false);
      parent->replaceOperand(newMatrix, inv, false);
      return inv;
    }
    if (Integer::NaturalOrder(exponent, Integer(k_maxExactPowerMatrix)) > 0) {
      return this;
    }
    int exp = exponent.extractedInt(); // Ok, because 0 < exponent < k_maxExactPowerMatrix
    Matrix * id = Matrix::createIdentity(mat->numberOfRows());
    if (exp == 0) {
      return replaceWith(id, true);
    }
    Multiplication * result = new Multiplication(id, mat->clone());
    // TODO: implement a quick exponentiation
    for (int k = 1; k < exp; k++) {
      result->addOperand(mat->clone());
    }
    replaceWith(result, true);
    return result->shallowReduce(context, angleUnit);
  }
#endif

  /* Step 0: if both operands are true complexes, the result is undefined.
   * We can assert that evaluations is a complex as matrix are not simplified */
  Complex<float> * op0 = static_cast<Complex<float> *>(childAtIndex(0)->privateApproximate(float(), context, angleUnit));
  Complex<float> * op1 = static_cast<Complex<float> *>(childAtIndex(1)->privateApproximate(float(), context, angleUnit));
  bool bothOperandsComplexes = op0->imag() != 0 && op1->imag() != 0;
  bool nonComplexNegativeOperand0 = op0->imag() == 0 && op0->real() < 0;
  delete op0;
  delete op1;
  if (bothOperandsComplexes) {
    return this;
  }

  /* Step 1: We handle simple cases as x^0, x^1, 0^x and 1^x first for 2 reasons:
   * - we can assert this step that there is no division by 0:
   *   for instance, 0^(-2)->undefined
   * - we save computational time by early escaping for these cases. */
  if (childAtIndex(1)->type() == Type::Rational) {
    const Rational * b = static_cast<const Rational *>(childAtIndex(1));
    // x^0
    if (b->isZero()) {
      // 0^0 = undef
      if (childAtIndex(0)->type() == Type::Rational && static_cast<const Rational *>(childAtIndex(0))->isZero()) {
        return replaceWith(new Undefined(), true);
      }
      /* Warning: in all other case but 0^0, we replace x^0 by one. This is
       * almost always true except when x = 0. However, not substituting x^0 by
       * one would prevent from simplifying many expressions like x/x->1. */
      return replaceWith(RationalReference(1), true);
    }
    // x^1
    if (b->isOne()) {
      return replaceWith(editableOperand(0), true);
    }
  }
  if (childAtIndex(0)->type() == Type::Rational) {
    Rational * a = static_cast<Rational *>(editableOperand(0));
    // 0^x
    if (a->isZero()) {
      if (childAtIndex(1)->sign() == Sign::Positive) {
        return replaceWith(RationalReference(0), true);
      }
      if (childAtIndex(1)->sign() == Sign::Negative) {
        return replaceWith(new Undefined(), true);
      }
    }
    // 1^x
    if (a->isOne()) {
      return replaceWith(RationalReference(1), true);
    }
  }

  /* Step 2: We look for square root and sum of square roots (two terms maximum
   * so far) at the denominator and move them to the numerator. */
  Expression * r = removeSquareRootsFromDenominator(context, angleUnit);
  if (r) {
    return r;
  }

  if (childAtIndex(1)->type() == Type::Rational) {
    const Rational * b = static_cast<const Rational *>(childAtIndex(1));
    // i^(p/q)
    if (childAtIndex(0)->type() == Type::Symbol && static_cast<const Symbol *>(childAtIndex(0))->name() == Ion::Charset::IComplex) {
      Rational r = Rational::Multiplication(*b, Rational(1, 2));
      return replaceWith(CreateNthRootOfUnity(r))->shallowReduce(context, angleUnit);
    }
  }
  bool letPowerAtRoot = parentIsALogarithmOfSameBase();
  if (childAtIndex(0)->type() == Type::Rational) {
    Rational * a = static_cast<Rational *>(editableOperand(0));
    // p^q with p, q rationals
    if (!letPowerAtRoot && childAtIndex(1)->type() == Type::Rational) {
      Rational * exp = static_cast<Rational *>(editableOperand(1));
      if (RationalExponentShouldNotBeReduced(a, exp)) {
        return this;
      }
      return simplifyRationalRationalPower(this, a, exp, context, angleUnit);
    }
  }
  // (a)^(1/2) with a < 0 --> i*(-a)^(1/2)
  if (!letPowerAtRoot && nonComplexNegativeOperand0 && childAtIndex(1)->type() == Type::Rational && static_cast<const Rational *>(childAtIndex(1))->numerator().isOne() && static_cast<const Rational *>(childAtIndex(1))->denominator().isTwo()) {
    Expression * o0 = editableOperand(0);
    Expression * m0 = new Multiplication(new Rational(-1), o0, false);
    replaceOperand(o0, m0, false);
    m0->shallowReduce(context, angleUnit);
    Multiplication * m1 = new Multiplication();
    replaceWith(m1, false);
    m1->addOperand(new Symbol(Ion::Charset::IComplex));
    m1->addOperand(this);
    shallowReduce(context, angleUnit);
    return m1->shallowReduce(context, angleUnit);
  }
  // e^(i*Pi*r) with r rational
  if (!letPowerAtRoot && isNthRootOfUnity()) {
    Expression * m = editableOperand(1);
    detachOperand(m);
    Expression * i = m->editableOperand(m->numberOfChildren()-1);
    static_cast<Multiplication *>(m)->removeOperand(i, false);
    if (angleUnit == Preferences::AngleUnit::Degree) {
      const Expression * pi = m->childAtIndex(m->numberOfChildren()-1);
      m->replaceOperand(pi, new Rational(180), true);
    }
    Expression * cos = new Cosine(m, false);
    m = m->shallowReduce(context, angleUnit);
    Expression * sin = new Sine(m, true);
    Expression * complexPart = new Multiplication(sin, i, false);
    sin->shallowReduce(context, angleUnit);
    Expression * a = new Addition(cos, complexPart, false);
    cos->shallowReduce(context, angleUnit);
    complexPart->shallowReduce(context, angleUnit);
    return replaceWith(a, true)->shallowReduce(context, angleUnit);
  }
  // x^log(y,x)->y if y > 0
  if (childAtIndex(1)->type() == Type::Logarithm) {
    if (childAtIndex(1)->numberOfChildren() == 2 && childAtIndex(0)->isIdenticalTo(childAtIndex(1)->childAtIndex(1))) {
      // y > 0
      if (childAtIndex(1)->childAtIndex(0)->sign() == Sign::Positive) {
        return replaceWith(editableOperand(1)->editableOperand(0), true);
      }
    }
    // 10^log(y)
    if (childAtIndex(1)->numberOfChildren() == 1 && childAtIndex(0)->type() == Type::Rational && static_cast<const Rational *>(childAtIndex(0))->isTen()) {
      return replaceWith(editableOperand(1)->editableOperand(0), true);
    }
  }
  // (a^b)^c -> a^(b*c) if a > 0 or c is integer
  if (childAtIndex(0)->type() == Type::Power) {
    Power * p = static_cast<Power *>(editableOperand(0));
    // Check is a > 0 or c is Integer
    if (p->childAtIndex(0)->sign() == Sign::Positive ||
        (childAtIndex(1)->type() == Type::Rational && static_cast<Rational *>(editableOperand(1))->denominator().isOne())) {
      return simplifyPowerPower(p, editableOperand(1), context, angleUnit);
    }
  }
  // (a*b*c*...)^r ?
  if (!letPowerAtRoot && childAtIndex(0)->type() == Type::Multiplication) {
    Multiplication * m = static_cast<Multiplication *>(editableOperand(0));
    // (a*b*c*...)^n = a^n*b^n*c^n*... if n integer
    if (childAtIndex(1)->type() == Type::Rational && static_cast<Rational *>(editableOperand(1))->denominator().isOne()) {
      return simplifyPowerMultiplication(m, editableOperand(1), context, angleUnit);
    }
    // (a*b*...)^r -> |a|^r*(sign(a)*b*...)^r if a rational
    for (int i = 0; i < m->numberOfChildren(); i++) {
      // a is signed and a != -1
      if (m->childAtIndex(i)->sign() != Sign::Unknown && (m->childAtIndex(i)->type() != Type::Rational || !static_cast<const Rational *>(m->childAtIndex(i))->isMinusOne())) {
      //if (m->childAtIndex(i)->sign() == Sign::Positive || m->childAtIndex(i)->type() == Type::Rational) {
        Expression * r = editableOperand(1);
        Expression * rCopy = r->clone();
        Expression * factor = m->editableOperand(i);
        if (factor->sign() == Sign::Negative) {
          m->replaceOperand(factor, new Rational(-1), false);
          factor = factor->setSign(Sign::Positive, context, angleUnit);
        } else {
          m->removeOperand(factor, false);
        }
        m->shallowReduce(context, angleUnit);
        Power * p = new Power(factor, rCopy, false);
        Multiplication * root = new Multiplication(p, clone(), false);
        p->shallowReduce(context, angleUnit);
        root->editableOperand(1)->shallowReduce(context, angleUnit);
        replaceWith(root, true);
        return root->shallowReduce(context, angleUnit);
      }
    }
  }
  // a^(b+c) -> Rational(a^b)*a^c with a and b rational and a != 0
  if (!letPowerAtRoot && childAtIndex(0)->type() == Type::Rational && !static_cast<const Rational *>(childAtIndex(0))->isZero() && childAtIndex(1)->type() == Type::Addition) {
    Addition * a = static_cast<Addition *>(editableOperand(1));
    // Check is b is rational
    if (a->childAtIndex(0)->type() == Type::Rational) {
      const Rational * rationalBase = static_cast<const Rational *>(childAtIndex(0));
      const Rational * rationalIndex = static_cast<const Rational *>(a->childAtIndex(0));
      if (RationalExponentShouldNotBeReduced(rationalBase, rationalIndex)) {
        return this;
      }
      Power * p1 = static_cast<Power *>(clone());
      replaceOperand(a, a->editableOperand(1), true);
      Power * p2 = static_cast<Power *>(clone());
      Multiplication * m = new Multiplication(p1, p2, false);
      simplifyRationalRationalPower(p1, static_cast<Rational *>(p1->editableOperand(0)), static_cast<Rational *>(p1->editableOperand(1)->editableOperand(0)), context, angleUnit);
      replaceWith(m, true);
      return m->shallowReduce(context, angleUnit);
    }
  }

  // (a0+a1+...am)^n with n integer -> a^n+?a^(n-1)*b+?a^(n-2)*b^2+...+b^n (Multinome)
  if (!letPowerAtRoot && childAtIndex(1)->type() == Type::Rational && static_cast<const Rational *>(childAtIndex(1))->denominator().isOne() && childAtIndex(0)->type() == Type::Addition) {
    // Exponent n
    Rational * nr = static_cast<Rational *>(editableOperand(1));
    Integer n = nr->numerator();
    n.setNegative(false);
    /* if n is above 25, the resulting sum would have more than
     * k_maxNumberOfTermsInExpandedMultinome terms so we do not expand it. */
    if (Integer(k_maxNumberOfTermsInExpandedMultinome).isLowerThan(n) || n.isOne()) {
      return this;
    }
    int clippedN = n.extractedInt(); // Authorized because n < k_maxNumberOfTermsInExpandedMultinome
    // Number of terms in addition m
    int m = childAtIndex(0)->numberOfChildren();
    /* The multinome (a0+a2+...+a(m-1))^n has BinomialCoefficient(n+m-1,n) terms;
     * we expand the multinome only when the number of terms in the resulting
     * sum has less than k_maxNumberOfTermsInExpandedMultinome terms. */
    if (k_maxNumberOfTermsInExpandedMultinome < BinomialCoefficient::compute(static_cast<double>(clippedN), static_cast<double>(clippedN+m-1))) {
      return this;
    }
    Expression * result = editableOperand(0);
    Expression * a = result->clone();
    for (int i = 2; i <= clippedN; i++) {
      if (result->type() == Type::Addition) {
        Expression * a0 = new Addition();
        for (int j = 0; j < a->numberOfChildren(); j++) {
          Multiplication * m = new Multiplication(result, a->editableOperand(j), true);
          SimplificationRoot rootM(m); // m need to have a parent when applying distributeOnOperandAtIndex
          Expression * expandM = m->distributeOnOperandAtIndex(0, context, angleUnit);
          rootM.detachOperands();
          if (a0->type() == Type::Addition) {
            static_cast<Addition *>(a0)->addOperand(expandM);
          } else {
            a0 = new Addition(a0, expandM, false);
          }
          SimplificationRoot rootA0(a0); // a0 need a parent to be reduced.
          a0 = a0->shallowReduce(context, angleUnit);
          rootA0.detachOperands();
        }
        result = result->replaceWith(a0, true);
      } else {
        Multiplication * m = new Multiplication(a, result, true);
        SimplificationRoot root(m);
        result = result->replaceWith(m->distributeOnOperandAtIndex(0, context, angleUnit), true);
        result = result->shallowReduce(context, angleUnit);
        root.detachOperands();
      }
    }
    delete a;
    if (nr->sign() == Sign::Negative) {
      nr->replaceWith(new Rational(-1), true);
      return shallowReduce(context, angleUnit);
    } else {
      return replaceWith(result, true);
    }
  }
#if 0
  /* We could use the Newton formula instead which is quicker but not immediate
   * to implement in the general case (Newton multinome). */
  // (a+b)^n with n integer -> a^n+?a^(n-1)*b+?a^(n-2)*b^2+...+b^n (Newton)
  if (!letPowerAtRoot && childAtIndex(1)->type() == Type::Rational && static_cast<const Rational *>(childAtIndex(1))->denominator().isOne() && childAtIndex(0)->type() == Type::Addition && childAtIndex(0)->numberOfChildren() == 2) {
    Rational * nr = static_cast<Rational *>(editableOperand(1));
    Integer n = nr->numerator();
    n.setNegative(false);
    if (Integer(k_maxExpandedBinome).isLowerThan(n) || n.isOne()) {
      return this;
    }
    int clippedN = n.extractedInt(); // Authorized because n < k_maxExpandedBinome < k_maxNValue
    Expression * x0 = editableOperand(0)->editableOperand(0);
    Expression * x1 = editableOperand(0)->editableOperand(1);
    Addition * a = new Addition();
    for (int i = 0; i <= clippedN; i++) {
      Rational * r = new Rational(static_cast<int>(BinomialCoefficient::compute(static_cast<double>(i), static_cast<double>(clippedN))));
      Power * p0 = new Power(x0->clone(), new Rational(i), false);
      Power * p1 = new Power(x1->clone(), new Rational(clippedN-i), false);
      const Expression * operands[3] = {r, p0, p1};
      Multiplication * m = new Multiplication(operands, 3, false);
      p0->shallowReduce(context, angleUnit);
      p1->shallowReduce(context, angleUnit);
      a->addOperand(m);
      m->shallowReduce(context, angleUnit);
    }
    if (nr->sign() == Sign::Negative) {
      nr->replaceWith(new Rational(-1), true);
      editableOperand(0)->replaceWith(a, true)->shallowReduce(context, angleUnit);
      return shallowReduce(context, angleUnit);
    } else {
      return replaceWith(a, true)->shallowReduce(context, angleUnit);
    }
  }
#endif
  return this;
#endif
}

Expression Power::shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression result = *this;
  return result;
  //TODO
#if 0
  // X^-y -> 1/(X->shallowBeautify)^y
  if (childAtIndex(1)->sign() == Sign::Negative) {
    Expression * p = denominator(context, angleUnit);
    Division * d = new Division(RationalReference(1), p, false);
    p->shallowReduce(context, angleUnit);
    replaceWith(d, true);
    return d->shallowBeautify(context, angleUnit);
  }
  if (childAtIndex(1)->type() == Type::Rational && static_cast<const Rational *>(childAtIndex(1))->numerator().isOne()) {
    Integer index = static_cast<const Rational *>(childAtIndex(1))->denominator();
    if (index.isEqualTo(Integer(2))) {
      const Expression * sqrtOperand[1] = {childAtIndex(0)};
      SquareRoot * sqr = new SquareRoot(sqrtOperand, true);
      return replaceWith(sqr, true);
    }
    const Expression * rootOperand[2] = {childAtIndex(0)->clone(), new Rational(index)};
    NthRoot * nr = new NthRoot(rootOperand, false);
    return replaceWith(nr, true);
  }
  // +(a,b)^c ->(+(a,b))^c
  if (childAtIndex(0)->type() == Type::Addition || childAtIndex(0)->type() == Type::Multiplication) {
    const Expression * o[1] = {childAtIndex(0)};
    Parenthesis * p = new Parenthesis(o, true);
    replaceOperand(childAtIndex(0), p, true);
  }
  return this;
#endif
}

template Complex<float> PowerNode::compute<float>(std::complex<float>, std::complex<float>);
template Complex<double> PowerNode::compute<double>(std::complex<double>, std::complex<double>);
}
