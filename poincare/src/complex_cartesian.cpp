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
#include <poincare/undefined.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

Expression ComplexCartesianNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return ComplexCartesian(this).shallowReduce();
}

Expression ComplexCartesianNode::shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return ComplexCartesian(this).shallowBeautify(context, complexFormat, angleUnit, target);
}

template<typename T>
Complex<T> ComplexCartesianNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> realEvaluation = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> imagEvalution = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  assert(realEvaluation.type() == EvaluationNode<T>::Type::Complex);
  assert(imagEvalution.type() == EvaluationNode<T>::Type::Complex);
  std::complex<T> a = static_cast<Complex<T> &>(realEvaluation).stdComplex();
  std::complex<T> b = static_cast<Complex<T> &>(imagEvalution).stdComplex();
  if ((a.imag() != 0.0 && !std::isnan(a.imag())) || (b.imag() != 0.0 && !std::isnan(b.imag()))) {
    /* a and b are supposed to be real (if they are not undefined). However,
     * due to double precision limit, the approximation of the real part or the
     * imaginary part can lead to complex values.
     * For instance, let the real part be
     * sqrt(2*sqrt(5E23+1)-1E12*sqrt(2)) ~ 1.1892E-6. Due to std::sqrt(2.0)
     * unprecision, 2*sqrt(5E23+1)-1E12*sqrt(2) < 0 which leads to
     * sqrt(2*sqrt(5E23+1)-1E12*sqrt(2)) being a complex number.
     * In this case, we return an undefined complex because the approximation
     * is very likely to be false. */
    return Complex<T>::Undefined();
  }
  assert(a.imag() == 0.0 || std::isnan(a.imag()));
  assert(b.imag() == 0.0 || std::isnan(b.imag()));
  return Complex<T>::Builder(a.real(), b.real());
}

Expression ComplexCartesian::shallowReduce() {
  if (imag().isRationalZero()) {
    Expression r = real();
    replaceWithInPlace(r);
    return r;
  }
  return *this;
}

Expression ComplexCartesian::shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  Expression oppositeA = a.makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, target);
  Expression oppositeB = b.makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, target);
  a = oppositeA.isUninitialized() ? a : oppositeA;
  b = oppositeB.isUninitialized() ? b : oppositeB;
  Expression e = Expression::CreateComplexExpression(a, b, Preferences::ComplexFormat::Cartesian,
      a.isUndefined() || b.isUndefined(),
      Expression::IsZero(a), Expression::IsOne(a), Expression::IsZero(b), Expression::IsOne(b),
      !oppositeA.isUninitialized(),
      !oppositeB.isUninitialized()
    );
  replaceWithInPlace(e);
  return e;
}

void ComplexCartesian::factorAndArgumentOfFunction(Expression e, ExpressionNode::Type searchedType, Expression * factor, Expression * argument, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  if (e.type() == searchedType) {
    *factor = Rational::Builder(1);
    *argument = e.childAtIndex(0);
    return;
  }
  if (e.type() == ExpressionNode::Type::Multiplication) {
    for (int i = 0; i < e.numberOfChildren(); i++) {
      if (e.childAtIndex(i).type() == searchedType) {
        *argument = e.childAtIndex(i).childAtIndex(0);
        *factor = e.clone();
        static_cast<Multiplication *>(factor)->removeChildAtIndexInPlace(i);
        *factor = factor->shallowReduce(context, complexFormat, angleUnit, target);
        Expression positiveFactor = factor->makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, target);
        *factor = positiveFactor.isUninitialized() ? *factor : positiveFactor;
        return;
      }
    }
  }
}

Expression ComplexCartesian::squareNorm(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  Expression aFactor, bFactor, aArgument, bArgument;
  factorAndArgumentOfFunction(a, ExpressionNode::Type::Cosine, &aFactor, &aArgument, context, complexFormat, angleUnit, target);
  factorAndArgumentOfFunction(b, ExpressionNode::Type::Sine, &bFactor, &bArgument, context, complexFormat, angleUnit, target);
  if (!aFactor.isUninitialized() && !aArgument.isUninitialized() && !bFactor.isUninitialized() && !bArgument.isUninitialized() && aFactor.isIdenticalTo(bFactor) && aArgument.isIdenticalTo(bArgument)) {
    Power result = Power::Builder(aFactor, Rational::Builder(2));
    aFactor.shallowReduce(context, complexFormat, angleUnit, target);
    return result;
  }
  Expression a2 = Power::Builder(a, Rational::Builder(2));
  Expression b2 = Power::Builder(b, Rational::Builder(2));
  Addition add = Addition::Builder(a2, b2);
  a2.shallowReduce(context, complexFormat, angleUnit, target);
  b2.shallowReduce(context, complexFormat, angleUnit, target);
  return add;
}

