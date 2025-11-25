#include <omg/unreachable.h>
#include <poincare/sign.h>
#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/bounds.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/number.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/pattern_matching.h>

namespace Poincare {

/* Must at least handle Additions, Multiplications, Numbers and Real/Imaginary
 * parts so that any simplified complex is sanitized. Also handle Exp, Ln and
 * Powers of positive integers so that abs(z) remains real after reduction.
 * Handling trig, exp, ln, abs and arg may also greatly help the polar complex
 * mode simplifications. */

Sign RelaxIntegerProperty(Sign s) {
  return Sign(s.canBeNull(), s.canBeStrictlyPositive(),
              s.canBeStrictlyNegative(), true, s.canBeInfinite());
}

Sign RelaxFiniteProperty(Sign s) {
  return Sign(s.canBeNull(), s.canBeStrictlyPositive(),
              s.canBeStrictlyNegative(), s.canBeNonInteger(), true);
}

Sign DecimalFunction(Sign s, Internal::Type type) {
  bool canBeNull = s.canBeNull();
  bool canBeStrictlyPositive = s.canBeStrictlyPositive();
  bool canBeStrictlyNegative = s.canBeStrictlyNegative();
  bool canBeNonInteger = s.canBeNonInteger();
  bool canBeInfinite = s.canBeInfinite();
  switch (type) {
    case Internal::Type::Ceil:
      canBeNull |= canBeStrictlyNegative && canBeNonInteger;
      canBeNonInteger = false;
      break;
    case Internal::Type::Floor:
      canBeNull |= canBeStrictlyPositive && canBeNonInteger;
      canBeNonInteger = false;
      break;
    case Internal::Type::Frac:
      canBeNull = true;
      canBeStrictlyPositive = canBeNonInteger;
      canBeStrictlyNegative = false;
      canBeInfinite = false;
      break;
    case Internal::Type::Round:
      canBeNull = true;
      break;
    default:
      OMG::unreachable();
  }
  return Sign(canBeNull, canBeStrictlyPositive, canBeStrictlyNegative,
              canBeNonInteger, canBeInfinite);
}

Sign Opposite(Sign s) {
  return Sign(s.canBeNull(), s.canBeStrictlyNegative(),
              s.canBeStrictlyPositive(), s.canBeNonInteger(),
              s.canBeInfinite());
}

Sign Mult(Sign s1, Sign s2) {
  return Sign(s1.canBeNull() || s2.canBeNull(),
              (s1.canBeStrictlyPositive() && s2.canBeStrictlyPositive()) ||
                  (s1.canBeStrictlyNegative() && s2.canBeStrictlyNegative()),
              (s1.canBeStrictlyPositive() && s2.canBeStrictlyNegative()) ||
                  (s1.canBeStrictlyNegative() && s2.canBeStrictlyPositive()),
              s1.canBeNonInteger() || s2.canBeNonInteger(),
              (s1.canBeNonNull() && s2.canBeNonNull() &&
               (s1.canBeInfinite() || s2.canBeInfinite())));
}

Sign Add(Sign s1, Sign s2) {
  return Sign((s1.canBeNull() && s2.canBeNull()) ||
                  (s1.canBeStrictlyPositive() && s2.canBeStrictlyNegative()) ||
                  (s1.canBeStrictlyNegative() && s2.canBeStrictlyPositive()),
              s1.canBeStrictlyPositive() || s2.canBeStrictlyPositive(),
              s1.canBeStrictlyNegative() || s2.canBeStrictlyNegative(),
              s1.canBeNonInteger() || s2.canBeNonInteger(),
              s1.canBeInfinite() || s2.canBeInfinite());
}

#if POINCARE_TREE_LOG
void Sign::log(std::ostream& stream, bool endOfLine) const {
  if (isNull()) {
    stream << "Zero";
  } else {
    if (!m_canBeInfinite) {
      stream << "Finite ";
    }
    if (!m_canBeNonInteger) {
      stream << "Integer and ";
    }
    if (hasUnknownSign()) {
      stream << "Unknown";
    } else {
      if (m_canBeStrictlyPositive && m_canBeStrictlyNegative) {
        stream << "Non Null";
      } else {
        if (!m_canBeNull) {
          stream << "Strictly ";
        }
        stream << (m_canBeStrictlyNegative ? "Negative" : "Positive");
      }
    }
  }
  if (endOfLine) {
    stream << "\n";
  }
}
#endif

ComplexProperties RelaxIntegerProperty(ComplexProperties p) {
  return ComplexProperties(RelaxIntegerProperty(p.realSign()),
                           RelaxIntegerProperty(p.imagSign()));
}

ComplexProperties Abs(ComplexProperties p) {
  return ComplexProperties(
      Sign(p.canBeNull(), !p.isNull(), false,
           p.canBeNonInteger() || !p.isPure(), p.canBeInfinite()),
      Sign::Zero());
}

ComplexProperties ArcCosine(ComplexProperties p) {
  // re(acos(z)) cannot be infinite, im(acos(z)) is finite if abs(z) is finite
  Sign re = p.realSign();
  Sign im = p.imagSign();
  return ComplexProperties(
      Sign(re.canBeStrictlyPositive(), true, false, true, false),
      Sign(im.canBeNull(),
           im.canBeStrictlyNegative() ||
               (im.canBeNull() && re.canBeStrictlyPositive()),
           im.canBeStrictlyPositive() ||
               (im.canBeNull() && re.canBeStrictlyNegative()),
           true, p.canBeInfinite()));
}

ComplexProperties ArcSine(ComplexProperties p) {
  /* - the sign of re(asin(z)) is always the same as re(z)
   * - the sign of im(asin(z)) is always the same as im(z) except when re(z)!=0
   *   and im(z)=0: im(asin(x)) = {>0 if x<-1, =0 if -1<=x<=1, and <0 if x>1}
   * - re(asin(z)) cannot be infinite, im(asin(z)) is finite if abs(z) is finite
   */
  Sign realSign = RelaxIntegerProperty(p.realSign());
  Sign imagSign = RelaxIntegerProperty(p.imagSign());
  if (imagSign.canBeNull() && realSign.canBeNonNull()) {
    imagSign = imagSign || Sign(true, realSign.canBeStrictlyNegative(),
                                realSign.canBeStrictlyPositive());
  }
  if (p.canBeInfinite()) {
    imagSign = RelaxFiniteProperty(imagSign);
  }
  realSign = realSign && Sign(true, true, true, true, false);
  return ComplexProperties(realSign, imagSign);
}

ComplexProperties ArcTangent(ComplexProperties p) {
  /* - the sign of im(atan(z)) is always the same as im(z)
   * - the sign of re(atan(z)) is always the same as re(z) except when im(z)!=0
   *   z=i*y: re(atan(i*y)) = {-π/2 if y<-1, 0 if -1<y<1, and π/2 if y>1}
   * - re(atan(z)) cannot be infinite, im(atan(z)) is finite if im(z) == 0 or
   *   re(z) != 0 (accounts for the fact that im(atan(z)) is infinite for z=i or
   * -i)
   */
  Sign realSign = RelaxIntegerProperty(p.realSign());
  Sign imagSign = RelaxIntegerProperty(p.imagSign());
  if (realSign.canBeNull() && imagSign.canBeNonNull()) {
    realSign = realSign || Sign(true, imagSign.canBeStrictlyPositive(),
                                imagSign.canBeStrictlyNegative());
  }
  realSign = realSign && Sign(true, true, true, true, false);
  if (imagSign.isNull() || !realSign.canBeNull()) {
    imagSign = imagSign && Sign(true, true, true, true, false);
  } else {
    imagSign = RelaxFiniteProperty(imagSign);
  }
  return ComplexProperties(realSign, imagSign);
}

ComplexProperties ComplexArgument(ComplexProperties p) {
  /* arg(z) ∈ ]-π,π].
   * arg(z) > 0 if im(z) > 0,
   * arg(z) < 0 if im(z) < 0,
   * arg(z) = 0 if im(z) = 0 and re(z) >= 0,
   * arg(z) = π if im(z) = 0 and re(z) < 0 */
  Sign re = p.realSign();
  Sign im = p.imagSign();
  return ComplexProperties(
      Sign(im.canBeNull() && (re.canBeNull() || re.canBeStrictlyPositive()),
           (im.canBeNull() && re.canBeStrictlyNegative()) ||
               im.canBeStrictlyPositive(),
           im.canBeStrictlyNegative(), true, false),
      Sign::Zero());
}

ComplexProperties Ln(const Internal::Tree* e) {
  const Internal::Tree* child = e->child(0);
  if (child->isRational()) {
    return Internal::Rational::ComplexPropertiesOfLn(e);
  }
  /* z = |z|e^(i*arg(z))
   * re(ln(z)) = ln(|z|)
   * im(ln(z)) = arg(z) */
  ComplexProperties p = GetComplexProperties(child);
  Sign realSign =
      (p.isFinite() && !p.canBeNull()) ? Sign::Finite() : Sign::Unknown();
  return ComplexProperties(realSign, ComplexArgument(p).realSign());
}

ComplexProperties Trig(ComplexProperties p, bool isSin) {
  if (p.isPureIm()) {
    return isSin ? ComplexProperties(Sign::Zero(),
                                     RelaxIntegerProperty(p.imagSign()))
                 : ComplexProperties::RealStrictlyPositive();
  }
  return ComplexProperties(p.isReal() ? Sign::Finite() : Sign::Unknown(),
                           p.isReal() ? Sign::Zero() : Sign::Unknown());
}

ComplexProperties Mult(ComplexProperties p1, ComplexProperties p2) {
  Sign a = Add(Mult(p1.realSign(), p2.realSign()),
               Opposite(Mult(p1.imagSign(), p2.imagSign())));
  Sign b = Add(Mult(p1.realSign(), p2.imagSign()),
               Mult(p1.imagSign(), p2.realSign()));
  return ComplexProperties(a, b);
}

ComplexProperties Add(ComplexProperties p1, ComplexProperties p2) {
  return ComplexProperties(Add(p1.realSign(), p2.realSign()),
                           Add(p1.imagSign(), p2.imagSign()));
}

ComplexProperties ExponentialReal(Sign s) {
  // exp(-inf) = 0, necessary so that x^y = exp(y*ln(x)) can be null.
  return s.isNull()
             ? ComplexProperties::RealFiniteStrictlyPositiveInteger()  // 1
         : !s.canBeInfinite() ? ComplexProperties::RealFiniteStrictlyPositive()
         : !s.canBeStrictlyNegative()
             ? ComplexProperties::RealStrictlyPositive()
             : ComplexProperties::RealPositive();
}

ComplexProperties ExponentialImag(Sign s) {
  // exp(i*x) is on the unit circle
  return s.isNull()
             ? ComplexProperties::RealFiniteStrictlyPositiveInteger()  // 1
             : ComplexProperties::Finite();
}

ComplexProperties Exponential(ComplexProperties p) {
  // exp(x + y*i) = exp(x) * exp(y*i)
  return Mult(ExponentialReal(p.realSign()), ExponentialImag(p.imagSign()));
}

// Note: we could get more info on canBeInfinite
ComplexProperties Power(ComplexProperties base, ComplexProperties exp,
                        bool expIsTwo) {
  if (base.isReal() && base.realSign().isStrictlyPositive() && exp.isReal() &&
      exp.realSign().isStrictlyPositive()) {
    // b^e with b and e strictly positive reals gives strictly positive real,
    // integer only if both b and e are integer
    return ComplexProperties(Sign(false, true, false,
                                  base.realSign().canBeNonInteger() ||
                                      exp.realSign().canBeNonInteger(),
                                  !(base.isFinite() && exp.isFinite())),
                             Sign::Zero());
  }
  if (exp.canBeNonReal() || exp.canBeNonInteger()) {
    return ComplexProperties::Unknown();
  }
  if (base.isNull()) {
    // 0^exp = 0
    return ComplexProperties::Zero();
  }
  if (exp.isNull()) {
    // base^0 = 1
    return ComplexProperties::RealFiniteStrictlyPositiveInteger();
  }
  bool canBeNonInteger = base.canBeNonInteger() || !exp.realSign().isPositive();
  /* If base can be null and exp can be strictly negative, then the result may
   * be infinite. Example: sin(x)^(-1) */
  bool canBeInfinite =
      base.canBeInfinite() || exp.canBeInfinite() ||
      (exp.realSign().canBeStrictlyNegative() && base.canBeNull());
  if (base.isReal()) {
    bool canBeNull = base.realSign().canBeNull();
    bool isPositive = expIsTwo || base.realSign().isPositive();
    return ComplexProperties(
        Sign(canBeNull, true, !isPositive, canBeNonInteger, canBeInfinite),
        Sign::Zero());
  }
  Sign sign = Sign(true, true, true, canBeNonInteger, canBeInfinite);
  return ComplexProperties(sign, sign);
}

ComplexProperties TypeSign(ComplexProperties p) {
  Sign realSign =
      Sign(p.realSign().canBeNull(), p.realSign().canBeStrictlyPositive(),
           p.realSign().canBeStrictlyNegative(), !p.isPure(), false);
  Sign imagSign =
      Sign(p.imagSign().canBeNull(), p.imagSign().canBeStrictlyPositive(),
           p.imagSign().canBeStrictlyNegative(), !p.isPure(), false);
  return ComplexProperties(realSign, imagSign);
}

ComplexProperties PercentAddition(ComplexProperties p1, ComplexProperties p2) {
  return Add(p1, Mult(p1, RelaxIntegerProperty(p2)));
}

ComplexProperties Quotient(ComplexProperties p1, ComplexProperties p2) {
  // a = q*b + r with 0 ≤ r < |b| and a, b, q, r integers and b ≠ 0
  Sign s1Real = p1.realSign();
  Sign s2Real = p2.realSign();
  if (!s1Real.hasKnownSign() || !s2Real.hasKnownSign()) {
    return ComplexProperties(Sign::FiniteInteger(), Sign::Zero());
  }
  /* If a and b have the same sign, q is positive.
   * If a and b have opposite signs, q is negative. */
  return ComplexProperties(s1Real.isPositive() == s2Real.isPositive()
                               ? Sign::FinitePositiveInteger()
                               : Sign::FiniteNegativeInteger(),
                           Sign::Zero());
}

ComplexProperties Binomial(ComplexProperties p) {
  /* Bin(n, k) is only defined for n and k real, and k integer.
   * It is always real, and integer if n is an integer. */
  return ComplexProperties(
      p.realSign().isInteger() ? Sign::Integer() : Sign::Unknown(),
      Sign::Zero());
}

namespace Internal {

// Note: A complex function plotter can be used to fill in these methods.
ComplexProperties GetComplexProperties(const Tree* e) {
  assert(Dimension::Get(e).isScalarOrUnit());
  if (e->isNumber()) {
    return ComplexProperties(Number::Sign(e), Sign::Zero());
  } else if (e->isDecimal()) {
    return ComplexProperties(Number::Sign(e->child(0)), Sign::Zero());
  } else if (e->isUserNamed()) {
    return Symbol::GetComplexProperties(e);
  }
  switch (e->type()) {
    case Type::Mult: {
      ComplexProperties p =
          ComplexProperties::RealFiniteStrictlyPositiveInteger();  // 1
      for (const Tree* c : e->children()) {
        p = Mult(p, GetComplexProperties(c));
        if (p.isUnknown() || p.isNull()) {
          break;
        }
      }
      return p;
    }
    case Type::Add: {
      ComplexProperties p = ComplexProperties::Zero();
      for (const Tree* c : e->children()) {
        p = Add(p, GetComplexProperties(c));
        if (p.isUnknown()) {
          break;
        }
      }
      if (p.isReal() && p.realSign().isFinite() &&
          !p.realSign().hasKnownStrictSign()) {
        assert(p.imagSign().isNull());
        Sign realSign = Bounds::Sign(e);
        if (realSign != Sign::Unknown()) {
          return ComplexProperties(realSign, Sign::Zero());
        }
      }
      return p;
    }
    case Type::PowReal:
    case Type::Pow:
      return Power(GetComplexProperties(e->child(0)),
                   GetComplexProperties(e->child(1)), e->child(1)->isTwo());
    case Type::Norm:
      // Child isn't a scalar
      return ComplexProperties(Sign::Positive(), Sign::Zero());
    case Type::Abs:
      return Abs(GetComplexProperties(e->child(0)));
    case Type::Exp:
      return Exponential(GetComplexProperties(e->child(0)));
    case Type::Ln:
      return Ln(e);
    case Type::Re:
      return ComplexProperties(GetComplexProperties(e->child(0)).realSign(),
                               Sign::Zero());
    case Type::Im:
      return ComplexProperties(GetComplexProperties(e->child(0)).imagSign(),
                               Sign::Zero());
    case Type::Var:
      return Variables::GetComplexProperties(e);
    case Type::ComplexI:
      return ComplexProperties(Sign::Zero(),
                               Sign::FiniteStrictlyPositiveInteger());
    case Type::Trig:
      assert(e->child(1)->isOne() || e->child(1)->isZero());
      return Trig(GetComplexProperties(e->child(0)), e->child(1)->isOne());
    case Type::ATanRad:
      return ArcTangent(GetComplexProperties(e->child(0)));
    case Type::Arg:
      return ComplexArgument(GetComplexProperties(e->child(0)));
    case Type::Dep:
      return GetComplexProperties(Dependency::Main(e));
    case Type::Inf:
      return ComplexProperties(Sign::StrictlyPositive(), Sign::Zero());
    case Type::PhysicalConstant:
    case Type::Unit:
      // Units are considered equivalent to their SI ratio
      return ComplexProperties(Sign::FinitePositive(), Sign::Zero());
    case Type::ATrig:
      assert(e->child(1)->isOne() || e->child(1)->isZero());
      return e->child(1)->isOne()
                 ? ArcSine(GetComplexProperties(e->child(0)))
                 : ArcCosine(GetComplexProperties(e->child(0)));
    case Type::Ceil:
    case Type::Floor:
    case Type::Frac:
    case Type::Round:
      return ComplexProperties(
          DecimalFunction(GetComplexProperties(e->child(0)).realSign(),
                          e->type()),
          Sign::Zero());
    case Type::Random:
      return ComplexProperties(Sign::FinitePositive(), Sign::Zero());
    case Type::RandInt:
      return ComplexProperties(Sign::FiniteInteger(), Sign::Zero());
    case Type::Sign:
      return TypeSign(GetComplexProperties(e->child(0)));
    case Type::Factor:
      return GetComplexProperties(e->child(0));
    case Type::GCD:
    case Type::LCM:
    case Type::Permute:
    case Type::Rem:
      return ComplexProperties(Sign::FinitePositiveInteger(), Sign::Zero());
    case Type::Quo:
      return Quotient(GetComplexProperties(e->child(0)),
                      GetComplexProperties(e->child(1)));
    case Type::PercentSimple:
      return RelaxIntegerProperty(GetComplexProperties(e->child(0)));
    case Type::PercentAddition:
      return PercentAddition(GetComplexProperties(e->child(0)),
                             GetComplexProperties(e->child(1)));
    case Type::Fact:
      return ComplexProperties::RealStrictlyPositiveInteger();
    case Type::Binomial:
      return Binomial(GetComplexProperties(e->child(0)));
    case Type::Diff:
      return ComplexProperties::Real();
#if 0
    // Activate these cases if necessary
    case Type::ATan:
      return ArcTangent(GetComplexProperties(e->child(0)));
    case Type::MixedFraction:
      return Add(GetComplexProperties(e->child(0)), GetComplexProperties(e->child(1)));
    case Type::Parentheses:
      return GetComplexProperties(e->child(0));
#endif
    default:
      return ComplexProperties::Unknown();
  }
}

Sign GetSign(const Tree* e) {
  assert(GetComplexProperties(e).isReal());
  return GetComplexProperties(e).realSign();
}

ComplexProperties ComplexPropertiesOfDifference(const Tree* e1,
                                                const Tree* e2) {
  Tree* difference = PatternMatching::CreateReduce(KAdd(KA, KMult(-1_e, KB)),
                                                   {.KA = e1, .KB = e2});
  ComplexProperties result = GetComplexProperties(difference);
  if (AdvancedReduction::DeepExpandAlgebraic(difference)) {
    /* We do not use advance reduction here but it might be useful to expand
     * Mult since we are creating an Add with Mult */
    result = result && GetComplexProperties(difference);
  }
  difference->removeTree();
  return result;
}

ComplexProperties PropertiesOfTreeOrApproximation(const Tree* e) {
  assert(Dimension::IsNonListScalar(e));
  ComplexProperties properties = GetComplexProperties(e);
  if (!properties.realSign().hasKnownStrictSign() ||
      !properties.imagSign().hasKnownStrictSign()) {
    Tree* approximation =
        Approximation::ToTree<double>(e, Approximation::Parameters{});
    properties = GetComplexProperties(approximation);
    approximation->removeTree();
  }
  return properties;
}

}  // namespace Internal

#if POINCARE_TREE_LOG
void ComplexProperties::log(std::ostream& stream, bool endOfLine) const {
  stream << "(";
  realSign().log(stream, false);
  stream << ") + i*(";
  imagSign().log(stream, false);
  stream << ")";
  if (endOfLine) {
    stream << "\n";
  }
}
#endif

}  // namespace Poincare
