#ifndef POINCARE_EXPRESSION_UNIT_H
#define POINCARE_EXPRESSION_UNIT_H

#include <poincare_junior/src/memory/tree.h>

#include <array>

#include "approximation.h"
#include "builtin.h"
#include "context.h"
#include "k_tree.h"

namespace PoincareJ {

/* The units having the same physical dimension are grouped together.
 * Each such group has a standard representative with a standard prefix.
 *
 * Each representative has
 *  - a root symbol
 *  - a definition, as the conversion ratio with the SI unit of the same
 *    dimensions
 *  - informations on how the representative should be prefixed.
 *
 * Given an representative and a UnitPrefix allowed for that representative, one
 * may get a symbol and an expression tree. */

// Copied from poincare/helpers.h
/* FIXME : This can be replaced by std::string_view when moving to C++17 */
constexpr static bool StringsAreEqual(const char* s1, const char* s2) {
  return *s1 == *s2 &&
         ((*s1 == '\0' && *s2 == '\0') || StringsAreEqual(s1 + 1, s2 + 1));
}

class UnitPrefix {
  friend class Unit;

 public:
  constexpr static int k_numberOfPrefixes = 13;
  static const UnitPrefix* Prefixes();
  static const UnitPrefix* EmptyPrefix();
  // Assigning an id to each accessible prefixes
  static uint8_t ToId(const UnitPrefix* representative);
  static const UnitPrefix* FromId(uint8_t id);

  const char* symbol() const { return m_symbol; }
  int8_t exponent() const { return m_exponent; }
#if 0
  int serialize(char* buffer, int bufferSize) const;
#endif

 private:
  constexpr UnitPrefix(const char* symbol, int8_t exponent)
      : m_symbol(symbol), m_exponent(exponent) {}

  const char* m_symbol;
  int8_t m_exponent;
};

struct DimensionVector {
  constexpr static uint8_t k_numberOfBaseUnits = 8;
  // Operators
  bool operator==(const DimensionVector&) const = default;
  bool operator!=(const DimensionVector&) const = default;
  // SupportSize is defined as the number of distinct base units.
  constexpr size_t supportSize() const {
    size_t supportSize = 0;
    for (uint8_t i = 0; i < k_numberOfBaseUnits; i++) {
      if (coefficientAtIndex(i) == 0) {
        continue;
      }
      supportSize++;
    }
    return supportSize;
  }
  constexpr bool isEmpty() const { return supportSize() == 0; }
  constexpr static DimensionVector Empty() { return {}; }

  static DimensionVector FromBaseUnits(const Tree* baseUnits);
  // Push SI units matching the vector
  Tree* toBaseUnits() const;

  void addAllCoefficients(const DimensionVector other, int8_t factor);
  void setCoefficientAtIndex(int8_t coefficient, uint8_t i);
  void setCoefficientAtIndex(int coefficient, uint8_t i) {
    assert(coefficient <= INT8_MAX && coefficient >= INT8_MIN);
    setCoefficientAtIndex(static_cast<int8_t>(coefficient), i);
  }
  constexpr int8_t coefficientAtIndex(uint8_t i) const {
    assert(i < k_numberOfBaseUnits);
    const int8_t coefficients[] = {time,
                                   distance,
                                   angle,
                                   mass,
                                   current,
                                   temperature,
                                   amountOfSubstance,
                                   luminousIntensity};
    static_assert(std::size(coefficients) == k_numberOfBaseUnits);
    return coefficients[i];
  }

  int8_t time = 0;
  int8_t distance = 0;
  int8_t angle = 0;
  int8_t mass = 0;
  int8_t current = 0;
  int8_t temperature = 0;
  int8_t amountOfSubstance = 0;
  int8_t luminousIntensity = 0;
};
static_assert(sizeof(DimensionVector) ==
              sizeof(uint8_t) * DimensionVector::k_numberOfBaseUnits);
static_assert(DimensionVector::Empty().isEmpty());

class UnitRepresentative {
  friend class Unit;

 public:
  // Operators
  bool operator==(const UnitRepresentative&) const = default;
  bool operator!=(const UnitRepresentative&) const = default;
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
  static uint8_t ToId(const UnitRepresentative* representative);
  static const UnitRepresentative* FromId(uint8_t id);

  static const UnitRepresentative* const* DefaultRepresentatives();
#if 0
  static const UnitRepresentative* RepresentativeForDimension(
      DimensionVector vector);
#endif
  constexpr UnitRepresentative(Aliases rootSymbol, const Block* ratioExpression,
                               Prefixable inputPrefixable,
                               Prefixable outputPrefixable)
      : m_rootSymbols(rootSymbol),
        m_ratioExpression(ratioExpression),
        m_inputPrefixable(inputPrefixable),
        m_outputPrefixable(outputPrefixable) {}

  virtual const DimensionVector dimensionVector() const {
    return DimensionVector::Empty();
  };
  virtual int numberOfRepresentatives() const { return 0; };
  /* representativesOfSameDimension returns a pointer to the array containing
   * all representatives for this's dimension. */
  virtual const UnitRepresentative* representativesOfSameDimension() const {
    return nullptr;
  };
  virtual const UnitPrefix* basePrefix() const {
    return UnitPrefix::EmptyPrefix();
  }
  virtual bool isBaseUnit() const { return false; }
#if 0
  virtual const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const {
    return defaultFindBestRepresentative(value, exponent,
                                         representativesOfSameDimension(),
                                         numberOfRepresentatives(), prefix);
  }
    /* hasSpecialAdditionalExpressions return true if the unit has special
     * forms suchas as splits (for time and imperial units) or common
     * conversions (such as speed and energy). */
    virtual bool hasSpecialAdditionalExpressions(double value,
                                                 UnitFormat unitFormat) const {
      return false;
    }
    virtual int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const {
      return 0;
    }
#endif

