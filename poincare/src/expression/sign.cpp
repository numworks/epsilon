#include "sign.h"

#include <poincare/src/memory/pattern_matching.h>

#include "dependency.h"
#include "dimension.h"
#include "number.h"
#include "symbol.h"
#include "variables.h"

#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare::Internal {

/* Must at least handle Additions, Multiplications, Numbers and Real/Imaginary
 * parts so that any simplified complex is sanitized. Also handle Exp, Ln and
 * Powers of positive integers so that abs(z) remains real after reduction.
 * Handling trig, exp, ln, abs and arg may also greatly help the polar complex
 * mode simplifications. */

Sign RelaxIntegerProperty(Sign s) {
  return Sign(s.canBeNull(), s.canBeStriclyPositive(),
              s.canBeStriclyNegative());
}

Sign DecimalFunction(Sign s, Type type) {
  bool canBeNull = s.canBeNull();
  bool canBeStriclyPositive = s.canBeStriclyPositive();
  bool canBeStriclyNegative = s.canBeStriclyNegative();
  bool canBeNonInteger = s.canBeNonInteger();
  switch (type) {
    case Type::Ceil:
      canBeNull |= canBeStriclyNegative && canBeNonInteger;
      canBeNonInteger = false;
      break;
    case Type::Floor:
      canBeNull |= canBeStriclyPositive && canBeNonInteger;
      canBeNonInteger = false;
      break;
    case Type::Frac:
      canBeNull = true;
      canBeStriclyPositive = canBeNonInteger;
      canBeStriclyNegative = false;
      break;
    case Type::Round:
      canBeNull = true;
      break;
    default:
      assert(false);
  }
  return Sign(canBeNull, canBeStriclyPositive, canBeStriclyNegative,
              canBeNonInteger);
}

Sign Opposite(Sign s) {
  return Sign(s.canBeNull(), s.canBeStriclyNegative(), s.canBeStriclyPositive(),
              s.canBeNonInteger());
}

Sign Mult(Sign s1, Sign s2) {
  return Sign(s1.canBeNull() || s2.canBeNull(),
              (s1.canBeStriclyPositive() && s2.canBeStriclyPositive()) ||
                  (s1.canBeStriclyNegative() && s2.canBeStriclyNegative()),
              (s1.canBeStriclyPositive() && s2.canBeStriclyNegative()) ||
                  (s1.canBeStriclyNegative() && s2.canBeStriclyPositive()),
              s1.canBeNonInteger() || s2.canBeNonInteger());
}

Sign Add(Sign s1, Sign s2) {
  return Sign((s1.canBeNull() && s2.canBeNull()) ||
                  (s1.canBeStriclyPositive() && s2.canBeStriclyNegative()) ||
                  (s1.canBeStriclyNegative() && s2.canBeStriclyPositive()),
              s1.canBeStriclyPositive() || s2.canBeStriclyPositive(),
              s1.canBeStriclyNegative() || s2.canBeStriclyNegative(),
              s1.canBeNonInteger() || s2.canBeNonInteger());
}

Sign Sign::Get(const Tree* t) {
  assert(ComplexSign::Get(t).isReal());
  return ComplexSign::Get(t).realSign();
}

#if POINCARE_TREE_LOG
void Sign::log(bool endOfLine) const {
  if (isZero()) {
    std::cout << "Zero";
  } else {
    if (!m_canBeNonInteger) {
      std::cout << "Integer and ";
    }
    if (isUnknown()) {
      std::cout << "Unknown";
    } else {
      if (m_canBeStriclyPositive && m_canBeStriclyNegative) {
        std::cout << "Non Null";
      } else {
        if (!m_canBeNull) {
          std::cout << "Strictly ";
        }
        std::cout << (m_canBeStriclyNegative ? "Negative" : "Positive");
      }
    }
  }
  if (endOfLine) {
    std::cout << "\n";
  }
}
#endif

ComplexSign RelaxIntegerProperty(ComplexSign s) {
  return ComplexSign(RelaxIntegerProperty(s.realSign()),
                     RelaxIntegerProperty(s.imagSign()));
}

ComplexSign Abs(ComplexSign s) {
  return ComplexSign(Sign(s.canBeNull(), !s.isZero(), false,
                          s.canBeNonInteger() || !s.isPure()),
                     Sign::Zero());
}

ComplexSign ArcCosine(ComplexSign s) {
  Sign re = s.realSign();
  Sign im = s.imagSign();
  return ComplexSign(Sign(re.canBeStriclyPositive(), true, false),
                     Sign(im.canBeNull(),
                          im.canBeStriclyNegative() ||
                              (im.canBeNull() && re.canBeStriclyPositive()),
                          im.canBeStriclyPositive() ||
                              (im.canBeNull() && re.canBeStriclyNegative())));
}

