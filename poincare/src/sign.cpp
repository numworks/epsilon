#include <poincare/sign.h>
#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/number.h>
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
      assert(false);
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

ComplexSign RelaxIntegerProperty(ComplexSign s) {
  return ComplexSign(RelaxIntegerProperty(s.realSign()),
                     RelaxIntegerProperty(s.imagSign()));
}

ComplexSign Abs(ComplexSign s) {
  return ComplexSign(
      Sign(s.canBeNull(), !s.isNull(), false,
           s.canBeNonInteger() || !s.isPure(), s.canBeInfinite()),
      Sign::Zero());
}

ComplexSign ArcCosine(ComplexSign s) {
  // re(acos(z)) cannot be infinite, im(acos(z)) is finite if abs(z) is finite
  Sign re = s.realSign();
  Sign im = s.imagSign();
  return ComplexSign(Sign(re.canBeStrictlyPositive(), true, false, true, false),
                     Sign(im.canBeNull(),
                          im.canBeStrictlyNegative() ||
                              (im.canBeNull() && re.canBeStrictlyPositive()),
                          im.canBeStrictlyPositive() ||
                              (im.canBeNull() && re.canBeStrictlyNegative()),
                          true, s.canBeInfinite()));
}

ComplexSign ArcSine(ComplexSign s) {
  /* - the sign of re(asin(z)) is always the same as re(z)
   * - the sign of im(asin(z)) is always the same as im(z) except when re(z)!=0
   *   and im(z)=0: im(asin(x)) = {>0 if x<-1, =0 if -1<=x<=1, and <0 if x>1}
   * - re(asin(z)) cannot be infinite, im(asin(z)) is finite if abs(z) is finite
   */
  Sign realSign = RelaxIntegerProperty(s.realSign());
  Sign imagSign = RelaxIntegerProperty(s.imagSign());
  if (imagSign.canBeNull() && realSign.canBeNonNull()) {
    imagSign = imagSign || Sign(true, realSign.canBeStrictlyNegative(),
                                realSign.canBeStrictlyPositive());
  }
  if (s.canBeInfinite()) {
    imagSign = RelaxFiniteProperty(imagSign);
  }
  realSign = realSign && Sign(true, true, true, true, false);
  return ComplexSign(realSign, imagSign);
}

ComplexSign ArcTangent(ComplexSign s) {
  /* - the sign of im(atan(z)) is always the same as im(z)
   * - the sign of re(atan(z)) is always the same as re(z) except when im(z)!=0
   *   z=i*y: re(atan(i*y)) = {-π/2 if y<-1, 0 if -1<y<1, and π/2 if y>1}
   * - re(atan(z)) cannot be infinite, im(atan(z)) is finite if im(z) == 0 or
   *   re(z) != 0 (accounts for the fact that im(atan(z)) is infinite for z=i or
   * -i)
   */
  Sign realSign = RelaxIntegerProperty(s.realSign());
  Sign imagSign = RelaxIntegerProperty(s.imagSign());
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
  return ComplexSign(realSign, imagSign);
}

ComplexSign ComplexArgument(ComplexSign s) {
  /* arg(z) ∈ ]-π,π].
   * arg(z) > 0 if im(z) > 0,
   * arg(z) < 0 if im(z) < 0,
   * arg(z) = 0 if im(z) = 0 and re(z) >= 0,
   * arg(z) = π if im(z) = 0 and re(z) < 0 */
  Sign re = s.realSign();
  Sign im = s.imagSign();
  return ComplexSign(
      Sign(im.canBeNull() && (re.canBeNull() || re.canBeStrictlyPositive()),
           (im.canBeNull() && re.canBeStrictlyNegative()) ||
               im.canBeStrictlyPositive(),
           im.canBeStrictlyNegative(), true, false),
      Sign::Zero());
}

ComplexSign Ln(ComplexSign s) {
  /* z = |z|e^(i*arg(z))
   * re(ln(z)) = ln(|z|)
   * im(ln(z)) = arg(z) */
  Sign realSign =
      (s.isFinite() && !s.canBeNull()) ? Sign::Finite() : Sign::Unknown();
  return ComplexSign(realSign, ComplexArgument(s).realSign());
}

ComplexSign DecimalFunction(ComplexSign s, Internal::Type type) {
  return ComplexSign(DecimalFunction(s.realSign(), type),
                     DecimalFunction(s.imagSign(), type));
}

