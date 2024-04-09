#include "domain.h"

#include <poincare/src/expression/rational.h>
#include <poincare/src/expression/sign.h>

namespace Poincare::Internal {

OMG::Troolean Domain::ExpressionIsIn(const Tree* expression, Type type,
                                     Context* context) {
  if (expression->isUndef() || expression->isInf()) {
    return OMG::Troolean::False;
  }

  ComplexSign sign = ComplexSign::Get(expression);

  if (!sign.isReal()) {
    return OMG::Troolean::Unknown;
  }

  OMG::Troolean isPositive = sign.realSign().isPositive() ? OMG::Troolean::True
                             : sign.realSign().canBePositive()
                                 ? OMG::Troolean::Unknown
                                 : OMG::Troolean::False;
  if (type & k_onlyPositive) {
    if (isPositive != OMG::Troolean::True) {
      return isPositive;
    }
  }

  if (type & k_onlyNegative) {
    if (isPositive != OMG::Troolean::False) {
      return isPositive == OMG::Troolean::True ? OMG::Troolean::False
                                               : OMG::Troolean::Unknown;
    }
  }

  if (!expression->isRational()) {
    // TODO we could leverage sign analysis to give an anwser on some domains
    return OMG::Troolean::Unknown;
  }

  if (type & k_onlyIntegers && !expression->isInteger()) {
    return OMG::Troolean::False;
  }

  if (type & k_nonZero && expression->isZero()) {
    return OMG::Troolean::False;
  }

  if (type & (ZeroToOne | ZeroExcludedToOne | ZeroExcludedToOneExcluded) &&
      Rational::IsGreaterThanOne(expression)) {
    assert(Rational::Sign(expression).isPositive());
    return OMG::Troolean::False;
  }

  if (type == ZeroExcludedToOneExcluded && expression->isOne()) {
    return OMG::Troolean::False;
  }

  return OMG::Troolean::True;
}

}  // namespace Poincare::Internal
