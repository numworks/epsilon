#ifndef POINCARE_UNIT_H
#define POINCARE_UNIT_H

#include <poincare/expression.h>
#include <apps/i18n.h>

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
   * Given an representative and a Prefix allowed for that representative, one may get
   * a symbol and an Expression.
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
  static constexpr int k_numberOfBaseUnits = 7;

  class Prefix {
    friend class Unit;
  public:
    static constexpr int k_numberOfPrefixes = 13;
    static const Prefix * Prefixes();
    static const Prefix * EmptyPrefix();
    const char * symbol() const { return m_symbol; }
    int8_t exponent() const { return m_exponent; }
    int serialize(char * buffer, int bufferSize) const;
  private:
    constexpr Prefix(const char * symbol, int8_t exponent) :
      m_symbol(symbol),
      m_exponent(exponent)
    {}

    const char * m_symbol;
    int8_t m_exponent;
  };

  template<typename T>
  struct Vector {
    // SupportSize is defined as the number of distinct base units.
    size_t supportSize() const;
    static Vector FromBaseUnits(const Expression baseUnits);
    const T coefficientAtIndex(size_t i) const {
      assert(i < k_numberOfBaseUnits);
      const T coefficients[k_numberOfBaseUnits] = {time, distance, mass, current, temperature, amountOfSubstance, luminousIntensity};
      return coefficients[i];
    }
    void setCoefficientAtIndex(size_t i, T c) {
      assert(i < k_numberOfBaseUnits);
      T * coefficientsAddresses[k_numberOfBaseUnits] = {&time, &distance, &mass, &current, &temperature, &amountOfSubstance, &luminousIntensity};
      *(coefficientsAddresses[i]) = c;
    }
    bool operator==(const Vector &rhs) const { return time == rhs.time && distance == rhs.distance && mass == rhs.mass && current == rhs.current && temperature == rhs.temperature && amountOfSubstance == rhs.amountOfSubstance && luminousIntensity == rhs.luminousIntensity; }
    bool operator!=(const Vector &rhs) const { return !(*this == rhs); }
    void addAllCoefficients(const Vector other, int factor);
    Expression toBaseUnits() const;
    T time;
    T distance;
    T mass;
    T current;
    T temperature;
    T amountOfSubstance;
    T luminousIntensity;
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
    static constexpr int k_numberOfDimensions = 24;
    static const Representative * const * DefaultRepresentatives();
    static const Representative * RepresentativeForDimension(Vector<int> vector);
    constexpr Representative(const char * rootSymbol, double ratio, Prefixable inputPrefixable, Prefixable outputPrefixable) :
      m_rootSymbol(rootSymbol),
      m_ratio(ratio),
      m_inputPrefixable(inputPrefixable),
      m_outputPrefixable(outputPrefixable)
    {}

    virtual const Vector<int> dimensionVector() const { return Vector<int>{.time = 0, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; };
    virtual const I18n::Message dimensionMessage() const = 0;
    virtual int numberOfRepresentatives() const { return 0; };
    /* representativesOfSameDimension returns a pointer to the array containing
     * all representatives for this's dimension. */
    virtual const Representative * representativesOfSameDimension() const { return nullptr; };
    virtual const Prefix * basePrefix() const { return Prefix::EmptyPrefix(); }
    virtual bool isBaseUnit() const { return false; }
    virtual const Representative * standardRepresentative(double value, double exponent, ExpressionNode::ReductionContext reductionContext, const Prefix * * prefix) const { return DefaultFindBestRepresentative(value, exponent, representativesOfSameDimension(), numberOfRepresentatives(), prefix); }
    /* hasSpecialAdditionalExpressions return true if the unit has special
     * forms such as as splits (for time and imperial units) or common
     * conversions (such as speed and energy). */
    virtual bool hasSpecialAdditionalExpressions(double value, Preferences::UnitFormat unitFormat) const { return false; }
    virtual int setAdditionalExpressions(double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext) const { return 0; }

    const char * rootSymbol() const { return m_rootSymbol; }
    double ratio() const { return m_ratio; }
    bool isInputPrefixable() const { return m_inputPrefixable != Prefixable::None; }
    bool isOutputPrefixable() const { return m_outputPrefixable != Prefixable::None; }
    int serialize(char * buffer, int bufferSize, const Prefix * prefix) const;
    bool canParseWithEquivalents(const char * symbol, size_t length, const Representative * * representative, const Prefix * * prefix) const;
    bool canParse(const char * symbol, size_t length, const Prefix * * prefix) const;
    Expression toBaseUnits() const;
    bool canPrefix(const Prefix * prefix, bool input) const;
    const Prefix * findBestPrefix(double value, double exponent) const;

  protected:
    static const Representative * DefaultFindBestRepresentative(double value, double exponent, const Representative * representatives, int length, const Prefix * * prefix);
    const char * m_rootSymbol;
    /* m_ratio is the factor used to convert a unit made of the representative
     * and its base prefix into base SI units.
     * ex : m_ratio for Liter is 1e-3 (as 1_L = 1e-3_m).
     *      m_ratio for gram is 1 (as k is its best prefix and _kg is SI) */
    const double m_ratio;
    const Prefixable m_inputPrefixable;
    const Prefixable m_outputPrefixable;
  };

  class TimeRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static TimeRepresentative Default() { return TimeRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 1, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::TimeDimension; }
    int numberOfRepresentatives() const override { return 7; }
    const Representative * representativesOfSameDimension() const override;
    bool isBaseUnit() const override { return this == representativesOfSameDimension(); }
    bool hasSpecialAdditionalExpressions(double value, Preferences::UnitFormat unitFormat) const override { return m_ratio * value >= representativesOfSameDimension()[1].ratio(); }
    int setAdditionalExpressions(double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext) const override;
  private:
    using Representative::Representative;
  };

  class DistanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static DistanceRepresentative Default() { return DistanceRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 1, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::DistanceDimension; }
    int numberOfRepresentatives() const override { return 8; }
    const Representative * representativesOfSameDimension() const override;
    bool isBaseUnit() const override { return this == representativesOfSameDimension(); }
    const Representative * standardRepresentative(double value, double exponent, ExpressionNode::ReductionContext reductionContext, const Prefix * * prefix) const override;
    bool hasSpecialAdditionalExpressions(double value, Preferences::UnitFormat unitFormat) const override { return unitFormat == Preferences::UnitFormat::Imperial; }
    int setAdditionalExpressions(double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext) const override;
  private:
    using Representative::Representative;
  };

  class MassRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static MassRepresentative Default() { return MassRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 0, .mass = 1, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::MassDimension; }
    int numberOfRepresentatives() const override { return 7; }
    const Representative * representativesOfSameDimension() const override;
    const Prefix * basePrefix() const override;
    bool isBaseUnit() const override { return this == representativesOfSameDimension(); }
    const Representative * standardRepresentative(double value, double exponent, ExpressionNode::ReductionContext reductionContext, const Prefix * * prefix) const override;
    bool hasSpecialAdditionalExpressions(double value, Preferences::UnitFormat unitFormat) const override { return unitFormat == Preferences::UnitFormat::Imperial; }
    int setAdditionalExpressions(double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext) const override;
  private:
    using Representative::Representative;
  };

  class CurrentRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static CurrentRepresentative Default() { return CurrentRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 0, .mass = 0, .current = 1, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::CurrentDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
    bool isBaseUnit() const override { return this == representativesOfSameDimension(); }
  private:
    using Representative::Representative;
  };

  class TemperatureRepresentative : public Representative {
    friend class Unit;
  public:
    static double ConvertTemperatures(double value, const Representative * source, const Representative * target);
    constexpr static TemperatureRepresentative Default() { return TemperatureRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 0, .mass = 0, .current = 0, .temperature = 1, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::TemperatureDimension; }
    int numberOfRepresentatives() const override { return 3; }
    const Representative * representativesOfSameDimension() const override;
    bool isBaseUnit() const override { return this == representativesOfSameDimension(); }
    const Representative * standardRepresentative(double value, double exponent, ExpressionNode::ReductionContext reductionContext, const Prefix * * prefix) const override { return this; }
    bool hasSpecialAdditionalExpressions(double value, Preferences::UnitFormat unitFormat) const override { return true; }
    int setAdditionalExpressions(double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext) const override;
  private:
    static constexpr double k_celsiusOrigin = 273.15;
    static constexpr double k_fahrenheitOrigin = 459.67;
    using Representative::Representative;
  };

  class AmountOfSubstanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static AmountOfSubstanceRepresentative Default() { return AmountOfSubstanceRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 1, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::AmountOfSubstanceDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
    bool isBaseUnit() const override { return this == representativesOfSameDimension(); }
  private:
    using Representative::Representative;
  };

  class LuminousIntensityRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static LuminousIntensityRepresentative Default() { return LuminousIntensityRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 1}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::LuminousIntensityDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
    bool isBaseUnit() const override { return this == representativesOfSameDimension(); }
  private:
    using Representative::Representative;
  };

  class FrequencyRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static FrequencyRepresentative Default() { return FrequencyRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -1, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::FrequencyDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class ForceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static ForceRepresentative Default() { return ForceRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = 1, .mass = 1, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::ForceDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class PressureRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static PressureRepresentative Default() { return PressureRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = -1, .mass = 1, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::PressureDimension; }
    int numberOfRepresentatives() const override { return 3; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class EnergyRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static EnergyRepresentative Default() { return EnergyRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = 2, .mass = 1, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::EnergyDimension; }
    int numberOfRepresentatives() const override { return 2; }
    const Representative * representativesOfSameDimension() const override;
    bool hasSpecialAdditionalExpressions(double value, Preferences::UnitFormat unitFormat) const override { return true; }
    int setAdditionalExpressions(double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext) const override;
  private:
    using Representative::Representative;
  };

  class PowerRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static PowerRepresentative Default() { return PowerRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -3, .distance = 2, .mass = 1, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::PowerDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class ElectricChargeRepresentative : public Representative {
    friend class Unit;
  public:
    using Representative::Representative;
    constexpr static ElectricChargeRepresentative Default() { return ElectricChargeRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 1, .distance = 0, .mass = 0, .current = 1, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::ElectricChargeDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  };

  class ElectricPotentialRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static ElectricPotentialRepresentative Default() { return ElectricPotentialRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -3, .distance = 2, .mass = 1, .current = -1, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::ElectricPotentialDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class ElectricCapacitanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static ElectricCapacitanceRepresentative Default() { return ElectricCapacitanceRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 4, .distance = -2, .mass = -1, .current = 2, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::ElectricCapacitanceDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class ElectricResistanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static ElectricResistanceRepresentative Default() { return ElectricResistanceRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -3, .distance = 2, .mass = 1, .current = -2, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::ElectricResistanceDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class ElectricConductanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static ElectricConductanceRepresentative Default() { return ElectricConductanceRepresentative(nullptr, 1., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 3, .distance = -2, .mass = -1, .current = 2, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::ElectricConductanceDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class MagneticFluxRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static MagneticFluxRepresentative Default() { return MagneticFluxRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = 2, .mass = 1, .current = -1, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::MagneticFluxDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class MagneticFieldRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static MagneticFieldRepresentative Default() { return MagneticFieldRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = 0, .mass = 1, .current = -1, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::MagneticFieldDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class InductanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static InductanceRepresentative Default() { return InductanceRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = 2, .mass = 1, .current = -2, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::InductanceDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class CatalyticActivityRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static CatalyticActivityRepresentative Default() { return CatalyticActivityRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -1, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 1, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::CatalyticActivityDimension; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class SurfaceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static SurfaceRepresentative Default() { return SurfaceRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 2, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::SurfaceDimension; }
    int numberOfRepresentatives() const override { return 2; }
    const Representative * representativesOfSameDimension() const override;
    const Representative * standardRepresentative(double value, double exponent, ExpressionNode::ReductionContext reductionContext, const Prefix * * prefix) const override;
    bool hasSpecialAdditionalExpressions(double value, Preferences::UnitFormat unitFormat) const override { return true; }
    int setAdditionalExpressions(double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext) const override;
  private:
    using Representative::Representative;
  };

  class VolumeRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static VolumeRepresentative Default() { return VolumeRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 3, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::VolumeDimension; }
    int numberOfRepresentatives() const override { return 8; }
    const Representative * representativesOfSameDimension() const override;
    const Representative * standardRepresentative(double value, double exponent, ExpressionNode::ReductionContext reductionContext, const Prefix * * prefix) const override;
    bool hasSpecialAdditionalExpressions(double value, Preferences::UnitFormat unitFormat) const override { return true; }
    int setAdditionalExpressions(double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext) const override;
  private:
    using Representative::Representative;
  };

  class SpeedRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static SpeedRepresentative Default() { return SpeedRepresentative(nullptr, 0., Prefixable::None, Prefixable::None); }
    const Vector<int>dimensionVector() const override { return Vector<int>{.time = -1, .distance = 1, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminousIntensity = 0}; }
    const I18n::Message dimensionMessage() const override { return I18n::Message::SpeedDimension; }
    const Representative * standardRepresentative(double value, double exponent, ExpressionNode::ReductionContext reductionContext, const Prefix * * prefix) const override { return nullptr; }
    bool hasSpecialAdditionalExpressions(double value, Preferences::UnitFormat unitFormat) const override { return true; }
    int setAdditionalExpressions(double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext) const override;
  private:
    using Representative::Representative;
  };

  UnitNode(const Representative * representative, const Prefix * prefix) :
    ExpressionNode(),
    m_representative(representative),
    m_prefix(prefix)
  {}

  // TreeNode
  size_t size() const override { return sizeof(UnitNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Unit";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " prefix=\"" << m_prefix->symbol() << "\"";
    stream << " rootSymbol=\"" << m_representative->rootSymbol() << "\"";
  }
#endif

  // Expression Properties
  Type type() const override { return Type::Unit; }
  Sign sign(Context * context) const override { return Sign::Positive; }
  NullStatus nullStatus(Context * context) const override { return NullStatus::NonNull; }
  Expression removeUnit(Expression * unit) override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }

  // Comparison
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression shallowBeautify(ReductionContext * reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::OneLetter; } // TODO

  const Representative * representative() const { return m_representative; }
  const Prefix * prefix() const { return m_prefix; }
  void setRepresentative(const Representative * representative) { m_representative = representative; }
  void setPrefix(const Prefix * prefix) { m_prefix = prefix; }

private:
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;

  const Representative * m_representative;
  const Prefix * m_prefix;
};

/* FIXME : This can be replaced by std::string_view when moving to C++17 */
constexpr bool strings_equal(const char * s1, const char * s2) { return *s1 == *s2 && (*s1 == '\0' || strings_equal(s1 + 1, s2 + 1)); }

class Unit : public Expression {
  friend class UnitNode;
public:
  /* Prefixes and Representatives defined below must be defined only once and
   * all units must be constructed from their pointers. This way we can easily
   * check if two Unit objects are equal by comparing pointers. This saves us
   * from overloading the == operator on Prefix and Representative and saves
   * time at execution. As such, their constructor are private and can only be
   * accessed by their friend class Unit. */
  typedef UnitNode::Prefix Prefix;
  static constexpr const Prefix k_prefixes[Prefix::k_numberOfPrefixes] = {
    Prefix("p", -12),
    Prefix("n", -9),
    Prefix("μ", -6),
    Prefix("m", -3),
    Prefix("c", -2),
    Prefix("d", -1),
    Prefix("", 0),
    Prefix("da", 1),
    Prefix("h", 2),
    Prefix("k", 3),
    Prefix("M", 6),
    Prefix("G", 9),
    Prefix("T", 12),
  };
  typedef UnitNode::Representative Representative;
  typedef Representative::Prefixable Prefixable;
  typedef UnitNode::TimeRepresentative TimeRepresentative;
  static constexpr const TimeRepresentative k_timeRepresentatives[] = {
    TimeRepresentative("s", 1., Prefixable::All, Prefixable::NegativeLongScale),
    TimeRepresentative("min", 60., Prefixable::None, Prefixable::None),
    TimeRepresentative("h", 3600., Prefixable::None, Prefixable::None),
    TimeRepresentative("day", 24.*3600., Prefixable::None, Prefixable::None),
    TimeRepresentative("week", 7.*24.*3600., Prefixable::None, Prefixable::None),
    TimeRepresentative("month", 365.25/12.*24.*3600., Prefixable::None, Prefixable::None),
    TimeRepresentative("year", 365.25*24.*3600., Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::DistanceRepresentative DistanceRepresentative;
  static constexpr const DistanceRepresentative k_distanceRepresentatives[] = {
    DistanceRepresentative("m", 1., Prefixable::All, Prefixable::NegativeAndKilo),
    DistanceRepresentative("au", 149597870700., Prefixable::None, Prefixable::None),
    DistanceRepresentative("ly", 299792458.*365.25*24.*3600., Prefixable::None, Prefixable::None),
    DistanceRepresentative("pc", 180.*3600./M_PI*149587870700., Prefixable::None, Prefixable::None),
    DistanceRepresentative("in", 0.0254, Prefixable::None, Prefixable::None),
    DistanceRepresentative("ft", 12*0.0254, Prefixable::None, Prefixable::None),
    DistanceRepresentative("yd", 3*12*0.0254, Prefixable::None, Prefixable::None),
    DistanceRepresentative("mi", 1760*3*12*0.0254, Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::MassRepresentative MassRepresentative;
  static constexpr const MassRepresentative k_massRepresentatives[] = {
    MassRepresentative("g", 1., Prefixable::All, Prefixable::NegativeAndKilo),
    MassRepresentative("t", 1e3, Prefixable::PositiveLongScale, Prefixable::PositiveLongScale),
    MassRepresentative("Da", 1/(6.02214076e26), Prefixable::All, Prefixable::All),
    MassRepresentative("oz", 0.028349523125, Prefixable::None, Prefixable::None),
    MassRepresentative("lb", 16*0.028349523125, Prefixable::None, Prefixable::None),
    MassRepresentative("shtn", 2000*16*0.028349523125, Prefixable::None, Prefixable::None),
    MassRepresentative("lgtn", 2240*16*0.028349523125, Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::CurrentRepresentative CurrentRepresentative;
  static constexpr const CurrentRepresentative k_currentRepresentatives[] = { CurrentRepresentative("A", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::TemperatureRepresentative TemperatureRepresentative;
  static constexpr const TemperatureRepresentative k_temperatureRepresentatives[] = {
    TemperatureRepresentative("K", 1., Prefixable::All, Prefixable::None),
    TemperatureRepresentative("°C", 1., Prefixable::None, Prefixable::None),
    TemperatureRepresentative("°F", 5./9., Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::AmountOfSubstanceRepresentative AmountOfSubstanceRepresentative;
  static constexpr const AmountOfSubstanceRepresentative k_amountOfSubstanceRepresentatives[] = { AmountOfSubstanceRepresentative("mol", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::LuminousIntensityRepresentative LuminousIntensityRepresentative;
  static constexpr const LuminousIntensityRepresentative k_luminousIntensityRepresentatives[] = { LuminousIntensityRepresentative("cd", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::FrequencyRepresentative FrequencyRepresentative;
  static constexpr const FrequencyRepresentative k_frequencyRepresentatives[] = { FrequencyRepresentative("Hz", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ForceRepresentative ForceRepresentative;
  static constexpr const ForceRepresentative k_forceRepresentatives[] = { ForceRepresentative("N", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::PressureRepresentative PressureRepresentative;
  static constexpr const PressureRepresentative k_pressureRepresentatives[] = {
    PressureRepresentative("Pa", 1., Prefixable::All, Prefixable::LongScale),
    PressureRepresentative("bar", 100000, Prefixable::All, Prefixable::LongScale),
    PressureRepresentative("atm", 101325, Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::EnergyRepresentative EnergyRepresentative;
  static constexpr const EnergyRepresentative k_energyRepresentatives[] = {
    EnergyRepresentative("J", 1., Prefixable::All, Prefixable::LongScale),
    EnergyRepresentative("eV", 1.602176634e-19, Prefixable::All, Prefixable::LongScale),
  };
  typedef UnitNode::PowerRepresentative PowerRepresentative;
  static constexpr const PowerRepresentative k_powerRepresentatives[] = { PowerRepresentative("W", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ElectricChargeRepresentative ElectricChargeRepresentative;
  static constexpr const ElectricChargeRepresentative k_electricChargeRepresentatives[] = { ElectricChargeRepresentative("C", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ElectricPotentialRepresentative ElectricPotentialRepresentative;
  static constexpr const ElectricPotentialRepresentative k_electricPotentialRepresentatives[] = { ElectricPotentialRepresentative("V", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ElectricCapacitanceRepresentative ElectricCapacitanceRepresentative;
  static constexpr const ElectricCapacitanceRepresentative k_electricCapacitanceRepresentatives[] = { ElectricCapacitanceRepresentative("F", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ElectricResistanceRepresentative ElectricResistanceRepresentative;
  static constexpr const ElectricResistanceRepresentative k_electricResistanceRepresentatives[] = { ElectricResistanceRepresentative("Ω", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ElectricConductanceRepresentative ElectricConductanceRepresentative;
  static constexpr const ElectricConductanceRepresentative k_electricConductanceRepresentatives[] = { ElectricConductanceRepresentative("S", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::MagneticFluxRepresentative MagneticFluxRepresentative;
  static constexpr const MagneticFluxRepresentative k_magneticFluxRepresentatives[] = { MagneticFluxRepresentative("Wb", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::MagneticFieldRepresentative MagneticFieldRepresentative;
  static constexpr const MagneticFieldRepresentative k_magneticFieldRepresentatives[] = { MagneticFieldRepresentative("T", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::InductanceRepresentative InductanceRepresentative;
  static constexpr const InductanceRepresentative k_inductanceRepresentatives[] = { InductanceRepresentative("H", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::CatalyticActivityRepresentative CatalyticActivityRepresentative;
  static constexpr const CatalyticActivityRepresentative k_catalyticActivityRepresentatives[] = { CatalyticActivityRepresentative("kat", 1., Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::SurfaceRepresentative SurfaceRepresentative;
  static constexpr const SurfaceRepresentative k_surfaceRepresentatives[] = {
    SurfaceRepresentative("ha", 10000., Prefixable::None, Prefixable::None),
    SurfaceRepresentative("acre", 4046.8564224, Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::VolumeRepresentative VolumeRepresentative;
  static constexpr const VolumeRepresentative k_volumeRepresentatives[] = {
    VolumeRepresentative("L", 0.001, Prefixable::All, Prefixable::Negative),
    VolumeRepresentative("tsp", 0.00000492892159375, Prefixable::None, Prefixable::None),
    VolumeRepresentative("tbsp", 3*0.00000492892159375, Prefixable::None, Prefixable::None),
    VolumeRepresentative("floz", 0.0000295735295625, Prefixable::None, Prefixable::None),
    VolumeRepresentative("cup", 8*0.0000295735295625, Prefixable::None, Prefixable::None),
    VolumeRepresentative("pt", 16*0.0000295735295625, Prefixable::None, Prefixable::None),
    VolumeRepresentative("qt", 32*0.0000295735295625, Prefixable::None, Prefixable::None),
    VolumeRepresentative("gal", 128*0.0000295735295625, Prefixable::None, Prefixable::None),
  };
  /* FIXME : Some ratio are too precise too be well approximated by double.
   * Maybe switch to a rational representation with two int. */

  /* Define access points to some prefixes and representatives. */
  static constexpr int k_emptyPrefixIndex = 6;
  static_assert(k_prefixes[k_emptyPrefixIndex].m_exponent == 0, "Index for the Empty Prefix is incorrect.");
  static constexpr int k_kiloPrefixIndex = 9;
  static_assert(k_prefixes[k_kiloPrefixIndex].m_exponent == 3, "Index for the Kilo Prefix is incorrect.");
  static constexpr int k_secondRepresentativeIndex = 0;
  static_assert(strings_equal(k_timeRepresentatives[k_secondRepresentativeIndex].m_rootSymbol, "s"), "Index for the Second Representative is incorrect.");
  static constexpr int k_minuteRepresentativeIndex = 1;
  static_assert(strings_equal(k_timeRepresentatives[k_minuteRepresentativeIndex].m_rootSymbol, "min"), "Index for the Minute Representative is incorrect.");
  static constexpr int k_hourRepresentativeIndex = 2;
  static_assert(strings_equal(k_timeRepresentatives[k_hourRepresentativeIndex].m_rootSymbol, "h"), "Index for the Hour Representative is incorrect.");
  static constexpr int k_dayRepresentativeIndex = 3;
  static_assert(strings_equal(k_timeRepresentatives[k_dayRepresentativeIndex].m_rootSymbol, "day"), "Index for the Day Representative is incorrect.");
  static constexpr int k_monthRepresentativeIndex = 5;
  static_assert(strings_equal(k_timeRepresentatives[k_monthRepresentativeIndex].m_rootSymbol, "month"), "Index for the Month Representative is incorrect.");
  static constexpr int k_yearRepresentativeIndex = 6;
  static_assert(strings_equal(k_timeRepresentatives[k_yearRepresentativeIndex].m_rootSymbol, "year"), "Index for the Year Representative is incorrect.");
  static constexpr int k_meterRepresentativeIndex = 0;
  static_assert(strings_equal(k_distanceRepresentatives[k_meterRepresentativeIndex].m_rootSymbol, "m"), "Index for the Meter Representative is incorrect.");
  static constexpr int k_inchRepresentativeIndex = 4;
  static_assert(strings_equal(k_distanceRepresentatives[k_inchRepresentativeIndex].m_rootSymbol, "in"), "Index for the Inch Representative is incorrect.");
  static constexpr int k_footRepresentativeIndex = 5;
  static_assert(strings_equal(k_distanceRepresentatives[k_footRepresentativeIndex].m_rootSymbol, "ft"), "Index for the Foot Representative is incorrect.");
  static constexpr int k_yardRepresentativeIndex = 6;
  static_assert(strings_equal(k_distanceRepresentatives[k_yardRepresentativeIndex].m_rootSymbol, "yd"), "Index for the Yard Representative is incorrect.");
  static constexpr int k_mileRepresentativeIndex = 7;
  static_assert(strings_equal(k_distanceRepresentatives[k_mileRepresentativeIndex].m_rootSymbol, "mi"), "Index for the Mile Representative is incorrect.");
  static constexpr int k_ounceRepresentativeIndex = 3;
  static_assert(strings_equal(k_massRepresentatives[k_ounceRepresentativeIndex].m_rootSymbol, "oz"), "Index for the Ounce Representative is incorrect.");
  static constexpr int k_poundRepresentativeIndex = 4;
  static_assert(strings_equal(k_massRepresentatives[k_poundRepresentativeIndex].m_rootSymbol, "lb"), "Index for the Pound Representative is incorrect.");
  static constexpr int k_shortTonRepresentativeIndex = 5;
  static_assert(strings_equal(k_massRepresentatives[k_shortTonRepresentativeIndex].m_rootSymbol, "shtn"), "Index for the Short Ton Representative is incorrect.");
  static constexpr int k_kelvinRepresentativeIndex = 0;
  static_assert(strings_equal(k_temperatureRepresentatives[k_kelvinRepresentativeIndex].m_rootSymbol, "K"), "Index for the Kelvin Representative is incorrect.");
  static constexpr int k_celsiusRepresentativeIndex = 1;
  static_assert(strings_equal(k_temperatureRepresentatives[k_celsiusRepresentativeIndex].m_rootSymbol, "°C"), "Index for the Celsius Representative is incorrect.");
  static constexpr int k_fahrenheitRepresentativeIndex = 2;
  static_assert(strings_equal(k_temperatureRepresentatives[k_fahrenheitRepresentativeIndex].m_rootSymbol, "°F"), "Index for the Fahrenheit Representative is incorrect.");
  static constexpr int k_jouleRepresentativeIndex = 0;
  static_assert(strings_equal(k_energyRepresentatives[k_jouleRepresentativeIndex].m_rootSymbol, "J"), "Index for the Joule Representative is incorrect.");
  static constexpr int k_electronVoltRepresentativeIndex = 1;
  static_assert(strings_equal(k_energyRepresentatives[k_electronVoltRepresentativeIndex].m_rootSymbol, "eV"), "Index for the Electron Volt Representative is incorrect.");
  static constexpr int k_wattRepresentativeIndex = 0;
  static_assert(strings_equal(k_powerRepresentatives[k_wattRepresentativeIndex].m_rootSymbol, "W"), "Index for the Watt Representative is incorrect.");
  static constexpr int k_hectareRepresentativeIndex = 0;
  static_assert(strings_equal(k_surfaceRepresentatives[k_hectareRepresentativeIndex].m_rootSymbol, "ha"), "Index for the Hectare Representative is incorrect.");
  static constexpr int k_acreRepresentativeIndex = 1;
  static_assert(strings_equal(k_surfaceRepresentatives[k_acreRepresentativeIndex].m_rootSymbol, "acre"), "Index for the Acre Representative is incorrect.");
  static constexpr int k_literRepresentativeIndex = 0;
  static_assert(strings_equal(k_volumeRepresentatives[k_literRepresentativeIndex].m_rootSymbol, "L"), "Index for the Liter Representative is incorrect.");
  static constexpr int k_cupRepresentativeIndex = 4;
  static_assert(strings_equal(k_volumeRepresentatives[k_cupRepresentativeIndex].m_rootSymbol, "cup"), "Index for the Cup Representative is incorrect.");
  static constexpr int k_pintRepresentativeIndex = 5;
  static_assert(strings_equal(k_volumeRepresentatives[k_pintRepresentativeIndex].m_rootSymbol, "pt"), "Index for the Pint Representative is incorrect.");
  static constexpr int k_quartRepresentativeIndex = 6;
  static_assert(strings_equal(k_volumeRepresentatives[k_quartRepresentativeIndex].m_rootSymbol, "qt"), "Index for the Quart Representative is incorrect.");
  static constexpr int k_gallonRepresentativeIndex = 7;
  static_assert(strings_equal(k_volumeRepresentatives[k_gallonRepresentativeIndex].m_rootSymbol, "gal"), "Index for the Gallon Representative is incorrect.");

  Unit(const UnitNode * node) : Expression(node) {}
  static Unit Builder(const Representative * representative, const Prefix * prefix);
  static bool CanParse(const char * symbol, size_t length, const Representative * * representative, const Prefix * * prefix);
  static void ChooseBestRepresentativeAndPrefixForValue(Expression units, double * value, ExpressionNode::ReductionContext reductionContext);
  static bool HaveAdditionalOutputs(double value, Expression unit, Preferences::UnitFormat unitFormat);
  static int SetAdditionalExpressionsAndMessage(Expression units, double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext, I18n::Message * message);
  static Expression BuildSplit(double value, const Unit * units, int length, ExpressionNode::ReductionContext reductionContext);
  static Expression ConvertTemperatureUnits(Expression e, Unit unit, ExpressionNode::ReductionContext reductionContext);

  // Simplification
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify();

  bool isBaseUnit() const { return node()->representative()->isBaseUnit() && node()->prefix() == node()->representative()->basePrefix(); }
  void chooseBestRepresentativeAndPrefix(double * value, double exponent, ExpressionNode::ReductionContext reductionContext, bool optimizePrefix);

  const Representative * representative() const { return node()->representative(); }

private:
  UnitNode * node() const { return static_cast<UnitNode *>(Expression::node()); }
  Expression removeUnit(Expression * unit);
};

}

#endif
