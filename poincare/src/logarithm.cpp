#include <poincare/logarithm.h>
#include <poincare/addition.h>
#include <poincare/approximation_helper.h>
#include <poincare/arithmetic.h>
#include <poincare/constant.h>
#include <poincare/division.h>
#include <poincare/infinity.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <poincare/unreal.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include <stdlib.h>
#include <utility>

namespace Poincare {

constexpr Expression::FunctionHelper Logarithm::s_functionHelper;

constexpr Expression::FunctionHelper CommonLogarithm::s_functionHelper;

template<>
int LogarithmNode<1>::numberOfChildren() const { return CommonLogarithm::s_functionHelper.numberOfChildren(); }

template<>
int LogarithmNode<2>::numberOfChildren() const { return Logarithm::s_functionHelper.numberOfChildren(); }

template<>
Layout LogarithmNode<1>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, CommonLogarithm::s_functionHelper.name());
}

template<>
Layout LogarithmNode<2>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Logarithm(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

template<int T>
int LogarithmNode<T>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, T == 1 ? CommonLogarithm::s_functionHelper.name() : Logarithm::s_functionHelper.name());
}

template<>
void LogarithmNode<2>::deepReduceChildren(ExpressionNode::ReductionContext reductionContext) {
  Logarithm(this).deepReduceChildren(reductionContext);
}

template<>
void LogarithmNode<1>::deepReduceChildren(ExpressionNode::ReductionContext reductionContext) {
  return Expression(this).defaultDeepReduceChildren(reductionContext);
}

template<>
Expression LogarithmNode<1>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  return CommonLogarithm(this).shallowReduce(reductionContext);
}

template<>
Expression LogarithmNode<2>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  return Logarithm(this).shallowReduce(reductionContext);
}

template<>
Expression LogarithmNode<1>::shallowBeautify(ReductionContext reductionContext) {
  return CommonLogarithm(this);
}

template<>
Expression LogarithmNode<2>::shallowBeautify(ReductionContext reductionContext) {
  return Logarithm(this).shallowBeautify();
}

template<>
template<typename U> Evaluation<U> LogarithmNode<1>::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return ApproximationHelper::Map(this, context, complexFormat, angleUnit, computeOnComplex<U>);
}

template<>
template<typename U> Evaluation<U> LogarithmNode<2>::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<U> x = childAtIndex(0)->approximate(U(), context, complexFormat, angleUnit);
  Evaluation<U> n = childAtIndex(1)->approximate(U(), context, complexFormat, angleUnit);
  std::complex<U> result = std::complex<U>(NAN, NAN);
  if (x.type() == EvaluationNode<U>::Type::Complex && n.type() == EvaluationNode<U>::Type::Complex) {
    std::complex<U> xc = (static_cast<Complex<U>&>(x)).stdComplex();
    std::complex<U> nc = (static_cast<Complex<U>&>(n)).stdComplex();
    result = DivisionNode::compute<U>(computeOnComplex(xc, complexFormat, angleUnit).stdComplex(), computeOnComplex(nc, complexFormat, angleUnit).stdComplex(), complexFormat).stdComplex();
  }
  return Complex<U>::Builder(result);
}

void Logarithm::deepReduceChildren(ExpressionNode::ReductionContext reductionContext) {
  /* We reduce the base first because of the case log(x1^y, x2) with x1 == x2.
   * When reducing x1^y, we want to be able to compare x1 of x2 so x2 need to be
   * reduced first. */
  childAtIndex(1).deepReduce(reductionContext);
  childAtIndex(0).deepReduce(reductionContext);
}

Expression CommonLogarithm::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixFirstChild(reductionContext);
  }
  Logarithm log = Logarithm::Builder(childAtIndex(0), Rational::Builder(10));
  replaceWithInPlace(log);
  return log.shallowReduce(reductionContext);
}

