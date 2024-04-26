#include <assert.h>
#include <poincare/old/addition.h>
#include <poincare/old/arc_tangent.h>
#include <poincare/old/binomial_coefficient.h>
#include <poincare/old/complex_cartesian.h>
#include <poincare/old/constant.h>
#include <poincare/old/cosine.h>
#include <poincare/old/division.h>
#include <poincare/old/exception_checkpoint.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/naperian_logarithm.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/sign_function.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/sine.h>
#include <poincare/old/square_root.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/undefined.h>

#include <cmath>
#include <utility>

namespace Poincare {

TrinaryBoolean ComplexCartesianNode::isNull(Context* context) const {
  TrinaryBoolean realIsNull = childAtIndex(0)->isNull(context);
  TrinaryBoolean imagIsNull = childAtIndex(1)->isNull(context);
  return TrinaryAnd(realIsNull, imagIsNull);
}

OExpression ComplexCartesianNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ComplexCartesian(this).shallowReduce(reductionContext);
}

OExpression ComplexCartesianNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return ComplexCartesian(this).shallowBeautify(reductionContext);
}

template <typename T>
Complex<T> ComplexCartesianNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Evaluation<T> realEvaluation =
      childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> imagEvalution =
      childAtIndex(1)->approximate(T(), approximationContext);
  assert(realEvaluation.otype() == EvaluationNode<T>::Type::Complex);
  assert(imagEvalution.otype() == EvaluationNode<T>::Type::Complex);
  std::complex<T> a = realEvaluation.complexAtIndex(0);
  std::complex<T> b = imagEvalution.complexAtIndex(0);
  if ((a.imag() != static_cast<T>(0.0) && !std::isnan(a.imag())) ||
      (b.imag() != static_cast<T>(0.0) && !std::isnan(b.imag()))) {
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

OExpression ComplexCartesian::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  if (imag().isNull(reductionContext.context()) == TrinaryBoolean::True) {
    OExpression r = real();
    replaceWithInPlace(r);
    return r;
  }
  return *this;
}

OExpression ComplexCartesian::shallowBeautify(
    const ReductionContext& reductionContext) {
  OExpression a = real();
  OExpression b = imag();
  OExpression oppositeA =
      a.makePositiveAnyNegativeNumeralFactor(reductionContext);
  OExpression oppositeB =
      b.makePositiveAnyNegativeNumeralFactor(reductionContext);
  bool aIsPositive = oppositeA.isUninitialized();
  bool bIsPositive = oppositeB.isUninitialized();
  a = aIsPositive ? a : oppositeA;
  b = bIsPositive ? b : oppositeB;
  OExpression e = OExpression::CreateComplexExpression(
      a, b, Preferences::ComplexFormat::Cartesian, !aIsPositive, !bIsPositive);
  replaceWithInPlace(e);
  return e;
}

void ComplexCartesian::factorAndArgumentOfFunction(
    OExpression e, ExpressionNode::Type searchedType, OExpression* factor,
    OExpression* argument, const ReductionContext& reductionContext) {
  if (e.otype() == searchedType) {
    *factor = Rational::Builder(1);
    *argument = e.childAtIndex(0);
    return;
  }
  if (e.otype() == ExpressionNode::Type::Multiplication) {
    int childrenNumber = e.numberOfChildren();
    for (int i = 0; i < childrenNumber; i++) {
      if (e.childAtIndex(i).otype() == searchedType) {
        *argument = e.childAtIndex(i).childAtIndex(0);
        *factor = e.clone();
        static_cast<Multiplication*>(factor)->removeChildAtIndexInPlace(i);
        *factor = factor->shallowReduce(reductionContext);
        OExpression positiveFactor =
            factor->makePositiveAnyNegativeNumeralFactor(reductionContext);
        *factor = positiveFactor.isUninitialized() ? *factor : positiveFactor;
        return;
      }
    }
  }
}

OExpression ComplexCartesian::squareNorm(
    const ReductionContext& reductionContext) {
  OExpression a = real();
  OExpression b = imag();
  OExpression aFactor, bFactor, aArgument, bArgument;
  factorAndArgumentOfFunction(a, ExpressionNode::Type::Cosine, &aFactor,
                              &aArgument, reductionContext);
  factorAndArgumentOfFunction(b, ExpressionNode::Type::Sine, &bFactor,
                              &bArgument, reductionContext);
  if (!aFactor.isUninitialized() && !aArgument.isUninitialized() &&
      !bFactor.isUninitialized() && !bArgument.isUninitialized() &&
      aFactor.isIdenticalTo(bFactor) && aArgument.isIdenticalTo(bArgument)) {
    Power result = Power::Builder(aFactor, Rational::Builder(2));
    aFactor.shallowReduce(reductionContext);
    return std::move(result);
  }
  OExpression a2 = Power::Builder(a, Rational::Builder(2));
  OExpression b2 = Power::Builder(b, Rational::Builder(2));
  Addition add = Addition::Builder(a2, b2);
  a2.shallowReduce(reductionContext);
  b2.shallowReduce(reductionContext);
  return std::move(add);
}

