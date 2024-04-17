#include "dimension_vector.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

#include "approximation.h"
#include "integer.h"
#include "unit.h"

namespace Poincare::Internal {

namespace Units {
DimensionVector DimensionVector::FromBaseUnits(const Tree* baseUnits) {
  /* Returns the vector of Base units with integer exponents. If rational, the
   * closest integer will be used. */
  DimensionVector vector = Empty();
  int numberOfFactors;
  int factorIndex = 0;
  const Tree* factor;
  if (baseUnits->isMult()) {
    numberOfFactors = baseUnits->numberOfChildren();
    factor = baseUnits->child(0);
  } else {
    numberOfFactors = 1;
    factor = baseUnits;
  }
  do {
    // Get the unit's exponent
    int8_t exponent = 1;
    if (factor->isPow()) {
      const Tree* exp = factor->child(1);
      assert(exp->isRational());
      // Using the closest integer to the exponent.
      float exponentFloat = Approximation::To<float>(exp);
      if (exponentFloat != std::round(exponentFloat)) {
        /* If non-integer exponents are found, we round a null vector so that
         * Multiplication::shallowBeautify will not attempt to find derived
         * units. */
        return Empty();
      }
      /* We limit to INT_MAX / 3 because an exponent might get bigger with
       * simplification. As a worst case scenario, (_s²_m²_kg/_A²)^n should be
       * simplified to (_s^5_S)^n. If 2*n is under INT_MAX, 5*n might not. */
      if (std::fabs(exponentFloat) < INT8_MAX / 3) {
        // Exponent can be safely casted as int
        exponent = static_cast<int8_t>(std::round(exponentFloat));
        assert(std::fabs(exponentFloat - static_cast<float>(exponent)) <= 0.5f);
      } else {
        /* Base units vector will ignore this coefficient, to avoid exponent
         * overflow. In any way, shallowBeautify will conserve homogeneity. */
        exponent = 0;
      }
      factor = factor->child(0);
    }
    // Fill the vector with the unit's exponent
    assert(factor->isUnit());
    vector.addAllCoefficients(
        Unit::GetRepresentative(factor)->dimensionVector(), exponent);
    if (++factorIndex >= numberOfFactors) {
      break;
    }
    factor = baseUnits->child(factorIndex);
  } while (true);
  return vector;
}

Tree* DimensionVector::toBaseUnits() const {
  Tree* result = SharedTreeStack->push<Type::Mult>(0);
  int numberOfChildren = 0;
  for (int i = 0; i < k_numberOfBaseUnits; i++) {
    // We require the base units to be the first seven in DefaultRepresentatives
    const Representative* representative =
        Representative::DefaultRepresentatives()[i];
    assert(representative);
    int8_t exponent = coefficientAtIndex(i);
    if (exponent == 0) {
      continue;
    }
    if (exponent != 1) {
      SharedTreeStack->push(Type::Pow);
    }
    Unit::Push(representative);
    if (exponent != 1) {
      Integer::Push(exponent);
    }
    NAry::SetNumberOfChildren(result, ++numberOfChildren);
  }
  // assert(numberOfChildren > 0);
  NAry::SquashIfUnary(result) || NAry::SquashIfEmpty(result);
  return result;
}

}  // namespace Units
}  // namespace Poincare::Internal
