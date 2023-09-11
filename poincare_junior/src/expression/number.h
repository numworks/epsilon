#ifndef POINCARE_EXPRESSION_NUMBER_H
#define POINCARE_EXPRESSION_NUMBER_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "sign.h"

namespace PoincareJ {

class Number {
 public:
  /* If one of these asserts cannot be maintained, either numbers should be
   * sanitized on systematicReduce or these method should handle sub-optimal
   * numbers (such as zero stored on a IntegerPosBig. More generally, many
   * features such as pattern matching depends on unique number representation.
   */
  static bool IsZero(const Tree* t) {
    assert(IsSanitized(t));
    return t->type() == BlockType::Zero;
  }
  static bool IsOne(const Tree* t) {
    assert(IsSanitized(t));
    return t->type() == BlockType::One;
  }
  static bool IsMinusOne(const Tree* t) {
    assert(IsSanitized(t));
    return t->type() == BlockType::MinusOne;
  }
  static bool IsTwo(const Tree* t) {
    assert(IsSanitized(t));
    return t->type() == BlockType::Two;
  }
  static bool IsHalf(const Tree* t) {
    assert(IsSanitized(t));
    return t->type() == BlockType::Half;
  }
  static bool IsStrictRational(const Tree* t) {
    assert(IsSanitized(t));
    return t->type().isOfType({BlockType::Half, BlockType::RationalShort,
                               BlockType::RationalNegBig,
                               BlockType::RationalPosBig});
  }

  static EditionReference Addition(const Tree* i, const Tree* j);
  static EditionReference Multiplication(const Tree* i, const Tree* j);
  static Sign::Sign Sign(const Tree* node);
  // Return false if tree is a number and isn't the unique best representation.
  static bool IsSanitized(const Tree* n);
};

}  // namespace PoincareJ

#endif