Expression Logarithm::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }

  if (childAtIndex(1).deepIsMatrix(reductionContext.context())) {
    return replaceWithUndefinedInPlace();
  }

  Expression c = childAtIndex(0);
  if (c.sign(reductionContext.context()) == ExpressionNode::Sign::Negative
      || childAtIndex(1).sign(reductionContext.context()) == ExpressionNode::Sign::Negative)
  {
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      Expression result = Unreal::Builder();
      replaceWithInPlace(result);
      return result;
    }
    return *this;
  }
  Expression f = simpleShallowReduce(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
  if (f.type() != ExpressionNode::Type::Logarithm) {
    return f;
  }

  /* We do not apply some rules if:
   * - the parent node is a power of b. In this case there is a simplication of
   *   form e^ln(3^(1/2))->3^(1/2).
   * - the reduction is being BottomUp. In this case, we do not yet have any
   *   information on the parent which could later be a power of b.
   */
  bool letLogAtRoot = parentIsAPowerOfSameBase();
  if (letLogAtRoot) {
    return *this;
  }

  // log(+inf, a) ?
  if (c.type() == ExpressionNode::Type::Infinity && c.sign(reductionContext.context()) == ExpressionNode::Sign::Positive) {
    Expression base = childAtIndex(1);
    // log(+inf, a) --> ±inf with a rational and a > 0
    if (base.type() == ExpressionNode::Type::Rational && !static_cast<Rational&>(base).isNegative() && !static_cast<Rational&>(base).isZero()) {
      // log(+inf,a) with a < 1 --> -inf
      // log(+inf,a) with a > 1 --> inf
      if (static_cast<Rational&>(base).signedIntegerNumerator().isLowerThan(static_cast<Rational&>(base).integerDenominator())) {
        c = c.setSign(ExpressionNode::Sign::Negative, reductionContext);
      }
      replaceWithInPlace(c);
      return c;
    } else if (base.type() == ExpressionNode::Type::Constant && (static_cast<Constant &>(base).isExponential() || static_cast<Constant &>(base).isPi())) {
      replaceWithInPlace(c);
      return c;
    }
  }

  // log(x^y, b)->y*log(x, b) if x>0
  if (c.type() == ExpressionNode::Type::Power && c.childAtIndex(0).sign(reductionContext.context()) == ExpressionNode::Sign::Positive) {
    Power p = static_cast<Power &>(c);
    Expression x = p.childAtIndex(0);
    Expression y = p.childAtIndex(1);
    replaceChildInPlace(p, x);
    Multiplication mult = Multiplication::Builder(y);
    replaceWithInPlace(mult);
    mult.addChildAtIndexInPlace(*this, 1, 1); // --> y*log(x,b)
    shallowReduce(reductionContext); // reduce log (ie log(e, e) = 1)
    return mult.shallowReduce(reductionContext);
  }
  // log(x*y, b)->log(x,b)+log(y, b) if x,y>0
  if (c.type() == ExpressionNode::Type::Multiplication) {
    Addition a = Addition::Builder();
    for (int i = 0; i < c.numberOfChildren()-1; i++) {
      Expression factor = c.childAtIndex(i);
      if (factor.sign(reductionContext.context()) == ExpressionNode::Sign::Positive) {
        Expression newLog = clone();
        static_cast<Multiplication &>(c).removeChildInPlace(factor, factor.numberOfChildren());
        newLog.replaceChildAtIndexInPlace(0, factor);
        a.addChildAtIndexInPlace(newLog, a.numberOfChildren(), a.numberOfChildren());
        newLog.shallowReduce(reductionContext);
      }
    }
    if (a.numberOfChildren() > 0) {
      c.shallowReduce(reductionContext);
      Expression reducedLastLog = shallowReduce(reductionContext);
      reducedLastLog.replaceWithInPlace(a);
      a.addChildAtIndexInPlace(reducedLastLog, a.numberOfChildren(), a.numberOfChildren());
      return a.shallowReduce(reductionContext);
    }
  }
  // log(r) with r Rational
  if (c.type() == ExpressionNode::Type::Rational) {
    Rational r = static_cast<Rational &>(c);
    Addition a = Addition::Builder();
    // if the log base is Integer: log_b(r) = c + log_b(r') with r = b^c*r'
    if (childAtIndex(1).type() == ExpressionNode::Type::Rational && childAtIndex(1).convert<Rational>().isInteger()) {
      Integer b = childAtIndex(1).convert<Rational>().signedIntegerNumerator();
      Integer newNumerator = simplifyLogarithmIntegerBaseInteger(r.signedIntegerNumerator(), b, a, false);
      Integer newDenomitor = simplifyLogarithmIntegerBaseInteger(r.integerDenominator(), b, a, true);
      r = Rational::Builder(newNumerator, newDenomitor);
    }
    // log(r) = a0log(p0)+a1log(p1)+... with r = p0^a0*p1^a1*... (Prime decomposition)
    a.addChildAtIndexInPlace(splitLogarithmInteger(r.signedIntegerNumerator(), false, reductionContext), a.numberOfChildren(), a.numberOfChildren());
    a.addChildAtIndexInPlace(splitLogarithmInteger(r.integerDenominator(), true, reductionContext), a.numberOfChildren(), a.numberOfChildren());
    replaceWithInPlace(a);
    return a.shallowReduce(reductionContext);
  }

  // log(m) with m Matrix
  if (c.type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixFirstChild(reductionContext);
  }

  return *this;
}

