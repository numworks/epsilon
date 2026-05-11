#pragma once

#include "unit.h"

namespace Poincare::Internal {
namespace Units {

// Greedy rewrite of an SI exponent vector into a product of named derived
// units. Mutates *unitCoefficients to the remainder vector and returns the
// accumulator Mult of chosen derived units.
Tree* ChooseBestDerivedUnits(SIVector* unitCoefficients);

/* Optimize prefix for the given unit and value. Can optimize representative as
 * well. */
void ChooseBestRepresentativeAndPrefix(Tree* unit, double* value,
                                       double exponent,
                                       Preferences::UnitFormat unitFormat,
                                       bool optimizeRepresentative);

// Optimize representative and prefix for a single unit factor (or Pow(unit)).
bool ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(
    Tree* unit, double* value, Preferences::UnitFormat unitFormat,
    bool optimizeRepresentative);

// Apply ChooseBestRepresentativeAndPrefixForValueOnSingleUnit to each factor.
void ChooseBestRepresentativeAndPrefixForValue(
    Tree* units, double* value, Preferences::UnitFormat unitFormat);

}  // namespace Units
}  // namespace Poincare::Internal
