#include <poincare/logarithm.h>
#include <poincare/addition.h>
#include <poincare/approximation_helper.h>
#include <poincare/arithmetic.h>
#include <poincare/division.h>
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

template<int I>
LogarithmNode<I> * LogarithmNode<I>::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<LogarithmNode<I> > failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

template<>
LayoutReference LogarithmNode<1>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, "log");
}

template<>
LayoutReference LogarithmNode<2>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Logarithm(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

template<int I>
Expression LogarithmNode<I>::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  return Logarithm(this).shallowReduce(context, angleUnit);
}

template<int I>
Expression LogarithmNode<I>::shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const {
  return Logarithm(this).shallowBeautify(context, angleUnit);
}

template<>
template<typename T> Evaluation<T> LogarithmNode<1>::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return ApproximationHelper::Map(this, context, angleUnit, computeOnComplex<T>);
}

template<>
template<typename T> Evaluation<T> LogarithmNode<2>::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> x = childAtIndex(0)->approximate(T(), context, angleUnit);
  Evaluation<T> n = childAtIndex(1)->approximate(T(), context, angleUnit);
  std::complex<T> result = std::complex<T>(NAN, NAN);
  if (x.type() == EvaluationNode<T>::Type::Complex && n.type() == EvaluationNode<T>::Type::Complex) {
    std::complex<T> xc = (static_cast<Complex<T>&>(x)).stdComplex();
    std::complex<T> nc = (static_cast<Complex<T>&>(n)).stdComplex();
    result = DivisionNode::compute<T>(computeOnComplex(xc, angleUnit).stdComplex(), computeOnComplex(nc, angleUnit).stdComplex()).stdComplex();
  }
  return Complex<T>(result);
}

Expression Logarithm::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent){
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (numberOfChildren() == 1 && c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
  }
  if (numberOfChildren() == 2 && (c.type() == ExpressionNode::Type::Matrix || childAtIndex(1).type() == ExpressionNode::Type::Matrix)) {
    return Undefined();
  }
#endif
  if (c.sign() == ExpressionNode::Sign::Negative || (numberOfChildren() == 2 && childAtIndex(1).sign() == ExpressionNode::Sign::Negative)) {
    return *this;
  }
  Expression f = simpleShallowReduce(context, angleUnit);
  if (f.isUndefinedOrAllocationFailure()) {
    return f;
  }

  //TODO EMILIE DEEP REDUCE
  /* We do not apply some rules if the parent node is a power of b. In this
   * case there is a simplication of form e^ln(3^(1/2))->3^(1/2) */
#if 0
  bool letLogAtRoot = parentIsAPowerOfSameBase();
  // log(x^y, b)->y*log(x, b) if x>0
  if (!letLogAtRoot && op->type() == Type::Power && op->childAtIndex(0)->sign() == Sign::Positive) {
    Power * p = static_cast<Power *>(op);
    Expression * x = p->childAtIndex(0);
    Expression * y = p->childAtIndex(1);
    p->detachOperands();
    replaceOperand(p, x, true);
    Expression * newLog = shallowReduce(context, angleUnit);
    newLog = newLog->replaceWith(new Multiplication(y, newLog->clone(), false), true);
    return newLog->shallowReduce(context, angleUnit);
  }
  // log(x*y, b)->log(x,b)+log(y, b) if x,y>0
  if (!letLogAtRoot && op->type() == Type::Multiplication) {
    Addition * a = new Addition();
    for (int i = 0; i<op->numberOfChildren()-1; i++) {
      Expression * factor = op->childAtIndex(i);
      if (factor->sign() == Sign::Positive) {
        Expression * newLog = clone();
        static_cast<Multiplication *>(op)->removeOperand(factor, false);
        newLog->replaceOperand(newLog->childAtIndex(0), factor, true);
        a->addOperand(newLog);
        newLog->shallowReduce(context, angleUnit);
      }
    }
    if (a->numberOfChildren() > 0) {
      op->shallowReduce(context, angleUnit);
      Expression * reducedLastLog = shallowReduce(context, angleUnit);
      reducedLastLog->replaceWith(a, false);
      a->addOperand(reducedLastLog);
      return a->shallowReduce(context, angleUnit);
    } else {
      delete a;
    }
  }
  // log(r) = a0log(p0)+a1log(p1)+... with r = p0^a0*p1^a1*... (Prime decomposition)
  if (!letLogAtRoot && op->type() == Type::Rational) {
    const Rational * r = static_cast<const Rational *>(childAtIndex(0));
    Expression * n = splitInteger(r->numerator(), false, context, angleUnit);
    Expression * d = splitInteger(r->denominator(), true, context, angleUnit);
    Addition * a = new Addition(n, d, false);
    replaceWith(a, true);
    return a->shallowReduce(context, angleUnit);
  }
#endif
  return *this;
}

