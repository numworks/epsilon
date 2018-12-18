#include <poincare/complex_cartesian.h>
#include <poincare/addition.h>
#include <poincare/arc_tangent.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/cosine.h>
#include <poincare/constant.h>
#include <poincare/division.h>
#include <poincare/multiplication.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/rational.h>
#include <poincare/square_root.h>
#include <poincare/sine.h>
#include <poincare/sign_function.h>
#include <poincare/subtraction.h>
#include <poincare/power.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

Expression ComplexCartesianNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return ComplexCartesian(this).shallowReduce(context, angleUnit);
}

Expression ComplexCartesianNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  return ComplexCartesian(this).shallowBeautify(context, angleUnit);
}

template<typename T>
Complex<T> ComplexCartesianNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> realEvaluation = childAtIndex(0)->approximate(T(), context, angleUnit);
  Evaluation<T> imagEvalution = childAtIndex(1)->approximate(T(), context, angleUnit);
  assert(realEvaluation.type() == EvaluationNode<T>::Type::Complex);
  assert(imagEvalution.type() == EvaluationNode<T>::Type::Complex);
  std::complex<T> a = static_cast<Complex<T> &>(realEvaluation).stdComplex();
  std::complex<T> b = static_cast<Complex<T> &>(imagEvalution).stdComplex();
  assert(a.imag() == 0.0);
  assert(b.imag() == 0.0);
  return Complex<T>(a.real(), b.real());
}

Expression ComplexCartesian::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  if (imag().isRationalZero()) {
    Expression r = real();
    replaceWithInPlace(r);
    return r;
  }
  return *this;
}

Expression ComplexCartesian::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  Expression a = real();
  Expression b = imag();
  Expression oppositeA = a.makePositiveAnyNegativeNumeralFactor(context, angleUnit, ExpressionNode::ReductionTarget::User);
  Expression oppositeB = b.makePositiveAnyNegativeNumeralFactor(context, angleUnit, ExpressionNode::ReductionTarget::User);
  a = oppositeA.isUninitialized() ? a : oppositeA;
  b = oppositeB.isUninitialized() ? b : oppositeB;
  Expression e = Expression::CreateComplexExpression(a, b, Preferences::ComplexFormat::Cartesian,
      a.type() == ExpressionNode::Type::Undefined || b.type() == ExpressionNode::Type::Undefined,
      Expression::isZero(a), Expression::isOne(a), Expression::isZero(b), Expression::isOne(b),
      !oppositeA.isUninitialized(),
      !oppositeB.isUninitialized()
    );
  replaceWithInPlace(e);
  return e;
}

Expression ComplexCartesian::squareNorm(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  Expression a2 = Power(a, Rational(2));
  Expression b2 = Power(b, Rational(2));
  Addition add(a2, b2);
  a2.shallowReduce(context, angleUnit, target);
  b2.shallowReduce(context, angleUnit, target);
  return add;
}

Expression ComplexCartesian::norm(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  if (imag().isRationalZero()) {
    Expression a = real();
    ExpressionNode::Sign s = a.sign(&context, angleUnit);
    if (s == ExpressionNode::Sign::Positive) {
      // Case 1: the expression is positive real
      return a;;
    } else if (s == ExpressionNode::Sign::Negative) {
      // Case 2: the argument is negative real
      return a.setSign(ExpressionNode::Sign::Positive, &context, angleUnit, target);
    }
  }
  Expression n2 = squareNorm(context, angleUnit, target);
  Expression n =  SquareRoot::Builder(n2);
  n2.shallowReduce(context, angleUnit, target);
  return n;
}

Expression ComplexCartesian::argument(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  if (!b.isRationalZero()) {
    // if b != 0, argument = sign(b) * Pi/2 - arctan(a/b)
    // First, compute arctan(a/b) or (Pi/180)*arctan(a/b)
    Expression divab = Division(a, b.clone());
    Expression arcTangent = ArcTangent::Builder(divab);
    divab.shallowReduce(context, angleUnit, target);
    if (angleUnit == Preferences::AngleUnit::Degree) {
      Expression temp = arcTangent.degreeToRadian();
      arcTangent.shallowReduce(context, angleUnit, target);
      arcTangent = temp;
    }
    // Then, compute sign(b) * Pi/2 - arctan(a/b)
    Expression signb = SignFunction::Builder(b);
    Expression signbPi2 = Multiplication(Rational(1,2), signb, Constant(Ion::Charset::SmallPi));
    signb.shallowReduce(context, angleUnit, target);
    Expression sub = Subtraction(signbPi2, arcTangent);
    signbPi2.shallowReduce(context, angleUnit, target);
    arcTangent.shallowReduce(context, angleUnit, target);
    return sub;
  } else {
    // if b == 0, argument = (1-sign(a))*Pi/2
    Expression signa = SignFunction::Builder(a).shallowReduce(context, angleUnit, target);
    Subtraction sub(Rational(1), signa);
    signa.shallowReduce(context, angleUnit, target);
    Multiplication mul(Rational(1,2), Constant(Ion::Charset::SmallPi), sub);
    sub.shallowReduce(context, angleUnit, target);
    return mul;
  }
}