OExpression ComplexCartesian::norm(const ReductionContext& reductionContext) {
  OExpression a;
  // Special case for pure real or pure imaginary cartesian
  if (imag().isNull(reductionContext.context()) == TrinaryBoolean::True) {
    a = real();
  } else if (real().isNull(reductionContext.context()) ==
             TrinaryBoolean::True) {
    a = imag();
  }
  if (!a.isUninitialized()) {
    // norm = sign(a) * a
    OExpression signa = SignFunction::Builder(a.clone());
    OExpression norm = Multiplication::Builder(a, signa);
    signa.shallowReduce(reductionContext);
    return norm;
  }
  OExpression n2 = squareNorm(reductionContext);
  OExpression n = SquareRoot::Builder(n2);
  n2.shallowReduce(reductionContext);
  return n;
}

OExpression ComplexCartesian::argument(
    const ReductionContext& reductionContext) {
  OExpression a = real();
  OExpression b = imag();
  if (b.isNull(reductionContext.context()) != TrinaryBoolean::True) {
    /* TODO: Handle TrinaryBoolean::Unknown
     * if b != 0, argument = sign(b) * π/2 - atan(a/b)
     * First, compute atan(a/b) or (π/180)*atan(a/b) */
    OExpression divab = Division::Builder(a, b.clone());
    OExpression arcTangent = ArcTangent::Builder(divab);
    divab.shallowReduce(reductionContext);
    if (reductionContext.angleUnit() != Preferences::AngleUnit::Radian) {
      OExpression temp =
          arcTangent.angleUnitToRadian(reductionContext.angleUnit());
      arcTangent.shallowReduce(reductionContext);
      arcTangent = temp;
    }
    // Then, compute sign(b) * π/2 - atan(a/b)
    OExpression signb = SignFunction::Builder(b);
    OExpression signbPi2 = Multiplication::Builder(
        Rational::Builder(1, 2), signb, Constant::PiBuilder());
    signb.shallowReduce(reductionContext);
    OExpression sub = Subtraction::Builder(signbPi2, arcTangent);
    signbPi2.shallowReduce(reductionContext);
    arcTangent.shallowReduce(reductionContext);
    return sub;
  } else {
    // if b == 0, argument = (1-sign(a))*π/2
    OExpression signa =
        SignFunction::Builder(a).shallowReduce(reductionContext);
    Subtraction sub = Subtraction::Builder(Rational::Builder(1), signa);
    signa.shallowReduce(reductionContext);
    Multiplication mul = Multiplication::Builder(Rational::Builder(1, 2),
                                                 Constant::PiBuilder(), sub);
    sub.shallowReduce(reductionContext);
    return std::move(mul);
  }
}

ComplexCartesian ComplexCartesian::inverse(
    const ReductionContext& reductionContext) {
  OExpression a = real();
  OExpression b = imag();
  // 1/(a+ib) = a/(a^2+b^2)+i*(-b/(a^2+b^2))
  OExpression denominatorReal =
      clone().convert<ComplexCartesian>().squareNorm(reductionContext);
  OExpression denominatorImag = denominatorReal.clone();
  OExpression denominatorRealInv =
      Power::Builder(denominatorReal, Rational::Builder(-1));
  denominatorReal.shallowReduce(reductionContext);
  OExpression denominatorImagInv =
      Power::Builder(denominatorImag, Rational::Builder(-1));
  denominatorImag.shallowReduce(reductionContext);
  Multiplication A = Multiplication::Builder(a, denominatorRealInv);
  denominatorRealInv.shallowReduce(reductionContext);
  OExpression numeratorImag = Multiplication::Builder(Rational::Builder(-1), b);
  Multiplication B = Multiplication::Builder(numeratorImag, denominatorImagInv);
  numeratorImag.shallowReduce(reductionContext);
  denominatorImagInv.shallowReduce(reductionContext);
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  A.shallowReduce(reductionContext);
  B.shallowReduce(reductionContext);
  return result.interruptComputationIfManyNodes();
}