ComplexSign ArcSine(ComplexSign s) {
  /* - the sign of re(actan(z)) is always the same as re(z)
   * - the sign of im(actan(z)) is always the same as im(z) except when re(z)!=0
   *   and im(z)=0: im(asin(x)) = {>0 if x<-1, =0 if -1<=x<=1, and <0 if x>1} */
  Sign realSign = RelaxIntegerProperty(s.realSign());
  Sign imagSign = RelaxIntegerProperty(s.imagSign());
  if (imagSign.canBeNull() && realSign.canBeNonNull()) {
    imagSign = imagSign || Sign(true, realSign.canBeStriclyNegative(),
                                realSign.canBeStriclyPositive());
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
    realSign = realSign || Sign(true, imagSign.canBeStriclyPositive(),
                                imagSign.canBeStriclyNegative());
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
      Sign(im.canBeNull() && (re.canBeNull() || re.canBeStriclyPositive()),
           (im.canBeNull() && re.canBeStriclyNegative()) ||
               im.canBeStriclyPositive(),
           im.canBeStriclyNegative()),
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
  if (!exp.isReal() || exp.canBeNonInteger()) {
    return ComplexSign::Unknown();
  }
  if (base.isZero()) {
    // 0^exp = 0
    return ComplexSign::Zero();
  }
  if (exp.isZero()) {
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
ComplexSign ComplexSign::Get(const Tree* t) {
  assert(Dimension::GetDimension(t).isScalar());
  if (t->isNumber()) {
    return ComplexSign(Number::Sign(t), Sign::Zero());
  }
  switch (t->type()) {
    case Type::Mult: {
      ComplexSign s = RealStrictlyPositiveInteger();  // 1
      for (const Tree* c : t->children()) {
        s = Mult(s, Get(c));
        if (s.isUnknown() || s.isZero()) {
          break;
        }
      }
      return s;
    }
    case Type::Add: {
      ComplexSign s = Zero();
      for (const Tree* c : t->children()) {
        s = Add(s, Get(c));
        if (s.isUnknown()) {
          break;
        }
      }
      return s;
    }
    case Type::PowReal:
    case Type::Pow:
      return Power(Get(t->firstChild()), Get(t->child(1)),
                   t->child(1)->isTwo());
    case Type::Norm:
      // Child isn't a scalar
      return ComplexSign(Sign::Positive(), Sign::Zero());
    case Type::Abs:
      return Abs(Get(t->firstChild()));
    case Type::Exp:
      return Exponential(Get(t->firstChild()));
    case Type::Ln:
      return Ln(Get(t->firstChild()));
    case Type::Re:
      return ComplexSign(Get(t->firstChild()).realSign(), Sign::Zero());
    case Type::Im:
      return ComplexSign(Get(t->firstChild()).imagSign(), Sign::Zero());
    case Type::Var:
      return Variables::GetComplexSign(t);
    case Type::ComplexI:
      return ComplexSign(Sign::Zero(), Sign::StrictlyPositiveInteger());
    case Type::Trig:
      assert(t->child(1)->isOne() || t->child(1)->isZero());
      return Trig(Get(t->firstChild()), t->child(1)->isOne());
    case Type::ATanRad:
      return ArcTangent(Get(t->firstChild()));
    case Type::Arg:
      return ComplexArgument(Get(t->firstChild()));
    case Type::Dependency:
      return Get(Dependency::Main(t));
    case Type::UserSymbol:
      return Symbol::GetComplexSign(t);
#if 0
    // Activate these cases if necessary
    case Type::ACos:
      return ArcCosine(Get(t->firstChild()));
    case Type::ASin:
      return ArcSine(Get(t->firstChild()));
    case Type::ATan:
      return ArcTangent(Get(t->firstChild()));
    case Type::Fact:
      assert(Get(t->firstChild()) == ComplexSign(Sign::PositiveInteger(), Sign::Zero()));
      return RealStrictlyPositiveInteger();
    case Type::Ceil:
    case Type::Floor:
    case Type::Frac:
    case Type::Round:
      return DecimalFunction(Get(t->firstChild()), t->type());
    case Type::PercentSimple:
      return RelaxIntegerProperty(Get(t->firstChild()));
    case Type::MixedFraction:
      return Add(Get(t->firstChild()), Get(t->child(1)));
#endif
    default:
      return Unknown();
  }
}

ComplexSign ComplexSign::SignOfDifference(const Tree* a, const Tree* b) {
  Tree* difference = PatternMatching::CreateSimplify(KAdd(KA, KMult(-1_e, KB)),
                                                     {.KA = a, .KB = b});
  ComplexSign result = Get(difference);
  difference->removeTree();
  return result;
}

#if POINCARE_TREE_LOG
void ComplexSign::log() const {
  std::cout << "(";
  realSign().log(false);
  std::cout << ") + i*(";
  imagSign().log(false);
  std::cout << ")\n";
}
#endif

}  // namespace Poincare::Internal