Expression Logarithm::simpleShallowReduce(Context & context, Preferences::AngleUnit angleUnit) const {
  Expression c = childAtIndex(0);
  // log(x,x)->1
  if (numberOfChildren() == 2 && c.isIdenticalTo(childAtIndex(1))) {
    return Rational(1);
  }
  if (c.type() == ExpressionNode::Type::Rational) {
    const Rational r = static_cast<Rational>(childAtIndex(0));
    // log(0) = undef
    if (r.isZero()) {
      return Undefined();
    }
    // log(1) = 0;
    if (r.isOne()) {
      return Rational(0);
    }
    // log(10) ->1
    if (numberOfChildren() == 1 && r.isTen()) {
      return Rational(1);
    }
  }
  return *this;
}

//TODO EMILIE
#if 0
bool Logarithm::parentIsAPowerOfSameBase() const {
  // We look for expressions of types e^ln(x) or e^(ln(x)) where ln is this
  const Expression * parentExpression = parent();
  bool thisIsPowerExponent = parentExpression->type() == Type::Power ? parentExpression->childAtIndex(1) == this : false;
  if (parentExpression->type() == Type::Parenthesis) {
    const Expression * parentParentExpression = parentExpression->parent();
    if (parentExpression == nullptr) {
      return false;
    }
    thisIsPowerExponent = parentParentExpression->type() == Type::Power ? parentParentExpression->childAtIndex(1) == parentExpression : false;
    parentExpression = parentParentExpression;
  }
  if (thisIsPowerExponent) {
    const Expression * powerOperand0 = parentExpression->childAtIndex(0);
    if (numberOfChildren() == 1) {
      if (powerOperand0->type() == Type::Rational && static_cast<const Rational *>(powerOperand0)->isTen()) {
        return true;
      }
    }
    if (numberOfChildren() == 2) {
      if (powerOperand0->isIdenticalTo(childAtIndex(1))){
        return true;
      }
    }
  }
  return false;
}
#endif

Expression Logarithm::splitInteger(Integer i, bool isDenominator, Context & context, Preferences::AngleUnit angleUnit) {
  assert(!i.isZero());
  assert(!i.isNegative());
  if (i.isOne()) {
    return Rational(0);
  }
  assert(!i.isOne());
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  Arithmetic::PrimeFactorization(i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  if (coefficients[0].isMinusOne()) {
    /* We could not break i in prime factor (either it might take too many
     * factors or too much time). */
    Expression e = *this;
    e.replaceChildAtIndexInPlace(0, Rational(i));
    if (!isDenominator) {
      return e;
    }
    Multiplication m = Multiplication(Rational(-1), e);
    return m;
  }
  Addition a;
  int index = 0;
  while (!coefficients[index].isZero() && index < Arithmetic::k_maxNumberOfPrimeFactors) {
    if (isDenominator) {
      coefficients[index].setNegative(true);
    }
    Logarithm e = *this;
    e.replaceChildAtIndexInPlace(0, Rational(factors[index]));
    Multiplication m = Multiplication(Rational(coefficients[index]), e.simpleShallowReduce(context, angleUnit));
    a.addChildAtIndexInPlace(m.shallowReduce(context, angleUnit), a.numberOfChildren(), a.numberOfChildren());
    index++;
  }
  return a;
}


Expression Logarithm::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const {
  if (numberOfChildren() == 1) {
    return *this;
  }
  assert(numberOfChildren() == 2);
  Symbol e = Symbol(Ion::Charset::Exponential);
  Rational one(1);
  if (childAtIndex(1).isIdenticalTo(e)) {
    return NaperianLogarithm(childAtIndex(0));
  }
  if (childAtIndex(1).isIdenticalTo(one)) {
    return Logarithm(childAtIndex(0));
  }
  return *this;
}

template LogarithmNode<1> * LogarithmNode<1>::FailedAllocationStaticNode();
template LogarithmNode<2> * LogarithmNode<2>::FailedAllocationStaticNode();
template Evaluation<float> LogarithmNode<1>::templatedApproximate<float>(Poincare::Context&, Poincare::Preferences::AngleUnit) const;
template Evaluation<double> LogarithmNode<1>::templatedApproximate<double>(Poincare::Context&, Poincare::Preferences::AngleUnit) const;
template Evaluation<float> LogarithmNode<2>::templatedApproximate<float>(Poincare::Context&, Poincare::Preferences::AngleUnit) const;
template Evaluation<double> LogarithmNode<2>::templatedApproximate<double>(Poincare::Context&, Poincare::Preferences::AngleUnit) const;
template Expression LogarithmNode<1>::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent);
template Expression LogarithmNode<2>::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent);
template Expression LogarithmNode<1>::shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const;
template Expression LogarithmNode<2>::shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const;

}