ComplexCartesian ComplexCartesian::inverse(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  // 1/(a+ib) = a/(a^2+b^2)+i*(-b/(a^2+b^2))
  Expression denominatorReal = clone().convert<ComplexCartesian>().squareNorm(context, angleUnit, target);
  Expression denominatorImag = denominatorReal.clone();
  Expression denominatorRealInv = Power(denominatorReal, Rational(-1));
  denominatorReal.shallowReduce(context, angleUnit, target);
  Expression denominatorImagInv = Power(denominatorImag, Rational(-1));
  denominatorImag.shallowReduce(context, angleUnit, target);
  Multiplication A(a, denominatorRealInv);
  denominatorRealInv.shallowReduce(context, angleUnit, target);
  Expression numeratorImag = Multiplication(Rational(-1), b);
  Multiplication B(numeratorImag, denominatorImagInv);
  numeratorImag.shallowReduce(context, angleUnit, target);
  denominatorImagInv.shallowReduce(context, angleUnit, target);
  ComplexCartesian result(A,B);
  A.shallowReduce(context, angleUnit, target);
  B.shallowReduce(context, angleUnit, target);
  return result;
}

Multiplication ComplexCartesian::squareRootHelper(Expression e, Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  //(1/2)*sqrt(2*e)
  Multiplication doubleE(Rational(2), e);
  e.shallowReduce(context, angleUnit, target);
  Expression sqrt = SquareRoot::Builder(doubleE);
  doubleE.shallowReduce(context, angleUnit, target);
  Multiplication result(Rational(1,2), sqrt);
  sqrt.shallowReduce(context, angleUnit, target);
  return result;
}

ComplexCartesian ComplexCartesian::squareRoot(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  // A: (1/2)*sqrt(2*(sqrt(a^2+b^2)+a))
  // B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))*sign(b)
  Expression normA = clone().convert<ComplexCartesian>().norm(context, angleUnit, target);
  Expression normB = normA.clone();
  // A = (1/2)*sqrt(2*(sqrt(a^2+b^2)+a))
  Addition normAdda(normA, a.clone());
  normA.shallowReduce(context, angleUnit, target);
  Multiplication A = squareRootHelper(normAdda, context, angleUnit, target);
  // B = B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))
  Subtraction normSuba(normB, a);
  normB.shallowReduce(context, angleUnit, target);
  Multiplication B = squareRootHelper(normSuba, context, angleUnit, target);
  // B = B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))*sign(b)
  Expression signb = SignFunction::Builder(b);
  B.addChildAtIndexInPlace(signb, B.numberOfChildren(), B.numberOfChildren());
  signb.shallowReduce(context, angleUnit, target);
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  A.shallowReduce(context, angleUnit, target);
  B.shallowReduce(context, angleUnit, target);
  return result;
}


ComplexCartesian ComplexCartesian::powerInteger(int n, Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  assert(n > 0);
  assert(!b.isRationalZero());

  // Special case: a == 0 (otherwise, we are going to introduce undefined expressions - a^0 = NAN)
  // (b*i)^n = b^n*i^n with i^n == i, -i, 1 or -1
  if (a.isRationalZero()) {
    ComplexCartesian result;
    Expression bpow = Power(b, Rational(n));
    if (n/2%2 == 1) {
      Expression temp = Multiplication(Rational(-1), bpow);
      bpow.shallowReduce(context, angleUnit, target);
      bpow = temp;
    }
    if (n%2 == 0) {
      result = ComplexCartesian(bpow, Rational(0));
    } else {
      result = ComplexCartesian(Rational(0), bpow);
    }
    bpow.shallowReduce(context, angleUnit, target);
    return result;
  }
  // (a+ib) = a^n+i*b*a^(n-1)+(-1)*b^2*a^(n-2)+(-i)*b^3*a^(n-3)+b^3*a^(n-4)+...
  // Real part: A = a^n+(-1)*b^2*a^(n-2)+...
  // Imaginary part: B = b*a^(n-1)
  Addition A;
  Addition B;
  for (int i = 0; i <= n; i++) {
    BinomialCoefficient binom = BinomialCoefficient::Builder(Rational(n), Rational(i));
    Expression aclone = i == n ? a : a.clone();
    Expression bclone = i == n ? b : b.clone();
    Power apow(aclone, Rational(n-i));
    Power bpow(bclone, Rational(i));
    Multiplication m(binom, apow, bpow);
    binom.shallowReduce(context, angleUnit);
    apow.shallowReduce(context, angleUnit, target);
    bpow.shallowReduce(context, angleUnit, target);
    if (i/2%2 == 1) {
      m.addChildAtIndexInPlace(Rational(-1), 0, m.numberOfChildren());
    }
    if (i%2 == 0) {
      A.addChildAtIndexInPlace(m, A.numberOfChildren(), A.numberOfChildren());
    } else {
      B.addChildAtIndexInPlace(m, B.numberOfChildren(), B.numberOfChildren());
    }
    m.shallowReduce(context, angleUnit, target);
  }
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  A.shallowReduce(context, angleUnit, target);
  B.shallowReduce(context, angleUnit, target);
  return result;
}

