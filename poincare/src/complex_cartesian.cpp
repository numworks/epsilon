#include <poincare/complex_cartesian.h>
#include <assert.h>
#include <cmath>
#include <poincare/addition.h>
#include <poincare/arc_tangent.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/cosine.h>
#include <poincare/constant.h>
#include <poincare/division.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/rational.h>
#include <poincare/square_root.h>
#include <poincare/simplification_helper.h>
#include <poincare/sine.h>
#include <poincare/sign_function.h>
#include <poincare/subtraction.h>
#include <poincare/power.h>
#include <poincare/undefined.h>
#include <utility>

namespace Poincare {

TrinaryBoolean ComplexCartesianNode::isNull(Context * context) const {
  TrinaryBoolean realIsNull = childAtIndex(0)->isNull(context);
  TrinaryBoolean imagIsNull = childAtIndex(1)->isNull(context);
  return TrinaryAnd(realIsNull, imagIsNull);
}

Expression ComplexCartesianNode::shallowReduce(const ReductionContext& reductionContext) {
  return ComplexCartesian(this).shallowReduce(reductionContext);
}

Expression ComplexCartesianNode::shallowBeautify(const ReductionContext& reductionContext) {
  return ComplexCartesian(this).shallowBeautify(reductionContext);
}

template<typename T>
Complex<T> ComplexCartesianNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> realEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> imagEvalution = childAtIndex(1)->approximate(T(), approximationContext);
  assert(realEvaluation.type() == EvaluationNode<T>::Type::Complex);
  assert(imagEvalution.type() == EvaluationNode<T>::Type::Complex);
  std::complex<T> a = realEvaluation.complexAtIndex(0);
  std::complex<T> b = imagEvalution.complexAtIndex(0);
  if ((a.imag() != static_cast<T>(0.0) && !std::isnan(a.imag())) || (b.imag() != static_cast<T>(0.0) && !std::isnan(b.imag()))) {
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

Expression ComplexCartesian::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  if (imag().isNull(reductionContext.context()) == TrinaryBoolean::True) {
    Expression r = real();
    replaceWithInPlace(r);
    return r;
  }
  return *this;
}

Expression ComplexCartesian::shallowBeautify(const ReductionContext& reductionContext) {
  Expression a = real();
  Expression b = imag();
  Expression oppositeA = a.makePositiveAnyNegativeNumeralFactor(reductionContext);
  Expression oppositeB = b.makePositiveAnyNegativeNumeralFactor(reductionContext);
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

void ComplexCartesian::factorAndArgumentOfFunction(Expression e, ExpressionNode::Type searchedType, Expression * factor, Expression * argument, const ReductionContext& reductionContext) {
  if (e.type() == searchedType) {
    *factor = Rational::Builder(1);
    *argument = e.childAtIndex(0);
    return;
  }
  if (e.type() == ExpressionNode::Type::Multiplication) {
    int childrenNumber = e.numberOfChildren();
    for (int i = 0; i < childrenNumber; i++) {
      if (e.childAtIndex(i).type() == searchedType) {
        *argument = e.childAtIndex(i).childAtIndex(0);
        *factor = e.clone();
        static_cast<Multiplication *>(factor)->removeChildAtIndexInPlace(i);
        *factor = factor->shallowReduce(reductionContext);
        Expression positiveFactor = factor->makePositiveAnyNegativeNumeralFactor(reductionContext);
        *factor = positiveFactor.isUninitialized() ? *factor : positiveFactor;
        return;
      }
    }
  }
}

Expression ComplexCartesian::squareNorm(const ReductionContext& reductionContext) {
  Expression a = real();
  Expression b = imag();
  Expression aFactor, bFactor, aArgument, bArgument;
  factorAndArgumentOfFunction(a, ExpressionNode::Type::Cosine, &aFactor, &aArgument, reductionContext);
  factorAndArgumentOfFunction(b, ExpressionNode::Type::Sine, &bFactor, &bArgument, reductionContext);
  if (!aFactor.isUninitialized() && !aArgument.isUninitialized() && !bFactor.isUninitialized() && !bArgument.isUninitialized() && aFactor.isIdenticalTo(bFactor) && aArgument.isIdenticalTo(bArgument)) {
    Power result = Power::Builder(aFactor, Rational::Builder(2));
    aFactor.shallowReduce(reductionContext);
    return std::move(result);
  }
  Expression a2 = Power::Builder(a, Rational::Builder(2));
  Expression b2 = Power::Builder(b, Rational::Builder(2));
  Addition add = Addition::Builder(a2, b2);
  a2.shallowReduce(reductionContext);
  b2.shallowReduce(reductionContext);
  return std::move(add);
}

Expression ComplexCartesian::norm(const ReductionContext& reductionContext) {
  Expression a;
  // Special case for pure real or pure imaginary cartesian
  if (imag().isNull(reductionContext.context()) == TrinaryBoolean::True) {
    a = real();
  } else if (real().isNull(reductionContext.context()) == TrinaryBoolean::True) {
    a = imag();
  }
  if (!a.isUninitialized()) {
    // norm = sign(a) * a
    Expression signa = SignFunction::Builder(a.clone());
    Expression norm = Multiplication::Builder(a, signa);
    signa.shallowReduce(reductionContext);
    return norm;
  }
  Expression n2 = squareNorm(reductionContext);
  Expression n =  SquareRoot::Builder(n2);
  n2.shallowReduce(reductionContext);
  return n;
}

Expression ComplexCartesian::argument(const ReductionContext& reductionContext) {
  Expression a = real();
  Expression b = imag();
  if (b.isNull(reductionContext.context()) != TrinaryBoolean::True) {
    // TODO: Handle TrinaryBoolean::Unknown
    // if b != 0, argument = sign(b) * π/2 - atan(a/b)
    // First, compute atan(a/b) or (π/180)*atan(a/b)
    Expression divab = Division::Builder(a, b.clone());
    Expression arcTangent = ArcTangent::Builder(divab);
    divab.shallowReduce(reductionContext);
    if (reductionContext.angleUnit() != Preferences::AngleUnit::Radian) {
      Expression temp = arcTangent.angleUnitToRadian(reductionContext.angleUnit());
      arcTangent.shallowReduce(reductionContext);
      arcTangent = temp;
    }
    // Then, compute sign(b) * π/2 - atan(a/b)
    Expression signb = SignFunction::Builder(b);
    Expression signbPi2 = Multiplication::Builder(Rational::Builder(1,2), signb, Constant::Builder("π"));
    signb.shallowReduce(reductionContext);
    Expression sub = Subtraction::Builder(signbPi2, arcTangent);
    signbPi2.shallowReduce(reductionContext);
    arcTangent.shallowReduce(reductionContext);
    return sub;
  } else {
    // if b == 0, argument = (1-sign(a))*π/2
    Expression signa = SignFunction::Builder(a).shallowReduce(reductionContext);
    Subtraction sub = Subtraction::Builder(Rational::Builder(1), signa);
    signa.shallowReduce(reductionContext);
    Multiplication mul = Multiplication::Builder(Rational::Builder(1,2), Constant::Builder("π"), sub);
    sub.shallowReduce(reductionContext);
    return std::move(mul);
  }
}

ComplexCartesian ComplexCartesian::inverse(const ReductionContext& reductionContext) {
  Expression a = real();
  Expression b = imag();
  // 1/(a+ib) = a/(a^2+b^2)+i*(-b/(a^2+b^2))
  Expression denominatorReal = clone().convert<ComplexCartesian>().squareNorm(reductionContext);
  Expression denominatorImag = denominatorReal.clone();
  Expression denominatorRealInv = Power::Builder(denominatorReal, Rational::Builder(-1));
  denominatorReal.shallowReduce(reductionContext);
  Expression denominatorImagInv = Power::Builder(denominatorImag, Rational::Builder(-1));
  denominatorImag.shallowReduce(reductionContext);
  Multiplication A = Multiplication::Builder(a, denominatorRealInv);
  denominatorRealInv.shallowReduce(reductionContext);
  Expression numeratorImag = Multiplication::Builder(Rational::Builder(-1), b);
  Multiplication B = Multiplication::Builder(numeratorImag, denominatorImagInv);
  numeratorImag.shallowReduce(reductionContext);
  denominatorImagInv.shallowReduce(reductionContext);
  ComplexCartesian result = ComplexCartesian::Builder(A,B);
  A.shallowReduce(reductionContext);
  B.shallowReduce(reductionContext);
  return result.interruptComputationIfManyNodes();
}

Multiplication ComplexCartesian::squareRootHelper(Expression e, const ReductionContext& reductionContext) {
  //(1/2)*sqrt(2*e)
  Multiplication doubleE = Multiplication::Builder(Rational::Builder(2), e);
  e.shallowReduce(reductionContext);
  Expression sqrt = SquareRoot::Builder(doubleE);
  doubleE.shallowReduce(reductionContext);
  Multiplication result = Multiplication::Builder(Rational::Builder(1,2), sqrt);
  sqrt.shallowReduce(reductionContext);
  return result;
}

ComplexCartesian ComplexCartesian::squareRoot(const ReductionContext& reductionContext) {
  Expression a = real();
  Expression b = imag();
  // A: (1/2)*sqrt(2*(sqrt(a^2+b^2)+a))
  // B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))*sign(b)
  Expression normA = clone().convert<ComplexCartesian>().norm(reductionContext);
  Expression normB = normA.clone();
  // A = (1/2)*sqrt(2*(sqrt(a^2+b^2)+a))
  Addition normAdda = Addition::Builder(normA, a.clone());
  normA.shallowReduce(reductionContext);
  Multiplication A = squareRootHelper(normAdda, reductionContext);
  // B = B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))
  Subtraction normSuba = Subtraction::Builder(normB, a);
  normB.shallowReduce(reductionContext);
  Multiplication B = squareRootHelper(normSuba, reductionContext);
  // B = B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))*sign(b)
  Expression signb = SignFunction::Builder(b);
  B.addChildAtIndexInPlace(signb, B.numberOfChildren(), B.numberOfChildren());
  signb.shallowReduce(reductionContext);
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  A.shallowReduce(reductionContext);
  B.shallowReduce(reductionContext);
  return result.interruptComputationIfManyNodes();
}


ComplexCartesian ComplexCartesian::powerInteger(int n, const ReductionContext& reductionContext) {
  Expression a = real();
  Expression b = imag();
  assert(n > 0);
  assert(b.isNull(reductionContext.context()) != TrinaryBoolean::True);

  // Special case: a == 0 (otherwise, we are going to introduce undefined expressions - a^0 = NAN)
  // (b*i)^n = b^n*i^n with i^n == i, -i, 1 or -1
  if (a.isNull(reductionContext.context()) == TrinaryBoolean::True) {
    ComplexCartesian result;
    Expression bpow = Power::Builder(b, Rational::Builder(n));
    if (n/2%2 == 1) {
      Expression temp = Multiplication::Builder(Rational::Builder(-1), bpow);
      bpow.shallowReduce(reductionContext);
      bpow = temp;
    }
    if (n%2 == 0) {
      result = ComplexCartesian::Builder(bpow, Rational::Builder(0));
    } else {
      result = ComplexCartesian::Builder(Rational::Builder(0), bpow);
    }
    bpow.shallowReduce(reductionContext);
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
    binom.shallowReduce(reductionContext);
    apow.shallowReduce(reductionContext);
    bpow.shallowReduce(reductionContext);
    if (i/2%2 == 1) {
      m.addChildAtIndexInPlace(Rational::Builder(-1), 0, m.numberOfChildren());
    }
    if (i%2 == 0) {
      A.addChildAtIndexInPlace(m, A.numberOfChildren(), A.numberOfChildren());
    } else {
      B.addChildAtIndexInPlace(m, B.numberOfChildren(), B.numberOfChildren());
    }
    m.shallowReduce(reductionContext);
    result = result.interruptComputationIfManyNodes();
    if (result.real().isUndefined()) {
      return result;
    }
  }
  A.shallowReduce(reductionContext);
  B.shallowReduce(reductionContext);
  return result;
}

ComplexCartesian ComplexCartesian::multiply(ComplexCartesian & other, const ReductionContext& reductionContext) {
  Expression a = real();
  Expression b = imag();
  Expression c = other.real();
  Expression d = other.imag();
  // (a+ib) * (c+id) = (ac-bd)+i*(ad+bc)
  // Compute ac-bd
  Expression ac =  Multiplication::Builder(a.clone(), c.clone());
  Expression bd =  Multiplication::Builder(b.clone(), d.clone());
  Subtraction A = Subtraction::Builder(ac, bd);
  ac.shallowReduce(reductionContext);
  bd.shallowReduce(reductionContext);
  // Compute ad+bc
  Expression ad =  Multiplication::Builder(a, d);
  Expression bc =  Multiplication::Builder(b, c);
  Addition B = Addition::Builder(ad, bc);
  ad.shallowReduce(reductionContext);
  bc.shallowReduce(reductionContext);
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  A.shallowReduce(reductionContext);
  B.shallowReduce(reductionContext);
  return result.interruptComputationIfManyNodes();
}

Expression ComplexCartesian::powerHelper(Expression norm, Expression trigo, const ReductionContext& reductionContext) {
  Multiplication m = Multiplication::Builder(norm, trigo);
  norm.shallowReduce(reductionContext);
  trigo.shallowReduce(reductionContext);
  return std::move(m);
}

ComplexCartesian ComplexCartesian::power(ComplexCartesian & other, const ReductionContext& reductionContext) {
  Expression r = clone().convert<ComplexCartesian>().norm(reductionContext);
  Expression rclone = r.clone();
  Expression th = argument(reductionContext);
  Expression thclone = th.clone();
  Expression c = other.real();
  Expression d = other.imag();
  // R = r^c*e^(-th*d)
  Expression rpowc = Power::Builder(rclone, c.clone());
  rclone.shallowReduce(reductionContext);
  Expression thmuld = Multiplication::Builder(Rational::Builder(-1), thclone, d.clone());
  thclone.shallowReduce(reductionContext);
  Expression exp = Power::Builder(Constant::Builder("e"), thmuld);
  thmuld.shallowReduce(reductionContext);
  Multiplication norm = Multiplication::Builder(rpowc, exp);
  rpowc.shallowReduce(reductionContext);
  exp.shallowReduce(reductionContext);

  // TH = d*ln(r)+c*th
  Expression lnr = NaperianLogarithm::Builder(r);
  r.shallowReduce(reductionContext);
  Multiplication dlnr = Multiplication::Builder(d, lnr);
  lnr.shallowReduce(reductionContext);
  Multiplication thc = Multiplication::Builder(th, c);
  th.shallowReduce(reductionContext);
  Expression argument = Addition::Builder(thc, dlnr);
  thc.shallowReduce(reductionContext);
  dlnr.shallowReduce(reductionContext);

  if (reductionContext.angleUnit() != Preferences::AngleUnit::Radian) {
    Expression temp = argument.radianToAngleUnit(reductionContext.angleUnit());
    argument.shallowReduce(reductionContext);
    argument = temp;
  }
  // Result = (norm*cos(argument), norm*sin(argument))
  Expression normClone = norm.clone();
  Expression argClone = argument.clone();
  Expression cos = Cosine::Builder(argClone);
  argClone.shallowReduce(reductionContext);
  Expression normcosarg = powerHelper(normClone, cos, reductionContext);
  Expression sin = Sine::Builder(argument);
  argument.shallowReduce(reductionContext);
  Expression normsinarg = powerHelper(norm, sin, reductionContext);
  ComplexCartesian result = ComplexCartesian::Builder(normcosarg, normsinarg);
  normcosarg.shallowReduce(reductionContext);
  normsinarg.shallowReduce(reductionContext);
  return result.interruptComputationIfManyNodes();
}

ComplexCartesian ComplexCartesian::interruptComputationIfManyNodes() {
  if (numberOfDescendants(true) > k_maxNumberOfNodesBeforeInterrupting) {
    ExceptionCheckpoint::Raise();
    return ComplexCartesian::Builder(Undefined::Builder(), Undefined::Builder());
  }
  return *this;
}

}
