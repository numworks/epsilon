#include <poincare/logarithm.h>
#include <poincare/addition.h>
#include <poincare/approximation_helper.h>
#include <poincare/arithmetic.h>
#include <poincare/division.h>
#include <poincare/infinity.h>
#include <poincare/multiplication.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <cmath>
#include <ion.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

template<>
Layout LogarithmNode<1>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, "log");
}

template<>
Layout LogarithmNode<2>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Logarithm(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

template<int T>
Expression LogarithmNode<T>::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Logarithm(this).shallowReduce(context, angleUnit);
}

template<int T>
Expression LogarithmNode<T>::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  return Logarithm(this).shallowBeautify(context, angleUnit);
}

template<>
template<typename U> Evaluation<U> LogarithmNode<1>::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return ApproximationHelper::Map(this, context, angleUnit, computeOnComplex<U>);
}

template<>
template<typename U> Evaluation<U> LogarithmNode<2>::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<U> x = childAtIndex(0)->approximate(U(), context, angleUnit);
  Evaluation<U> n = childAtIndex(1)->approximate(U(), context, angleUnit);
  std::complex<U> result = std::complex<U>(NAN, NAN);
  if (x.type() == EvaluationNode<U>::Type::Complex && n.type() == EvaluationNode<U>::Type::Complex) {
    std::complex<U> xc = (static_cast<Complex<U>&>(x)).stdComplex();
    std::complex<U> nc = (static_cast<Complex<U>&>(n)).stdComplex();
    result = DivisionNode::compute<U>(computeOnComplex(xc, angleUnit).stdComplex(), computeOnComplex(nc, angleUnit).stdComplex()).stdComplex();
  }
  return Complex<U>(result);
}

Expression Logarithm::shallowReduce(Context & context, Preferences::AngleUnit angleUnit){
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
#if 0
  if (numberOfChildren() == 1 && c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
  }
  if (numberOfChildren() == 2 && (c.type() == ExpressionNode::Type::Matrix || childAtIndex(1).type() == ExpressionNode::Type::Matrix)) {
    return Undefined();
  }
#endif
#endif
  if (c.sign() == ExpressionNode::Sign::Negative || (numberOfChildren() == 2 && childAtIndex(1).sign() == ExpressionNode::Sign::Negative)) {
    return *this;
  }
  Expression f = simpleShallowReduce(context, angleUnit);
  if (f.type() != ExpressionNode::Type::Logarithm) {
    return f;
  }

  /* We do not apply some rules if the parent node is a power of b. In this
   * case there is a simplication of form e^ln(3^(1/2))->3^(1/2) */
  bool letLogAtRoot = parentIsAPowerOfSameBase();
  // log(x^y, b)->y*log(x, b) if x>0
  if (!letLogAtRoot && c.type() == ExpressionNode::Type::Power && c.childAtIndex(0).sign() == ExpressionNode::Sign::Positive) {
    Power p = static_cast<Power &>(c);
    Expression x = p.childAtIndex(0);
    Expression y = p.childAtIndex(1);
    replaceChildInPlace(p, x);
    Multiplication mult(y);
    replaceWithInPlace(mult);
    mult.addChildAtIndexInPlace(*this, 1, 1); // --> y*log(x,b)
    shallowReduce(context, angleUnit); // reduce log (ie log(e, e) = 1)
    return mult.shallowReduce(context, angleUnit);
  }
  // log(x*y, b)->log(x,b)+log(y, b) if x,y>0
  if (!letLogAtRoot && c.type() == ExpressionNode::Type::Multiplication) {
    Addition a = Addition();
    for (int i = 0; i < c.numberOfChildren()-1; i++) {
      Expression factor = c.childAtIndex(i);
      if (factor.sign() == ExpressionNode::Sign::Positive) {
        Expression newLog = clone();
        static_cast<Multiplication &>(c).removeChildInPlace(factor, factor.numberOfChildren());
        newLog.replaceChildAtIndexInPlace(0, factor);
        a.addChildAtIndexInPlace(newLog, a.numberOfChildren(), a.numberOfChildren());
        newLog.shallowReduce(context, angleUnit);
      }
    }
    if (a.numberOfChildren() > 0) {
      c.shallowReduce(context, angleUnit);
      Expression reducedLastLog = shallowReduce(context, angleUnit);
      reducedLastLog.replaceWithInPlace(a);
      a.addChildAtIndexInPlace(reducedLastLog, a.numberOfChildren(), a.numberOfChildren());
      return a.shallowReduce(context, angleUnit);
    }
  }
  // log(r) with r Rational
  if (!letLogAtRoot && c.type() == ExpressionNode::Type::Rational) {
    Rational r = static_cast<Rational &>(c);
    Addition a;
    // if the log base is Integer: log_b(r) = c + log_b(r') with r = b^c*r'
    if (numberOfChildren() == 1 || (childAtIndex(1).type() == ExpressionNode::Type::Rational && childAtIndex(1).convert<Rational>().integerDenominator().isOne())) {
      Integer b = numberOfChildren() == 2 ? childAtIndex(1).convert<Rational>().signedIntegerNumerator() : Integer(10);
      Integer newNumerator = simplifyLogarithmIntegerBaseInteger(r.signedIntegerNumerator(), b, a, false);
      Integer newDenomitor = simplifyLogarithmIntegerBaseInteger(r.integerDenominator(), b, a, true);
      r = Rational(newNumerator, newDenomitor);
    }
    // log(r) = a0log(p0)+a1log(p1)+... with r = p0^a0*p1^a1*... (Prime decomposition)
    a.addChildAtIndexInPlace(splitLogarithmInteger(r.signedIntegerNumerator(), false, context, angleUnit), a.numberOfChildren(), a.numberOfChildren());
    a.addChildAtIndexInPlace(splitLogarithmInteger(r.integerDenominator(), true, context, angleUnit), a.numberOfChildren(), a.numberOfChildren());
    replaceWithInPlace(a);
    return a.shallowReduce(context, angleUnit);
  }
  return *this;
}

