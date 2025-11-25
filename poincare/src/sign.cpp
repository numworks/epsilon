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

Properties RelaxIntegerProperty(Properties properties) {
  return Properties(properties.sign(), true, properties.canBeInfinite());
}

Properties RelaxFiniteProperty(Properties properties) {
  return Properties(properties.sign(), properties.canBeNonInteger(), true);
}

Properties DecimalFunction(Properties properties, Internal::Type type) {
  bool canBeNull = properties.sign().canBeNull();
  bool canBeStrictlyPositive = properties.sign().canBeStrictlyPositive();
  bool canBeStrictlyNegative = properties.sign().canBeStrictlyNegative();
  bool canBeNonInteger = properties.canBeNonInteger();
  bool canBeInfinite = properties.canBeInfinite();
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
  return Properties(
      Sign(canBeNull, canBeStrictlyPositive, canBeStrictlyNegative),
      canBeNonInteger, canBeInfinite);
}

Sign Opposite(Sign s) {
  return Sign(s.canBeNull(), s.canBeStrictlyNegative(),
              s.canBeStrictlyPositive());
}

Properties Opposite(Properties p) {
  return Properties(Opposite(p.sign()), p.canBeNonInteger(), p.canBeInfinite());
}

Sign Mult(Sign s1, Sign s2) {
  return Sign(s1.canBeNull() || s2.canBeNull(),
              (s1.canBeStrictlyPositive() && s2.canBeStrictlyPositive()) ||
                  (s1.canBeStrictlyNegative() && s2.canBeStrictlyNegative()),
              (s1.canBeStrictlyPositive() && s2.canBeStrictlyNegative()) ||
                  (s1.canBeStrictlyNegative() && s2.canBeStrictlyPositive()));
}

Properties Mult(Properties p1, Properties p2) {
  return Properties(Mult(p1.sign(), p2.sign()),
                    p1.canBeNonInteger() || p2.canBeNonInteger(),
                    (p1.sign().canBeNonNull() && p2.sign().canBeNonNull() &&
                     (p1.canBeInfinite() || p2.canBeInfinite())));
}

Sign Add(Sign s1, Sign s2) {
  return Sign((s1.canBeNull() && s2.canBeNull()) ||
                  (s1.canBeStrictlyPositive() && s2.canBeStrictlyNegative()) ||
                  (s1.canBeStrictlyNegative() && s2.canBeStrictlyPositive()),
              s1.canBeStrictlyPositive() || s2.canBeStrictlyPositive(),
              s1.canBeStrictlyNegative() || s2.canBeStrictlyNegative());
}

Properties Add(Properties p1, Properties p2) {
  return Properties(Add(p1.sign(), p2.sign()),
                    p1.canBeNonInteger() || p2.canBeNonInteger(),
                    p1.canBeInfinite() || p2.canBeInfinite());
}

