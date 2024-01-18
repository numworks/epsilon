#include "sign.h"

#include "dimension.h"
#include "number.h"
#include "variables.h"

#if POINCARE_MEMORY_TREE_LOG
#include <iostream>
#endif

namespace PoincareJ {

/* Must at least handle Addition, Multiplications, Numbers and Real/Imaginary
 * parts so that any simplified complex is sanitized. Also handle Exp, Ln and
 * Powers of positive integers so that abs(z) remains real after reduction.
 * Handling trig, exp, ln, abs and arg may also greatly help the polar complex
 * mode simplifications. */

Sign NoIntegers(Sign s) {
  return Sign(s.canBeNull(), s.canBeNegative(), s.canBePositive());
}

Sign DecimalFunction(Sign s, BlockType type) {
  bool canBeNull = s.canBeNull();
  bool canBePositive = s.canBePositive();
  bool canBeNegative = s.canBeNegative();
  bool isInteger = s.isInteger();
  switch (type) {
    case BlockType::Ceiling:
      canBeNull |= canBeNegative;
      isInteger = true;
      break;
    case BlockType::Floor:
      canBeNull |= canBePositive;
      isInteger = true;
      break;
    case BlockType::FracPart:
      canBeNull = true;
      canBePositive = !isInteger;
      canBeNegative = false;
    case BlockType::Round:
      canBeNull = true;
    default:
      assert(false);
  }
  return Sign(canBeNull, canBePositive, canBeNegative, isInteger);
}

Sign Oppose(Sign s) {
  return Sign(s.canBeNull(), s.canBeNegative(), s.canBePositive(),
              s.isInteger());
}

Sign Mult(Sign s1, Sign s2) {
  return Sign(s1.canBeNull() || s2.canBeNull(),
              (s1.canBePositive() && s2.canBePositive()) ||
                  (s1.canBeNegative() && s2.canBeNegative()),
              (s1.canBePositive() && s2.canBeNegative()) ||
                  (s1.canBeNegative() && s2.canBePositive()),
              s1.isInteger() && s2.isInteger());
}

Sign Add(Sign s1, Sign s2) {
  return Sign((s1.canBeNull() && s2.canBeNull()) ||
                  (s1.canBePositive() && s2.canBeNegative()) ||
                  (s1.canBeNegative() && s2.canBePositive()),
              s1.canBePositive() || s2.canBePositive(),
              s1.canBeNegative() || s2.canBeNegative(),
              s1.isInteger() && s2.isInteger());
}

Sign Sign::Get(const Tree* t) {
  assert(ComplexSign::Get(t).isReal());
  return ComplexSign::Get(t).realSign();
}

#if POINCARE_MEMORY_TREE_LOG
void Sign::log(bool endOfLine) const {
  if (isZero()) {
    std::cout << "Zero";
  } else {
    if (m_isInteger) {
      std::cout << "Integer and ";
    }
    if (isUnknown()) {
      std::cout << "Unknown";
    } else {
      if (m_canBePositive && m_canBeNegative) {
        std::cout << "Non Null";
      } else {
        std::cout << (m_canBeNegative ? "Negative" : "Positive");
        if (m_canBeNull) {
          std::cout << " or Null";
        }
      }
    }
  }
  if (endOfLine) {
    std::cout << "\n";
  }
}
#endif

ComplexSign NoIntegers(ComplexSign s) {
  return ComplexSign(NoIntegers(s.realSign()), NoIntegers(s.imagSign()));
}

ComplexSign Abs(ComplexSign s) {
  return ComplexSign(
      Sign(s.canBeNull(), !s.isZero(), false,
           s.isInteger() && (s.isReal() || s.realSign().isZero())),
      Sign::Zero());
}

ComplexSign ArcCosine(ComplexSign s) {
  return ComplexSign(
      Sign(s.realSign().canBePositive(), true, false),
      Sign(s.imagSign().canBeNull(),
           s.imagSign().canBeNegative() ||
               (s.imagSign().canBeNull() && s.realSign().canBePositive()),
           s.imagSign().canBePositive() ||
               (s.imagSign().canBeNull() && s.realSign().canBeNegative())));
}

ComplexSign Exponential(ComplexSign s) {
  bool childIsReal = s.isReal();
  return ComplexSign(Sign(!childIsReal, childIsReal, !childIsReal),
                     Sign(true, !childIsReal, !childIsReal));
}

ComplexSign Ln(ComplexSign s) {
  bool lnIsReal = s.isReal() && s.realSign().isPositive();
  return ComplexSign(Sign::Unknown(),
                     lnIsReal ? Sign::Zero()
                              : Sign(false, !s.imagSign().isStrictlyNegative(),
                                     s.imagSign().canBeNegative()));
}

ComplexSign ComplexArgument(ComplexSign s) {
  return ComplexSign(
      Sign(s.imagSign().canBeNull() && s.realSign().canBePositive(), true,
           true),
      Sign::Zero());
}

ComplexSign DecimalFunction(ComplexSign s, BlockType type) {
  return ComplexSign(DecimalFunction(s.realSign(), type),
                     DecimalFunction(s.imagSign(), type));
}

ComplexSign Trig(ComplexSign s, bool isSin) {
  if (s.realSign().isZero()) {
    return isSin ? ComplexSign(Sign::Zero(), s.imagSign())
                 : ComplexSign(Sign::Positive(), Sign::Zero());
  }
  return ComplexSign(Sign::Unknown(),
                     s.isReal() ? Sign::Zero() : Sign::Unknown());
}

ComplexSign Mult(ComplexSign s1, ComplexSign s2) {
  Sign a = Add(Mult(s1.realSign(), s2.realSign()),
               Oppose(Mult(s1.imagSign(), s2.imagSign())));
  Sign b = Add(Mult(s1.realSign(), s2.imagSign()),
               Mult(s1.imagSign(), s2.realSign()));
  return ComplexSign(a, b);
}

ComplexSign Add(ComplexSign s1, ComplexSign s2) {
  return ComplexSign(Add(s1.realSign(), s2.realSign()),
                     Add(s1.imagSign(), s2.imagSign()));
}

ComplexSign Power(ComplexSign base, ComplexSign exp) {
  // If this assert can't be maintained, escape with Unknown.
  assert(exp.isReal() && exp.isInteger());
  if (base.isZero()) {
    return ComplexSign::ComplexZero();
  }
  if (exp.isZero()) {
    return ComplexSign::ComplexOne();
  }
  bool isInteger = (base.isInteger() && !exp.realSign().isPositive());
  bool baseIsReal = base.isReal();
  return ComplexSign(
      Sign(base.realSign().canBeNull(), true,
           !(baseIsReal && base.realSign().isPositive()), isInteger),
      Sign(base.imagSign().canBeNull(), !baseIsReal, !baseIsReal, isInteger));
}

ComplexSign Complex(ComplexSign s0, ComplexSign s1) {
  assert(s0.isReal() && s1.isReal());
  return ComplexSign(s0.realSign(), s1.realSign());
}

// Note: A complex function plotter can be used to fill in these methods.
ComplexSign ComplexSign::Get(const Tree* t) {
  assert(Dimension::GetDimension(t).isScalar());
  if (t->isNumber()) {
    return ComplexSign(Number::Sign(t), Sign::Zero());
  }
  switch (t->type()) {
    case BlockType::Multiplication: {
      ComplexSign s = ComplexOne();
      for (const Tree* c : t->children()) {
        s = Mult(s, Get(c));
        if (s.isUnknown() || s.isZero()) {
          break;
        }
      }
      return s;
    }
    case BlockType::Addition: {
      ComplexSign s = ComplexZero();
      for (const Tree* c : t->children()) {
        s = Add(s, Get(c));
        if (s.isUnknown()) {
          break;
        }
      }
      return s;
    }
    case BlockType::PowerReal:
    case BlockType::Power:
      return Power(Get(t->firstChild()), Get(t->child(1)));
    case BlockType::Norm:
      // Child isn't a scalar
      return ComplexSign(Sign::PositiveOrNull(), Sign::Zero());
    case BlockType::Abs:
      return Abs(Get(t->firstChild()));
    case BlockType::Exponential:
      return Exponential(Get(t->firstChild()));
    case BlockType::Ln:
      return Ln(Get(t->firstChild()));
    case BlockType::RealPart:
      return ComplexSign(Get(t->firstChild()).realSign(), Sign::Zero());
    case BlockType::ImaginaryPart:
      return ComplexSign(Get(t->firstChild()).imagSign(), Sign::Zero());
    case BlockType::Variable:
      return Variables::GetComplexSign(t);
    case BlockType::Complex:
      return Complex(Get(t->firstChild()), Get(t->child(1)));
    case BlockType::Trig:
      assert(t->child(1)->isOne() || t->child(1)->isZero());
      return Trig(Get(t->firstChild()), t->child(1)->isOne());
    case BlockType::ComplexArgument:
      return ComplexArgument(Get(t->firstChild()));
#if 0
    // Activate these cases if necessary
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
      // Both real and imaginary part keep the same sign
      return NoIntegers(Get(t->firstChild()));
    case BlockType::ArcCosine:
      return ArcCosine(Get(t->firstChild()));
    case BlockType::Factorial:
      assert(Get(t->firstChild()).isReal() && Get(t->firstChild()).isInteger());
      return ComplexOne();
    case BlockType::Ceiling:
    case BlockType::Floor:
    case BlockType::FracPart:
    case BlockType::Round:
      return DecimalFunction(Get(t->firstChild()), t->type());
#endif
    default:
      return ComplexUnknown();
  }
}

#if POINCARE_MEMORY_TREE_LOG
void ComplexSign::log() const {
  std::cout << "(";
  realSign().log(false);
  std::cout << ") + i*(";
  imagSign().log(false);
  std::cout << ")\n";
}
#endif

}  // namespace PoincareJ
