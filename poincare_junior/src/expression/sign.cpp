#include "sign.h"

#include "dimension.h"
#include "number.h"

namespace PoincareJ {

namespace Sign {

Sign Mult(Sign s1, Sign s2) {
  return Make(MayBeNull(s1) || MayBeNull(s2),
              (MayBePos(s1) && MayBePos(s2)) || (MayBeNeg(s1) && MayBeNeg(s2)),
              (MayBePos(s1) && MayBeNeg(s2)) || (MayBeNeg(s1) && MayBePos(s2)));
}

Sign Add(Sign s1, Sign s2) {
  return Make((MayBeNull(s1) && MayBeNull(s2)) ||
                  (MayBePos(s1) && MayBeNeg(s2)) ||
                  (MayBeNeg(s1) && MayBePos(s2)),
              MayBePos(s1) || MayBePos(s2), MayBeNeg(s1) || MayBeNeg(s2));
}

Sign GetSign(const Tree* t) {
  assert(Dimension::GetDimension(t).isScalar());
  if (t->block()->isNumber()) {
    StrictSign s = Number::StrictSign(t);
    return Make(s == StrictSign::Null, s == StrictSign::Positive,
                s == StrictSign::Negative);
  }
  switch (t->type()) {
    case BlockType::Multiplication: {
      Sign s = Sign::Positive;
      for (const Tree* c : t->children()) {
        s = Mult(s, GetSign(c));
        if (s == Sign::Unknown) {
          break;
        }
      }
      return s;
    }
    case BlockType::Addition: {
      Sign s = Sign::Null;
      for (const Tree* c : t->children()) {
        s = Add(s, GetSign(c));
        if (s == Sign::Unknown) {
          break;
        }
      }
      return s;
    }
    case BlockType::Power: {
      Sign s = GetSign(t->firstChild());
      return Make(MayBeNull(s), true, MayBeNeg(s));
    }
    case BlockType::Abs:
    case BlockType::Norm: {
      Sign s = GetSign(t->firstChild());
      return Make(MayBeNull(s), MayBePos(s) || MayBeNeg(s), false);
    }
    case BlockType::ArcCosine:
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
    case BlockType::Factorial:
      return GetSign(t->firstChild());
    case BlockType::Exponential:
      return Sign::Positive;
    default:
      return Sign::Unknown;
  }
}

}  // namespace Sign
}  // namespace PoincareJ