Expression ComplexCartesian::norm(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  if (imag().isRationalZero()) {
    Expression a = real();
    ExpressionNode::Sign s = a.sign(&context);
    if (s == ExpressionNode::Sign::Positive) {
      // Case 1: the expression is positive real
      return a;;
    } else if (s == ExpressionNode::Sign::Negative) {
      // Case 2: the argument is negative real
      return a.setSign(ExpressionNode::Sign::Positive, &context, complexFormat, angleUnit, target);
    }
  }
  Expression n2 = squareNorm(context, complexFormat, angleUnit, target);
  Expression n =  SquareRoot::Builder(n2);
  n2.shallowReduce(context, complexFormat, angleUnit, target);
  return n;
}

Expression ComplexCartesian::argument(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target, bool symbolicComputation) {
  Expression a = real();
  Expression b = imag();
  if (!b.isRationalZero()) {
    // if b != 0, argument = sign(b) * π/2 - atan(a/b)
    // First, compute atan(a/b) or (π/180)*atan(a/b)
    Expression divab = Division::Builder(a, b.clone());
    Expression arcTangent = ArcTangent::Builder(divab);
    divab.shallowReduce(context, complexFormat, angleUnit, target);
    if (angleUnit == Preferences::AngleUnit::Degree) {
      Expression temp = arcTangent.degreeToRadian();
      arcTangent.shallowReduce(context, complexFormat, angleUnit, target);
      arcTangent = temp;
    }
    // Then, compute sign(b) * π/2 - atan(a/b)
    Expression signb = SignFunction::Builder(b);
    Expression signbPi2 = Multiplication::Builder(Rational::Builder(1,2), signb, Constant::Builder(UCodePointGreekSmallLetterPi));
    signb.shallowReduce(context, complexFormat, angleUnit, target);
    Expression sub = Subtraction::Builder(signbPi2, arcTangent);
    signbPi2.shallowReduce(context, complexFormat, angleUnit, target);
    arcTangent.shallowReduce(context, complexFormat, angleUnit, target);
    return sub;
  } else {
    // if b == 0, argument = (1-sign(a))*π/2
    Expression signa = SignFunction::Builder(a).shallowReduce(context, complexFormat, angleUnit, target, symbolicComputation);
    Subtraction sub = Subtraction::Builder(Rational::Builder(1), signa);
    signa.shallowReduce(context, complexFormat, angleUnit, target);
    Multiplication mul = Multiplication::Builder(Rational::Builder(1,2), Constant::Builder(UCodePointGreekSmallLetterPi), sub);
    sub.shallowReduce(context, complexFormat, angleUnit, target);
    return mul;
  }
}

ComplexCartesian ComplexCartesian::inverse(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  // 1/(a+ib) = a/(a^2+b^2)+i*(-b/(a^2+b^2))
  Expression denominatorReal = clone().convert<ComplexCartesian>().squareNorm(context, complexFormat, angleUnit, target);
  Expression denominatorImag = denominatorReal.clone();
  Expression denominatorRealInv = Power::Builder(denominatorReal, Rational::Builder(-1));
  denominatorReal.shallowReduce(context, complexFormat, angleUnit, target);
  Expression denominatorImagInv = Power::Builder(denominatorImag, Rational::Builder(-1));
  denominatorImag.shallowReduce(context, complexFormat, angleUnit, target);
  Multiplication A = Multiplication::Builder(a, denominatorRealInv);
  denominatorRealInv.shallowReduce(context, complexFormat, angleUnit, target);
  Expression numeratorImag = Multiplication::Builder(Rational::Builder(-1), b);
  Multiplication B = Multiplication::Builder(numeratorImag, denominatorImagInv);
  numeratorImag.shallowReduce(context, complexFormat, angleUnit, target);
  denominatorImagInv.shallowReduce(context, complexFormat, angleUnit, target);
  ComplexCartesian result = ComplexCartesian::Builder(A,B);
  A.shallowReduce(context, complexFormat, angleUnit, target);
  B.shallowReduce(context, complexFormat, angleUnit, target);
  return result.interruptComputationIfManyNodes();
}

Multiplication ComplexCartesian::squareRootHelper(Expression e, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  //(1/2)*sqrt(2*e)
  Multiplication doubleE = Multiplication::Builder(Rational::Builder(2), e);
  e.shallowReduce(context, complexFormat, angleUnit, target);
  Expression sqrt = SquareRoot::Builder(doubleE);
  doubleE.shallowReduce(context, complexFormat, angleUnit, target);
  Multiplication result = Multiplication::Builder(Rational::Builder(1,2), sqrt);
  sqrt.shallowReduce(context, complexFormat, angleUnit, target);
  return result;
}

