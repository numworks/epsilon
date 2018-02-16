extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include <math.h>
#include <ion.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/matrix_inverse.h>
#include <poincare/matrix.h>
#include <poincare/power.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/addition.h>
#include <poincare/undefined.h>
#include <poincare/square_root.h>
#include <poincare/nth_root.h>
#include <poincare/division.h>
#include <poincare/matrix_inverse.h>
#include <poincare/arithmetic.h>
#include <poincare/symbol.h>
#include <poincare/subtraction.h>
#include <poincare/cosine.h>
#include <poincare/sine.h>
#include <poincare/simplification_root.h>
#include "layout/baseline_relative_layout.h"

namespace Poincare {

Expression::Type Power::type() const {
  return Type::Power;
}

Expression * Power::clone() const {
  return new Power(m_operands, true);
}

Expression::Sign Power::sign() const {
  if (shouldStopProcessing()) {
    return Sign::Unknown;
  }
  if (operand(0)->sign() == Sign::Positive && operand(1)->sign() != Sign::Unknown) {
    return Sign::Positive;
  }
  if (operand(0)->sign() == Sign::Negative && operand(1)->type() == Type::Rational) {
    const Rational * r = static_cast<const Rational *>(operand(1));
    if (r->denominator().isOne()) {
      if (Integer::Division(r->numerator(), Integer(2)).remainder.isZero()) {
        return Sign::Positive;
      } else {
        return Sign::Negative;
      }
    }
  }
  return Sign::Unknown;
}

int Power::polynomialDegree(char symbolName) const {
  Rational op0Deg = Rational(operand(0)->polynomialDegree(symbolName));
  if (op0Deg.sign() == Sign::Negative) {
    return -1;
  }
  if (operand(1)->type() == Type::Rational) {
    op0Deg = Rational::Multiplication(op0Deg, *(static_cast<const Rational *>(operand(1))));
    if (!op0Deg.denominator().isOne()) {
      return -1;
    }
    if (Integer::NaturalOrder(op0Deg.numerator(), Integer(Integer::k_maxExtractableInteger)) > 0) {
      return -1;
    }
    return op0Deg.numerator().extractedInt();
  }
  return -1;
}

Expression * Power::setSign(Sign s, Context & context, AngleUnit angleUnit) {
  assert(s == Sign::Positive);
  assert(operand(0)->sign() == Sign::Negative);
  editableOperand(0)->setSign(Sign::Positive, context, angleUnit);
  return this;
}

template<typename T>
Complex<T> Power::compute(const Complex<T> c, const Complex<T> d) {
  // c == c.r * e^(c.th*i)
  // d == d.a + d.b*i
  // c^d == e^(ln(c^d))
  //     == e^(ln(c) * d)
  //     == e^(ln(c.r * e^(c.th*i))  *  (d.a + d.b*i))
  //     == e^((ln(c.r) + ln(e^(c.th*i)))  *  (d.a + d.b*i))
  //     == e^((ln(c.r) + c.th*i)  *  (d.a + d.b*i))
  //     == e^(ln(c.r)*d.a + ln(c.r)*d.b*i + c.th*i*d.a + c.th*i*d.b*i)
  //     == e^((ln(c.r^d.a) + ln(e^(c.th*d.b*i^2)))  +  (ln(c.r)*d.b + c.th*d.a)*i)
  //     == e^(ln(c.r^d.a * e^(-c.th*d.b)))  *  e^((ln(c.r)*d.b + c.th*d.a)*i)
  //     == c.r^d.a*e^(-c.th*d.b) * e^((ln(c.r)*d.b + c.th*d.a)*i)
  if (c.a() == 0 && c.b() == 0) { // ln(c.r) and c.th are undefined
    return Complex<T>::Float(d.a() > 0 ? 0 : NAN);
  }
  T radius = std::pow(c.r(), d.a()) * std::exp(-c.th() * d.b());
  T theta = std::log(c.r())*d.b() + c.th()*d.a();
  return Complex<T>::Polar(radius, theta);
}

template<typename T> Matrix * Power::computeOnMatrixAndComplex(const Matrix * m, const Complex<T> * d) {
 if (m->numberOfRows() != m->numberOfColumns()) {
    return nullptr;
  }
  T power = d->toScalar();
  if (std::isnan(power) || std::isinf(power) || power != (int)power || std::fabs(power) > k_maxApproximatePowerMatrix) {
    return nullptr;
  }
  if (power < 0) {
    Matrix * inverse = m->createInverse<T>();
    if (inverse == nullptr) {
      return nullptr;
    }
    Complex<T> minusC = Opposite::compute(*d, AngleUnit::Default);
    Matrix * result = Power::computeOnMatrixAndComplex(inverse, &minusC);
    delete inverse;
    return result;
  }
  Matrix * result = Matrix::createApproximateIdentity<T>(m->numberOfRows());
  // TODO: implement a quick exponentiation
  for (int k = 0; k < (int)power; k++) {
    if (shouldStopProcessing()) {
      delete result;
      return nullptr;
    }
    Matrix * mult = Multiplication::computeOnMatrices<T>(result, m);
    delete result;
    result = mult;
  }
  return result;
}

bool Power::needParenthesisWithParent(const Expression * e) const {
  Type types[] = {Type::Power, Type::Factorial};
  return e->isOfType(types, 2);
}

ExpressionLayout * Power::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  const Expression * indiceOperand = m_operands[1];
  // Delete eventual parentheses of the indice in the pretty print
  if (m_operands[1]->type() == Type::Parenthesis) {
    indiceOperand = m_operands[1]->operand(0);
  }
  return new BaselineRelativeLayout(m_operands[0]->createLayout(floatDisplayMode, complexFormat),indiceOperand->createLayout(floatDisplayMode, complexFormat), BaselineRelativeLayout::Type::Superscript);
}