Expression Logarithm::simpleShallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  Expression c = childAtIndex(0);
  // log(0,0)->Undefined
  if (numberOfChildren() == 2 && c.type() == ExpressionNode::Type::Rational && childAtIndex(1).type() == ExpressionNode::Type::Rational && childAtIndex(1).convert<Rational>().isZero() && static_cast<Rational &>(c).isZero()) {
    Expression result = Undefined();
    replaceWithInPlace(result);
    return result;
  }
  // log(x,1)->Undefined
  if (numberOfChildren() == 2 && childAtIndex(1).type() == ExpressionNode::Type::Rational && childAtIndex(1).convert<Rational>().isOne()) {
    Expression result = Undefined();
    replaceWithInPlace(result);
    return result;
  }
  // log(x,x)->1
  if (numberOfChildren() == 2 && c.isIdenticalTo(childAtIndex(1))) {
    Expression result = Rational(1);
    replaceWithInPlace(result);
    return result;
  }
  // log(x,0)->0
  if (numberOfChildren() == 2 && childAtIndex(1).type() == ExpressionNode::Type::Rational && childAtIndex(1).convert<Rational>().isZero()) {
    Expression result = Rational(0);
    replaceWithInPlace(result);
    return result;
  }
  if (c.type() == ExpressionNode::Type::Rational) {
    const Rational r = static_cast<Rational &>(c);
    // log(0, x) = -inf if x > 1 || inf x < 1 || undef if x < 0
    if (r.isZero()) {
      bool isNegative = true;
      Expression result;
      if (numberOfChildren() == 2) {
        Evaluation<float> baseApproximation = childAtIndex(1).node()->approximate(1.0f, context, angleUnit);
        std::complex<float> logDenominator = std::log10(static_cast<Complex<float>&>(baseApproximation).stdComplex());
        if (logDenominator.imag() != 0.0f || logDenominator.real() == 0.0f) {
          result = Undefined();
        }
        isNegative = logDenominator.real() > 0.0;
      }
      result = result.isUninitialized() ? Infinity(isNegative) : result;
      replaceWithInPlace(result);
      return result;
    }
    // log(1) = 0;
    if (r.isOne()) {
      Expression result = Rational(0);
      replaceWithInPlace(result);
      return result;
    }
    // log(10) ->1
    if (numberOfChildren() == 1 && r.isTen()) {
      Expression result = Rational(1);
      replaceWithInPlace(result);
      return result;
    }
  }
  return *this;
}

