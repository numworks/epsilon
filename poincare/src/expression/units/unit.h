#ifndef POINCARE_EXPRESSION_UNITS_UNIT_H
#define POINCARE_EXPRESSION_UNITS_UNIT_H

#include <poincare/src/memory/tree.h>

#include <array>

#include "../approximation.h"
#include "../builtin.h"
#include "../context.h"
#include "../dimension_vector.h"
#include "../k_tree.h"

namespace Poincare::Internal {
namespace Units {

/* The units having the same physical dimension are grouped together.
 * Each such group has a standard representative with a standard prefix.
 *
 * Each representative has
 *  - a root symbol
 *  - a definition, as the conversion ratio with the SI unit of the same
 *    dimensions
 *  - informations on how the representative should be prefixed.
 *
 * Given a representative and a Prefix allowed for that representative, one
 * may get a symbol and an expression tree. */

class Prefix {
  friend class Unit;

 public:
  constexpr static int k_numberOfPrefixes = 13;
  static const Prefix* Prefixes();
  static const Prefix* EmptyPrefix();
  // Assigning an id to each accessible prefixes
  static uint8_t ToId(const Prefix* representative);
  static const Prefix* FromId(uint8_t id);

  const char* symbol() const { return m_symbol; }
  int8_t exponent() const { return m_exponent; }

 private:
  constexpr Prefix(const char* symbol, int8_t exponent)
      : m_symbol(symbol), m_exponent(exponent) {}

  const char* m_symbol;
  int8_t m_exponent;
};

class Representative {
  friend class Unit;

 public:
  // Operators
  bool operator==(const Representative&) const = default;
  bool operator!=(const Representative&) const = default;
  enum class Prefixable {
    None,
    PositiveLongScale,
    NegativeLongScale,
    Positive,
    Negative,
    NegativeAndKilo,
    LongScale,
    All,
  };
  constexpr static int k_numberOfDimensions = 25;
  // Assigning an id to each accessible representatives
  static uint8_t ToId(const Representative* representative);
  static const Representative* FromId(uint8_t id);

  static const Representative* const* DefaultRepresentatives();
  static const Representative* RepresentativeForDimension(SIVector vector);

  virtual const SIVector siVector() const = 0;
  virtual int numberOfRepresentatives() const = 0;
  /* representativesOfSameDimension returns a pointer to the array containing
   * all representatives for this's dimension. */
  virtual const Representative* representativesOfSameDimension() const = 0;
  virtual bool isBaseUnit() const = 0;
  virtual const Representative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const Prefix** prefix) const {
    return defaultFindBestRepresentativeAndPrefix(
        value, exponent, representativesOfSameDimension(),
        representativesOfSameDimension() + numberOfRepresentatives(), prefix);
  }
  virtual const Representative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const Prefix** prefix, const Representative* forcedRepr) const {
    if (forcedRepr) {
      return defaultFindBestRepresentativeAndPrefix(value, exponent, forcedRepr,
                                                    forcedRepr + 1, prefix);
    }
    return standardRepresentative(value, exponent, unitFormat, prefix);
  }

  Aliases rootSymbols() const { return m_rootSymbols; }
  double ratio() const {
    return Approximation::To<double>(ratioExpression(),
                                     Approximation::Parameters{});
  }
  bool isInputPrefixable() const {
    return m_inputPrefixable != Prefixable::None;
  }
  bool isOutputPrefixable() const {
    return m_outputPrefixable != Prefixable::None;
  }
  bool isImperial() const { return m_isImperial; }
  bool canParseWithEquivalents(const char* symbol, size_t length,
                               const Representative** representative,
                               const Prefix** prefix) const;
  bool canParse(const char* symbol, size_t length, const Prefix** prefix) const;
  bool canPrefix(const Prefix* prefix, bool input) const;
  const Prefix* findBestPrefix(double value, double exponent) const;
  const Tree* ratioExpression() const { return m_ratioExpression; }
  // Push reduced ratio expression on TreeStack
  Tree* pushReducedRatioExpression() const;

 protected:
  // TODO it may be marked consteval with Clang but not with GCC
  template <KTreeConcept T>
  constexpr Representative(Aliases rootSymbol, T ratioExpression,
                           Prefixable inputPrefixable,
                           Prefixable outputPrefixable, bool isImperial = false)
      : m_rootSymbols(rootSymbol),
        m_ratioExpression(ratioExpression),
        m_inputPrefixable(inputPrefixable),
        m_outputPrefixable(outputPrefixable),
        m_isImperial(isImperial) {}

  const Representative* defaultFindBestRepresentativeAndPrefix(
      double value, double exponent, const Representative* begin,
      const Representative* end, const Prefix** prefix) const;