  Aliases rootSymbols() const { return m_rootSymbols; }
#if 0
  double ratio() const {
    return Approximation::To<double>(ratioExpressionReduced());
  }
#endif
  bool isInputPrefixable() const {
    return m_inputPrefixable != Prefixable::None;
  }
  bool isOutputPrefixable() const {
    return m_outputPrefixable != Prefixable::None;
  }
#if 0
  int serialize(char* buffer, int bufferSize, const UnitPrefix* prefix) const;
#endif
  bool canParseWithEquivalents(const char* symbol, size_t length,
                               const UnitRepresentative** representative,
                               const UnitPrefix** prefix) const;
  bool canParse(const char* symbol, size_t length,
                const UnitPrefix** prefix) const;
  bool canPrefix(const UnitPrefix* prefix, bool input) const;
#if 0
  const UnitPrefix* findBestPrefix(double value, double exponent) const;
#endif
  const Tree* ratioExpressionReduced() const {
    return Tree::FromBlocks(m_ratioExpression);
  }

 protected:
#if 0
  const UnitRepresentative* defaultFindBestRepresentative(
      double value, double exponent, const UnitRepresentative* representatives,
      int length, const UnitPrefix** prefix) const;
#endif

  Aliases m_rootSymbols;
  /* m_ratioExpression is the expression of the factor used to convert a unit
   * made of the representative and its base prefix into base SI units. ex :
   * m_ratio for Liter is 1e-3 (as 1_L = 1e-3_m).
   * m_ratio for gram is 1 (as k is its best prefix and _kg is SI)
   *
   * TODO: We have to use Block * instead of Tree * so that representatives
   * lists can be constexpr because casting Block* into Tree* requires a
   * reinterpret cast. */

  const Block* m_ratioExpression;
  const Prefixable m_inputPrefixable;
  const Prefixable m_outputPrefixable;
};

class TimeRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static TimeRepresentative Default() {
    return TimeRepresentative(nullptr, nullptr, Prefixable::None,
                              Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 1,
                           .distance = 0,
                           .angle = 0,
                           .mass = 0,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }
#if 0
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return ratio() * value >= representativesOfSameDimension()[1].ratio();
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

 private:
  using UnitRepresentative::UnitRepresentative;
};

class DistanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static DistanceRepresentative Default() {
    return DistanceRepresentative(nullptr, nullptr, Prefixable::None,
                                  Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 0,
                           .distance = 1,
                           .angle = 0,
                           .mass = 0,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }
#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return unitFormat == UnitFormat::Imperial;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

 private:
  using UnitRepresentative::UnitRepresentative;
};

class AngleRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static AngleRepresentative Default() {
    return AngleRepresentative(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
  }
  static const UnitRepresentative* DefaultRepresentativeForAngleUnit(
      AngleUnit angleUnit);

#if 0
  // Returns a beautified expression
  Tree* convertInto(Tree* value, const UnitRepresentative* other,
                    const ReductionContext& reductionContext) const;
#endif
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 0,
                           .distance = 0,
                           .angle = 1,
                           .mass = 0,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }
#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressionsWithExactValue(
        Expression exactValue, double value, Expression* dest,
        int availableLength, const ReductionContext& reductionContext) const;
#endif

 private:
  using UnitRepresentative::UnitRepresentative;
};

class MassRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static MassRepresentative Default() {
    return MassRepresentative(nullptr, nullptr, Prefixable::None,
                              Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 0,
                           .distance = 0,
                           .angle = 0,
                           .mass = 1,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  const UnitPrefix* basePrefix() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }
#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return unitFormat == UnitFormat::Imperial;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

 private:
  using UnitRepresentative::UnitRepresentative;
};

class CurrentRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static CurrentRepresentative Default() {
    return CurrentRepresentative(nullptr, nullptr, Prefixable::None,
                                 Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 0,
                           .distance = 0,
                           .angle = 0,
                           .mass = 0,
                           .current = 1,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }

 private:
  using UnitRepresentative::UnitRepresentative;
};

class TemperatureRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
#if 0
  static double ConvertTemperatures(double value,
                                    const UnitRepresentative* source,
                                    const UnitRepresentative* target);
#endif
  constexpr static TemperatureRepresentative Default() {
    return TemperatureRepresentative(nullptr, nullptr, Prefixable::None,
                                     Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 0,
                           .distance = 0,
                           .angle = 0,
                           .mass = 0,
                           .current = 0,
                           .temperature = 1,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }
#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override {
    return this;
  }
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

 private:
  constexpr static double k_celsiusOrigin = 273.15;
  constexpr static double k_fahrenheitOrigin = 459.67;
  using UnitRepresentative::UnitRepresentative;
};

class AmountOfSubstanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static AmountOfSubstanceRepresentative Default() {
    return AmountOfSubstanceRepresentative(nullptr, nullptr, Prefixable::None,
                                           Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 0,
                           .distance = 0,
                           .angle = 0,
                           .mass = 0,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 1,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }

 private:
  using UnitRepresentative::UnitRepresentative;
};

class LuminousIntensityRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static LuminousIntensityRepresentative Default() {
    return LuminousIntensityRepresentative(nullptr, nullptr, Prefixable::None,
                                           Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 0,
                           .distance = 0,
                           .angle = 0,
                           .mass = 0,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }

 private:
  using UnitRepresentative::UnitRepresentative;
};

class FrequencyRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static FrequencyRepresentative Default() {
    return FrequencyRepresentative(nullptr, nullptr, Prefixable::None,
                                   Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -1,
                           .distance = 0,
                           .angle = 0,
                           .mass = 0,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ForceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static ForceRepresentative Default() {
    return ForceRepresentative(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2,
                           .distance = 1,
                           .angle = 0,
                           .mass = 1,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class PressureRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static PressureRepresentative Default() {
    return PressureRepresentative(nullptr, nullptr, Prefixable::None,
                                  Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2,
                           .distance = -1,
                           .angle = 0,
                           .mass = 1,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class EnergyRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static EnergyRepresentative Default() {
    return EnergyRepresentative(nullptr, nullptr, Prefixable::None,
                                Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2,
                           .distance = 2,
                           .angle = 0,
                           .mass = 1,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

#if 0
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

 private:
  using UnitRepresentative::UnitRepresentative;
};

class PowerRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static PowerRepresentative Default() {
    return PowerRepresentative(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -3,
                           .distance = 2,
                           .angle = 0,
                           .mass = 1,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricChargeRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  using UnitRepresentative::UnitRepresentative;
  constexpr static ElectricChargeRepresentative Default() {
    return ElectricChargeRepresentative(nullptr, nullptr, Prefixable::None,
                                        Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 1,
                           .distance = 0,
                           .angle = 0,
                           .mass = 0,
                           .current = 1,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
};

class ElectricPotentialRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static ElectricPotentialRepresentative Default() {
    return ElectricPotentialRepresentative(nullptr, nullptr, Prefixable::None,
                                           Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -3,
                           .distance = 2,
                           .angle = 0,
                           .mass = 1,
                           .current = -1,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricCapacitanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static ElectricCapacitanceRepresentative Default() {
    return ElectricCapacitanceRepresentative(nullptr, nullptr, Prefixable::None,
                                             Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 4,
                           .distance = -2,
                           .angle = 0,
                           .mass = -1,
                           .current = 2,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricResistanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static ElectricResistanceRepresentative Default() {
    return ElectricResistanceRepresentative(nullptr, nullptr, Prefixable::None,
                                            Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -3,
                           .distance = 2,
                           .angle = 0,
                           .mass = 1,
                           .current = -2,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricConductanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static ElectricConductanceRepresentative Default() {
    return ElectricConductanceRepresentative(nullptr, nullptr, Prefixable::None,
                                             Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 3,
                           .distance = -2,
                           .angle = 0,
                           .mass = -1,
                           .current = 2,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class MagneticFluxRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static MagneticFluxRepresentative Default() {
    return MagneticFluxRepresentative(nullptr, nullptr, Prefixable::None,
                                      Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2,
                           .distance = 2,
                           .angle = 0,
                           .mass = 1,
                           .current = -1,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class MagneticFieldRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static MagneticFieldRepresentative Default() {
    return MagneticFieldRepresentative(nullptr, nullptr, Prefixable::None,
                                       Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2,
                           .distance = 0,
                           .angle = 0,
                           .mass = 1,
                           .current = -1,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class InductanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static InductanceRepresentative Default() {
    return InductanceRepresentative(nullptr, nullptr, Prefixable::None,
                                    Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2,
                           .distance = 2,
                           .angle = 0,
                           .mass = 1,
                           .current = -2,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class CatalyticActivityRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static CatalyticActivityRepresentative Default() {
    return CatalyticActivityRepresentative(nullptr, nullptr, Prefixable::None,
                                           Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -1,
                           .distance = 0,
                           .angle = 0,
                           .mass = 0,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 1,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class SurfaceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static SurfaceRepresentative Default() {
    return SurfaceRepresentative(nullptr, nullptr, Prefixable::None,
                                 Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 0,
                           .distance = 2,
                           .angle = 0,
                           .mass = 0,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

 private:
  using UnitRepresentative::UnitRepresentative;
};

class VolumeRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static VolumeRepresentative Default() {
    return VolumeRepresentative(nullptr, nullptr, Prefixable::None,
                                Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 0,
                           .distance = 3,
                           .angle = 0,
                           .mass = 0,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

 private:
  using UnitRepresentative::UnitRepresentative;
};

class SpeedRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static SpeedRepresentative Default() {
    return SpeedRepresentative(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -1,
                           .distance = 1,
                           .angle = 0,
                           .mass = 0,
                           .current = 0,
                           .temperature = 0,
                           .amountOfSubstance = 0,
                           .luminousIntensity = 0};
  }
#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override {
    return nullptr;
  }
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

 private:
  using UnitRepresentative::UnitRepresentative;
};

#if 0
class UnitNode final : public ExpressionNode {
 public:
  constexpr static int k_numberOfBaseUnits = 8;

  // Expression Properties
  TrinaryBoolean isPositive(Context* context) const override {
    return TrinaryBoolean::True;
  }
  TrinaryBoolean isNull(Context* context) const override {
    return TrinaryBoolean::False;
  }
  Expression removeUnit(Expression* unit) override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;

  /* Approximation */
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }

  // Comparison
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;

  // Simplification
  Expression shallowBeautify(const ReductionContext& reductionContext) override;
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::OneLetter;
  }  // TODO

};
#endif

class Unit {
 public:
  constexpr static int k_numberOfBaseUnits = 8;
  /* Prefixes and Representatives defined below must be defined only once and
   * all units must be constructed from their pointers. This way we can easily
   * check if two Unit objects are equal by comparing pointers. This saves us
   * from overloading the == operator on UnitPrefix and UnitRepresentative and
   * saves time at execution. As such, their constructor are private and can
   * only be accessed by their friend class Unit. */
  constexpr static const UnitPrefix k_prefixes[UnitPrefix::k_numberOfPrefixes] =
      {
          UnitPrefix("p", -12), UnitPrefix("n", -9), UnitPrefix("μ", -6),
          UnitPrefix("m", -3),  UnitPrefix("c", -2), UnitPrefix("d", -1),
          UnitPrefix("", 0),    UnitPrefix("da", 1), UnitPrefix("h", 2),
          UnitPrefix("k", 3),   UnitPrefix("M", 6),  UnitPrefix("G", 9),
          UnitPrefix("T", 12),
  };
  typedef UnitRepresentative::Prefixable Prefixable;
  // Use KTree(1.0_e).k_blocks to cast FloatLiteral into KTree into Blocks *
  constexpr static const TimeRepresentative k_timeRepresentatives[] = {
      TimeRepresentative("s", KTree(1.0_e).k_blocks, Prefixable::All,
                         Prefixable::NegativeLongScale),
      TimeRepresentative("min", KTree(60.0_e).k_blocks, Prefixable::None,
                         Prefixable::None),
      TimeRepresentative("h", KTree(3600.0_e).k_blocks, Prefixable::None,
                         Prefixable::None),
      TimeRepresentative("day", KTree(86400.0_e).k_blocks, Prefixable::None,
                         Prefixable::None),
      TimeRepresentative("week", KTree(604800.0_e).k_blocks, Prefixable::None,
                         Prefixable::None),
      TimeRepresentative("month", KTree(2629800.0_e).k_blocks, Prefixable::None,
                         Prefixable::None),
      TimeRepresentative("year", KTree(31557600.0_e).k_blocks, Prefixable::None,
                         Prefixable::None),
  };
  constexpr static const DistanceRepresentative k_distanceRepresentatives[] = {
      DistanceRepresentative("m", KTree(1.0_e).k_blocks, Prefixable::All,
                             Prefixable::NegativeAndKilo),
      DistanceRepresentative("au", KTree(149597870700.0_e).k_blocks,
                             Prefixable::None, Prefixable::None),
      DistanceRepresentative("ly", KMult(299792458.0_e, 31557600.0_e).k_blocks,
                             Prefixable::None, Prefixable::None),
      DistanceRepresentative(
          "pc", KMult(180.0_e, KDiv(3600.0_e, π_e), 149587870700.0_e).k_blocks,
          Prefixable::None, Prefixable::None),
      DistanceRepresentative("in", KTree(0.0254_e).k_blocks, Prefixable::None,
                             Prefixable::None),
      DistanceRepresentative("ft", KMult(12.0_e, 0.0254_e).k_blocks,
                             Prefixable::None, Prefixable::None),
      DistanceRepresentative("yd", KMult(36.0_e, 0.0254_e).k_blocks,
                             Prefixable::None, Prefixable::None),
      DistanceRepresentative("mi", KMult(63360.0_e, 0.0254_e).k_blocks,
                             Prefixable::None, Prefixable::None),
  };
  /* Only AngleRepresentative have non-float ratio expression because exact
   * result are expected. */
  static constexpr const AngleRepresentative k_angleRepresentatives[] = {
      AngleRepresentative("rad", KTree(1_e).k_blocks, Prefixable::None,
                          Prefixable::None),
      AngleRepresentative("\"", KDiv(π_e, 648000_e).k_blocks, Prefixable::None,
                          Prefixable::None),
      AngleRepresentative("'", KDiv(π_e, 10800_e).k_blocks, Prefixable::None,
                          Prefixable::None),
      AngleRepresentative("°", KDiv(π_e, 180_e).k_blocks, Prefixable::None,
                          Prefixable::None),
      AngleRepresentative("gon", KDiv(π_e, 200_e).k_blocks, Prefixable::None,
                          Prefixable::None),
  };
  constexpr static const MassRepresentative k_massRepresentatives[] = {
      MassRepresentative("g", KTree(1.0_e).k_blocks, Prefixable::All,
                         Prefixable::NegativeAndKilo),
      MassRepresentative("t", KTree(1000.0_e).k_blocks,
                         Prefixable::PositiveLongScale,
                         Prefixable::PositiveLongScale),
      MassRepresentative("Da",
                         KDiv(KPow(10.0_e, -26.0_e), 6.02214076_e).k_blocks,
                         Prefixable::All, Prefixable::All),
      MassRepresentative("oz", KTree(0.028349523125_e).k_blocks,
                         Prefixable::None, Prefixable::None),
      MassRepresentative("lb", KMult(16.0_e, 0.028349523125_e).k_blocks,
                         Prefixable::None, Prefixable::None),
      MassRepresentative("shtn",
                         KMult(2000.0_e, 16.0_e, 0.028349523125_e).k_blocks,
                         Prefixable::None, Prefixable::None),
      MassRepresentative("lgtn",
                         KMult(2240.0_e, 16.0_e, 0.028349523125_e).k_blocks,
                         Prefixable::None, Prefixable::None),
  };
  constexpr static const CurrentRepresentative k_currentRepresentatives[] = {
      CurrentRepresentative("A", KTree(1.0_e).k_blocks, Prefixable::All,
                            Prefixable::LongScale)};
  // Ratios are 1.0 because temperatures conversion are an exception.
  constexpr static const TemperatureRepresentative
      k_temperatureRepresentatives[] = {
          TemperatureRepresentative("K", KTree(1.0_e).k_blocks, Prefixable::All,
                                    Prefixable::None),
          TemperatureRepresentative("°C", KTree(1.0_e).k_blocks,
                                    Prefixable::None, Prefixable::None),
          TemperatureRepresentative("°F", KTree(1.0_e).k_blocks,
                                    Prefixable::None, Prefixable::None),
  };
  constexpr static const AmountOfSubstanceRepresentative
      k_amountOfSubstanceRepresentatives[] = {AmountOfSubstanceRepresentative(
          "mol", KTree(1.0_e).k_blocks, Prefixable::All,
          Prefixable::LongScale)};
  constexpr static const LuminousIntensityRepresentative
      k_luminousIntensityRepresentatives[] = {LuminousIntensityRepresentative(
          "cd", KTree(1.0_e).k_blocks, Prefixable::All, Prefixable::LongScale)};
  constexpr static const FrequencyRepresentative k_frequencyRepresentatives[] =
      {FrequencyRepresentative("Hz", KTree(1.0_e).k_blocks, Prefixable::All,
                               Prefixable::LongScale)};
  constexpr static const ForceRepresentative k_forceRepresentatives[] = {
      ForceRepresentative("N", KTree(1.0_e).k_blocks, Prefixable::All,
                          Prefixable::LongScale)};
  constexpr static const PressureRepresentative k_pressureRepresentatives[] = {
      PressureRepresentative("Pa", KTree(1.0_e).k_blocks, Prefixable::All,
                             Prefixable::LongScale),
      PressureRepresentative("bar", KTree(100000.0_e).k_blocks, Prefixable::All,
                             Prefixable::LongScale),
      PressureRepresentative("atm", KTree(101325.0_e).k_blocks,
                             Prefixable::None, Prefixable::None),
  };
  constexpr static const EnergyRepresentative k_energyRepresentatives[] = {
      EnergyRepresentative("J", KTree(1.0_e).k_blocks, Prefixable::All,
                           Prefixable::LongScale),
      EnergyRepresentative("eV",
                           KMult(1.602176634_e, KPow(10.0_e, -19_e)).k_blocks,
                           Prefixable::All, Prefixable::LongScale),
  };
  constexpr static const PowerRepresentative k_powerRepresentatives[] = {

      PowerRepresentative("W", KTree(1.0_e).k_blocks, Prefixable::All,
                          Prefixable::LongScale),
      PowerRepresentative("hp", KTree(745.699872_e).k_blocks, Prefixable::None,
                          Prefixable::None)};
  constexpr static const ElectricChargeRepresentative
      k_electricChargeRepresentatives[] = {ElectricChargeRepresentative(
          "C", KTree(1.0_e).k_blocks, Prefixable::All, Prefixable::LongScale)};
  constexpr static const ElectricPotentialRepresentative
      k_electricPotentialRepresentatives[] = {ElectricPotentialRepresentative(
          "V", KTree(1.0_e).k_blocks, Prefixable::All, Prefixable::LongScale)};
  constexpr static const ElectricCapacitanceRepresentative
      k_electricCapacitanceRepresentatives[] = {
          ElectricCapacitanceRepresentative("F", KTree(1.0_e).k_blocks,
                                            Prefixable::All,
                                            Prefixable::LongScale)};
  constexpr static const ElectricResistanceRepresentative
      k_electricResistanceRepresentatives[] = {ElectricResistanceRepresentative(
          "Ω", KTree(1.0_e).k_blocks, Prefixable::All, Prefixable::LongScale)};
  constexpr static const ElectricConductanceRepresentative
      k_electricConductanceRepresentatives[] = {
          ElectricConductanceRepresentative("S", KTree(1.0_e).k_blocks,
                                            Prefixable::All,
                                            Prefixable::LongScale)};
  constexpr static const MagneticFluxRepresentative
      k_magneticFluxRepresentatives[] = {MagneticFluxRepresentative(
          "Wb", KTree(1.0_e).k_blocks, Prefixable::All, Prefixable::LongScale)};
  constexpr static const MagneticFieldRepresentative
      k_magneticFieldRepresentatives[] = {MagneticFieldRepresentative(
          "T", KTree(1.0_e).k_blocks, Prefixable::All, Prefixable::LongScale)};
  constexpr static const InductanceRepresentative
      k_inductanceRepresentatives[] = {InductanceRepresentative(
          "H", KTree(1.0_e).k_blocks, Prefixable::All, Prefixable::LongScale)};
  constexpr static const CatalyticActivityRepresentative
      k_catalyticActivityRepresentatives[] = {CatalyticActivityRepresentative(
          "kat", KTree(1.0_e).k_blocks, Prefixable::All,
          Prefixable::LongScale)};
  constexpr static const SurfaceRepresentative k_surfaceRepresentatives[] = {
      SurfaceRepresentative("ha", KTree(10000.0_e).k_blocks, Prefixable::None,
                            Prefixable::None),
      SurfaceRepresentative("acre", KTree(4046.8564224_e).k_blocks,
                            Prefixable::None, Prefixable::None),
  };
  constexpr static const VolumeRepresentative k_volumeRepresentatives[] = {
      VolumeRepresentative(BuiltinsAliases::k_litersAliases,
                           KTree(0.001_e).k_blocks, Prefixable::All,
                           Prefixable::Negative),
      VolumeRepresentative("tsp", KTree(0.00000492892159375_e).k_blocks,
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("tbsp", KMult(3.0_e, 0.00000492892159375_e).k_blocks,
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("floz", KTree(0.0000295735295625_e).k_blocks,
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("cup", KMult(8.0_e, 0.0000295735295625_e).k_blocks,
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("pt", KMult(16.0_e, 0.0000295735295625_e).k_blocks,
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("qt", KMult(32.0_e, 0.0000295735295625_e).k_blocks,
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("gal", KMult(128.0_e, 0.0000295735295625_e).k_blocks,
                           Prefixable::None, Prefixable::None),
  };

  /* Define access points to some prefixes and representatives. */
  constexpr static int k_emptyPrefixIndex = 6;
  static_assert(k_prefixes[k_emptyPrefixIndex].m_exponent == 0,
                "Index for the Empty UnitPrefix is incorrect.");
  constexpr static int k_kiloPrefixIndex = 9;
  static_assert(k_prefixes[k_kiloPrefixIndex].m_exponent == 3,
                "Index for the Kilo UnitPrefix is incorrect.");
  constexpr static int k_secondRepresentativeIndex = 0;
  static_assert(
      StringsAreEqual(
          k_timeRepresentatives[k_secondRepresentativeIndex].m_rootSymbols,
          "s"),
      "Index for the Second UnitRepresentative is incorrect.");
  constexpr static int k_minuteRepresentativeIndex = 1;
  static_assert(
      StringsAreEqual(
          k_timeRepresentatives[k_minuteRepresentativeIndex].m_rootSymbols,
          "min"),
      "Index for the Minute UnitRepresentative is incorrect.");
  constexpr static int k_hourRepresentativeIndex = 2;
  static_assert(StringsAreEqual(k_timeRepresentatives[k_hourRepresentativeIndex]
                                    .m_rootSymbols,
                                "h"),
                "Index for the Hour UnitRepresentative is incorrect.");
  constexpr static int k_dayRepresentativeIndex = 3;
  static_assert(StringsAreEqual(k_timeRepresentatives[k_dayRepresentativeIndex]
                                    .m_rootSymbols,
                                "day"),
                "Index for the Day UnitRepresentative is incorrect.");
  constexpr static int k_monthRepresentativeIndex = 5;
  static_assert(
      StringsAreEqual(
          k_timeRepresentatives[k_monthRepresentativeIndex].m_rootSymbols,
          "month"),
      "Index for the Month UnitRepresentative is incorrect.");
  constexpr static int k_yearRepresentativeIndex = 6;
  static_assert(StringsAreEqual(k_timeRepresentatives[k_yearRepresentativeIndex]
                                    .m_rootSymbols,
                                "year"),
                "Index for the Year UnitRepresentative is incorrect.");
  constexpr static int k_meterRepresentativeIndex = 0;
  static_assert(
      StringsAreEqual(
          k_distanceRepresentatives[k_meterRepresentativeIndex].m_rootSymbols,
          "m"),
      "Index for the Meter UnitRepresentative is incorrect.");
  constexpr static int k_inchRepresentativeIndex = 4;
  static_assert(
      StringsAreEqual(
          k_distanceRepresentatives[k_inchRepresentativeIndex].m_rootSymbols,
          "in"),
      "Index for the Inch UnitRepresentative is incorrect.");
  constexpr static int k_footRepresentativeIndex = 5;
  static_assert(
      StringsAreEqual(
          k_distanceRepresentatives[k_footRepresentativeIndex].m_rootSymbols,
          "ft"),
      "Index for the Foot UnitRepresentative is incorrect.");
  constexpr static int k_yardRepresentativeIndex = 6;
  static_assert(
      StringsAreEqual(
          k_distanceRepresentatives[k_yardRepresentativeIndex].m_rootSymbols,
          "yd"),
      "Index for the Yard UnitRepresentative is incorrect.");
  constexpr static int k_mileRepresentativeIndex = 7;
  static_assert(
      StringsAreEqual(
          k_distanceRepresentatives[k_mileRepresentativeIndex].m_rootSymbols,
          "mi"),
      "Index for the Mile UnitRepresentative is incorrect.");
  constexpr static int k_radianRepresentativeIndex = 0;
  static_assert(
      StringsAreEqual(
          k_angleRepresentatives[k_radianRepresentativeIndex].m_rootSymbols,
          "rad"),
      "Index for the Radian UnitRepresentative is incorrect.");
  constexpr static int k_arcSecondRepresentativeIndex = 1;
  static_assert(
      StringsAreEqual(
          k_angleRepresentatives[k_arcSecondRepresentativeIndex].m_rootSymbols,
          "\""),
      "Index for the ArcSecond UnitRepresentative is incorrect.");
  constexpr static int k_arcMinuteRepresentativeIndex = 2;
  static_assert(
      StringsAreEqual(
          k_angleRepresentatives[k_arcMinuteRepresentativeIndex].m_rootSymbols,
          "'"),
      "Index for the ArcMinute UnitRepresentative is incorrect.");
  constexpr static int k_degreeRepresentativeIndex = 3;
  static_assert(
      StringsAreEqual(
          k_angleRepresentatives[k_degreeRepresentativeIndex].m_rootSymbols,
          "°"),
      "Index for the Degree UnitRepresentative is incorrect.");
  constexpr static int k_gradianRepresentativeIndex = 4;
  static_assert(
      StringsAreEqual(
          k_angleRepresentatives[k_gradianRepresentativeIndex].m_rootSymbols,
          "gon"),
      "Index for the Gradian UnitRepresentative is incorrect.");
  constexpr static int k_gramRepresentativeIndex = 0;
  static_assert(StringsAreEqual(k_massRepresentatives[k_gramRepresentativeIndex]
                                    .m_rootSymbols,
                                "g"),
                "Index for the Gram UnitRepresentative is incorrect.");
  constexpr static int k_tonRepresentativeIndex = 1;
  static_assert(StringsAreEqual(k_massRepresentatives[k_tonRepresentativeIndex]
                                    .m_rootSymbols,
                                "t"),
                "Index for the Ton UnitRepresentative is incorrect.");
  constexpr static int k_ounceRepresentativeIndex = 3;
  static_assert(
      StringsAreEqual(
          k_massRepresentatives[k_ounceRepresentativeIndex].m_rootSymbols,
          "oz"),
      "Index for the Ounce UnitRepresentative is incorrect.");
  constexpr static int k_poundRepresentativeIndex = 4;
  static_assert(
      StringsAreEqual(
          k_massRepresentatives[k_poundRepresentativeIndex].m_rootSymbols,
          "lb"),
      "Index for the Pound UnitRepresentative is incorrect.");
  constexpr static int k_shortTonRepresentativeIndex = 5;
  static_assert(
      StringsAreEqual(
          k_massRepresentatives[k_shortTonRepresentativeIndex].m_rootSymbols,
          "shtn"),
      "Index for the Short Ton UnitRepresentative is incorrect.");
  constexpr static int k_kelvinRepresentativeIndex = 0;
  static_assert(
      StringsAreEqual(k_temperatureRepresentatives[k_kelvinRepresentativeIndex]
                          .m_rootSymbols,
                      "K"),
      "Index for the Kelvin UnitRepresentative is incorrect.");
  constexpr static int k_celsiusRepresentativeIndex = 1;
  static_assert(
      StringsAreEqual(k_temperatureRepresentatives[k_celsiusRepresentativeIndex]
                          .m_rootSymbols,
                      "°C"),
      "Index for the Celsius UnitRepresentative is incorrect.");
  constexpr static int k_fahrenheitRepresentativeIndex = 2;
  static_assert(
      StringsAreEqual(
          k_temperatureRepresentatives[k_fahrenheitRepresentativeIndex]
              .m_rootSymbols,
          "°F"),
      "Index for the Fahrenheit UnitRepresentative is incorrect.");
  constexpr static int k_jouleRepresentativeIndex = 0;
  static_assert(
      StringsAreEqual(
          k_energyRepresentatives[k_jouleRepresentativeIndex].m_rootSymbols,
          "J"),
      "Index for the Joule UnitRepresentative is incorrect.");
  constexpr static int k_electronVoltRepresentativeIndex = 1;
  static_assert(
      StringsAreEqual(k_energyRepresentatives[k_electronVoltRepresentativeIndex]
                          .m_rootSymbols,
                      "eV"),
      "Index for the Electron Volt UnitRepresentative is incorrect.");
  constexpr static int k_wattRepresentativeIndex = 0;
  static_assert(
      StringsAreEqual(
          k_powerRepresentatives[k_wattRepresentativeIndex].m_rootSymbols, "W"),
      "Index for the Watt UnitRepresentative is incorrect.");
  constexpr static int k_hectareRepresentativeIndex = 0;
  static_assert(
      StringsAreEqual(
          k_surfaceRepresentatives[k_hectareRepresentativeIndex].m_rootSymbols,
          "ha"),
      "Index for the Hectare UnitRepresentative is incorrect.");
  constexpr static int k_acreRepresentativeIndex = 1;
  static_assert(
      StringsAreEqual(
          k_surfaceRepresentatives[k_acreRepresentativeIndex].m_rootSymbols,
          "acre"),
      "Index for the Acre UnitRepresentative is incorrect.");
  constexpr static int k_literRepresentativeIndex = 0;
  static_assert(
      StringsAreEqual(
          k_volumeRepresentatives[k_literRepresentativeIndex].m_rootSymbols,
          BuiltinsAliases::k_litersAliases),
      "Index for the Liter UnitRepresentative is incorrect.");
  constexpr static int k_cupRepresentativeIndex = 4;
  static_assert(
      StringsAreEqual(
          k_volumeRepresentatives[k_cupRepresentativeIndex].m_rootSymbols,
          "cup"),
      "Index for the Cup UnitRepresentative is incorrect.");
  constexpr static int k_pintRepresentativeIndex = 5;
  static_assert(
      StringsAreEqual(
          k_volumeRepresentatives[k_pintRepresentativeIndex].m_rootSymbols,
          "pt"),
      "Index for the Pint UnitRepresentative is incorrect.");
  constexpr static int k_quartRepresentativeIndex = 6;
  static_assert(
      StringsAreEqual(
          k_volumeRepresentatives[k_quartRepresentativeIndex].m_rootSymbols,
          "qt"),
      "Index for the Quart UnitRepresentative is incorrect.");
  constexpr static int k_gallonRepresentativeIndex = 7;
  static_assert(
      StringsAreEqual(
          k_volumeRepresentatives[k_gallonRepresentativeIndex].m_rootSymbols,
          "gal"),
      "Index for the Gallon UnitRepresentative is incorrect.");

  static bool CanParse(UnicodeDecoder* name,
                       const UnitRepresentative** representative,
                       const UnitPrefix** prefix);
#if 0
  static void ChooseBestRepresentativeAndPrefixForValue(
      const Tree* units, double* value,
      const ReductionContext& reductionContext);
  static bool ShouldDisplayAdditionalOutputs(double value, const Tree* unit,
                                             UnitFormat unitFormat);
  static int SetAdditionalExpressions(const Tree* units, double value,
                                      Tree* dest, int availableLength,
                                      const ReductionContext& reductionContext,
                                      const Tree exactOutput);
  static Tree* BuildSplit(double value, const Unit* units, int length,
                          const ReductionContext& reductionContext);
  static Tree* ConvertTemperatureUnits(
      Tree* e, Unit unit, const ReductionContext& reductionContext);
#endif

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitRepresentative*
      k_timeRepresentativesAllowingImplicitAddition[] = {
          &k_timeRepresentatives[0],  // s
          &k_timeRepresentatives[1],  // min
          &k_timeRepresentatives[2],  // h
          &k_timeRepresentatives[3],  // day
          &k_timeRepresentatives[5],  // month
          &k_timeRepresentatives[6],  // year
  };
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[0]
                                    ->m_rootSymbols,
                                "s"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[1]
                                    ->m_rootSymbols,
                                "min"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[2]
                                    ->m_rootSymbols,
                                "h"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[3]
                                    ->m_rootSymbols,
                                "day"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[4]
                                    ->m_rootSymbols,
                                "month"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[5]
                                    ->m_rootSymbols,
                                "year"),
                "Implicit addition between units has wrong unit");

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitRepresentative*
      k_distanceRepresentativesAllowingImplicitAddition[] = {
          &k_distanceRepresentatives[4],  // in
          &k_distanceRepresentatives[5],  // ft
          &k_distanceRepresentatives[6],  // yd
          &k_distanceRepresentatives[7]   // mi
  };
  static_assert(
      StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[0]->m_rootSymbols,
          "in"),
      "Implicit addition between units has wrong unit");
  static_assert(
      StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[1]->m_rootSymbols,
          "ft"),
      "Implicit addition between units has wrong unit");
  static_assert(
      StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[2]->m_rootSymbols,
          "yd"),
      "Implicit addition between units has wrong unit");
  static_assert(
      StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[3]->m_rootSymbols,
          "mi"),
      "Implicit addition between units has wrong unit");

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitRepresentative*
      k_massRepresentativesAllowingImplicitAddition[] = {
          &k_massRepresentatives[3],  // oz
          &k_massRepresentatives[4]   // lb
  };
  static_assert(StringsAreEqual(k_massRepresentativesAllowingImplicitAddition[0]
                                    ->m_rootSymbols,
                                "oz"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_massRepresentativesAllowingImplicitAddition[1]
                                    ->m_rootSymbols,
                                "lb"),
                "Implicit addition between units has wrong unit");

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitRepresentative*
      k_angleRepresentativesAllowingImplicitAddition[] = {
          &k_angleRepresentatives[1],  // "
          &k_angleRepresentatives[2],  // '
          &k_angleRepresentatives[3]   // °
  };
  static_assert(
      StringsAreEqual(
          k_angleRepresentativesAllowingImplicitAddition[0]->m_rootSymbols,
          "\""),
      "Implicit addition between units has wrong unit");
  static_assert(
      StringsAreEqual(
          k_angleRepresentativesAllowingImplicitAddition[1]->m_rootSymbols,
          "'"),
      "Implicit addition between units has wrong unit");
  static_assert(
      StringsAreEqual(
          k_angleRepresentativesAllowingImplicitAddition[2]->m_rootSymbols,
          "°"),
      "Implicit addition between units has wrong unit");

  struct RepresentativesList {
    const UnitRepresentative* const* representativesList;
    int length;
  };
  constexpr static RepresentativesList
      k_representativesAllowingImplicitAddition[] = {
          {k_timeRepresentativesAllowingImplicitAddition,
           std::size(k_timeRepresentativesAllowingImplicitAddition)},
          {k_distanceRepresentativesAllowingImplicitAddition,
           std::size(k_distanceRepresentativesAllowingImplicitAddition)},
          {k_massRepresentativesAllowingImplicitAddition,
           std::size(k_massRepresentativesAllowingImplicitAddition)},
          {k_angleRepresentativesAllowingImplicitAddition,
           std::size(k_angleRepresentativesAllowingImplicitAddition)}};
  constexpr static int k_representativesAllowingImplicitAdditionLength =
      std::size(k_representativesAllowingImplicitAddition);
  static bool AllowImplicitAddition(
      const UnitRepresentative* smallestRepresentative,
      const UnitRepresentative* biggestRepresentative);

  constexpr static const UnitRepresentative*
      k_representativesWithoutLeftMargin[] = {
          &k_angleRepresentatives[1],        // "
          &k_angleRepresentatives[2],        // '
          &k_angleRepresentatives[3],        // °
          &k_temperatureRepresentatives[1],  // °C
          &k_temperatureRepresentatives[2]   // °F
  };
  constexpr static int k_numberOfRepresentativesWithoutLeftMargin =
      std::size(k_representativesWithoutLeftMargin);
  static_assert(StringsAreEqual(
                    k_representativesWithoutLeftMargin[0]->m_rootSymbols, "\""),
                "Wrong unit without margin");
  static_assert(StringsAreEqual(
                    k_representativesWithoutLeftMargin[1]->m_rootSymbols, "'"),
                "Wrong unit without margin");
  static_assert(StringsAreEqual(
                    k_representativesWithoutLeftMargin[2]->m_rootSymbols, "°"),
                "Wrong unit without margin");
  static_assert(StringsAreEqual(
                    k_representativesWithoutLeftMargin[3]->m_rootSymbols, "°C"),
                "Wrong unit without margin");
  static_assert(StringsAreEqual(
                    k_representativesWithoutLeftMargin[4]->m_rootSymbols, "°F"),
                "Wrong unit without margin");

#if 0
  static bool ForceMarginLeftOfUnit(const Unit& unit);

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext);
  Expression shallowBeautify();

  static bool IsBaseUnit(const Tree* unit) {
    return GetRepresentative(unit)->isBaseUnit() &&
           GetPrefix(unit) == GetRepresentative(unit)->basePrefix();
  }
  void chooseBestRepresentativeAndPrefix(
      double* value, double exponent, const ReductionContext& reductionContext,
      bool optimizePrefix);
#endif

  // Replace with SI ratio only.
  static void RemoveUnit(Tree* unit);
  // Push Unit
  static Tree* Push(const UnitRepresentative* unitRepresentative,
                    const UnitPrefix* unitPrefix);
  // UnitRepresentative getter
  static const UnitRepresentative* GetRepresentative(const Tree* unit);
  // UnitPrefix getter
  static const UnitPrefix* GetPrefix(const Tree* unit);
};

}  // namespace PoincareJ

#endif