bool Logarithm::parentIsAPowerOfSameBase() const {
  // We look for expressions of types e^ln(x) or e^(ln(x)) where ln is this
  Expression parentExpression = parent();
  Expression logGroup = *this;
  if (!parentExpression.isUninitialized() && parentExpression.type() == ExpressionNode::Type::Parenthesis) {
    logGroup = parentExpression;
    parentExpression = parentExpression.parent();
  }
  if (parentExpression.isUninitialized()) {
    return false;
  }
  bool thisIsPowerExponent = parentExpression.type() == ExpressionNode::Type::Power ? parentExpression.childAtIndex(1) == logGroup : false;
  if (thisIsPowerExponent) {
    Expression powerOperand0 = parentExpression.childAtIndex(0);
    if (numberOfChildren() == 1) {
      if (powerOperand0.type() == ExpressionNode::Type::Rational && static_cast<Rational&>(powerOperand0).isTen()) {
        return true;
      }
    }
    if (numberOfChildren() == 2) {
      if (powerOperand0.isIdenticalTo(childAtIndex(1))) {
        return true;
      }
    }
  }
  return false;
}

Integer Logarithm::simplifyLogarithmIntegerBaseInteger(Integer i, Integer & base, Addition & a, bool isDenominator) {
  // log_b(i) = c+ log_b(i') with i = b^c*i'
  assert(!i.isNegative() && !base.isNegative());
  assert(!i.isZero() && !base.isZero() && !base.isOne());
  IntegerDivision div = Integer::Division(i, base);
  Rational one(1);
  while (!div.quotient.isInfinity() && div.remainder.isZero()) {
    i = div.quotient;
    a.addChildAtIndexInPlace(isDenominator ? Rational(-1) : Rational(1), a.numberOfChildren(), a.numberOfChildren()); // a++
    div = Integer::Division(i, base);
  }
  return i;
}

Expression Logarithm::splitLogarithmInteger(Integer i, bool isDenominator, Context & context, Preferences::AngleUnit angleUnit) {
  assert(!i.isZero());
  assert(!i.isNegative());
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  int numberOfPrimeFactors = Arithmetic::PrimeFactorization(i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  if (numberOfPrimeFactors == 0) {
    return Rational(0);
  }
  if (numberOfPrimeFactors < 0) {
    /* We could not break i in prime factor (either it might take too many
     * factors or too much time). */
    Expression e = clone();
    e.replaceChildAtIndexInPlace(0, Rational(i));
    if (!isDenominator) {
      return e;
    }
    Multiplication m = Multiplication(Rational(-1), e);
    return m;
  }
  Addition a;
  for (int index = 0; index < numberOfPrimeFactors; index++) {
    if (isDenominator) {
      coefficients[index].setNegative(true);
    }
    Logarithm e = clone().convert<Logarithm>();
    e.replaceChildAtIndexInPlace(0, Rational(factors[index]));
    Multiplication m = Multiplication(Rational(coefficients[index]), e);
    e.simpleShallowReduce(context, angleUnit);
    a.addChildAtIndexInPlace(m, a.numberOfChildren(), a.numberOfChildren());
    m.shallowReduce(context, angleUnit);
  }
  return a;
}


Expression Logarithm::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  if (numberOfChildren() == 1) {
    return *this;
  }
  assert(numberOfChildren() == 2);
  Symbol e = Symbol(Ion::Charset::Exponential);
  if (childAtIndex(1).isIdenticalTo(e)) {
    NaperianLogarithm np(childAtIndex(0));
    replaceWithInPlace(np);
    return np;
  }
  Rational ten(10);
  if (childAtIndex(1).isIdenticalTo(ten)) {
    Logarithm l(childAtIndex(0));
    replaceWithInPlace(l);
    return l;
  }
  return *this;
}

template Evaluation<float> LogarithmNode<1>::templatedApproximate<float>(Poincare::Context&, Poincare::Preferences::AngleUnit) const;
template Evaluation<double> LogarithmNode<1>::templatedApproximate<double>(Poincare::Context&, Poincare::Preferences::AngleUnit) const;
template Evaluation<float> LogarithmNode<2>::templatedApproximate<float>(Poincare::Context&, Poincare::Preferences::AngleUnit) const;
template Evaluation<double> LogarithmNode<2>::templatedApproximate<double>(Poincare::Context&, Poincare::Preferences::AngleUnit) const;
template Expression LogarithmNode<1>::shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
template Expression LogarithmNode<2>::shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
template Expression LogarithmNode<1>::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);
template Expression LogarithmNode<2>::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);

}
