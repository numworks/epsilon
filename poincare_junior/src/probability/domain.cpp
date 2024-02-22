#include "domain.h"

#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/sign.h>

namespace PoincareJ {

Troolean Domain::ExpressionIsIn(const Tree *expression, Type type,
                                Context *context) {
  if (expression->isUndefined() || expression->isInfinity()) {
    return Troolean::False;
  }

  ComplexSign sign = ComplexSign::Get(expression);

  Troolean isPositive = sign.realSign().isPositive()      ? Troolean::True
                        : sign.realSign().canBePositive() ? Troolean::Unknown
                                                          : Troolean::False;
  if (type & k_onlyPositive) {
    if (isPositive != Troolean::True) {
      return isPositive;
    }
  }

  if (type & k_onlyNegative) {
    if (isPositive != Troolean::False) {
      return isPositive == Troolean::True ? Troolean::False : Troolean::Unknown;
    }
  }

  if (!sign.isReal()) {
    return Troolean::Unknown;
  }

  if (!expression->isRational()) {
    // TODO we could leverage sign analysis to give an anwser on some domains
    return Troolean::Unknown;
  }

  if (type & k_onlyIntegers && !expression->isInteger()) {
    return Troolean::False;
  }

  if (type & k_nonZero && expression->isZero()) {
    return Troolean::False;
  }

  if (type & (UnitSegment | LeftOpenUnitSegment | OpenUnitSegment) &&
      Rational::IsGreaterThanOne(expression)) {
    return Troolean::False;
  }

  return Troolean::True;
}

}  // namespace PoincareJ
