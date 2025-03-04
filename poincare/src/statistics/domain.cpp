#include "domain.h"

#include <omg/troolean.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/expression/sign.h>

namespace Poincare::Internal::Domain {

template <>
OMG::Troolean Contains<const Tree*>(const Tree* e, Type type) {
  if (e->isUndefined() || e->isInf()) {
    return OMG::Troolean::False;
  }

  ComplexSign sign = GetComplexSign(e);

  if (!sign.isReal()) {
    return OMG::Troolean::Unknown;
  }

  OMG::Troolean isPositive = sign.realSign().isPositive() ? OMG::Troolean::True
                             : sign.realSign().canBeStrictlyPositive()
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

  if (!e->isRational()) {
    // TODO we could leverage sign analysis to give an anwser on some domains
    return OMG::Troolean::Unknown;
  }

  if (type & k_onlyIntegers && !e->isInteger()) {
    return OMG::Troolean::False;
  }

  if (type & k_nonZero && e->isZero()) {
    return OMG::Troolean::False;
  }

  if (type & (ZeroToOne | ZeroExcludedToOne | ZeroExcludedToOneExcluded) &&
      Rational::IsGreaterThanOne(e)) {
    assert(Rational::Sign(e).isPositive());
    return OMG::Troolean::False;
  }

  if (type == ZeroExcludedToOneExcluded && e->isOne()) {
    return OMG::Troolean::False;
  }

  return OMG::Troolean::True;
}

template <>
OMG::Troolean Contains<float>(const float val, Type domain) {
  return OMG::BoolToTroolean(ContainsFloat(val, domain));
}

template <>
OMG::Troolean Contains<double>(const double val, Type domain) {
  return OMG::BoolToTroolean(ContainsFloat(val, domain));
}

template <>
OMG::Troolean IsAGreaterThanB<float>(const float a, const float b,
                                     bool strictly) {
  return OMG::BoolToTroolean(a > b || (!strictly && a == b));
}

template <>
OMG::Troolean IsAGreaterThanB<double>(const double a, const double b,
                                      bool strictly) {
  return OMG::BoolToTroolean(a > b || (!strictly && a == b));
}

template <>
OMG::Troolean IsAGreaterThanB<const Tree*>(const Tree* a, const Tree* b,
                                           bool strictly) {
  // TODO_PCJ: If needed this can be implemented
  return OMG::Troolean::Unknown;
}

}  // namespace Poincare::Internal::Domain