ComplexCartesian ComplexCartesian::squareRoot(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  // A: (1/2)*sqrt(2*(sqrt(a^2+b^2)+a))
  // B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))*sign(b)
  Expression normA = clone().convert<ComplexCartesian>().norm(context, complexFormat, angleUnit, target);
  Expression normB = normA.clone();
  // A = (1/2)*sqrt(2*(sqrt(a^2+b^2)+a))
  Addition normAdda = Addition::Builder(normA, a.clone());
  normA.shallowReduce(context, complexFormat, angleUnit, target);
  Multiplication A = squareRootHelper(normAdda, context, complexFormat, angleUnit, target);
  // B = B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))
  Subtraction normSuba = Subtraction::Builder(normB, a);
  normB.shallowReduce(context, complexFormat, angleUnit, target);
  Multiplication B = squareRootHelper(normSuba, context, complexFormat, angleUnit, target);
  // B = B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))*sign(b)
  Expression signb = SignFunction::Builder(b);
  B.addChildAtIndexInPlace(signb, B.numberOfChildren(), B.numberOfChildren());
  signb.shallowReduce(context, complexFormat, angleUnit, target);
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  A.shallowReduce(context, complexFormat, angleUnit, target);
  B.shallowReduce(context, complexFormat, angleUnit, target);
  return result.interruptComputationIfManyNodes();
}


ComplexCartesian ComplexCartesian::powerInteger(int n, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target, bool symbolicComputation) {
  Expression a = real();
  Expression b = imag();
  assert(n > 0);
  assert(!b.isRationalZero());

  // Special case: a == 0 (otherwise, we are going to introduce undefined expressions - a^0 = NAN)
  // (b*i)^n = b^n*i^n with i^n == i, -i, 1 or -1
  if (a.isRationalZero()) {
    ComplexCartesian result;
    Expression bpow = Power::Builder(b, Rational::Builder(n));
    if (n/2%2 == 1) {
      Expression temp = Multiplication::Builder(Rational::Builder(-1), bpow);
      bpow.shallowReduce(context, complexFormat, angleUnit, target);
      bpow = temp;
    }
    if (n%2 == 0) {
      result = ComplexCartesian::Builder(bpow, Rational::Builder(0));
    } else {
      result = ComplexCartesian::Builder(Rational::Builder(0), bpow);
    }
    bpow.shallowReduce(context, complexFormat, angleUnit, target);
    return result;
  }
  // (a+ib) = a^n+i*b*a^(n-1)+(-1)*b^2*a^(n-2)+(-i)*b^3*a^(n-3)+b^3*a^(n-4)+...
  // Real part: A = a^n+(-1)*b^2*a^(n-2)+...
  // Imaginary part: B = b*a^(n-1)
  Addition A = Addition::Builder();
  Addition B = Addition::Builder();
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  for (int i = 0; i <= n; i++) {
    BinomialCoefficient binom = BinomialCoefficient::Builder(Rational::Builder(n), Rational::Builder(i));
    Expression aclone = i == n ? a : a.clone();
    Expression bclone = i == n ? b : b.clone();
    Power apow = Power::Builder(aclone, Rational::Builder(n-i));
    Power bpow = Power::Builder(bclone, Rational::Builder(i));
    Multiplication m = Multiplication::Builder(binom, apow, bpow);
    binom.shallowReduce(context);
    apow.shallowReduce(context, complexFormat, angleUnit, target, symbolicComputation);
    bpow.shallowReduce(context, complexFormat, angleUnit, target, symbolicComputation);
    if (i/2%2 == 1) {
      m.addChildAtIndexInPlace(Rational::Builder(-1), 0, m.numberOfChildren());
    }
    if (i%2 == 0) {
      A.addChildAtIndexInPlace(m, A.numberOfChildren(), A.numberOfChildren());
    } else {
      B.addChildAtIndexInPlace(m, B.numberOfChildren(), B.numberOfChildren());
    }
    m.shallowReduce(context, complexFormat, angleUnit, target);
    result = result.interruptComputationIfManyNodes();
    if (result.real().isUndefined()) {
      return result;
    }
  }
  A.shallowReduce(context, complexFormat, angleUnit, target);
  B.shallowReduce(context, complexFormat, angleUnit, target);
  return result;
}