ComplexSign Trig(ComplexSign s, bool isSin) {
  if (s.isPureIm()) {
    return isSin ? ComplexSign(Sign::Zero(), RelaxIntegerProperty(s.imagSign()))
                 : ComplexSign::RealStrictlyPositive();
  }
  return ComplexSign(s.isReal() ? Sign::Finite() : Sign::Unknown(),
                     s.isReal() ? Sign::Zero() : Sign::Unknown());
}

ComplexSign Mult(ComplexSign s1, ComplexSign s2) {
  Sign a = Add(Mult(s1.realSign(), s2.realSign()),
               Opposite(Mult(s1.imagSign(), s2.imagSign())));
  Sign b = Add(Mult(s1.realSign(), s2.imagSign()),
               Mult(s1.imagSign(), s2.realSign()));
  return ComplexSign(a, b);
}

ComplexSign Add(ComplexSign s1, ComplexSign s2) {
  return ComplexSign(Add(s1.realSign(), s2.realSign()),
                     Add(s1.imagSign(), s2.imagSign()));
}

ComplexSign ExponentialReal(Sign s) {
  // exp(-inf) = 0, necessary so that x^y = exp(y*ln(x)) can be null.
  return s.isNull() ? ComplexSign::RealFiniteStrictlyPositiveInteger()  // 1
         : !s.canBeInfinite() ? ComplexSign::RealFiniteStrictlyPositive()
         : !s.canBeStrictlyNegative() ? ComplexSign::RealStrictlyPositive()
                                      : ComplexSign::RealPositive();
}

ComplexSign ExponentialImag(Sign s) {
  // exp(i*x) is on the unit circle
  return s.isNull() ? ComplexSign::RealFiniteStrictlyPositiveInteger()  // 1
                    : ComplexSign::Finite();
}

ComplexSign Exponential(ComplexSign s) {
  // exp(x + y*i) = exp(x) * exp(y*i)
  return Mult(ExponentialReal(s.realSign()), ExponentialImag(s.imagSign()));
}

// Note: we could get more info on canBeInfinite
ComplexSign Power(ComplexSign base, ComplexSign exp, bool expIsTwo) {
  if (exp.canBeNonReal() || exp.canBeNonInteger()) {
    return ComplexSign::Unknown();
  }
  if (base.isNull()) {
    // 0^exp = 0
    return ComplexSign::Zero();
  }
  if (exp.isNull()) {
    // base^0 = 1
    return ComplexSign::RealFiniteStrictlyPositiveInteger();
  }
  bool canBeNonInteger = base.canBeNonInteger() || !exp.realSign().isPositive();
  bool canBeInfinite = base.canBeInfinite() || exp.canBeInfinite();
  if (base.isReal()) {
    bool canBeNull = base.realSign().canBeNull();
    bool isPositive = expIsTwo || base.realSign().isPositive();
    return ComplexSign(
        Sign(canBeNull, true, !isPositive, canBeNonInteger, canBeInfinite),
        Sign::Zero());
  }
  Sign sign = Sign(true, true, true, canBeNonInteger, canBeInfinite);
  return ComplexSign(sign, sign);
}

