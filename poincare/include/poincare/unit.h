#ifndef POINCARE_UNIT_H
#define POINCARE_UNIT_H

#include <poincare/aliases_list.h>
#include <poincare/expression.h>
#include <poincare/helpers.h>

#include <array>

namespace Poincare {

/* The units having the same physical dimension are grouped together.
 * Each such group has a standard representative with a standard prefix.
 *
 * Each representative has
 *  - a root symbol
 *  - a definition, as the conversion ratio with the SI unit of the same
 *    dimensions
 *  - informations on how the representative should be prefixed.
 *
 * Given an representative and a Prefix allowed for that representative, one may
 * get a symbol and an Expression.
 *
 * FIXME ?
 * The UnitNode class holds as members pointers to a Representative and a
 * Prefix. Those nested classes may not be forward
 * declared and must be defined in UnitNode and then aliased in Unit so as
 * to be used outside. That technical limitation could have been avoided if
 * UnitNode were itself a nested class of Unit, say Unit::Node. More
 * generally, turning all the Poincare::...Node classes into nested
 * Poincare::...::Node classes might be a more clever usage of namespaces
 * and scopes.
 */

class Unit;

class UnitNode final : public ExpressionNode {
 public:
  constexpr static int k_numberOfBaseUnits = 8;

  class Prefix {
    friend class Unit;

   public:
    constexpr static int k_numberOfPrefixes = 13;
    static const Prefix* Prefixes();
    static const Prefix* EmptyPrefix();
    const char* symbol() const { return m_symbol; }
    int8_t exponent() const { return m_exponent; }
    int serialize(char* buffer, int bufferSize) const;

   private:
    constexpr Prefix(const char* symbol, int8_t exponent)
        : m_symbol(symbol), m_exponent(exponent) {}

    const char* m_symbol;
    int8_t m_exponent;
  };

  struct DimensionVector {
    // SupportSize is defined as the number of distinct base units.
    size_t supportSize() const;
    static DimensionVector FromBaseUnits(const Expression baseUnits,
                                         bool canIgnoreCoefficients = true);
    const int coefficientAtIndex(size_t i) const {
      assert(i < k_numberOfBaseUnits);
      const int coefficients[k_numberOfBaseUnits] = {time,
                                                     distance,
                                                     angle,
                                                     mass,
                                                     current,
                                                     temperature,
                                                     amountOfSubstance,
                                                     luminuousIntensity};
      return coefficients[i];
    }
    void setCoefficientAtIndex(size_t i, int c) {
      assert(i < k_numberOfBaseUnits);
      int* coefficientsAddresses[k_numberOfBaseUnits] = {&time,
                                                         &distance,
                                                         &angle,
                                                         &mass,
                                                         &current,
                                                         &temperature,
                                                         &amountOfSubstance,
                                                         &luminuousIntensity};
      *(coefficientsAddresses[i]) = c;
    }
    bool operator==(const DimensionVector& rhs) const {
      return time == rhs.time && distance == rhs.distance &&
             angle == rhs.angle && mass == rhs.mass && current == rhs.current &&
             temperature == rhs.temperature &&
             amountOfSubstance == rhs.amountOfSubstance &&
             luminuousIntensity == rhs.luminuousIntensity;
    }
    bool operator!=(const DimensionVector& rhs) const {
      return !(*this == rhs);
    }
    void addAllCoefficients(const DimensionVector other, int factor);
    Expression toBaseUnits() const;
    int time;
    int distance;
    int angle;
    int mass;
    int current;
    int temperature;
    int amountOfSubstance;
    int luminuousIntensity;
  };

  class Representative {
    friend class Unit;

   public:
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
    static const Representative* const* DefaultRepresentatives();
    static const Representative* RepresentativeForDimension(
        DimensionVector vector);
    constexpr Representative(AliasesList rootSymbol,
                             const char* ratioExpression, double ratio,
                             Prefixable inputPrefixable,
                             Prefixable outputPrefixable)
        : m_rootSymbols(rootSymbol),
          m_ratioExpression(ratioExpression),
          m_inputPrefixable(inputPrefixable),
          m_outputPrefixable(outputPrefixable),
          m_ratio(ratio) {}

    virtual const DimensionVector dimensionVector() const {
      return DimensionVector{.time = 0,
                             .distance = 0,
                             .angle = 0,
                             .mass = 0,
                             .current = 0,
                             .temperature = 0,
                             .amountOfSubstance = 0,
                             .luminuousIntensity = 0};
    };
    virtual int numberOfRepresentatives() const { return 0; };
    /* representativesOfSameDimension returns a pointer to the array containing
     * all representatives for this's dimension. */
    virtual const Representative* representativesOfSameDimension() const {
      return nullptr;
    };
    virtual const Prefix* basePrefix() const { return Prefix::EmptyPrefix(); }
    virtual bool isBaseUnit() const { return false; }
    virtual const Representative* standardRepresentative(
        double value, double exponent, const ReductionContext& reductionContext,
        const Prefix** prefix) const {
      return defaultFindBestRepresentative(value, exponent,
                                           representativesOfSameDimension(),
                                           numberOfRepresentatives(), prefix);
    }
    /* hasSpecialAdditionalExpressions return true if the unit has special
     * forms suchas as splits (for time and imperial units) or common
     * conversions (such as speed and energy). */
    virtual bool hasSpecialAdditionalExpressions(
        double value, Preferences::UnitFormat unitFormat) const {
      return false;
    }
    virtual int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const {
      return 0;
    }