ComplexCartesian ComplexCartesian::multiply(ComplexCartesian & other, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression a = real();
  Expression b = imag();
  Expression c = other.real();
  Expression d = other.imag();
  // (a+ib) * (c+id) = (ac-bd)+i*(ad+bc)
  // Compute ac-bd
  Expression ac =  Multiplication::Builder(a.clone(), c.clone());
  Expression bd =  Multiplication::Builder(b.clone(), d.clone());
  Subtraction A = Subtraction::Builder(ac, bd);
  ac.shallowReduce(context, complexFormat, angleUnit, target);
  bd.shallowReduce(context, complexFormat, angleUnit, target);
  // Compute ad+bc
  Expression ad =  Multiplication::Builder(a, d);
  Expression bc =  Multiplication::Builder(b, c);
  Addition B = Addition::Builder(ad, bc);
  ad.shallowReduce(context, complexFormat, angleUnit, target);
  bc.shallowReduce(context, complexFormat, angleUnit, target);
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  A.shallowReduce(context, complexFormat, angleUnit, target);
  B.shallowReduce(context, complexFormat, angleUnit, target);
  return result.interruptComputationIfManyNodes();
}

Expression ComplexCartesian::powerHelper(Expression norm, Expression trigo, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Multiplication m = Multiplication::Builder(norm, trigo);
  norm.shallowReduce(context, complexFormat, angleUnit, target);
  trigo.shallowReduce(context, complexFormat, angleUnit, target);
  return m;
}

ComplexCartesian ComplexCartesian::power(ComplexCartesian & other, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target, bool symbolicComputation) {
  Expression r = clone().convert<ComplexCartesian>().norm(context, complexFormat, angleUnit, target);
  Expression rclone = r.clone();
  Expression th = argument(context, complexFormat, angleUnit, target, symbolicComputation);
  Expression thclone = th.clone();
  Expression c = other.real();
  Expression d = other.imag();
  // R = r^c*e^(-th*d)
  Expression rpowc = Power::Builder(rclone, c.clone());
  rclone.shallowReduce(context, complexFormat, angleUnit, target);
  Expression thmuld = Multiplication::Builder(Rational::Builder(-1), thclone, d.clone());
  thclone.shallowReduce(context, complexFormat, angleUnit, target);
  Expression exp = Power::Builder(Constant::Builder(UCodePointScriptSmallE), thmuld);
  thmuld.shallowReduce(context, complexFormat, angleUnit, target);
  Multiplication norm = Multiplication::Builder(rpowc, exp);
  rpowc.shallowReduce(context, complexFormat, angleUnit, target);
  exp.shallowReduce(context, complexFormat, angleUnit, target);

  // TH = d*ln(r)+c*th
  Expression lnr = NaperianLogarithm::Builder(r);
  r.shallowReduce(context, complexFormat, angleUnit, target);
  Multiplication dlnr = Multiplication::Builder(d, lnr);
  lnr.shallowReduce(context, complexFormat, angleUnit, target);
  Multiplication thc = Multiplication::Builder(th, c);
  th.shallowReduce(context, complexFormat, angleUnit, target);
  Expression argument = Addition::Builder(thc, dlnr);
  thc.shallowReduce(context, complexFormat, angleUnit, target);
  dlnr.shallowReduce(context, complexFormat, angleUnit, target);

  if (angleUnit == Preferences::AngleUnit::Degree) {
    Expression temp = argument.radianToDegree();
    argument.shallowReduce(context, complexFormat, angleUnit, target);
    argument = temp;
  }
  // Result = (norm*cos(argument), norm*sin(argument))
  Expression normClone = norm.clone();
  Expression argClone = argument.clone();
  Expression cos = Cosine::Builder(argClone);
  argClone.shallowReduce(context, complexFormat, angleUnit, target);
  Expression normcosarg = powerHelper(normClone, cos, context, complexFormat, angleUnit, target);
  Expression sin = Sine::Builder(argument);
  argument.shallowReduce(context, complexFormat, angleUnit, target);
  Expression normsinarg = powerHelper(norm, sin, context, complexFormat, angleUnit, target);
  ComplexCartesian result = ComplexCartesian::Builder(normcosarg, normsinarg);
  normcosarg.shallowReduce(context, complexFormat, angleUnit, target);
  normsinarg.shallowReduce(context, complexFormat, angleUnit, target);
  return result.interruptComputationIfManyNodes();
}

ComplexCartesian ComplexCartesian::interruptComputationIfManyNodes() {
  if (numberOfDescendants(true) > k_maxNumberOfNodesBeforeInterrupting) {
    Expression::SetInterruption(true);
    return ComplexCartesian::Builder(Undefined::Builder(), Undefined::Builder());
  }
  return *this;
}

}