  Aliases m_rootSymbols;
  /* m_ratioExpression is the expression of the factor used to convert a unit
   * made of the representative and its base prefix into base SI units. ex :
   * m_ratio for Liter is 1e-3 (as 1_L = 1e-3_m^3).
   * m_ratio for gram is 1 (as k is its best prefix and _kg is SI)
   * */

  const Tree* m_ratioExpression;
  const Prefixable m_inputPrefixable;
  const Prefixable m_outputPrefixable;
  const bool m_isImperial;
};

class Unit {
 public:
  constexpr static int k_numberOfBaseUnits = 8;
  /* Prefixes and Representatives defined below must be defined only once and
   * all units must be constructed from their pointers. This way we can easily
   * check if two Unit objects are equal by comparing pointers. This saves us
   * from overloading the == operator on Prefix and Representative and
   * saves time at execution. As such, their constructor are private and can
   * only be accessed by their friend class Unit. */
  constexpr static const Prefix k_prefixes[Prefix::k_numberOfPrefixes] = {
      Prefix("p", -12), Prefix("n", -9), Prefix("μ", -6), Prefix("m", -3),
      Prefix("c", -2),  Prefix("d", -1), Prefix("", 0),   Prefix("da", 1),
      Prefix("h", 2),   Prefix("k", 3),  Prefix("M", 6),  Prefix("G", 9),
      Prefix("T", 12),
  };

  /* Define access points to some prefixes. */
  constexpr static int k_emptyPrefixIndex = 6;
  static_assert(k_prefixes[k_emptyPrefixIndex].m_exponent == 0,
                "Index for the Empty Prefix is incorrect.");
  constexpr static int k_kiloPrefixIndex = 9;
  static_assert(k_prefixes[k_kiloPrefixIndex].m_exponent == 3,
                "Index for the Kilo Prefix is incorrect.");

  using Prefixable = Representative::Prefixable;

  static bool CanParse(ForwardUnicodeDecoder* name,
                       const Representative** representative,
                       const Prefix** prefix);
  static bool CanParse(LayoutSpan span, const Representative** representative,
                       const Prefix** prefix) {
    LayoutSpanDecoder decoder(span);
    return CanParse(&decoder, representative, prefix);
  }
  static void ChooseBestRepresentativeAndPrefixForValue(Tree* units,
                                                        double* value,
                                                        UnitFormat unitFormat);

  static bool AllowImplicitAddition(
      const Representative* smallestRepresentative,
      const Representative* biggestRepresentative);

  static bool IsUnitOrPowerOfUnit(const Tree* e);
  static bool ForceMarginLeftOfUnit(const Tree* e);

  static void ChooseBestRepresentativeAndPrefix(Tree* unit, double* value,
                                                double exponent,
                                                UnitFormat unitFormat,
                                                bool optimizePrefix,
                                                bool optimizeRepresentative);

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

  // Project expression and unit according to unitDisplay strategy
  static bool ProjectToBestUnits(Tree* e, Dimension dimension,
                                 UnitDisplay unitDisplay, AngleUnit angleUnit,
                                 UnitFormat unitFormat);

 private:
  // From Kelvin value, return value in given temperature representative
  static double KelvinValueToRepresentative(
      double value, const Representative* representative);
  /* Remove subtrees that don't contain units. Return true if the entire tree
   * has been removed. Only Mult, Pow, and Add (if preserveAdd) are preserved.
   */
  static bool RemoveNonUnits(Tree* e, bool preserveAdd);
  // Return true if Imperial units should be used in output
  static bool DisplayImperialUnitsInOutput(const Tree* inputUnits);
  static void ApplyMainOutputDisplay(Tree* e, TreeRef& extractedUnits,
                                     Dimension dimension, AngleUnit angleUnit);
  // Without taking input units into account, select best unit for tree
  static bool ApplyAutomaticDisplay(Tree* e, Dimension dimension,
                                    UnitDisplay unitDisplay);
  static bool ApplyAutomaticInputDisplay(Tree* e, TreeRef& inputUnits);
  static bool ApplyEquivalentDisplay(Tree* e, TreeRef& inputUnits,
                                     Dimension dimension);
  static bool ApplyDecompositionDisplay(Tree* e, TreeRef& inputUnits,
                                        Dimension dimension,
                                        UnitFormat unitFormat);
};

Tree* ChooseBestDerivedUnits(SIVector* unitsExponents);
bool IsCombinationOfUnits(const Tree* e);

bool HasUnit(const Tree* e);
bool IsPureAngleUnit(const Tree* e);

}  // namespace Units
}  // namespace Poincare::Internal

#endif