Expression Logarithm::simpleShallowReduce(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  Expression c = childAtIndex(0);
  Expression b = childAtIndex(1);
  // log(0,0)->Undefined
  if (c.type() == ExpressionNode::Type::Rational && b.type() == ExpressionNode::Type::Rational && static_cast<Rational &>(b).isZero() && static_cast<Rational &>(c).isZero()) {
    return replaceWithUndefinedInPlace();
  }
  // log(x,1)->Undefined
  if (b.type() == ExpressionNode::Type::Rational && static_cast<Rational &>(b).isOne()) {
    return replaceWithUndefinedInPlace();
  }
  bool infiniteArg = c.recursivelyMatches(Expression::IsInfinity, context);
  // log(x,x)->1 with x != inf and log(inf,inf) = undef
  if (c.isIdenticalTo(b)) {
    Expression result = infiniteArg ? Undefined::Builder().convert<Expression>() : Rational::Builder(1).convert<Expression>();
    replaceWithInPlace(result);
    return result;
  }
  // log(x,0)->0 with x != inf and log(inf,0) = undef
  if (b.type() == ExpressionNode::Type::Rational && static_cast<Rational &>(b).isZero()) {
    Expression result = infiniteArg ? Undefined::Builder().convert<Expression>() : Rational::Builder(0).convert<Expression>();
    replaceWithInPlace(result);
    return result;
  }

  if (c.type() == ExpressionNode::Type::Rational) {
    const Rational r = static_cast<Rational &>(c);
    // log(0, x) = -inf if x > 1 && x != inf || inf x < 1 || undef if x < 0
    if (r.isZero()) {
      bool infiniteBase = b.recursivelyMatches(Expression::IsInfinity, context);
      // Special case: log(0,inf) -> undef
      if (infiniteBase) {
        return replaceWithUndefinedInPlace();
      }
      bool isNegative = true;
      Expression result;
      Evaluation<float> baseApproximation = b.node()->approximate(1.0f, context, complexFormat, angleUnit);
      std::complex<float> logDenominator = std::log10(static_cast<Complex<float>&>(baseApproximation).stdComplex());
      if (logDenominator.imag() != 0.0f || logDenominator.real() == 0.0f) {
        result = Undefined::Builder();
      }
      isNegative = logDenominator.real() > 0.0f;
      result = result.isUninitialized() ? Infinity::Builder(isNegative) : result;
      replaceWithInPlace(result);
      return result;
    }
    // log(1) = 0;
    if (r.isOne()) {
      Expression result = Rational::Builder(0);
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
    // powerOperand0 has already been reduced so can be compared to childAtIndex(1)
    if (powerOperand0.isIdenticalTo(childAtIndex(1))) {
      return true;
    }
  }
  return false;
}

Integer Logarithm::simplifyLogarithmIntegerBaseInteger(Integer i, Integer & base, Addition & a, bool isDenominator) {
  // log_b(i) = c+ log_b(i') with i = b^c*i'
  assert(!i.isNegative() && !base.isNegative());
  assert(!i.isZero() && !base.isZero() && !base.isOne());
  IntegerDivision div = Integer::Division(i, base);
  while (!div.quotient.isOverflow() && div.remainder.isZero()) {
    i = div.quotient;
    a.addChildAtIndexInPlace(isDenominator ? Rational::Builder(-1) : Rational::Builder(1), a.numberOfChildren(), a.numberOfChildren()); // a++
    div = Integer::Division(i, base);
  }
  return i;
}

Expression Logarithm::splitLogarithmInteger(Integer i, bool isDenominator, ExpressionNode::ReductionContext reductionContext) {
  assert(!i.isZero());
  assert(!i.isNegative());
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  int numberOfPrimeFactors = Arithmetic::PrimeFactorization(i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  if (numberOfPrimeFactors == 0) {
    return Rational::Builder(0);
  }
  if (numberOfPrimeFactors < 0) {
    /* We could not break i in prime factor (either it might take too many
     * factors or too much time). */
    Expression e = clone();
    e.replaceChildAtIndexInPlace(0, Rational::Builder(i));
    if (!isDenominator) {
      return e;
    }
    Multiplication m = Multiplication::Builder(Rational::Builder(-1), e);
    return std::move(m);
  }
  Addition a = Addition::Builder();
  for (int index = 0; index < numberOfPrimeFactors; index++) {
    if (isDenominator) {
      coefficients[index].setNegative(true);
    }
    Logarithm e = clone().convert<Logarithm>();
    e.replaceChildAtIndexInPlace(0, Rational::Builder(factors[index]));
    Multiplication m = Multiplication::Builder(Rational::Builder(coefficients[index]), e);
    e.simpleShallowReduce(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
    a.addChildAtIndexInPlace(m, a.numberOfChildren(), a.numberOfChildren());
    m.shallowReduce(reductionContext);
  }
  return std::move(a);
}

Expression Logarithm::shallowBeautify() {
  assert(numberOfChildren() == 2);
  Constant e = Constant::Builder(UCodePointScriptSmallE);
  if (childAtIndex(1).isIdenticalTo(e)) {
    NaperianLogarithm np = NaperianLogarithm::Builder(childAtIndex(0));
    replaceWithInPlace(np);
    return std::move(np);
  }
  Rational ten = Rational::Builder(10);
  if (childAtIndex(1).isIdenticalTo(ten)) {
    CommonLogarithm l = CommonLogarithm::Builder(childAtIndex(0));
    replaceWithInPlace(l);
    return std::move(l);
  }
  return *this;
}

template Evaluation<float> LogarithmNode<1>::templatedApproximate<float>(Poincare::Context *, Poincare::Preferences::ComplexFormat, Poincare::Preferences::AngleUnit) const;
template Evaluation<double> LogarithmNode<1>::templatedApproximate<double>(Poincare::Context *, Poincare::Preferences::ComplexFormat, Poincare::Preferences::AngleUnit) const;
template Evaluation<float> LogarithmNode<2>::templatedApproximate<float>(Poincare::Context *, Poincare::Preferences::ComplexFormat, Poincare::Preferences::AngleUnit) const;
template Evaluation<double> LogarithmNode<2>::templatedApproximate<double>(Poincare::Context *, Poincare::Preferences::ComplexFormat, Poincare::Preferences::AngleUnit) const;
template int LogarithmNode<1>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int LogarithmNode<2>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;

}