int Power::simplificationOrderSameType(const Expression * e, bool canBeInterrupted) const {
  int baseComparison = SimplificationOrder(operand(0), e->operand(0), canBeInterrupted);
  if (baseComparison != 0) {
    return baseComparison;
  }
  return SimplificationOrder(operand(1), e->operand(1), canBeInterrupted);
}

int Power::simplificationOrderGreaterType(const Expression * e, bool canBeInterrupted) const {
  int baseComparison = SimplificationOrder(operand(0), e, canBeInterrupted);
  if (baseComparison != 0) {
    return baseComparison;
  }
  Rational one(1);
  return SimplificationOrder(operand(1), &one, canBeInterrupted);
}

Expression * Power::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  /* Step 0: get rid of matrix */
  if (operand(1)->type() == Type::Matrix) {
    return replaceWith(new Undefined(), true);
  }
  if (operand(0)->type() == Type::Matrix) {
    Matrix * mat = static_cast<Matrix *>(editableOperand(0));
    if (operand(1)->type() != Type::Rational || !static_cast<const Rational *>(operand(1))->denominator().isOne()) {
      return replaceWith(new Undefined(), true);
    }
    Integer exponent = static_cast<const Rational *>(operand(1))->numerator();
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
  Complex<float> * op0 = static_cast<Complex<float> *>(operand(0)->approximate<float>(context, angleUnit));
  Complex<float> * op1 = static_cast<Complex<float> *>(operand(1)->approximate<float>(context, angleUnit));
  bool bothOperandsComplexes = op0->b() != 0 && op1->b() != 0;
  delete op0;
  delete op1;
  if (bothOperandsComplexes) {
    return replaceWith(new Undefined(), true);
  }

  /* Step 1: We handle simple cases as x^0, x^1, 0^x and 1^x first for 2 reasons:
   * - we can assert this step that there is no division by 0:
   *   for instance, 0^(-2)->undefined
   * - we save computational time by early escaping for these cases. */
  if (operand(1)->type() == Type::Rational) {
    const Rational * b = static_cast<const Rational *>(operand(1));
    // x^0
    if (b->isZero()) {
      // 0^0 = undef
      if (operand(0)->type() == Type::Rational && static_cast<const Rational *>(operand(0))->isZero()) {
        return replaceWith(new Undefined(), true);
      }
      /* Warning: in all other case but 0^0, we replace x^0 by one. This is
       * almost always true except when x = 0. However, not substituting x^0 by
       * one would prevent from simplifying many expressions like x/x->1. */
      return replaceWith(new Rational(1), true);
    }
    // x^1
    if (b->isOne()) {
      return replaceWith(editableOperand(0), true);
    }
  }
  if (operand(0)->type() == Type::Rational) {
    Rational * a = static_cast<Rational *>(editableOperand(0));
    // 0^x
    if (a->isZero()) {
      if (operand(1)->sign() == Sign::Positive) {
        return replaceWith(new Rational(0), true);
      }
      if (operand(1)->sign() == Sign::Negative) {
        return replaceWith(new Undefined(), true);
      }
    }
    // 1^x
    if (a->isOne()) {
      return replaceWith(new Rational(1), true);
    }
  }

  /* Step 2: We look for square root and sum of square roots (two terms maximum
   * so far) at the denominator and move them to the numerator. */
  Expression * r = removeSquareRootsFromDenominator(context, angleUnit);
  if (r) {
    return r;
  }

  if (operand(1)->type() == Type::Rational) {
    const Rational * b = static_cast<const Rational *>(operand(1));
    // i^(p/q)
    if (operand(0)->type() == Type::Symbol && static_cast<const Symbol *>(operand(0))->name() == Ion::Charset::IComplex) {
      Rational r = Rational::Multiplication(*b, Rational(1, 2));
      return replaceWith(CreateNthRootOfUnity(r))->shallowReduce(context, angleUnit);
    }
  }
  bool letPowerAtRoot = parentIsALogarithmOfSameBase();
  if (operand(0)->type() == Type::Rational) {
    Rational * a = static_cast<Rational *>(editableOperand(0));
    // p^q with p, q rationals
    if (!letPowerAtRoot && operand(1)->type() == Type::Rational) {
      Rational * exp = static_cast<Rational *>(editableOperand(1));
      /* First, we check that the simplification does not involve too complex power
       * of integers (ie 3^999) that would take too much time to compute. */
      if (RationalExponentShouldNotBeReduced(exp)) {
        return this;
      }
      return simplifyRationalRationalPower(this, a, exp, context, angleUnit);
    }
  }
  // e^(i*Pi*r) with r rational
  if (!letPowerAtRoot && isNthRootOfUnity()) {
    Expression * m = editableOperand(1);
    detachOperand(m);
    Expression * i = m->editableOperand(m->numberOfOperands()-1);
    static_cast<Multiplication *>(m)->removeOperand(i, false);
    if (angleUnit == AngleUnit::Degree) {
      const Expression * pi = m->operand(m->numberOfOperands()-1);
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
  if (operand(1)->type() == Type::Logarithm) {
    if (operand(1)->numberOfOperands() == 2 && operand(0)->isIdenticalTo(operand(1)->operand(1))) {
      // y > 0
      if (operand(1)->operand(0)->sign() == Sign::Positive) {
        return replaceWith(editableOperand(1)->editableOperand(0), true);
      }
    }
    // 10^log(y)
    if (operand(1)->numberOfOperands() == 1 && operand(0)->type() == Type::Rational && static_cast<const Rational *>(operand(0))->isTen()) {
      return replaceWith(editableOperand(1)->editableOperand(0), true);
    }
  }
  // (a^b)^c -> a^(b*c) if a > 0 or c is integer
  if (operand(0)->type() == Type::Power) {
    Power * p = static_cast<Power *>(editableOperand(0));
    // Check is a > 0 or c is Integer
    if (p->operand(0)->sign() == Sign::Positive ||
        (operand(1)->type() == Type::Rational && static_cast<Rational *>(editableOperand(1))->denominator().isOne())) {
      return simplifyPowerPower(p, editableOperand(1), context, angleUnit);
    }
  }
  // (a*b*c*...)^r ?
  if (!letPowerAtRoot && operand(0)->type() == Type::Multiplication) {
    Multiplication * m = static_cast<Multiplication *>(editableOperand(0));
    // (a*b*c*...)^n = a^n*b^n*c^n*... if n integer
    if (operand(1)->type() == Type::Rational && static_cast<Rational *>(editableOperand(1))->denominator().isOne()) {
      return simplifyPowerMultiplication(m, editableOperand(1), context, angleUnit);
    }
    // (a*b*...)^r -> |a|^r*(sign(a)*b*...)^r if a rational
    for (int i = 0; i < m->numberOfOperands(); i++) {
      // a is signed and a != -1
      if (m->operand(i)->sign() != Sign::Unknown && (m->operand(i)->type() != Type::Rational || !static_cast<const Rational *>(m->operand(i))->isMinusOne())) {
      //if (m->operand(i)->sign() == Sign::Positive || m->operand(i)->type() == Type::Rational) {
        Expression * r = editableOperand(1);
        Expression * rCopy = r->clone();
        Expression * factor = m->editableOperand(i);
        if (factor->sign() == Sign::Negative) {
          m->replaceOperand(factor, new Rational(-1), false);
          factor->setSign(Sign::Positive, context, angleUnit);
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
  if (!letPowerAtRoot && operand(0)->type() == Type::Rational && !static_cast<const Rational *>(operand(0))->isZero() && operand(1)->type() == Type::Addition) {
    Addition * a = static_cast<Addition *>(editableOperand(1));
    // Check is b is rational
    if (a->operand(0)->type() == Type::Rational) {
      /* First, we check that the simplification does not involve too complex power
       * of integers (ie 3^999) that would take too much time to compute. */
      if (RationalExponentShouldNotBeReduced(static_cast<const Rational *>(a->operand(0)))) {
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
  if (!letPowerAtRoot && operand(1)->type() == Type::Rational && static_cast<const Rational *>(operand(1))->denominator().isOne() && operand(0)->type() == Type::Addition) {
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
    int m = operand(0)->numberOfOperands();
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
        for (int j = 0; j < a->numberOfOperands(); j++) {
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
  if (!letPowerAtRoot && operand(1)->type() == Type::Rational && static_cast<const Rational *>(operand(1))->denominator().isOne() && operand(0)->type() == Type::Addition && operand(0)->numberOfOperands() == 2) {
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
}

bool Power::parentIsALogarithmOfSameBase() const {
  if (parent()->type() == Type::Logarithm && parent()->operand(0) == this) {
    // parent = log(10^x)
    if (parent()->numberOfOperands() == 1) {
      if (operand(0)->type() == Type::Rational && static_cast<const Rational *>(operand(0))->isTen()) {
        return true;
      }
      return false;
    }
    // parent = log(x^y,x)
    if (operand(0)->isIdenticalTo(parent()->operand(1))) {
      return true;
    }
  }
  // parent = ln(e^x)
  if (parent()->type() == Type::NaperianLogarithm && parent()->operand(0) == this && operand(0)->type() == Type::Symbol && static_cast<const Symbol *>(operand(0))->name() == Ion::Charset::Exponential) {
    return true;
  }
  return false;
}

Expression * Power::simplifyPowerPower(Power * p, Expression * e, Context& context, AngleUnit angleUnit) {
  Expression * p0 = p->editableOperand(0);
  Expression * p1 = p->editableOperand(1);
  p->detachOperands();
  Multiplication * m = new Multiplication(p1, e, false);
  replaceOperand(e, m, false);
  replaceOperand(p, p0, true);
  m->shallowReduce(context, angleUnit);
  return shallowReduce(context, angleUnit);
}

Expression * Power::simplifyPowerMultiplication(Multiplication * m, Expression * r, Context& context, AngleUnit angleUnit) {
  for (int index = 0; index < m->numberOfOperands(); index++) {
    Expression * factor = m->editableOperand(index);
    Power * p = new Power(factor, r, true); // We copy r and factor to avoid inheritance issues
    m->replaceOperand(factor, p, true);
    p->shallowReduce(context, angleUnit);
  }
  detachOperand(m);
  return replaceWith(m, true)->shallowReduce(context, angleUnit); // delete r
}

Expression * Power::simplifyRationalRationalPower(Expression * result, Rational * a, Rational * b, Context& context, AngleUnit angleUnit) {
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

Expression * Power::CreateSimplifiedIntegerRationalPower(Integer i, Rational * r, bool isDenominator, Context & context, AngleUnit angleUnit) {
  assert(!i.isZero());
  assert(r->sign() == Sign::Positive);
  if (i.isOne()) {
    return new Rational(1);
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

Expression * Power::shallowBeautify(Context& context, AngleUnit angleUnit) {
  // X^-y -> 1/(X->shallowBeautify)^y
  if (operand(1)->sign() == Sign::Negative) {
    Expression * p = cloneDenominator(context, angleUnit);
    Division * d = new Division(new Rational(1), p, false);
    p->shallowReduce(context, angleUnit);
    replaceWith(d, true);
    return d->shallowBeautify(context, angleUnit);
  }
  if (operand(1)->type() == Type::Rational && static_cast<const Rational *>(operand(1))->numerator().isOne()) {
    Integer index = static_cast<const Rational *>(operand(1))->denominator();
    if (index.isEqualTo(Integer(2))) {
      const Expression * sqrtOperand[1] = {operand(0)};
      SquareRoot * sqr = new SquareRoot(sqrtOperand, true);
      return replaceWith(sqr, true);
    }
    const Expression * rootOperand[2] = {operand(0)->clone(), new Rational(index)};
    NthRoot * nr = new NthRoot(rootOperand, false);
    return replaceWith(nr, true);
  }
  // +(a,b)^c ->(+(a,b))^c
  if (operand(0)->type() == Type::Addition || operand(0)->type() == Type::Multiplication) {
    const Expression * o[1] = {operand(0)};
    Parenthesis * p = new Parenthesis(o, true);
    replaceOperand(operand(0), p, true);
  }
  return this;
}

Expression * Power::cloneDenominator(Context & context, AngleUnit angleUnit) const {
  if (operand(1)->sign() == Sign::Negative) {
    Expression * denominator = clone();
    Expression * newExponent = denominator->editableOperand(1)->setSign(Sign::Positive, context, angleUnit);
    if (newExponent->type() == Type::Rational && static_cast<Rational *>(newExponent)->isOne()) {
      delete denominator;
      return operand(0)->clone();
    }
    return denominator;
  }
  return nullptr;
}

bool Power::TermIsARationalSquareRootOrRational(const Expression * e) {
  if (e->type() == Type::Rational) {
    return true;
  }
  if (e->type() == Type::Power && e->operand(0)->type() == Type::Rational && e->operand(1)->type() == Type::Rational && static_cast<const Rational *>(e->operand(1))->isHalf()) {
    return true;
  }
  if (e->type() == Type::Multiplication && e->numberOfOperands() == 2 && e->operand(0)->type() == Type::Rational && e->operand(1)->type() == Type::Power && e->operand(1)->operand(0)->type() == Type::Rational && e->operand(1)->operand(1)->type() == Type::Rational && static_cast<const Rational *>(e->operand(1)->operand(1))->isHalf()) {
  return true;
  }
  return false;
}

const Rational * Power::RadicandInExpression(const Expression * e) {
  if (e->type() == Type::Rational) {
    return nullptr;
  } else if (e->type() == Type::Power) {
    assert(e->type() == Type::Power);
    assert(e->operand(0)->type() == Type::Rational);
    return static_cast<const Rational *>(e->operand(0));
  } else {
    assert(e->type() == Type::Multiplication);
    assert(e->operand(1)->type() == Type::Power);
    assert(e->operand(1)->operand(0)->type() == Type::Rational);
    return static_cast<const Rational *>(e->operand(1)->operand(0));
  }
}

const Rational * Power::RationalFactorInExpression(const Expression * e) {
  if (e->type() == Type::Rational) {
    return static_cast<const Rational *>(e);
  } else if (e->type() == Type::Power) {
    return nullptr;
  } else {
    assert(e->type() == Type::Multiplication);
    assert(e->operand(0)->type() == Type::Rational);
    return static_cast<const Rational *>(e->operand(0));
  }
}

Expression * Power::removeSquareRootsFromDenominator(Context & context, AngleUnit angleUnit) {
  Expression * result = nullptr;

  if (operand(0)->type() == Type::Rational && operand(1)->type() == Type::Rational && (static_cast<const Rational *>(operand(1))->isHalf() || static_cast<const Rational *>(operand(1))->isMinusHalf())) {
      /* We're considering a term of the form sqrt(p/q) (or 1/sqrt(p/q)), with
       * p and q integers.
       * We'll turn those into sqrt(p*q)/q (or sqrt(p*q)/p) . */
      Integer p = static_cast<const Rational *>(operand(0))->numerator();
      assert(!p.isZero()); // We eliminated case of form 0^(-1/2) at first step of shallowReduce
      Integer q = static_cast<const Rational *>(operand(0))->denominator();
      // We do nothing for terms of the form sqrt(p)
      if (!q.isOne() || static_cast<const Rational *>(operand(1))->isMinusHalf()) {
        Power * sqrt = new Power(new Rational(Integer::Multiplication(p, q)), new Rational(1, 2), false);
        if (static_cast<const Rational *>(operand(1))->isHalf()) {
          result = new Multiplication(new Rational(Integer(1), q), sqrt, false);
        } else {
          result = new Multiplication(new Rational(Integer(1), p), sqrt, false); // We use here the assertion that p != 0
        }
        sqrt->shallowReduce(context, angleUnit);
      }
  } else if (operand(1)->type() == Type::Rational && static_cast<const Rational *>(operand(1))->isMinusOne() && operand(0)->type() == Type::Addition && operand(0)->numberOfOperands() == 2 && TermIsARationalSquareRootOrRational(operand(0)->operand(0)) && TermIsARationalSquareRootOrRational(operand(0)->operand(1))) {
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
    const Rational * f1 = RationalFactorInExpression(operand(0)->operand(0));
    const Rational * f2 = RationalFactorInExpression(operand(0)->operand(1));
    const Rational * r1 = RadicandInExpression(operand(0)->operand(0));
    const Rational * r2 = RadicandInExpression(operand(0)->operand(1));
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
  if (operand(0)->type() != Type::Symbol || static_cast<const Symbol *>(operand(0))->name() != Ion::Charset::Exponential) {
    return false;
  }
  if (operand(1)->type() != Type::Multiplication) {
    return false;
  }
  if (operand(1)->numberOfOperands() < 2 || operand(1)->numberOfOperands() > 3) {
    return false;
  }
  const Expression * i = operand(1)->operand(operand(1)->numberOfOperands()-1);
  if (i->type() != Type::Symbol || static_cast<const Symbol *>(i)->name() != Ion::Charset::IComplex) {
    return false;
  }
  const Expression * pi = operand(1)->operand(operand(1)->numberOfOperands()-2);
  if (pi->type() != Type::Symbol || static_cast<const Symbol *>(pi)->name() != Ion::Charset::SmallPi) {
    return false;
  }
  if (numberOfOperands() == 2) {
    return true;
  }
  if (operand(1)->operand(0)->type() == Type::Rational) {
    return true;
  }
  return false;
}

bool Power::RationalExponentShouldNotBeReduced(const Rational * r) {
  Integer maxIntegerExponent = r->numerator();
  maxIntegerExponent.setNegative(false);
  if (Integer::NaturalOrder(maxIntegerExponent, Integer(k_maxIntegerPower)) > 0) {
    return true;
  }
  return false;
}

template Complex<float> Power::compute<float>(Complex<float>, Complex<float>);
template Complex<double> Power::compute<double>(Complex<double>, Complex<double>);
}