ComplexCartesian ComplexCartesian::multiply(ComplexCartesian & other, Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  Expression c = other.real();
  Expression d = other.imag();
  // (a+ib) * (c+id) = (ac-bd)+i*(ad+bc)
  // Compute ac-bd
  Expression ac =  Multiplication(a.clone(), c.clone());
  Expression bd =  Multiplication(b.clone(), d.clone());
  Subtraction A(ac, bd);
  ac.shallowReduce(context, angleUnit, target);
  bd.shallowReduce(context, angleUnit, target);
  // Compute ad+bc
  Expression ad =  Multiplication(a, d);
  Expression bc =  Multiplication(b, c);
  Addition B(ad, bc);
  ad.shallowReduce(context, angleUnit, target);
  bc.shallowReduce(context, angleUnit, target);
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  A.shallowReduce(context, angleUnit, target);
  B.shallowReduce(context, angleUnit, target);
  return result;
}

Expression ComplexCartesian::powerHelper(Expression norm, Expression trigo, Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Multiplication m(norm, trigo);
  norm.shallowReduce(context, angleUnit, target);
  trigo.shallowReduce(context, angleUnit, target);
  return m;
}

ComplexCartesian ComplexCartesian::power(ComplexCartesian & other, Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression r = clone().convert<ComplexCartesian>().norm(context, angleUnit, target);
  Expression rclone = r.clone();
  Expression th = argument(context, angleUnit, target);
  Expression thclone = th.clone();
  Expression c = other.real();
  Expression d = other.imag();
  // R = r^c*e^(-th*d)
  Expression rpowc = Power(rclone, c.clone());
  rclone.shallowReduce(context, angleUnit, target);
  Expression thmuld = Multiplication(Rational(-1), thclone, d.clone());
  thclone.shallowReduce(context, angleUnit, target);
  Expression exp = Power(Constant(Ion::Charset::Exponential), thmuld);
  thmuld.shallowReduce(context, angleUnit, target);
  Multiplication norm(rpowc, exp);
  rpowc.shallowReduce(context, angleUnit, target);
  exp.shallowReduce(context, angleUnit, target);

  // TH = d*ln(r)+c*th
  Expression lnr = NaperianLogarithm::Builder(r);
  r.shallowReduce(context, angleUnit, target);
  Multiplication dlnr(d, lnr);
  lnr.shallowReduce(context, angleUnit, target);
  Multiplication thc(th, c);
  th.shallowReduce(context, angleUnit, target);
  Expression argument = Addition(thc, dlnr);
  thc.shallowReduce(context, angleUnit, target);
  dlnr.shallowReduce(context, angleUnit, target);

  if (angleUnit == Preferences::AngleUnit::Degree) {
    Expression temp = argument.radianToDegree();
    argument.shallowReduce(context, angleUnit, target);
    argument = temp;
  }
  // Result = (norm*cos(argument), norm*sin(argument))
  Expression normClone = norm.clone();
  Expression argClone = argument.clone();
  Expression cos = Cosine::Builder(argClone);
  argClone.shallowReduce(context, angleUnit, target);
  Expression normcosarg = powerHelper(normClone, cos, context, angleUnit, target);
  Expression sin = Sine::Builder(argument);
  argument.shallowReduce(context, angleUnit, target);
  Expression normsinarg = powerHelper(norm, sin, context, angleUnit, target);
  ComplexCartesian result = ComplexCartesian::Builder(normcosarg, normsinarg);
  normcosarg.shallowReduce(context, angleUnit, target);
  normsinarg.shallowReduce(context, angleUnit, target);
  return result;
}

}