#if POINCARE_TREE_LOG
void Sign::log(std::ostream& stream, bool endOfLine) const {
  if (isNull()) {
    stream << "Zero";
  } else {
    if (isUnknown()) {
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

void Properties::log(std::ostream& stream, bool endOfLine) const {
  if (sign().isNull()) {
    stream << "Zero";
  } else {
    if (!m_canBeInfinite) {
      stream << "Finite ";
    }
    if (!m_canBeNonInteger) {
      stream << "Integer and ";
    }
    if (isUnknown()) {
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

ComplexProperties RelaxIntegerProperty(ComplexProperties properties) {
  return ComplexProperties(RelaxIntegerProperty(properties.realProperties()),
                           RelaxIntegerProperty(properties.imagProperties()));
}

ComplexProperties Abs(ComplexProperties properties) {
  return ComplexProperties(
      Properties(Sign(properties.canBeNull(), !properties.isNull(), false),
                 properties.canBeNonInteger() || !properties.isPure(),
                 properties.canBeInfinite()),
      Properties::Zero());
}

ComplexProperties ArcCosine(ComplexProperties s) {
  // re(acos(z)) cannot be infinite, im(acos(z)) is finite if abs(z) is finite
  Sign reSign = s.realSign();
  Sign imSign = s.imagSign();
  return ComplexProperties(
      Properties(Sign(reSign.canBeStrictlyPositive(), true, false), true,
                 false),
      Properties(
          Sign(imSign.canBeNull(),
               imSign.canBeStrictlyNegative() ||
                   (imSign.canBeNull() && reSign.canBeStrictlyPositive()),
               imSign.canBeStrictlyPositive() ||
                   (imSign.canBeNull() && reSign.canBeStrictlyNegative())),
          true, s.canBeInfinite()));
}

ComplexProperties ArcSine(ComplexProperties properties) {
  /* - the sign of re(asin(z)) is always the same as re(z)
   * - the sign of im(asin(z)) is always the same as im(z) except when
   * re(z)!=0 and im(z)=0: im(asin(x)) = {>0 if x<-1, =0 if -1<=x<=1, and <0
   * if x>1}
   * - re(asin(z)) cannot be infinite, im(asin(z)) is finite if abs(z) is
   * finite
   */
  Properties realProperties = RelaxIntegerProperty(properties.realProperties());
  Properties imagProperties = RelaxIntegerProperty(properties.imagProperties());
  if (imagProperties.sign().canBeNull() &&
      realProperties.sign().canBeNonNull()) {
    imagProperties =
        imagProperties ||
        Properties(Sign(true, realProperties.sign().canBeStrictlyNegative(),
                        realProperties.sign().canBeStrictlyPositive()));
  }
  if (properties.canBeInfinite()) {
    imagProperties = RelaxFiniteProperty(imagProperties);
  }
  realProperties =
      realProperties && Properties(Sign(true, true, true), true, false);
  return ComplexProperties(realProperties, imagProperties);
}

ComplexProperties ArcTangent(ComplexProperties properties) {
  /* - the sign of im(atan(z)) is always the same as im(z)
   * - the sign of re(atan(z)) is always the same as re(z) except when
   * im(z)!=0 z=i*y: re(atan(i*y)) = {-π/2 if y<-1, 0 if -1<y<1, and π/2 if
   * y>1}
   * - re(atan(z)) cannot be infinite, im(atan(z)) is finite if im(z) == 0 or
   *   re(z) != 0 (accounts for the fact that im(atan(z)) is infinite for z=i
   * or -i)
   */
  Properties realProperties = RelaxIntegerProperty(properties.realProperties());
  Properties imagProperties = RelaxIntegerProperty(properties.imagProperties());
  if (realProperties.sign().canBeNull() &&
      imagProperties.sign().canBeNonNull()) {
    realProperties =
        realProperties ||
        Properties(Sign(true, imagProperties.sign().canBeStrictlyPositive(),
                        imagProperties.sign().canBeStrictlyNegative()));
  }
  realProperties =
      realProperties && Properties(Sign(true, true, true), true, false);
  if (imagProperties.sign().isNull() || !realProperties.sign().canBeNull()) {
    imagProperties =
        imagProperties && Properties(Sign(true, true, true), true, false);
  } else {
    imagProperties = RelaxFiniteProperty(imagProperties);
  }
  return ComplexProperties(realProperties, imagProperties);
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
      Properties(
          Sign(im.canBeNull() && (re.canBeNull() || re.canBeStrictlyPositive()),
               (im.canBeNull() && re.canBeStrictlyNegative()) ||
                   im.canBeStrictlyPositive(),
               im.canBeStrictlyNegative()),
          true, false),
      Properties::Zero());
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
  Properties realProperties = (p.isFinite() && !p.canBeNull())
                                  ? Properties::Finite()
                                  : Properties::Unknown();
  return ComplexProperties(realProperties, ComplexArgument(p).realProperties());
}

ComplexProperties Trig(ComplexProperties p, bool isSin) {
  if (p.isPureIm()) {
    return isSin ? ComplexProperties(Properties::Zero(),
                                     RelaxIntegerProperty(p.imagProperties()))
                 : ComplexProperties::RealStrictlyPositive();
  }
  return ComplexProperties(
      p.isReal() ? Properties::Finite() : Properties::Unknown(),
      p.isReal() ? Properties::Zero() : Properties::Unknown());
}

ComplexProperties Mult(ComplexProperties p1, ComplexProperties p2) {
  Properties a = Add(Mult(p1.realProperties(), p2.realProperties()),
                     Opposite(Mult(p1.imagProperties(), p2.imagProperties())));
  Properties b = Add(Mult(p1.realProperties(), p2.imagProperties()),
                     Mult(p1.imagProperties(), p2.realProperties()));
  return ComplexProperties(a, b);
}

ComplexProperties Add(ComplexProperties p1, ComplexProperties p2) {
  return ComplexProperties(Add(p1.realProperties(), p2.realProperties()),
                           Add(p1.imagProperties(), p2.imagProperties()));
}

ComplexProperties ExponentialReal(Properties properties) {
  // exp(-inf) = 0, necessary so that x^y = exp(y*ln(x)) can be null.
  return properties.sign().isNull()
             ? ComplexProperties::RealFiniteStrictlyPositiveInteger()  // 1
         : !properties.canBeInfinite()
             ? ComplexProperties::RealFiniteStrictlyPositive()
         : !properties.sign().canBeStrictlyNegative()
             ? ComplexProperties::RealStrictlyPositive()
             : ComplexProperties::RealPositive();
}

ComplexProperties ExponentialImag(Properties properties) {
  // exp(i*x) is on the unit circle
  return properties.sign().isNull()
             ? ComplexProperties::RealFiniteStrictlyPositiveInteger()  // 1
             : ComplexProperties::Finite();
}

ComplexProperties Exponential(ComplexProperties properties) {
  // exp(x + y*i) = exp(x) * exp(y*i)
  return Mult(ExponentialReal(properties.realProperties()),
              ExponentialImag(properties.imagProperties()));
}

// Note: we could get more info on canBeInfinite
ComplexProperties Power(ComplexProperties base, ComplexProperties exp,
                        bool expIsTwo) {
  if (base.isReal() && base.realSign().isStrictlyPositive() && exp.isReal() &&
      exp.realSign().isStrictlyPositive()) {
    // b^e with b and e strictly positive reals gives strictly positive real,
    // integer only if both b and e are integer
    return ComplexProperties(
        Properties(Sign(false, true, false),
                   base.realProperties().canBeNonInteger() ||
                       exp.realProperties().canBeNonInteger(),
                   !(base.isFinite() && exp.isFinite())),
        Properties::Zero());
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
    return ComplexProperties(Properties(Sign(canBeNull, true, !isPositive),
                                        canBeNonInteger, canBeInfinite),
                             Properties::Zero());
  }
  Properties properties =
      Properties(Sign(true, true, true), canBeNonInteger, canBeInfinite);
  return ComplexProperties(properties, properties);
}

ComplexProperties TypeProperties(ComplexProperties s) {
  Properties realProperties = Properties(
      Sign(s.realSign().canBeNull(), s.realSign().canBeStrictlyPositive(),
           s.realSign().canBeStrictlyNegative()),
      !s.isPure(), false);
  Properties imagProperties = Properties(
      Sign(s.imagSign().canBeNull(), s.imagSign().canBeStrictlyPositive(),
           s.imagSign().canBeStrictlyNegative()),
      !s.isPure(), false);
  return ComplexProperties(realProperties, imagProperties);
}

ComplexProperties PercentAddition(ComplexProperties s1, ComplexProperties s2) {
  return Add(s1, Mult(s1, RelaxIntegerProperty(s2)));
}

ComplexProperties Quotient(ComplexProperties p1, ComplexProperties p2) {
  // a = q*b + r with 0 ≤ r < |b| and a, b, q, r integers and b ≠ 0
  Sign s1Real = p1.realSign();
  Sign s2Real = p2.realSign();
  if (!s1Real.hasKnownSign() || !s2Real.hasKnownSign()) {
    return ComplexProperties(Properties::FiniteInteger(), Properties::Zero());
  }
  /* If a and b have the same sign, q is positive.
   * If a and b have opposite signs, q is negative. */
  return ComplexProperties(s1Real.isPositive() == s2Real.isPositive()
                               ? Properties::FinitePositiveInteger()
                               : Properties::FiniteNegativeInteger(),
                           Properties::Zero());
}

ComplexProperties Binomial(ComplexProperties p) {
  /* Bin(n, k) is only defined for n and k real, and k integer.
   * It is always real, and integer if n is an integer. */
  return ComplexProperties(p.realProperties().isInteger()
                               ? Properties::Integer()
                               : Properties::Unknown(),
                           Properties::Zero());
}

namespace Internal {

// Note: A complex function plotter can be used to fill in these methods.
ComplexProperties GetComplexProperties(const Tree* e) {
  assert(Dimension::Get(e).isScalarOrUnit());
  if (e->isNumber()) {
    return ComplexProperties(Number::Properties(e), Properties::Zero());
  } else if (e->isDecimal()) {
    return ComplexProperties(Number::Properties(e->child(0)),
                             Properties::Zero());
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
      if (p.isReal() && p.realProperties().isFinite() &&
          !p.realSign().hasKnownStrictSign()) {
        assert(p.imagSign().isNull());
        Properties realProperties = Bounds::Properties(e);
        if (realProperties != Properties::Unknown()) {
          return ComplexProperties(realProperties, Properties::Zero());
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
      return ComplexProperties(Properties::Positive(), Properties::Zero());
    case Type::Abs:
      return Abs(GetComplexProperties(e->child(0)));
    case Type::Exp:
      return Exponential(GetComplexProperties(e->child(0)));
    case Type::Ln:
      return Ln(e);
    case Type::Re:
      return ComplexProperties(
          GetComplexProperties(e->child(0)).realProperties(),
          Properties::Zero());
    case Type::Im:
      return ComplexProperties(
          GetComplexProperties(e->child(0)).imagProperties(),
          Properties::Zero());
    case Type::Var:
      return Variables::GetComplexProperties(e);
    case Type::ComplexI:
      return ComplexProperties(Properties::Zero(),
                               Properties::FiniteStrictlyPositiveInteger());
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
      return ComplexProperties(Properties::StrictlyPositive(),
                               Properties::Zero());
    case Type::PhysicalConstant:
    case Type::Unit:
      // Units are considered equivalent to their SI ratio
      return ComplexProperties(Properties::FinitePositive(),
                               Properties::Zero());
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
          DecimalFunction(GetComplexProperties(e->child(0)).realProperties(),
                          e->type()),
          Properties::Zero());
    case Type::Random:
      return ComplexProperties(Properties::FinitePositive(),
                               Properties::Zero());
    case Type::RandInt:
      return ComplexProperties(Properties::FiniteInteger(), Properties::Zero());
    case Type::Sign:
      return TypeProperties(GetComplexProperties(e->child(0)));
    case Type::Factor:
      return GetComplexProperties(e->child(0));
    case Type::GCD:
    case Type::LCM:
    case Type::Permute:
    case Type::Rem:
      return ComplexProperties(Properties::FinitePositiveInteger(),
                               Properties::Zero());
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

Properties GetProperties(const Tree* e) {
  assert(GetComplexProperties(e).isReal());
  return GetComplexProperties(e).realProperties();
}

Sign GetSign(const Tree* e) { return GetProperties(e).sign(); }

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
  realProperties().log(stream, false);
  stream << ") + i*(";
  imagProperties().log(stream, false);
  stream << ")";
  if (endOfLine) {
    stream << "\n";
  }
}
#endif

}  // namespace Poincare
