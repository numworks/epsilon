// TODO: replace with config flavor
#if !POINCARE_UNIT_ONLY_ANGLES

#include "best_unit_selection.h"

#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "../approximation.h"
#include "prefix.h"
#include "representative.h"
#include "representatives.h"

namespace Poincare::Internal {
namespace Units {

/* This function tries to simplify a unit ('unitCoefficients') by multiplying
 * with another unit ('simplifyingUnitCoefficients') at a given exponent
 * ('simplifyingUnitExponent'). If the result of the operation is simpler,
 * 'unitSupportSize', 'bestRemainderExponents' and 'bestRemainderSupportSize'
 * are updated accordingly. */
static bool CanSimplifyUnitProduct(const SIVector* unitCoefficients,
                                   size_t& unitSupportSize,
                                   const SIVector* simplifyingUnitCoefficients,
                                   int simplifyingUnitExponent,
                                   SIVector& bestRemainderExponents,
                                   size_t& bestRemainderSupportSize) {
  SIVector simplifiedExponents;

  for (size_t i = 0; i < SIVector::k_numberOfBaseUnits; i++) {
    // Simplify unitCoefficients with base units from derived unit
    if (!simplifiedExponents.setCoefficientAtIndex(
            i, unitCoefficients->coefficientAtIndex(i) +
                   simplifyingUnitExponent *
                       simplifyingUnitCoefficients->coefficientAtIndex(i))) {
      // Unit vector overflowed
      return false;
    }
  }
  size_t simplifiedSupportSize = simplifiedExponents.supportSize();
  /* Note: A metric is considered simpler if the support size (number of
   * symbols) is reduced. A norm taking coefficients into account is possible.
   * One could use the sum of all coefficients to favor _C_s from _A_s^2.
   * However, replacing _m_s^-2 with _N_kg^-1 should be avoided. */
  bool isSimpler = (1 + simplifiedSupportSize < unitSupportSize);

  if (isSimpler) {
    bestRemainderExponents = simplifiedExponents;
    bestRemainderSupportSize = simplifiedSupportSize;
    /* unitSupportSize is updated and will be taken into
     * account in next iterations of CanSimplifyUnitProduct. */
    unitSupportSize = 1 + simplifiedSupportSize;
  }
  return isSimpler;
}

Tree* ChooseBestDerivedUnits(SIVector* unitCoefficients) {
  /* Recognize derived units
   * - Look up in the table of derived units, the one which itself or its
   * inverse simplifies 'units' the most.
   * - If an entry is found, simplify 'units' and add the corresponding unit
   * or its inverse in 'unitsAccu'.
   * - Repeat those steps until no more simplification is possible.
   */
  Tree* unitsAccu = KMult()->cloneTree();
  /* If exponents are not integers, GetSIVector will return a null
   * vector, preventing any attempt at simplification. This protects us
   * against undue "simplifications" such as _C^1.3 -> _C*_A^0.3*_s^0.3 */
  size_t unitSupportSize = unitCoefficients->supportSize();
  SIVector bestRemainderExponents;
  size_t bestRemainderSupportSize;
  while (unitSupportSize > 1) {
    const Representative* bestDim = nullptr;
    int8_t bestUnitExponent = 0;
    // Look up in the table of derived units.
    for (int i = SIVector::k_numberOfBaseUnits;
         i < Representative::k_numberOfDimensions - 1; i++) {
      const Representative* dim = Representative::DefaultRepresentatives()[i];
      const SIVector simplifyingUnitCoefficients = dim->siVector();
      // A simplification is tried by either dividing or multiplying.
      if (CanSimplifyUnitProduct(
              unitCoefficients, unitSupportSize, &simplifyingUnitCoefficients,
              -1, bestRemainderExponents, bestRemainderSupportSize)) {
        bestUnitExponent = 1;
        bestDim = dim;
      } else if (CanSimplifyUnitProduct(unitCoefficients, unitSupportSize,
                                        &simplifyingUnitCoefficients, 1,
                                        bestRemainderExponents,
                                        bestRemainderSupportSize)) {
        bestUnitExponent = -1;
        bestDim = dim;
      }
    }
    if (bestDim == nullptr) {
      // No simplification could be performed
      break;
    }
    // Build and add the best derived unit
    Tree* derivedUnit = Unit::Push(bestDim->representativesOfSameDimension());

    assert(bestUnitExponent == 1 || bestUnitExponent == -1);
    if (bestUnitExponent == -1) {
      PatternMatching::MatchReplace(derivedUnit, KA, KPow(KA, -1_e));
    }

    const int position = unitsAccu->numberOfChildren();
    NAry::AddChildAtIndex(unitsAccu, derivedUnit, position);
    // Update remainder units and their exponents for next simplifications
    *unitCoefficients = bestRemainderExponents;
    unitSupportSize = bestRemainderSupportSize;
  }
  NAry::SquashIfEmpty(unitsAccu);
  return unitsAccu;
}

void ChooseBestRepresentativeAndPrefix(Tree* unit, double* value,
                                       double exponent,
                                       Preferences::UnitFormat unitFormat,
                                       bool optimizeRepresentative) {
  assert(exponent != 0.f);

  if ((std::isinf(*value) ||
       (*value == 0.0 &&
        Unit::GetRepresentative(unit)->siVector() != Temperature::Dimension))) {
    /* Use the base unit to represent an infinite or null value, as all units
     * are equivalent.
     * This is not true for temperatures (0 K != 0°C != 0°F). */
    Unit::SetRepresentative(
        unit, Unit::GetRepresentative(unit)->representativesOfSameDimension());
    Unit::SetPrefix(unit, Prefix::EmptyPrefix());
    return;
  }
  // Convert value to base units
  double baseValue = *value * std::pow(Unit::GetValue(unit), exponent);
  const Prefix* bestPrefix = Prefix::EmptyPrefix();
  const Representative* bestRepresentative =
      Unit::GetRepresentative(unit)->bestRepresentativeAndPrefix(
          baseValue, exponent, unitFormat, &bestPrefix,
          optimizeRepresentative ? nullptr : Unit::GetRepresentative(unit));

  if (bestRepresentative != Unit::GetRepresentative(unit)) {
    *value = *value * std::pow(Unit::GetRepresentative(unit)->ratio() /
                                   bestRepresentative->ratio(),
                               exponent);
    Unit::SetRepresentative(unit, bestRepresentative);
  }
  if (bestPrefix != Unit::GetPrefix(unit)) {
    *value =
        *value * std::pow(10., exponent * (Unit::GetPrefix(unit)->exponent() -
                                           bestPrefix->exponent()));
    Unit::SetPrefix(unit, bestPrefix);
  }
}

// Return true if best representative and prefix have been chosen.
bool ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(
    Tree* unit, double* value, Preferences::UnitFormat unitFormat,
    bool optimizeRepresentative) {
  double exponent = 1.f;
  Tree* factor = unit;
  if (factor->isPow()) {
    Tree* childExponent = factor->child(1);
    assert(factor->child(0)->isUnit());
    assert(factor->child(1)->isRational());
    exponent = Approximation::To<double>(
        childExponent, Approximation::Parameters{.policy = {.optimize = true}});
    factor = factor->child(0);
  }
  if (!factor->isUnit()) {
    return false;
  }
  if (exponent == 0.f) {
    /* Finding the best representative for a unit with exponent 0 doesn't
     * really make sense, and should only happen with a weak ReductionTarget
     * (such as in Graph app), that only rely on approximations. We keep the
     * unit unchanged as it will approximate to undef anyway. */
    return false;
  }
  ChooseBestRepresentativeAndPrefix(factor, value, exponent, unitFormat,
                                    optimizeRepresentative);
  return true;
}

void ChooseBestRepresentativeAndPrefixForValue(
    Tree* units, double* value, Preferences::UnitFormat unitFormat) {
  if (units->isMult()) {
    for (Tree* factor : units->children()) {
      ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(factor, value,
                                                            unitFormat, true);
    }
  } else {
    ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(units, value,
                                                          unitFormat, true);
  }
}

}  // namespace Units
}  // namespace Poincare::Internal

#endif