Multiplication ComplexCartesian::squareRootHelper(
    OExpression e, const ReductionContext& reductionContext) {
  //(1/2)*sqrt(2*e)
  Multiplication doubleE = Multiplication::Builder(Rational::Builder(2), e);
  e.shallowReduce(reductionContext);
  OExpression sqrt = SquareRoot::Builder(doubleE);
  doubleE.shallowReduce(reductionContext);
  Multiplication result =
      Multiplication::Builder(Rational::Builder(1, 2), sqrt);
  sqrt.shallowReduce(reductionContext);
  return result;
}

ComplexCartesian ComplexCartesian::squareRoot(
    const ReductionContext& reductionContext) {
  OExpression a = real();
  OExpression b = imag();
  /* A: (1/2)*sqrt(2*(sqrt(a^2+b^2)+a))
   * B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))*sign(b) */
  OExpression normA =
      clone().convert<ComplexCartesian>().norm(reductionContext);
  OExpression normB = normA.clone();
  // A = (1/2)*sqrt(2*(sqrt(a^2+b^2)+a))
  Addition normAdda = Addition::Builder(normA, a.clone());
  normA.shallowReduce(reductionContext);
  Multiplication A = squareRootHelper(normAdda, reductionContext);
  // B = B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))
  Subtraction normSuba = Subtraction::Builder(normB, a);
  normB.shallowReduce(reductionContext);
  Multiplication B = squareRootHelper(normSuba, reductionContext);
  // B = B: (1/2)*sqrt(2*(sqrt(a^2+b^2)-a))*sign(b)
  OExpression signb = SignFunction::Builder(b);
  B.addChildAtIndexInPlace(signb, B.numberOfChildren(), B.numberOfChildren());
  signb.shallowReduce(reductionContext);
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  A.shallowReduce(reductionContext);
  B.shallowReduce(reductionContext);
  return result.interruptComputationIfManyNodes();
}