    AliasesList rootSymbols() const { return m_rootSymbols; }
    double ratio() const { return m_ratio; }
    bool isInputPrefixable() const {
      return m_inputPrefixable != Prefixable::None;
    }
    bool isOutputPrefixable() const {
      return m_outputPrefixable != Prefixable::None;
    }
    int serialize(char* buffer, int bufferSize, const Prefix* prefix) const;
    bool canParseWithEquivalents(const char* symbol, size_t length,
                                 const Representative** representative,
                                 const Prefix** prefix) const;
    bool canParse(const char* symbol, size_t length,
                  const Prefix** prefix) const;
    Expression toBaseUnits(const ReductionContext& reductionContext) const;
    bool canPrefix(const Prefix* prefix, bool input) const;
    const Prefix* findBestPrefix(double value, double exponent) const;
    Expression ratioExpressionReduced(
        const ReductionContext& reductionContext) const {
      return Expression::Parse(m_ratioExpression, nullptr)
          .deepReduce(reductionContext);
    }

   protected:
    const Representative* defaultFindBestRepresentative(
        double value, double exponent, const Representative* representatives,
        int length, const Prefix** prefix) const;

    AliasesList m_rootSymbols;
    /* m_ratio is the factor used to convert a unit made of the representative
     * and its base prefix into base SI units.
     * ex : m_ratio for Liter is 1e-3 (as 1_L = 1e-3_m).
     *      m_ratio for gram is 1 (as k is its best prefix and _kg is SI)
     * m_ratioExpression is the expression expressed as a formula, to be used
     * as an expression in reductions. */
    const char* m_ratioExpression;
    const Prefixable m_inputPrefixable;
    const Prefixable m_outputPrefixable;
    double m_ratio;
  };

  class TimeRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static TimeRepresentative Default() {
      return TimeRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    bool isBaseUnit() const override {
      return this == representativesOfSameDimension();
    }
    bool hasSpecialAdditionalExpressions(
        double value, Preferences::UnitFormat unitFormat) const override {
      return ratio() * value >= representativesOfSameDimension()[1].ratio();
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;

   private:
    using Representative::Representative;
  };

