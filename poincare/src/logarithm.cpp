#include <poincare/logarithm.h>
#include <poincare/addition.h>
#include <poincare/approximation_helper.h>
#include <poincare/arithmetic.h>
#include <poincare/constant.h>
#include <poincare/derivative.h>
#include <poincare/division.h>
#include <poincare/infinity.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/square_root.h>
#include <poincare/undefined.h>
#include <poincare/nonreal.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include <stdlib.h>
#include <utility>

namespace Poincare {

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
Expression LogarithmNode<1>::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  return CommonLogarithm(this).shallowReduce(reductionContext);
}

template<>
Expression LogarithmNode<2>::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  return Logarithm(this).shallowReduce(reductionContext);
}

template <>
bool LogarithmNode<2>::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return Logarithm(this).derivate(reductionContext, symbol, symbolValue);
}

template <>
Expression LogarithmNode<2>::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return Logarithm(this).unaryFunctionDifferential(reductionContext);
}

/* Those two methods will not be called, as CommonLogarithm disappears in
 * reduction */
template <>
bool LogarithmNode<1>::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  assert(false);
  return false;
}

template <>
Expression LogarithmNode<1>::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  assert(false);
  return Expression();
}
/**/

template<>
Expression LogarithmNode<1>::shallowBeautify(ReductionContext * reductionContext) {
  return CommonLogarithm(this);
}

template<>
Expression LogarithmNode<2>::shallowBeautify(ReductionContext * reductionContext) {
  return Logarithm(this).shallowBeautify();
}

template<>
template<typename U> Evaluation<U> LogarithmNode<1>::templatedApproximate(ApproximationContext approximationContext) const {
  return ApproximationHelper::MapOneChild<U>(this, approximationContext, computeOnComplex<U>);
}

template<>
template<typename U> Evaluation<U> LogarithmNode<2>::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<U> n = childAtIndex(1)->approximate(U(), approximationContext);
  if (Poincare::Preferences::sharedPreferences()->basedLogarithmIsForbidden()
      && n.toScalar() != static_cast<U>(10.0)
      && n.toScalar() != Complex<U>::Builder(M_E).toScalar()) {
    return Complex<U>::Undefined();
  }
  return ApproximationHelper::Map<U>(
      this,
      approximationContext,
      [] (const std::complex<U> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * context) {
        assert(numberOfComplexes == 2);
        std::complex<U> x = c[0];
        std::complex<U> n = c[1];
        return Complex<U>::Builder(
            DivisionNode::computeOnComplex<U>(
              computeOnComplex(x, complexFormat, angleUnit).complexAtIndex(0),
              computeOnComplex(n, complexFormat, angleUnit).complexAtIndex(0),
              complexFormat).complexAtIndex(0));
      });
}

void Logarithm::deepReduceChildren(const ExpressionNode::ReductionContext& reductionContext) {
  /* We reduce the base first because of the case log(x1^y, x2) with x1 == x2.
   * When reducing x1^y, we want to be able to compare x1 of x2 so x2 need to be
   * reduced first. */
  childAtIndex(1).deepReduce(reductionContext);
  childAtIndex(0).deepReduce(reductionContext);
}

Expression CommonLogarithm::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  Logarithm log = Logarithm::Builder(childAtIndex(0), Rational::Builder(10));
  replaceWithInPlace(log);
  return log.shallowReduce(reductionContext);
}