ComplexCartesian ComplexCartesian::powerInteger(
    int n, const ReductionContext& reductionContext) {
  OExpression a = real();
  OExpression b = imag();
  assert(n > 0);
  assert(b.isNull(reductionContext.context()) != TrinaryBoolean::True);

  /* Special case: a == 0 (otherwise, we are going to introduce undefined
   * expressions - a^0 = NAN) (b*i)^n = b^n*i^n with i^n == i, -i, 1 or -1 */
  if (a.isNull(reductionContext.context()) == TrinaryBoolean::True) {
    ComplexCartesian result;
    OExpression bpow = Power::Builder(b, Rational::Builder(n));
    if (n / 2 % 2 == 1) {
      OExpression temp = Multiplication::Builder(Rational::Builder(-1), bpow);
      bpow.shallowReduce(reductionContext);
      bpow = temp;
    }
    if (n % 2 == 0) {
      result = ComplexCartesian::Builder(bpow, Rational::Builder(0));
    } else {
      result = ComplexCartesian::Builder(Rational::Builder(0), bpow);
    }
    bpow.shallowReduce(reductionContext);
    return result;
  }
  /* (a+ib) = a^n+i*b*a^(n-1)+(-1)*b^2*a^(n-2)+(-i)*b^3*a^(n-3)+b^3*a^(n-4)+...
   * Real part: A = a^n+(-1)*b^2*a^(n-2)+...
   * Imaginary part: B = b*a^(n-1) */
  Addition A = Addition::Builder();
  Addition B = Addition::Builder();
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  for (int i = 0; i <= n; i++) {
    BinomialCoefficient binom = BinomialCoefficient::Builder(
        Rational::Builder(n), Rational::Builder(i));
    OExpression aclone = i == n ? a : a.clone();
    OExpression bclone = i == n ? b : b.clone();
    Power apow = Power::Builder(aclone, Rational::Builder(n - i));
    Power bpow = Power::Builder(bclone, Rational::Builder(i));
    Multiplication m = Multiplication::Builder(binom, apow, bpow);
    binom.shallowReduce(reductionContext);
    apow.shallowReduce(reductionContext);
    bpow.shallowReduce(reductionContext);
    if (i / 2 % 2 == 1) {
      m.addChildAtIndexInPlace(Rational::Builder(-1), 0, m.numberOfChildren());
    }
    if (i % 2 == 0) {
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

ComplexCartesian ComplexCartesian::add(
    ComplexCartesian& other, const ReductionContext& reductionContext) {
  OExpression a = real();
  OExpression b = imag();
  OExpression c = other.real();
  OExpression d = other.imag();
  /* (a+ib) + (c+id) = (a+c)+i*(b+d) */
  OExpression ac = Addition::Builder(a.clone(), c.clone());
  OExpression bd = Addition::Builder(b.clone(), d.clone());
  ComplexCartesian result = ComplexCartesian::Builder(ac, bd);
  ac.shallowReduce(reductionContext);
  bd.shallowReduce(reductionContext);
  return result.interruptComputationIfManyNodes();
}

ComplexCartesian ComplexCartesian::multiply(
    ComplexCartesian& other, const ReductionContext& reductionContext) {
  OExpression a = real();
  OExpression b = imag();
  OExpression c = other.real();
  OExpression d = other.imag();
  /* (a+ib) * (c+id) = (ac-bd)+i*(ad+bc)
   * Compute ac-bd */
  OExpression ac = Multiplication::Builder(a.clone(), c.clone());
  OExpression bd = Multiplication::Builder(b.clone(), d.clone());
  Subtraction A = Subtraction::Builder(ac, bd);
  ac.shallowReduce(reductionContext);
  bd.shallowReduce(reductionContext);
  // Compute ad+bc
  OExpression ad = Multiplication::Builder(a, d);
  OExpression bc = Multiplication::Builder(b, c);
  Addition B = Addition::Builder(ad, bc);
  ad.shallowReduce(reductionContext);
  bc.shallowReduce(reductionContext);
  ComplexCartesian result = ComplexCartesian::Builder(A, B);
  A.shallowReduce(reductionContext);
  B.shallowReduce(reductionContext);
  return result.interruptComputationIfManyNodes();
}

OExpression ComplexCartesian::powerHelper(
    OExpression norm, OExpression trigo,
    const ReductionContext& reductionContext) {
  Multiplication m = Multiplication::Builder(norm, trigo);
  norm.shallowReduce(reductionContext);
  trigo.shallowReduce(reductionContext);
  return std::move(m);
}

ComplexCartesian ComplexCartesian::power(
    ComplexCartesian& other, const ReductionContext& reductionContext) {
  OExpression r = clone().convert<ComplexCartesian>().norm(reductionContext);
  OExpression rclone = r.clone();
  OExpression th = argument(reductionContext);
  OExpression thclone = th.clone();
  OExpression c = other.real();
  OExpression d = other.imag();
  // R = r^c*e^(-th*d)
  OExpression rpowc = Power::Builder(rclone, c.clone());
  rclone.shallowReduce(reductionContext);
  OExpression thmuld =
      Multiplication::Builder(Rational::Builder(-1), thclone, d.clone());
  thclone.shallowReduce(reductionContext);
  OExpression exp = Power::Builder(Constant::ExponentialEBuilder(), thmuld);
  thmuld.shallowReduce(reductionContext);
  Multiplication norm = Multiplication::Builder(rpowc, exp);
  rpowc.shallowReduce(reductionContext);
  exp.shallowReduce(reductionContext);

  // TH = d*ln(r)+c*th
  OExpression lnr = NaperianLogarithm::Builder(r);
  r.shallowReduce(reductionContext);
  Multiplication dlnr = Multiplication::Builder(d, lnr);
  lnr.shallowReduce(reductionContext);
  Multiplication thc = Multiplication::Builder(th, c);
  th.shallowReduce(reductionContext);
  OExpression argument = Addition::Builder(thc, dlnr);
  thc.shallowReduce(reductionContext);
  dlnr.shallowReduce(reductionContext);

  if (reductionContext.angleUnit() != Preferences::AngleUnit::Radian) {
    OExpression temp = argument.radianToAngleUnit(reductionContext.angleUnit());
    argument.shallowReduce(reductionContext);
    argument = temp;
  }
  // Result = (norm*cos(argument), norm*sin(argument))
  OExpression normClone = norm.clone();
  OExpression argClone = argument.clone();
  OExpression cos = Cosine::Builder(argClone);
  argClone.shallowReduce(reductionContext);
  OExpression normcosarg = powerHelper(normClone, cos, reductionContext);
  OExpression sin = Sine::Builder(argument);
  argument.shallowReduce(reductionContext);
  OExpression normsinarg = powerHelper(norm, sin, reductionContext);
  ComplexCartesian result = ComplexCartesian::Builder(normcosarg, normsinarg);
  normcosarg.shallowReduce(reductionContext);
  normsinarg.shallowReduce(reductionContext);
  return result.interruptComputationIfManyNodes();
}

ComplexCartesian ComplexCartesian::interruptComputationIfManyNodes() {
  if (numberOfDescendants(true) > k_maxNumberOfNodesBeforeInterrupting) {
    ExceptionCheckpoint::Raise();
    return ComplexCartesian::Builder(Undefined::Builder(),
                                     Undefined::Builder());
  }
  return *this;
}

}  // namespace Poincare