namespace Internal {

// Note: A complex function plotter can be used to fill in these methods.
ComplexSign GetComplexSign(const Tree* e) {
  assert(Dimension::Get(e).isScalarOrUnit());
  if (e->isNumber()) {
    return ComplexSign(Number::Sign(e), Sign::Zero());
  } else if (e->isDecimal()) {
    return ComplexSign(Number::Sign(e->child(0)), Sign::Zero());
  } else if (e->isUserNamed()) {
    return Symbol::GetComplexSign(e);
  }
  switch (e->type()) {
    case Type::Mult: {
      ComplexSign s = ComplexSign::RealFiniteStrictlyPositiveInteger();  // 1
      for (const Tree* c : e->children()) {
        s = Mult(s, GetComplexSign(c));
        if (s.isUnknown() || s.isNull()) {
          break;
        }
      }
      return s;
    }
    case Type::Add: {
      ComplexSign s = ComplexSign::Zero();
      for (const Tree* c : e->children()) {
        s = Add(s, GetComplexSign(c));
        if (s.isUnknown()) {
          break;
        }
      }
      return s;
    }
    case Type::PowReal:
    case Type::Pow:
      return Power(GetComplexSign(e->child(0)), GetComplexSign(e->child(1)),
                   e->child(1)->isTwo());
    case Type::Norm:
      // Child isn't a scalar
      return ComplexSign(Sign::Positive(), Sign::Zero());
    case Type::Abs:
      return Abs(GetComplexSign(e->child(0)));
    case Type::Exp:
      return Exponential(GetComplexSign(e->child(0)));
    case Type::Ln:
      return Ln(GetComplexSign(e->child(0)));
    case Type::Re:
      return ComplexSign(GetComplexSign(e->child(0)).realSign(), Sign::Zero());
    case Type::Im:
      return ComplexSign(GetComplexSign(e->child(0)).imagSign(), Sign::Zero());
    case Type::Var:
      return Variables::GetComplexSign(e);
    case Type::ComplexI:
      return ComplexSign(Sign::Zero(), Sign::FiniteStrictlyPositiveInteger());
    case Type::Trig:
      assert(e->child(1)->isOne() || e->child(1)->isZero());
      return Trig(GetComplexSign(e->child(0)), e->child(1)->isOne());
    case Type::ATanRad:
      return ArcTangent(GetComplexSign(e->child(0)));
    case Type::Arg:
      return ComplexArgument(GetComplexSign(e->child(0)));
    case Type::Dep:
      return GetComplexSign(Dependency::Main(e));
    case Type::Inf:
      return ComplexSign(Sign::StrictlyPositive(), Sign::Zero());
    case Type::PhysicalConstant:
    case Type::Unit:
      // Units are considered equivalent to their SI ratio
      return ComplexSign(Sign::FinitePositive(), Sign::Zero());
    case Type::ATrig:
      assert(e->child(1)->isOne() || e->child(1)->isZero());
      return e->child(1)->isOne() ? ArcSine(GetComplexSign(e->child(0)))
                                  : ArcCosine(GetComplexSign(e->child(0)));
    case Type::Ceil:
    case Type::Floor:
    case Type::Frac:
    case Type::Round:
      return DecimalFunction(GetComplexSign(e->child(0)), e->type());
    case Type::Random:
      return ComplexSign(Sign::FinitePositive(), Sign::Zero());
    case Type::RandInt:
      return ComplexSign(Sign::FiniteInteger(), Sign::Zero());
#if 0
    // Activate these cases if necessary
    case Type::ATan:
      return ArcTangent(GetComplexSign(e->child(0)));
    case Type::Fact:
      assert(GetComplexSign(e->child(0)) ==
             ComplexSign(Sign::PositiveInteger(), Sign::Zero()));
      return ComplexSign::RealStrictlyPositiveInteger();
    case Type::PercentSimple:
      return RelaxIntegerProperty(GetComplexSign(e->child(0)));
    case Type::MixedFraction:
      return Add(GetComplexSign(e->child(0)), GetComplexSign(e->child(1)));
    case Type::Parentheses:
      return GetComplexSign(e->child(0));
#endif
    default:
      return ComplexSign::Unknown();
  }
}

Sign GetSign(const Tree* e) {
  assert(GetComplexSign(e).isReal());
  return GetComplexSign(e).realSign();
}

ComplexSign ComplexSignOfDifference(const Tree* e1, const Tree* e2) {
  Tree* difference = PatternMatching::CreateSimplify(KAdd(KA, KMult(-1_e, KB)),
                                                     {.KA = e1, .KB = e2});
  ComplexSign result = GetComplexSign(difference);
  if (AdvancedReduction::DeepExpandAlgebraic(difference)) {
    /* We do not use advance reduction here but it might be useful to expand
     * Mult since we are creating an Add with Mult */
    result = result && GetComplexSign(difference);
  }
  difference->removeTree();
  return result;
}

ComplexSign SignOfTreeOrApproximation(const Tree* e) {
  assert(Dimension::IsNonListScalar(e));
  ComplexSign sign = GetComplexSign(e);
  if (!sign.realSign().hasKnownStrictSign() ||
      !sign.imagSign().hasKnownStrictSign()) {
    Tree* approximation =
        Approximation::ToTree<double>(e, Approximation::Parameters{});
    sign = GetComplexSign(approximation);
    approximation->removeTree();
  }
  return sign;
}

}  // namespace Internal

#if POINCARE_TREE_LOG
void ComplexSign::log(std::ostream& stream, bool endOfLine) const {
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