  class DistanceRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static DistanceRepresentative Default() {
      return DistanceRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    bool isBaseUnit() const override {
      return this == representativesOfSameDimension();
    }
    const Representative* standardRepresentative(
        double value, double exponent, const ReductionContext& reductionContext,
        const Prefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(
        double value, Preferences::UnitFormat unitFormat) const override {
      return unitFormat == Preferences::UnitFormat::Imperial;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;

   private:
    using Representative::Representative;
  };

  class AngleRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static AngleRepresentative Default() {
      return AngleRepresentative(nullptr, nullptr, NAN, Prefixable::None,
                                 Prefixable::None);
    }
    static const Representative* DefaultRepresentativeForAngleUnit(
        Preferences::AngleUnit angleUnit);

    // Returns a beautified expression
    Expression convertInto(Expression value, const Representative* other,
                           const ReductionContext& reductionContext) const;
    const DimensionVector dimensionVector() const override {
      return DimensionVector{.time = 0,
                             .distance = 0,
                             .angle = 1,
                             .mass = 0,
                             .current = 0,
                             .temperature = 0,
                             .amountOfSubstance = 0,
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    bool isBaseUnit() const override {
      return this == representativesOfSameDimension();
    }
    const Representative* standardRepresentative(
        double value, double exponent, const ReductionContext& reductionContext,
        const Prefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(
        double value, Preferences::UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressionsWithExactValue(
        Expression exactValue, double value, Expression* dest,
        int availableLength, const ReductionContext& reductionContext) const;

   private:
    using Representative::Representative;
  };

  class MassRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static MassRepresentative Default() {
      return MassRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    const Prefix* basePrefix() const override;
    bool isBaseUnit() const override {
      return this == representativesOfSameDimension();
    }
    const Representative* standardRepresentative(
        double value, double exponent, const ReductionContext& reductionContext,
        const Prefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(
        double value, Preferences::UnitFormat unitFormat) const override {
      return unitFormat == Preferences::UnitFormat::Imperial;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;

   private:
    using Representative::Representative;
  };

  class CurrentRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static CurrentRepresentative Default() {
      return CurrentRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    bool isBaseUnit() const override {
      return this == representativesOfSameDimension();
    }

   private:
    using Representative::Representative;
  };

  class TemperatureRepresentative : public Representative {
    friend class Unit;

   public:
    static double ConvertTemperatures(double value,
                                      const Representative* source,
                                      const Representative* target);
    constexpr static TemperatureRepresentative Default() {
      return TemperatureRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    bool isBaseUnit() const override {
      return this == representativesOfSameDimension();
    }
    const Representative* standardRepresentative(
        double value, double exponent, const ReductionContext& reductionContext,
        const Prefix** prefix) const override {
      return this;
    }
    bool hasSpecialAdditionalExpressions(
        double value, Preferences::UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;

   private:
    constexpr static double k_celsiusOrigin = 273.15;
    constexpr static double k_fahrenheitOrigin = 459.67;
    using Representative::Representative;
  };

  class AmountOfSubstanceRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static AmountOfSubstanceRepresentative Default() {
      return AmountOfSubstanceRepresentative(
          nullptr, nullptr, NAN, Prefixable::None, Prefixable::None);
    }
    const DimensionVector dimensionVector() const override {
      return DimensionVector{.time = 0,
                             .distance = 0,
                             .angle = 0,
                             .mass = 0,
                             .current = 0,
                             .temperature = 0,
                             .amountOfSubstance = 1,
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    bool isBaseUnit() const override {
      return this == representativesOfSameDimension();
    }

   private:
    using Representative::Representative;
  };

  class LuminousIntensityRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static LuminousIntensityRepresentative Default() {
      return LuminousIntensityRepresentative(
          nullptr, nullptr, NAN, Prefixable::None, Prefixable::None);
    }
    const DimensionVector dimensionVector() const override {
      return DimensionVector{.time = 0,
                             .distance = 0,
                             .angle = 0,
                             .mass = 0,
                             .current = 0,
                             .temperature = 0,
                             .amountOfSubstance = 0,
                             .luminuousIntensity = 1};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    bool isBaseUnit() const override {
      return this == representativesOfSameDimension();
    }

   private:
    using Representative::Representative;
  };

  class FrequencyRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static FrequencyRepresentative Default() {
      return FrequencyRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class ForceRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static ForceRepresentative Default() {
      return ForceRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class PressureRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static PressureRepresentative Default() {
      return PressureRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class EnergyRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static EnergyRepresentative Default() {
      return EnergyRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    bool hasSpecialAdditionalExpressions(
        double value, Preferences::UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;

   private:
    using Representative::Representative;
  };

  class PowerRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static PowerRepresentative Default() {
      return PowerRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class ElectricChargeRepresentative : public Representative {
    friend class Unit;

   public:
    using Representative::Representative;
    constexpr static ElectricChargeRepresentative Default() {
      return ElectricChargeRepresentative(nullptr, nullptr, NAN,
                                          Prefixable::None, Prefixable::None);
    }
    const DimensionVector dimensionVector() const override {
      return DimensionVector{.time = 1,
                             .distance = 0,
                             .angle = 0,
                             .mass = 0,
                             .current = 1,
                             .temperature = 0,
                             .amountOfSubstance = 0,
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
  };

  class ElectricPotentialRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static ElectricPotentialRepresentative Default() {
      return ElectricPotentialRepresentative(
          nullptr, nullptr, NAN, Prefixable::None, Prefixable::None);
    }
    const DimensionVector dimensionVector() const override {
      return DimensionVector{.time = -3,
                             .distance = 2,
                             .angle = 0,
                             .mass = 1,
                             .current = -1,
                             .temperature = 0,
                             .amountOfSubstance = 0,
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class ElectricCapacitanceRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static ElectricCapacitanceRepresentative Default() {
      return ElectricCapacitanceRepresentative(
          nullptr, nullptr, NAN, Prefixable::None, Prefixable::None);
    }
    const DimensionVector dimensionVector() const override {
      return DimensionVector{.time = 4,
                             .distance = -2,
                             .angle = 0,
                             .mass = -1,
                             .current = 2,
                             .temperature = 0,
                             .amountOfSubstance = 0,
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class ElectricResistanceRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static ElectricResistanceRepresentative Default() {
      return ElectricResistanceRepresentative(
          nullptr, nullptr, NAN, Prefixable::None, Prefixable::None);
    }
    const DimensionVector dimensionVector() const override {
      return DimensionVector{.time = -3,
                             .distance = 2,
                             .angle = 0,
                             .mass = 1,
                             .current = -2,
                             .temperature = 0,
                             .amountOfSubstance = 0,
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class ElectricConductanceRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static ElectricConductanceRepresentative Default() {
      return ElectricConductanceRepresentative(
          nullptr, nullptr, NAN, Prefixable::None, Prefixable::None);
    }
    const DimensionVector dimensionVector() const override {
      return DimensionVector{.time = 3,
                             .distance = -2,
                             .angle = 0,
                             .mass = -1,
                             .current = 2,
                             .temperature = 0,
                             .amountOfSubstance = 0,
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class MagneticFluxRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static MagneticFluxRepresentative Default() {
      return MagneticFluxRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class MagneticFieldRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static MagneticFieldRepresentative Default() {
      return MagneticFieldRepresentative(nullptr, nullptr, NAN,
                                         Prefixable::None, Prefixable::None);
    }
    const DimensionVector dimensionVector() const override {
      return DimensionVector{.time = -2,
                             .distance = 0,
                             .angle = 0,
                             .mass = 1,
                             .current = -1,
                             .temperature = 0,
                             .amountOfSubstance = 0,
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class InductanceRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static InductanceRepresentative Default() {
      return InductanceRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class CatalyticActivityRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static CatalyticActivityRepresentative Default() {
      return CatalyticActivityRepresentative(
          nullptr, nullptr, NAN, Prefixable::None, Prefixable::None);
    }
    const DimensionVector dimensionVector() const override {
      return DimensionVector{.time = -1,
                             .distance = 0,
                             .angle = 0,
                             .mass = 0,
                             .current = 0,
                             .temperature = 0,
                             .amountOfSubstance = 1,
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;

   private:
    using Representative::Representative;
  };

  class SurfaceRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static SurfaceRepresentative Default() {
      return SurfaceRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    const Representative* standardRepresentative(
        double value, double exponent, const ReductionContext& reductionContext,
        const Prefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(
        double value, Preferences::UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;

   private:
    using Representative::Representative;
  };

  class VolumeRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static VolumeRepresentative Default() {
      return VolumeRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    int numberOfRepresentatives() const override;
    const Representative* representativesOfSameDimension() const override;
    const Representative* standardRepresentative(
        double value, double exponent, const ReductionContext& reductionContext,
        const Prefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(
        double value, Preferences::UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;

   private:
    using Representative::Representative;
  };

  class SpeedRepresentative : public Representative {
    friend class Unit;

   public:
    constexpr static SpeedRepresentative Default() {
      return SpeedRepresentative(nullptr, nullptr, NAN, Prefixable::None,
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
                             .luminuousIntensity = 0};
    }
    const Representative* standardRepresentative(
        double value, double exponent, const ReductionContext& reductionContext,
        const Prefix** prefix) const override {
      return nullptr;
    }
    bool hasSpecialAdditionalExpressions(
        double value, Preferences::UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;

   private:
    using Representative::Representative;
  };

  UnitNode(const Representative* representative, const Prefix* prefix)
      : ExpressionNode(), m_representative(representative), m_prefix(prefix) {}

  // TreeNode
  size_t size() const override { return sizeof(UnitNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Unit"; }
  void logAttributes(std::ostream& stream) const override {
    stream << " prefix=\"" << m_prefix->symbol() << "\"";
    stream << " rootSymbol=\"" << m_representative->rootSymbols().mainAlias()
           << "\"";
  }
#endif

  // Expression Properties
  Type type() const override { return Type::Unit; }
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

  const Representative* representative() const { return m_representative; }
  const Prefix* prefix() const { return m_prefix; }
  void setRepresentative(const Representative* representative) {
    m_representative = representative;
  }
  void setPrefix(const Prefix* prefix) { m_prefix = prefix; }

 private:
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;

  const Representative* m_representative;
  const Prefix* m_prefix;
};

/* For each representative, we need two definitions at compile time : one as a
 * parsable expression (to be used in reductions) and one as a floating-point
 * number, to be used for all other use cases. We use a macro to duplicate the
 * definition, so as to not write it twice. */
#define STR(x) #x
#define DEFINE_TWICE(x) STR(x), x

class Unit : public Expression {
  friend class UnitNode;

 public:
  /* Prefixes and Representativees defined below must be defined only once and
   * all units must be constructed from their pointers. This way we can easily
   * check if two Unit objects are equal by comparing pointers. This saves us
   * from overloading the == operator on Prefix and Representative and saves
   * time at execution. As such, their constructor are private and can only be
   * accessed by their friend class Unit. */
  typedef UnitNode::Prefix Prefix;
  constexpr static const Prefix k_prefixes[Prefix::k_numberOfPrefixes] = {
      Prefix("p", -12), Prefix("n", -9), Prefix("μ", -6), Prefix("m", -3),
      Prefix("c", -2),  Prefix("d", -1), Prefix("", 0),   Prefix("da", 1),
      Prefix("h", 2),   Prefix("k", 3),  Prefix("M", 6),  Prefix("G", 9),
      Prefix("T", 12),
  };
  typedef UnitNode::Representative Representative;
  typedef Representative::Prefixable Prefixable;
  typedef UnitNode::TimeRepresentative TimeRepresentative;
  constexpr static const TimeRepresentative k_timeRepresentatives[] = {
      TimeRepresentative("s", DEFINE_TWICE(1.), Prefixable::All,
                         Prefixable::NegativeLongScale),
      TimeRepresentative("min", DEFINE_TWICE(60.), Prefixable::None,
                         Prefixable::None),
      TimeRepresentative("h", DEFINE_TWICE(3600.), Prefixable::None,
                         Prefixable::None),
      TimeRepresentative("day", DEFINE_TWICE(86400.), Prefixable::None,
                         Prefixable::None),
      TimeRepresentative("week", DEFINE_TWICE(604800.), Prefixable::None,
                         Prefixable::None),
      TimeRepresentative("month", DEFINE_TWICE(2629800.), Prefixable::None,
                         Prefixable::None),
      TimeRepresentative("year", DEFINE_TWICE(31557600.), Prefixable::None,
                         Prefixable::None),
  };
  typedef UnitNode::DistanceRepresentative DistanceRepresentative;
  constexpr static const DistanceRepresentative k_distanceRepresentatives[] = {
      DistanceRepresentative("m", DEFINE_TWICE(1.), Prefixable::All,
                             Prefixable::NegativeAndKilo),
      DistanceRepresentative("au", DEFINE_TWICE(149597870700.),
                             Prefixable::None, Prefixable::None),
      DistanceRepresentative("ly", DEFINE_TWICE(299792458. * 31557600.),
                             Prefixable::None, Prefixable::None),
      DistanceRepresentative("pc", "180*3600/π*149587870700",
                             180 * 3600 / M_PI * 149587870700, Prefixable::None,
                             Prefixable::None),
      DistanceRepresentative("in", DEFINE_TWICE(0.0254), Prefixable::None,
                             Prefixable::None),
      DistanceRepresentative("ft", DEFINE_TWICE(12 * 0.0254), Prefixable::None,
                             Prefixable::None),
      DistanceRepresentative("yd", DEFINE_TWICE(36 * 0.0254), Prefixable::None,
                             Prefixable::None),
      DistanceRepresentative("mi", DEFINE_TWICE(63360 * 0.0254),
                             Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::AngleRepresentative AngleRepresentative;
  static constexpr const AngleRepresentative k_angleRepresentatives[] = {
      AngleRepresentative("rad", DEFINE_TWICE(1.), Prefixable::None,
                          Prefixable::None),
      AngleRepresentative("\"", "π/648000", M_PI / 648000, Prefixable::None,
                          Prefixable::None),
      AngleRepresentative("'", "π/10800", M_PI / 10800, Prefixable::None,
                          Prefixable::None),
      AngleRepresentative("°", "π/180", M_PI / 180, Prefixable::None,
                          Prefixable::None),
      AngleRepresentative("gon", "π/200", M_PI / 200, Prefixable::None,
                          Prefixable::None),
  };
  typedef UnitNode::MassRepresentative MassRepresentative;
  constexpr static const MassRepresentative k_massRepresentatives[] = {
      MassRepresentative("g", DEFINE_TWICE(1.), Prefixable::All,
                         Prefixable::NegativeAndKilo),
      MassRepresentative("t", DEFINE_TWICE(1000.),
                         Prefixable::PositiveLongScale,
                         Prefixable::PositiveLongScale),
      MassRepresentative("Da", "1/(6.02214076ᴇ26)", 1 / (6.02214076e26),
                         Prefixable::All, Prefixable::All),
      MassRepresentative("oz", DEFINE_TWICE(0.028349523125), Prefixable::None,
                         Prefixable::None),
      MassRepresentative("lb", DEFINE_TWICE(16 * 0.028349523125),
                         Prefixable::None, Prefixable::None),
      MassRepresentative("shtn", DEFINE_TWICE(2000 * 16 * 0.028349523125),
                         Prefixable::None, Prefixable::None),
      MassRepresentative("lgtn", DEFINE_TWICE(2240 * 16 * 0.028349523125),
                         Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::CurrentRepresentative CurrentRepresentative;
  constexpr static const CurrentRepresentative k_currentRepresentatives[] = {
      CurrentRepresentative("A", DEFINE_TWICE(1.), Prefixable::All,
                            Prefixable::LongScale)};
  typedef UnitNode::TemperatureRepresentative TemperatureRepresentative;
  constexpr static const TemperatureRepresentative
      k_temperatureRepresentatives[] = {
          TemperatureRepresentative("K", DEFINE_TWICE(1.), Prefixable::All,
                                    Prefixable::None),
          TemperatureRepresentative("°C", DEFINE_TWICE(1.), Prefixable::None,
                                    Prefixable::None),
          TemperatureRepresentative("°F", DEFINE_TWICE(5. / 9.),
                                    Prefixable::None, Prefixable::None),
      };
  typedef UnitNode::AmountOfSubstanceRepresentative
      AmountOfSubstanceRepresentative;
  constexpr static const AmountOfSubstanceRepresentative
      k_amountOfSubstanceRepresentatives[] = {AmountOfSubstanceRepresentative(
          "mol", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::LuminousIntensityRepresentative
      LuminousIntensityRepresentative;
  constexpr static const LuminousIntensityRepresentative
      k_luminousIntensityRepresentatives[] = {LuminousIntensityRepresentative(
          "cd", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::FrequencyRepresentative FrequencyRepresentative;
  constexpr static const FrequencyRepresentative k_frequencyRepresentatives[] =
      {FrequencyRepresentative("Hz", DEFINE_TWICE(1.), Prefixable::All,
                               Prefixable::LongScale)};
  typedef UnitNode::ForceRepresentative ForceRepresentative;
  constexpr static const ForceRepresentative k_forceRepresentatives[] = {
      ForceRepresentative("N", DEFINE_TWICE(1.), Prefixable::All,
                          Prefixable::LongScale)};
  typedef UnitNode::PressureRepresentative PressureRepresentative;
  constexpr static const PressureRepresentative k_pressureRepresentatives[] = {
      PressureRepresentative("Pa", DEFINE_TWICE(1.), Prefixable::All,
                             Prefixable::LongScale),
      PressureRepresentative("bar", DEFINE_TWICE(100000.), Prefixable::All,
                             Prefixable::LongScale),
      PressureRepresentative("atm", DEFINE_TWICE(101325.), Prefixable::None,
                             Prefixable::None),
  };
  typedef UnitNode::EnergyRepresentative EnergyRepresentative;
  constexpr static const EnergyRepresentative k_energyRepresentatives[] = {
      EnergyRepresentative("J", DEFINE_TWICE(1.), Prefixable::All,
                           Prefixable::LongScale),
      EnergyRepresentative("eV", "1.602176634ᴇ-19", 1.602176634e-19,
                           Prefixable::All, Prefixable::LongScale),
      EnergyRepresentative("Cal", "4.184", 4.184,
                           Prefixable::All, Prefixable::LongScale),
  };
  typedef UnitNode::PowerRepresentative PowerRepresentative;
  constexpr static const PowerRepresentative k_powerRepresentatives[] = {

      PowerRepresentative("W", DEFINE_TWICE(1.), Prefixable::All,
                          Prefixable::LongScale),
      PowerRepresentative("hp", DEFINE_TWICE(745.699872), Prefixable::None,
                          Prefixable::None)};
  typedef UnitNode::ElectricChargeRepresentative ElectricChargeRepresentative;
  constexpr static const ElectricChargeRepresentative
      k_electricChargeRepresentatives[] = {ElectricChargeRepresentative(
          "C", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::ElectricPotentialRepresentative
      ElectricPotentialRepresentative;
  constexpr static const ElectricPotentialRepresentative
      k_electricPotentialRepresentatives[] = {ElectricPotentialRepresentative(
          "V", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::ElectricCapacitanceRepresentative
      ElectricCapacitanceRepresentative;
  constexpr static const ElectricCapacitanceRepresentative
      k_electricCapacitanceRepresentatives[] = {
          ElectricCapacitanceRepresentative(
              "F", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::ElectricResistanceRepresentative
      ElectricResistanceRepresentative;
  constexpr static const ElectricResistanceRepresentative
      k_electricResistanceRepresentatives[] = {ElectricResistanceRepresentative(
          "Ω", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::ElectricConductanceRepresentative
      ElectricConductanceRepresentative;
  constexpr static const ElectricConductanceRepresentative
      k_electricConductanceRepresentatives[] = {
          ElectricConductanceRepresentative(
              "S", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::MagneticFluxRepresentative MagneticFluxRepresentative;
  constexpr static const MagneticFluxRepresentative
      k_magneticFluxRepresentatives[] = {MagneticFluxRepresentative(
          "Wb", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::MagneticFieldRepresentative MagneticFieldRepresentative;
  constexpr static const MagneticFieldRepresentative
      k_magneticFieldRepresentatives[] = {MagneticFieldRepresentative(
          "T", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::InductanceRepresentative InductanceRepresentative;
  constexpr static const InductanceRepresentative
      k_inductanceRepresentatives[] = {InductanceRepresentative(
          "H", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::CatalyticActivityRepresentative
      CatalyticActivityRepresentative;
  constexpr static const CatalyticActivityRepresentative
      k_catalyticActivityRepresentatives[] = {CatalyticActivityRepresentative(
          "kat", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale)};
  typedef UnitNode::SurfaceRepresentative SurfaceRepresentative;
  constexpr static const SurfaceRepresentative k_surfaceRepresentatives[] = {
      SurfaceRepresentative("ha", DEFINE_TWICE(10000.), Prefixable::None,
                            Prefixable::None),
      SurfaceRepresentative("acre", DEFINE_TWICE(4046.8564224),
                            Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::VolumeRepresentative VolumeRepresentative;
  constexpr static const VolumeRepresentative k_volumeRepresentatives[] = {
      VolumeRepresentative(AliasesLists::k_litersAliases, DEFINE_TWICE(0.001),
                           Prefixable::All, Prefixable::Negative),
      VolumeRepresentative("tsp", DEFINE_TWICE(0.00000492892159375),
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("tbsp", DEFINE_TWICE(3 * 0.00000492892159375),
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("floz", DEFINE_TWICE(0.0000295735295625),
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("cup", DEFINE_TWICE(8 * 0.0000295735295625),
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("pt", DEFINE_TWICE(16 * 0.0000295735295625),
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("qt", DEFINE_TWICE(32 * 0.0000295735295625),
                           Prefixable::None, Prefixable::None),
      VolumeRepresentative("gal", DEFINE_TWICE(128 * 0.0000295735295625),
                           Prefixable::None, Prefixable::None),
  };

  /* Define access points to some prefixes and representatives. */
  constexpr static int k_emptyPrefixIndex = 6;
  static_assert(k_prefixes[k_emptyPrefixIndex].m_exponent == 0,
                "Index for the Empty Prefix is incorrect.");
  constexpr static int k_kiloPrefixIndex = 9;
  static_assert(k_prefixes[k_kiloPrefixIndex].m_exponent == 3,
                "Index for the Kilo Prefix is incorrect.");
  constexpr static int k_secondRepresentativeIndex = 0;
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentatives[k_secondRepresentativeIndex].m_rootSymbols,
          "s"),
      "Index for the Second Representative is incorrect.");
  constexpr static int k_minuteRepresentativeIndex = 1;
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentatives[k_minuteRepresentativeIndex].m_rootSymbols,
          "min"),
      "Index for the Minute Representative is incorrect.");
  constexpr static int k_hourRepresentativeIndex = 2;
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentatives[k_hourRepresentativeIndex].m_rootSymbols, "h"),
      "Index for the Hour Representative is incorrect.");
  constexpr static int k_dayRepresentativeIndex = 3;
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentatives[k_dayRepresentativeIndex].m_rootSymbols, "day"),
      "Index for the Day Representative is incorrect.");
  constexpr static int k_monthRepresentativeIndex = 5;
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentatives[k_monthRepresentativeIndex].m_rootSymbols,
          "month"),
      "Index for the Month Representative is incorrect.");
  constexpr static int k_yearRepresentativeIndex = 6;
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentatives[k_yearRepresentativeIndex].m_rootSymbols,
          "year"),
      "Index for the Year Representative is incorrect.");
  constexpr static int k_meterRepresentativeIndex = 0;
  static_assert(
      Helpers::StringsAreEqual(
          k_distanceRepresentatives[k_meterRepresentativeIndex].m_rootSymbols,
          "m"),
      "Index for the Meter Representative is incorrect.");
  constexpr static int k_inchRepresentativeIndex = 4;
  static_assert(
      Helpers::StringsAreEqual(
          k_distanceRepresentatives[k_inchRepresentativeIndex].m_rootSymbols,
          "in"),
      "Index for the Inch Representative is incorrect.");
  constexpr static int k_footRepresentativeIndex = 5;
  static_assert(
      Helpers::StringsAreEqual(
          k_distanceRepresentatives[k_footRepresentativeIndex].m_rootSymbols,
          "ft"),
      "Index for the Foot Representative is incorrect.");
  constexpr static int k_yardRepresentativeIndex = 6;
  static_assert(
      Helpers::StringsAreEqual(
          k_distanceRepresentatives[k_yardRepresentativeIndex].m_rootSymbols,
          "yd"),
      "Index for the Yard Representative is incorrect.");
  constexpr static int k_mileRepresentativeIndex = 7;
  static_assert(
      Helpers::StringsAreEqual(
          k_distanceRepresentatives[k_mileRepresentativeIndex].m_rootSymbols,
          "mi"),
      "Index for the Mile Representative is incorrect.");
  constexpr static int k_radianRepresentativeIndex = 0;
  static_assert(
      Helpers::StringsAreEqual(
          k_angleRepresentatives[k_radianRepresentativeIndex].m_rootSymbols,
          "rad"),
      "Index for the Radian Representative is incorrect.");
  constexpr static int k_arcSecondRepresentativeIndex = 1;
  static_assert(
      Helpers::StringsAreEqual(
          k_angleRepresentatives[k_arcSecondRepresentativeIndex].m_rootSymbols,
          "\""),
      "Index for the ArcSecond Representative is incorrect.");
  constexpr static int k_arcMinuteRepresentativeIndex = 2;
  static_assert(
      Helpers::StringsAreEqual(
          k_angleRepresentatives[k_arcMinuteRepresentativeIndex].m_rootSymbols,
          "'"),
      "Index for the ArcMinute Representative is incorrect.");
  constexpr static int k_degreeRepresentativeIndex = 3;
  static_assert(
      Helpers::StringsAreEqual(
          k_angleRepresentatives[k_degreeRepresentativeIndex].m_rootSymbols,
          "°"),
      "Index for the Degree Representative is incorrect.");
  constexpr static int k_gradianRepresentativeIndex = 4;
  static_assert(
      Helpers::StringsAreEqual(
          k_angleRepresentatives[k_gradianRepresentativeIndex].m_rootSymbols,
          "gon"),
      "Index for the Gradian Representative is incorrect.");
  constexpr static int k_gramRepresentativeIndex = 0;
  static_assert(
      Helpers::StringsAreEqual(
          k_massRepresentatives[k_gramRepresentativeIndex].m_rootSymbols, "g"),
      "Index for the Gram Representative is incorrect.");
  constexpr static int k_tonRepresentativeIndex = 1;
  static_assert(
      Helpers::StringsAreEqual(
          k_massRepresentatives[k_tonRepresentativeIndex].m_rootSymbols, "t"),
      "Index for the Ton Representative is incorrect.");
  constexpr static int k_ounceRepresentativeIndex = 3;
  static_assert(
      Helpers::StringsAreEqual(
          k_massRepresentatives[k_ounceRepresentativeIndex].m_rootSymbols,
          "oz"),
      "Index for the Ounce Representative is incorrect.");
  constexpr static int k_poundRepresentativeIndex = 4;
  static_assert(
      Helpers::StringsAreEqual(
          k_massRepresentatives[k_poundRepresentativeIndex].m_rootSymbols,
          "lb"),
      "Index for the Pound Representative is incorrect.");
  constexpr static int k_shortTonRepresentativeIndex = 5;
  static_assert(
      Helpers::StringsAreEqual(
          k_massRepresentatives[k_shortTonRepresentativeIndex].m_rootSymbols,
          "shtn"),
      "Index for the Short Ton Representative is incorrect.");
  constexpr static int k_kelvinRepresentativeIndex = 0;
  static_assert(Helpers::StringsAreEqual(
                    k_temperatureRepresentatives[k_kelvinRepresentativeIndex]
                        .m_rootSymbols,
                    "K"),
                "Index for the Kelvin Representative is incorrect.");
  constexpr static int k_celsiusRepresentativeIndex = 1;
  static_assert(Helpers::StringsAreEqual(
                    k_temperatureRepresentatives[k_celsiusRepresentativeIndex]
                        .m_rootSymbols,
                    "°C"),
                "Index for the Celsius Representative is incorrect.");
  constexpr static int k_fahrenheitRepresentativeIndex = 2;
  static_assert(
      Helpers::StringsAreEqual(
          k_temperatureRepresentatives[k_fahrenheitRepresentativeIndex]
              .m_rootSymbols,
          "°F"),
      "Index for the Fahrenheit Representative is incorrect.");
  constexpr static int k_jouleRepresentativeIndex = 0;
  static_assert(
      Helpers::StringsAreEqual(
          k_energyRepresentatives[k_jouleRepresentativeIndex].m_rootSymbols,
          "J"),
      "Index for the Joule Representative is incorrect.");
  constexpr static int k_electronVoltRepresentativeIndex = 1;
  static_assert(Helpers::StringsAreEqual(
                    k_energyRepresentatives[k_electronVoltRepresentativeIndex]
                        .m_rootSymbols,
                    "eV"),
                "Index for the Electron Volt Representative is incorrect.");
  constexpr static int k_wattRepresentativeIndex = 0;
  static_assert(
      Helpers::StringsAreEqual(
          k_powerRepresentatives[k_wattRepresentativeIndex].m_rootSymbols, "W"),
      "Index for the Watt Representative is incorrect.");
  constexpr static int k_hectareRepresentativeIndex = 0;
  static_assert(
      Helpers::StringsAreEqual(
          k_surfaceRepresentatives[k_hectareRepresentativeIndex].m_rootSymbols,
          "ha"),
      "Index for the Hectare Representative is incorrect.");
  constexpr static int k_acreRepresentativeIndex = 1;
  static_assert(
      Helpers::StringsAreEqual(
          k_surfaceRepresentatives[k_acreRepresentativeIndex].m_rootSymbols,
          "acre"),
      "Index for the Acre Representative is incorrect.");
  constexpr static int k_literRepresentativeIndex = 0;
  static_assert(
      Helpers::StringsAreEqual(
          k_volumeRepresentatives[k_literRepresentativeIndex].m_rootSymbols,
          AliasesLists::k_litersAliases),
      "Index for the Liter Representative is incorrect.");
  constexpr static int k_cupRepresentativeIndex = 4;
  static_assert(
      Helpers::StringsAreEqual(
          k_volumeRepresentatives[k_cupRepresentativeIndex].m_rootSymbols,
          "cup"),
      "Index for the Cup Representative is incorrect.");
  constexpr static int k_pintRepresentativeIndex = 5;
  static_assert(
      Helpers::StringsAreEqual(
          k_volumeRepresentatives[k_pintRepresentativeIndex].m_rootSymbols,
          "pt"),
      "Index for the Pint Representative is incorrect.");
  constexpr static int k_quartRepresentativeIndex = 6;
  static_assert(
      Helpers::StringsAreEqual(
          k_volumeRepresentatives[k_quartRepresentativeIndex].m_rootSymbols,
          "qt"),
      "Index for the Quart Representative is incorrect.");
  constexpr static int k_gallonRepresentativeIndex = 7;
  static_assert(
      Helpers::StringsAreEqual(
          k_volumeRepresentatives[k_gallonRepresentativeIndex].m_rootSymbols,
          "gal"),
      "Index for the Gallon Representative is incorrect.");

  Unit(const UnitNode* node) : Expression(node) {}
  static Unit Builder(const Representative* representative,
                      const Prefix* prefix = Prefix::EmptyPrefix());
  static bool CanParse(const char* symbol, size_t length,
                       const Representative** representative,
                       const Prefix** prefix);
  static void ChooseBestRepresentativeAndPrefixForValue(
      Expression units, double* value,
      const ReductionContext& reductionContext);
  static bool ShouldDisplayAdditionalOutputs(
      double value, Expression unit, Preferences::UnitFormat unitFormat);
  static int SetAdditionalExpressions(Expression units, double value,
                                      Expression* dest, int availableLength,
                                      const ReductionContext& reductionContext,
                                      const Expression exactOutput);
  static Expression BuildSplit(double value, const Unit* units, int length,
                               const ReductionContext& reductionContext);
  static Expression ConvertTemperatureUnits(
      Expression e, Unit unit, const ReductionContext& reductionContext);
  static bool IsForbiddenTemperatureProduct(Expression e);

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitNode::Representative*
      k_timeRepresentativesAllowingImplicitAddition[] = {
          &k_timeRepresentatives[0],  // s
          &k_timeRepresentatives[1],  // min
          &k_timeRepresentatives[2],  // h
          &k_timeRepresentatives[3],  // day
          &k_timeRepresentatives[5],  // month
          &k_timeRepresentatives[6],  // year
      };
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentativesAllowingImplicitAddition[0]->m_rootSymbols, "s"),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentativesAllowingImplicitAddition[1]->m_rootSymbols,
          "min"),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentativesAllowingImplicitAddition[2]->m_rootSymbols, "h"),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentativesAllowingImplicitAddition[3]->m_rootSymbols,
          "day"),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentativesAllowingImplicitAddition[4]->m_rootSymbols,
          "month"),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_timeRepresentativesAllowingImplicitAddition[5]->m_rootSymbols,
          "year"),
      "Implicit addition between units has wrong unit");

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitNode::Representative*
      k_distanceRepresentativesAllowingImplicitAddition[] = {
          &k_distanceRepresentatives[4],  // in
          &k_distanceRepresentatives[5],  // ft
          &k_distanceRepresentatives[6],  // yd
          &k_distanceRepresentatives[7]   // mi
      };
  static_assert(
      Helpers::StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[0]->m_rootSymbols,
          "in"),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[1]->m_rootSymbols,
          "ft"),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[2]->m_rootSymbols,
          "yd"),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[3]->m_rootSymbols,
          "mi"),
      "Implicit addition between units has wrong unit");

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitNode::Representative*
      k_massRepresentativesAllowingImplicitAddition[] = {
          &k_massRepresentatives[3],  // oz
          &k_massRepresentatives[4]   // lb
      };
  static_assert(
      Helpers::StringsAreEqual(
          k_massRepresentativesAllowingImplicitAddition[0]->m_rootSymbols,
          "oz"),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_massRepresentativesAllowingImplicitAddition[1]->m_rootSymbols,
          "lb"),
      "Implicit addition between units has wrong unit");

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitNode::Representative*
      k_angleRepresentativesAllowingImplicitAddition[] = {
          &k_angleRepresentatives[1],  // "
          &k_angleRepresentatives[2],  // '
          &k_angleRepresentatives[3]   // °
      };
  static_assert(
      Helpers::StringsAreEqual(
          k_angleRepresentativesAllowingImplicitAddition[0]->m_rootSymbols,
          "\""),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_angleRepresentativesAllowingImplicitAddition[1]->m_rootSymbols,
          "'"),
      "Implicit addition between units has wrong unit");
  static_assert(
      Helpers::StringsAreEqual(
          k_angleRepresentativesAllowingImplicitAddition[2]->m_rootSymbols,
          "°"),
      "Implicit addition between units has wrong unit");

  struct RepresentativesList {
    const UnitNode::Representative* const* representativesList;
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
      const UnitNode::Representative* smallestRepresentative,
      const UnitNode::Representative* biggestRepresentative);

  constexpr static const UnitNode::Representative*
      k_representativesWithoutLeftMargin[] = {
          &k_angleRepresentatives[1],        // "
          &k_angleRepresentatives[2],        // '
          &k_angleRepresentatives[3],        // °
          &k_temperatureRepresentatives[1],  // °C
          &k_temperatureRepresentatives[2]   // °F
      };
  constexpr static int k_numberOfRepresentativesWithoutLeftMargin =
      std::size(k_representativesWithoutLeftMargin);
  static_assert(Helpers::StringsAreEqual(
                    k_representativesWithoutLeftMargin[0]->m_rootSymbols, "\""),
                "Wrong unit without margin");
  static_assert(Helpers::StringsAreEqual(
                    k_representativesWithoutLeftMargin[1]->m_rootSymbols, "'"),
                "Wrong unit without margin");
  static_assert(Helpers::StringsAreEqual(
                    k_representativesWithoutLeftMargin[2]->m_rootSymbols, "°"),
                "Wrong unit without margin");
  static_assert(Helpers::StringsAreEqual(
                    k_representativesWithoutLeftMargin[3]->m_rootSymbols, "°C"),
                "Wrong unit without margin");
  static_assert(Helpers::StringsAreEqual(
                    k_representativesWithoutLeftMargin[4]->m_rootSymbols, "°F"),
                "Wrong unit without margin");

  static bool ForceMarginLeftOfUnit(const Unit& unit);

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext);
  Expression shallowBeautify();

  bool isBaseUnit() const {
    return node()->representative()->isBaseUnit() &&
           node()->prefix() == node()->representative()->basePrefix();
  }
  void chooseBestRepresentativeAndPrefix(
      double* value, double exponent, const ReductionContext& reductionContext,
      bool optimizePrefix);

  const Representative* representative() const {
    return node()->representative();
  }

 private:
  UnitNode* node() const { return static_cast<UnitNode*>(Expression::node()); }
  Expression removeUnit(Expression* unit);
};

}  // namespace Poincare

#endif