Expression Logarithm::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression base = childAtIndex(1);
  if (Poincare::Preferences::sharedPreferences()->basedLogarithmIsForbidden()) {
    if (!((base.type() == ExpressionNode::Type::ConstantMaths && static_cast<Constant&>(base).isConstant("e")) ||
          (base.type() == ExpressionNode::Type::Rational && static_cast<Rational&>(base).isTen()))) {
      return replaceWithUndefinedInPlace();
    }
  }
  Expression c = childAtIndex(0);
  if (c.sign(reductionContext.context()) == ExpressionNode::Sign::Negative
      || base.sign(reductionContext.context()) == ExpressionNode::Sign::Negative)
  {
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      Expression result = Nonreal::Builder();
      replaceWithInPlace(result);
      return result;
    }
    return *this;
  }
  Expression f = simpleShallowReduce(reductionContext);
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
    // log(+inf, a) --> ±inf with a rational and a > 0
    if (base.type() == ExpressionNode::Type::Rational && !static_cast<Rational&>(base).isNegative() && !static_cast<Rational&>(base).isZero()) {
      // log(+inf,a) with a < 1 --> -inf
      // log(+inf,a) with a > 1 --> inf
      if (static_cast<Rational&>(base).signedIntegerNumerator().isLowerThan(static_cast<Rational&>(base).integerDenominator())) {
        c = c.setSign(ExpressionNode::Sign::Negative, reductionContext);
      }
      replaceWithInPlace(c);
      return c;
    } else if (base.type() == ExpressionNode::Type::ConstantMaths && (static_cast<Constant &>(base).isConstant("e") || static_cast<Constant &>(base).isConstant("π"))) {
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
    if (base.type() == ExpressionNode::Type::Rational && base.convert<Rational>().isInteger()) {
      Integer b = base.convert<Rational>().signedIntegerNumerator();
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

  return *this;
}

Expression Logarithm::simpleShallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  Expression c = childAtIndex(0);
  Expression b = childAtIndex(1);

  // log(x,0) = log(x,1) = undef
  if (b.type() == ExpressionNode::Type::Rational && (static_cast<Rational &>(b).isZero() || static_cast<Rational &>(b).isOne())) {
    return replaceWithUndefinedInPlace();
  }
  if (c.type() == ExpressionNode::Type::Rational) {
    const Rational r = static_cast<Rational &>(c);
    // log(0,x) = undef
    if (r.isZero()) {
      return replaceWithUndefinedInPlace();
    }
    // log(1,x) = 0;
    if (r.isOne()) {
      Expression result = Rational::Builder(0);
      replaceWithInPlace(result);
      return result;
    }
  }
  bool infiniteArg = c.recursivelyMatches(Expression::IsInfinity, reductionContext.context());
  // log(x,x) = 1 with x != inf, and log(inf,inf) = undef
  if (c.isIdenticalTo(b)) {
    Expression result = infiniteArg ? Undefined::Builder().convert<Expression>() : Rational::Builder(1).convert<Expression>();
    replaceWithInPlace(result);
    return result;
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

bool Logarithm::derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  /* We do nothing if the base is a function of the derivation variable, as the
   * log is then not an unary function anymore.
   * TODO : Check whether we want to deal with the case log(..., f(x)). */
  if (childAtIndex(1).polynomialDegree(reductionContext.context(), symbol.name()) != 0) {
    return false;
  }
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue, reductionContext);
  return true;
}

Expression Logarithm::unaryFunctionDifferential(const ExpressionNode::ReductionContext& reductionContext) {
  /* log(x, b)` = (ln(x)/ln(b))`
   *            = 1 / (x * ln(b))
   */
  return Power::Builder(Multiplication::Builder(childAtIndex(0).clone(), NaperianLogarithm::Builder(childAtIndex(1).clone())), Rational::Builder(-1));
}

Expression Logarithm::splitLogarithmInteger(Integer i, bool isDenominator, const ExpressionNode::ReductionContext& reductionContext) {
  assert(!i.isZero());
  assert(!i.isNegative());
  Arithmetic arithmetic;
  int numberOfPrimeFactors = arithmetic.PrimeFactorization(i);
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
      arithmetic.coefficientAtIndex(index)->setNegative(true);
    }
    Logarithm e = clone().convert<Logarithm>();
    e.replaceChildAtIndexInPlace(0, Rational::Builder(*arithmetic.factorAtIndex(index)));
    Multiplication m = Multiplication::Builder(Rational::Builder(*arithmetic.coefficientAtIndex(index)), e);
    e.simpleShallowReduce(reductionContext);
    a.addChildAtIndexInPlace(m, a.numberOfChildren(), a.numberOfChildren());
    m.shallowReduce(reductionContext);
  }
  return std::move(a);
}

Expression Logarithm::shallowBeautify() {
  assert(numberOfChildren() == 2);
  Constant e = Constant::Builder("e");
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

template Evaluation<float> LogarithmNode<1>::templatedApproximate<float>(ApproximationContext) const;
template Evaluation<double> LogarithmNode<1>::templatedApproximate<double>(ApproximationContext) const;
template Evaluation<float> LogarithmNode<2>::templatedApproximate<float>(ApproximationContext) const;
template Evaluation<double> LogarithmNode<2>::templatedApproximate<double>(ApproximationContext) const;
template int LogarithmNode<1>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int LogarithmNode<2>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;

}
