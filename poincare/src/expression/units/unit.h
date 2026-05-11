#pragma once

#include <poincare/simplification_parameters.h>
#include <poincare/src/memory/tree.h>

#include "../approximation.h"
#include "../builtin.h"
#include "../dimension_vector.h"
#include "prefix.h"
#include "representative.h"

namespace Poincare::Internal {
namespace Units {

class Unit {
 public:
  constexpr static int k_numberOfBaseUnits = 8;

  constexpr static int k_maxTextLen =
      Prefix::k_maxTextLen + Representative::k_maxTextLen;

  static bool CanParse(ForwardUnicodeDecoder* name,
                       const Representative** representative,
                       const Prefix** prefix);
  static bool CanParse(LayoutSpan span, const Representative** representative,
                       const Prefix** prefix) {
    LayoutSpanDecoder decoder(span);
    return CanParse(&decoder, representative, prefix);
  }

  // Returns true if the unit name cannot be used as a variable name
  static bool IsNameReserved(const Representative* representative);

  static void ChooseBestRepresentativeAndPrefixForValue(
      Tree* units, double* value, Preferences::UnitFormat unitFormat);

  static bool AllowImplicitAddition(
      const Representative* smallestRepresentative,
      const Representative* biggestRepresentative);

  static bool IsUnitOrPowerOfUnit(const Tree* e);
  static bool ForceMarginLeftOfUnit(const Tree* e);

  /* Optimize prefix for the given unit and value.
   * Can optimize representative as well. */
  static void ChooseBestRepresentativeAndPrefix(
      Tree* unit, double* value, double exponent,
      Preferences::UnitFormat unitFormat, bool optimizeRepresentative);

  static bool IsNonKelvinTemperature(const Representative* representative);
  // Remove units and Convert tree to Kelvin.
  static void RemoveTemperatureUnit(Tree* root);
  // Replace with reduced SI ratio only.
  static void RemoveUnit(Tree* unit);
  // Replace units and physical constants with their reduced Basic SI value
  static bool ShallowRemoveUnit(Tree* e, void*);
  // Push Unit
  static Tree* Push(const Representative* unitRepresentative,
                    const Prefix* unitPrefix = Prefix::EmptyPrefix());
  static Tree* Push(AngleUnit angleUnit);
  static const Representative* GetRepresentative(const Tree* unit);
  static void SetRepresentative(Tree* unit,
                                const Representative* representative);
  static const Prefix* GetPrefix(const Tree* unit);
  static void SetPrefix(Tree* unit, const Prefix* prefix);

  static double GetValue(const Tree* unit);

  /* SI Vector of base units (Mult, Unit or Pow) with integer exponents. If
   * rational exponents, the closest integer will be used. */
  static SIVector GetSIVector(const Tree* baseUnits);
  // Push base SI units matching the vector
  static Tree* GetBaseUnits(SIVector vector);

  // From Kelvin value, return value in given temperature representative
  static double KelvinValueToRepresentative(
      double value, const Representative* representative);
  /* Remove subtrees that don't contain units. Return true if the entire tree
   * has been removed. Only Mult, Pow, and Add (if preserveAdd) are preserved.
   * Warning: the caller of RemoveNonUnits must imperatively check or assert the
   * return value, to prevent crashes if "e" has been removed.
   */
  [[nodiscard]] static bool RemoveNonUnits(Tree* e, bool preserveAdd);
  // Convert tree from base unit to target unit without approximation
  static Tree* ExactConvertToUnit(const Tree* e, TreeRef& targetUnit);
  // Return true if Imperial units should be used in output
  static bool DisplayImperialUnitsInOutput(const Tree* inputUnits);

  // Optimize representative and prefix for a single unit factor (or Pow(unit)).
  static bool ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(
      Tree* unit, double* value, Preferences::UnitFormat unitFormat,
      bool optimizeRepresentative);
};

Tree* ChooseBestDerivedUnits(SIVector* unitCoefficients);
bool IsCombinationOfUnits(const Tree* e);

bool HasUnit(const Tree* e);
bool IsPureAngleUnit(const Tree* e);

}  // namespace Units
}  // namespace Poincare::Internal
