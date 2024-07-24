#include "sign.h"

#include <poincare/src/memory/pattern_matching.h>

#include "advanced_reduction.h"
#include "dependency.h"
#include "dimension.h"
#include "number.h"
#include "symbol.h"
#include "variables.h"

namespace Poincare::Internal {

/* Must at least handle Additions, Multiplications, Numbers and Real/Imaginary
 * parts so that any simplified complex is sanitized. Also handle Exp, Ln and
 * Powers of positive integers so that abs(z) remains real after reduction.
 * Handling trig, exp, ln, abs and arg may also greatly help the polar complex
 * mode simplifications. */

Sign RelaxIntegerProperty(Sign s) {
  return Sign(s.canBeNull(), s.canBeStrictlyPositive(),
              s.canBeStrictlyNegative());
}

Sign DecimalFunction(Sign s, Type type) {
  bool canBeNull = s.canBeNull();
  bool canBeStrictlyPositive = s.canBeStrictlyPositive();
  bool canBeStrictlyNegative = s.canBeStrictlyNegative();
  bool canBeNonInteger = s.canBeNonInteger();
  switch (type) {
    case Type::Ceil:
      canBeNull |= canBeStrictlyNegative && canBeNonInteger;
      canBeNonInteger = false;
      break;
    case Type::Floor:
      canBeNull |= canBeStrictlyPositive && canBeNonInteger;
      canBeNonInteger = false;
      break;
    case Type::Frac:
      canBeNull = true;
      canBeStrictlyPositive = canBeNonInteger;
      canBeStrictlyNegative = false;
      break;
    case Type::Round:
      canBeNull = true;
      break;
    default:
      assert(false);
  }
  return Sign(canBeNull, canBeStrictlyPositive, canBeStrictlyNegative,
              canBeNonInteger);
}

Sign Opposite(Sign s) {
  return Sign(s.canBeNull(), s.canBeStrictlyNegative(),
              s.canBeStrictlyPositive(), s.canBeNonInteger());
}

Sign Mult(Sign s1, Sign s2) {
  return Sign(s1.canBeNull() || s2.canBeNull(),
              (s1.canBeStrictlyPositive() && s2.canBeStrictlyPositive()) ||
                  (s1.canBeStrictlyNegative() && s2.canBeStrictlyNegative()),
              (s1.canBeStrictlyPositive() && s2.canBeStrictlyNegative()) ||
                  (s1.canBeStrictlyNegative() && s2.canBeStrictlyPositive()),
              s1.canBeNonInteger() || s2.canBeNonInteger());
}

Sign Add(Sign s1, Sign s2) {
  return Sign((s1.canBeNull() && s2.canBeNull()) ||
                  (s1.canBeStrictlyPositive() && s2.canBeStrictlyNegative()) ||
                  (s1.canBeStrictlyNegative() && s2.canBeStrictlyPositive()),
              s1.canBeStrictlyPositive() || s2.canBeStrictlyPositive(),
              s1.canBeStrictlyNegative() || s2.canBeStrictlyNegative(),
              s1.canBeNonInteger() || s2.canBeNonInteger());
}

Sign Sign::Get(const Tree* e) {
  assert(ComplexSign::Get(e).isReal());
  return ComplexSign::Get(e).realSign();
}

#if POINCARE_TREE_LOG
void Sign::log(std::ostream& stream, bool endOfLine) const {
  if (isNull()) {
    stream << "Zero";
  } else {
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

ComplexSign RelaxIntegerProperty(ComplexSign s) {
  return ComplexSign(RelaxIntegerProperty(s.realSign()),
                     RelaxIntegerProperty(s.imagSign()));
}

ComplexSign Abs(ComplexSign s) {
  return ComplexSign(Sign(s.canBeNull(), !s.isNull(), false,
                          s.canBeNonInteger() || !s.isPure()),
                     Sign::Zero());
}

ComplexSign ArcCosine(ComplexSign s) {
  Sign re = s.realSign();
  Sign im = s.imagSign();
  return ComplexSign(Sign(re.canBeStrictlyPositive(), true, false),
                     Sign(im.canBeNull(),
                          im.canBeStrictlyNegative() ||
                              (im.canBeNull() && re.canBeStrictlyPositive()),
                          im.canBeStrictlyPositive() ||
                              (im.canBeNull() && re.canBeStrictlyNegative())));
}

ComplexSign ArcSine(ComplexSign s) {
  /* - the sign of re(actan(z)) is always the same as re(z)
   * - the sign of im(actan(z)) is always the same as im(z) except when re(z)!=0
   *   and im(z)=0: im(asin(x)) = {>0 if x<-1, =0 if -1<=x<=1, and <0 if x>1} */
  Sign realSign = RelaxIntegerProperty(s.realSign());
  Sign imagSign = RelaxIntegerProperty(s.imagSign());
  if (imagSign.canBeNull() && realSign.canBeNonNull()) {
    imagSign = imagSign || Sign(true, realSign.canBeStrictlyNegative(),
                                realSign.canBeStrictlyPositive());
  }
  return ComplexSign(realSign, imagSign);
}

ComplexSign ArcTangent(ComplexSign s) {
  /* - the sign of im(actan(z)) is always the same as im(z)
   * - the sign of re(actan(z)) is always the same as re(z) except when im(z)!=0
       z=i*y: re(atan(i*y)) = {-π/2 if y<-1, 0 if -1<y<1, and π/2 if y>1} */
  Sign realSign = RelaxIntegerProperty(s.realSign());
  Sign imagSign = RelaxIntegerProperty(s.imagSign());
  if (realSign.canBeNull() && imagSign.canBeNonNull()) {
    realSign = realSign || Sign(true, imagSign.canBeStrictlyPositive(),
                                imagSign.canBeStrictlyNegative());
  }
  return ComplexSign(realSign, imagSign);
}

ComplexSign Exponential(ComplexSign s) {
  return s.isReal() ? ComplexSign::RealStrictlyPositive()
                    : ComplexSign::Unknown();
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
           im.canBeStrictlyNegative()),
      Sign::Zero());
}

ComplexSign Ln(ComplexSign s) {
  /* z = |z|e^(i*arg(z))
   * re(ln(z)) = ln(|z|)
   * im(ln(z)) = arg(z) */
  return ComplexSign(Sign::Unknown(), ComplexArgument(s).realSign());
}

ComplexSign DecimalFunction(ComplexSign s, Type type) {
  return ComplexSign(DecimalFunction(s.realSign(), type),
                     DecimalFunction(s.imagSign(), type));
}

ComplexSign Trig(ComplexSign s, bool isSin) {
  if (s.isPureIm()) {
    return isSin ? ComplexSign(Sign::Zero(), RelaxIntegerProperty(s.imagSign()))
                 : ComplexSign::RealStrictlyPositive();
  }
  return ComplexSign(Sign::Unknown(),
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
    return ComplexSign::RealStrictlyPositiveInteger();
  }
  bool canBeNull = base.realSign().canBeNull();
  bool canBeNonInteger = base.canBeNonInteger() || !exp.realSign().isPositive();
  if (base.isReal()) {
    bool isPositive = expIsTwo || base.realSign().isPositive();
    return ComplexSign(Sign(canBeNull, true, !isPositive, canBeNonInteger),
                       Sign::Zero());
  }
  Sign sign = Sign(true, true, true, canBeNonInteger);
  return ComplexSign(sign, sign);
}

// Note: A complex function plotter can be used to fill in these methods.
ComplexSign ComplexSign::Get(const Tree* e) {
  assert(Dimension::Get(e).isScalarOrUnit());
  if (e->isNumber()) {
    return ComplexSign(Number::Sign(e), Sign::Zero());
  } else if (e->isUserNamed()) {
    return Symbol::GetComplexSign(e);
  }
  switch (e->type()) {
    case Type::Mult: {
      ComplexSign s = RealStrictlyPositiveInteger();  // 1
      for (const Tree* c : e->children()) {
        s = Mult(s, Get(c));
        if (s.isUnknown() || s.isNull()) {
          break;
        }
      }
      return s;
    }
    case Type::Add: {
      ComplexSign s = Zero();
      for (const Tree* c : e->children()) {
        s = Add(s, Get(c));
        if (s.isUnknown()) {
          break;
        }
      }
      return s;
    }
    case Type::PowReal:
    case Type::Pow:
      return Power(Get(e->child(0)), Get(e->child(1)), e->child(1)->isTwo());
    case Type::Norm:
      // Child isn't a scalar
      return ComplexSign(Sign::Positive(), Sign::Zero());
    case Type::Abs:
      return Abs(Get(e->child(0)));
    case Type::Exp:
      return Exponential(Get(e->child(0)));
    case Type::Ln:
      return Ln(Get(e->child(0)));
    case Type::Re:
      return ComplexSign(Get(e->child(0)).realSign(), Sign::Zero());
    case Type::Im:
      return ComplexSign(Get(e->child(0)).imagSign(), Sign::Zero());
    case Type::Var:
      return Variables::GetComplexSign(e);
    case Type::ComplexI:
      return ComplexSign(Sign::Zero(), Sign::StrictlyPositiveInteger());
    case Type::Trig:
      assert(e->child(1)->isOne() || e->child(1)->isZero());
      return Trig(Get(e->child(0)), e->child(1)->isOne());
    case Type::ATanRad:
      return ArcTangent(Get(e->child(0)));
    case Type::Arg:
      return ComplexArgument(Get(e->child(0)));
    case Type::Dependency:
      return Get(Dependency::Main(e));
    case Type::Inf:
      return ComplexSign(Sign::StrictlyPositive(), Sign::Zero());
    case Type::PhysicalConstant:
    case Type::Unit:
      // Units are considered equivalent to their SI ratio
      return ComplexSign(Sign::Positive(), Sign::Zero());
#if 0
    // Activate these cases if necessary
    case Type::ACos:
      return ArcCosine(Get(e->child(0)));
    case Type::ASin:
      return ArcSine(Get(e->child(0)));
    case Type::ATan:
      return ArcTangent(Get(e->child(0)));
    case Type::Fact:
      assert(Get(e->child(0)) == ComplexSign(Sign::PositiveInteger(), Sign::Zero()));
      return RealStrictlyPositiveInteger();
    case Type::Ceil:
    case Type::Floor:
    case Type::Frac:
    case Type::Round:
      return DecimalFunction(Get(e->child(0)), e->type());
    case Type::PercentSimple:
      return RelaxIntegerProperty(Get(e->child(0)));
    case Type::MixedFraction:
      return Add(Get(e->child(0)), Get(e->child(1)));
    case Type::Parenthesis:
      return Get(e->child(0));
#endif
    default:
      return Unknown();
  }
}

ComplexSign ComplexSign::SignOfDifference(const Tree* e1, const Tree* e2) {
  Tree* difference = PatternMatching::CreateSimplify(KAdd(KA, KMult(-1_e, KB)),
                                                     {.KA = e1, .KB = e2});
  ComplexSign result = Get(difference);
  if (AdvancedReduction::DeepExpand(difference)) {
    /* We do not use advance reduction here but it might be usefull to expand
     * Mult since we are creating an Add with Mult */
    result = result && Get(difference);
  }
  difference->removeTree();
  return result;
}

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

}  // namespace Poincare::Internal
