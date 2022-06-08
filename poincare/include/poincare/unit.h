#ifndef POINCARE_UNIT_H
#define POINCARE_UNIT_H

#include <poincare/expression.h>

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
      const T coefficients[k_numberOfBaseUnits] = {time, distance, mass, current, temperature, amountOfSubstance, luminuousIntensity};
      return coefficients[i];
    }
    void setCoefficientAtIndex(size_t i, T c) {
      assert(i < k_numberOfBaseUnits);
      T * coefficientsAddresses[k_numberOfBaseUnits] = {&time, &distance, &mass, &current, &temperature, &amountOfSubstance, &luminuousIntensity};
      *(coefficientsAddresses[i]) = c;
    }
    bool operator==(const Vector &rhs) const { return time == rhs.time && distance == rhs.distance && mass == rhs.mass && current == rhs.current && temperature == rhs.temperature && amountOfSubstance == rhs.amountOfSubstance && luminuousIntensity == rhs.luminuousIntensity; }
    bool operator!=(const Vector &rhs) const { return !(*this == rhs); }
    void addAllCoefficients(const Vector other, int factor);
    Expression toBaseUnits() const;
    T time;
    T distance;
    T mass;
    T current;
    T temperature;
    T amountOfSubstance;
    T luminuousIntensity;
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
    constexpr Representative(const char * rootSymbol, const char * ratioExpression, double ratio, Prefixable inputPrefixable, Prefixable outputPrefixable) :
      m_rootSymbol(rootSymbol),
      m_ratioExpression(ratioExpression),
      m_inputPrefixable(inputPrefixable),
      m_outputPrefixable(outputPrefixable),
      m_ratio(ratio)
    {}

    virtual const Vector<int> dimensionVector() const { return Vector<int>{.time = 0, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; };
    virtual int numberOfRepresentatives() const { return 0; };
    /* representativesOfSameDimension returns a pointer to the array containing
     * all representatives for this's dimension. */
    virtual const Representative * representativesOfSameDimension() const { return nullptr; };
    virtual const Prefix * basePrefix() const { return Prefix::EmptyPrefix(); }
    virtual bool isBaseUnit() const { return false; }
    virtual const Representative * standardRepresentative(double value, double exponent, ExpressionNode::ReductionContext reductionContext, const Prefix * * prefix) const { return DefaultFindBestRepresentative(value, exponent, representativesOfSameDimension(), numberOfRepresentatives(), prefix); }
    /* hasSpecialAdditionalExpressions return true if the unit has special
     * forms suchas as splits (for time and imperial units) or common
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
    Expression toBaseUnits(ExpressionNode::ReductionContext reductionContext) const;
    bool canPrefix(const Prefix * prefix, bool input) const;
    const Prefix * findBestPrefix(double value, double exponent) const;

  protected:
    static const Representative * DefaultFindBestRepresentative(double value, double exponent, const Representative * representatives, int length, const Prefix * * prefix);

    Expression ratioExpressionReduced(ExpressionNode::ReductionContext reductionContext) const { return Expression::Parse(m_ratioExpression, nullptr).deepReduce(reductionContext); }

    const char * m_rootSymbol;
    /* m_ratio is the factor used to convert a unit made of the representative
     * and its base prefix into base SI units.
     * ex : m_ratio for Liter is 1e-3 (as 1_L = 1e-3_m).
     *      m_ratio for gram is 1 (as k is its best prefix and _kg is SI)
     * m_ratioExpression is the expression expressed as a formula, to be used
     * as an expression in reductions. */
    const char * m_ratioExpression;
    const Prefixable m_inputPrefixable;
    const Prefixable m_outputPrefixable;
    double m_ratio;
  };

  class TimeRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static TimeRepresentative Default() { return TimeRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 1, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 7; }
    const Representative * representativesOfSameDimension() const override;
    bool isBaseUnit() const override { return this == representativesOfSameDimension(); }
    bool hasSpecialAdditionalExpressions(double value, Preferences::UnitFormat unitFormat) const override { return ratio() * value >= representativesOfSameDimension()[1].ratio(); }
    int setAdditionalExpressions(double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext) const override;
  private:
    using Representative::Representative;
  };

  class DistanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static DistanceRepresentative Default() { return DistanceRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 1, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
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
    constexpr static MassRepresentative Default() { return MassRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 0, .mass = 1, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
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
    constexpr static CurrentRepresentative Default() { return CurrentRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 0, .mass = 0, .current = 1, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
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
    constexpr static TemperatureRepresentative Default() { return TemperatureRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 0, .mass = 0, .current = 0, .temperature = 1, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
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
    constexpr static AmountOfSubstanceRepresentative Default() { return AmountOfSubstanceRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 1, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
    bool isBaseUnit() const override { return this == representativesOfSameDimension(); }
  private:
    using Representative::Representative;
  };

  class LuminousIntensityRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static LuminousIntensityRepresentative Default() { return LuminousIntensityRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 1}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
    bool isBaseUnit() const override { return this == representativesOfSameDimension(); }
  private:
    using Representative::Representative;
  };

  class FrequencyRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static FrequencyRepresentative Default() { return FrequencyRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -1, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class ForceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static ForceRepresentative Default() { return ForceRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = 1, .mass = 1, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class PressureRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static PressureRepresentative Default() { return PressureRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = -1, .mass = 1, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 3; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class EnergyRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static EnergyRepresentative Default() { return EnergyRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = 2, .mass = 1, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
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
    constexpr static PowerRepresentative Default() { return PowerRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -3, .distance = 2, .mass = 1, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class ElectricChargeRepresentative : public Representative {
    friend class Unit;
  public:
    using Representative::Representative;
    constexpr static ElectricChargeRepresentative Default() { return ElectricChargeRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 1, .distance = 0, .mass = 0, .current = 1, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  };

  class ElectricPotentialRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static ElectricPotentialRepresentative Default() { return ElectricPotentialRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -3, .distance = 2, .mass = 1, .current = -1, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class ElectricCapacitanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static ElectricCapacitanceRepresentative Default() { return ElectricCapacitanceRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 4, .distance = -2, .mass = -1, .current = 2, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class ElectricResistanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static ElectricResistanceRepresentative Default() { return ElectricResistanceRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -3, .distance = 2, .mass = 1, .current = -2, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class ElectricConductanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static ElectricConductanceRepresentative Default() { return ElectricConductanceRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 3, .distance = -2, .mass = -1, .current = 2, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class MagneticFluxRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static MagneticFluxRepresentative Default() { return MagneticFluxRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = 2, .mass = 1, .current = -1, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class MagneticFieldRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static MagneticFieldRepresentative Default() { return MagneticFieldRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = 0, .mass = 1, .current = -1, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class InductanceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static InductanceRepresentative Default() { return InductanceRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -2, .distance = 2, .mass = 1, .current = -2, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class CatalyticActivityRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static CatalyticActivityRepresentative Default() { return CatalyticActivityRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = -1, .distance = 0, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 1, .luminuousIntensity = 0}; }
    int numberOfRepresentatives() const override { return 1; }
    const Representative * representativesOfSameDimension() const override;
  private:
    using Representative::Representative;
  };

  class SurfaceRepresentative : public Representative {
    friend class Unit;
  public:
    constexpr static SurfaceRepresentative Default() { return SurfaceRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 2, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
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
    constexpr static VolumeRepresentative Default() { return VolumeRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int> dimensionVector() const override { return Vector<int>{.time = 0, .distance = 3, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
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
    constexpr static SpeedRepresentative Default() { return SpeedRepresentative(nullptr, nullptr, NAN, Prefixable::None, Prefixable::None); }
    const Vector<int>dimensionVector() const override { return Vector<int>{.time = -1, .distance = 1, .mass = 0, .current = 0, .temperature = 0, .amountOfSubstance = 0, .luminuousIntensity = 0}; }
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
  void logAttributes(std::ostream & stream) const override {
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
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const override;

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
    TimeRepresentative("s", DEFINE_TWICE(1.), Prefixable::All, Prefixable::NegativeLongScale),
    TimeRepresentative("min", DEFINE_TWICE(60.), Prefixable::None, Prefixable::None),
    TimeRepresentative("h", DEFINE_TWICE(3600.), Prefixable::None, Prefixable::None),
    TimeRepresentative("day", DEFINE_TWICE(86400.), Prefixable::None, Prefixable::None),
    TimeRepresentative("week", DEFINE_TWICE(604800.), Prefixable::None, Prefixable::None),
    TimeRepresentative("month", DEFINE_TWICE(2629800.), Prefixable::None, Prefixable::None),
    TimeRepresentative("year", DEFINE_TWICE(31557600.), Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::DistanceRepresentative DistanceRepresentative;
  static constexpr const DistanceRepresentative k_distanceRepresentatives[] = {
    DistanceRepresentative("m", DEFINE_TWICE(1.), Prefixable::All, Prefixable::NegativeAndKilo),
    DistanceRepresentative("au", DEFINE_TWICE(149597870700.), Prefixable::None, Prefixable::None),
    DistanceRepresentative("ly", DEFINE_TWICE(299792458.*31557600.), Prefixable::None, Prefixable::None),
    DistanceRepresentative("pc", "180*3600/π*149587870700", 180*3600/M_PI*149587870700, Prefixable::None, Prefixable::None),
    DistanceRepresentative("in", DEFINE_TWICE(0.0254), Prefixable::None, Prefixable::None),
    DistanceRepresentative("ft", DEFINE_TWICE(12*0.0254), Prefixable::None, Prefixable::None),
    DistanceRepresentative("yd", DEFINE_TWICE(36*0.0254), Prefixable::None, Prefixable::None),
    DistanceRepresentative("mi", DEFINE_TWICE(63360*0.0254), Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::MassRepresentative MassRepresentative;
  static constexpr const MassRepresentative k_massRepresentatives[] = {
    MassRepresentative("g", DEFINE_TWICE(1.), Prefixable::All, Prefixable::NegativeAndKilo),
    MassRepresentative("t", DEFINE_TWICE(1000.), Prefixable::PositiveLongScale, Prefixable::PositiveLongScale),
    MassRepresentative("Da", "1/(6.02214076ᴇ26)", 1/(6.02214076e26), Prefixable::All, Prefixable::All),
    MassRepresentative("oz", DEFINE_TWICE(0.028349523125), Prefixable::None, Prefixable::None),
    MassRepresentative("lb", DEFINE_TWICE(16*0.028349523125), Prefixable::None, Prefixable::None),
    MassRepresentative("shtn", DEFINE_TWICE(2000*16*0.028349523125), Prefixable::None, Prefixable::None),
    MassRepresentative("lgtn", DEFINE_TWICE(2240*16*0.028349523125), Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::CurrentRepresentative CurrentRepresentative;
  static constexpr const CurrentRepresentative k_currentRepresentatives[] = { CurrentRepresentative("A", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::TemperatureRepresentative TemperatureRepresentative;
  static constexpr const TemperatureRepresentative k_temperatureRepresentatives[] = {
    TemperatureRepresentative("K", DEFINE_TWICE(1.), Prefixable::All, Prefixable::None),
    TemperatureRepresentative("°C", DEFINE_TWICE(1.), Prefixable::None, Prefixable::None),
    TemperatureRepresentative("°F", DEFINE_TWICE(5./9.), Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::AmountOfSubstanceRepresentative AmountOfSubstanceRepresentative;
  static constexpr const AmountOfSubstanceRepresentative k_amountOfSubstanceRepresentatives[] = { AmountOfSubstanceRepresentative("mol", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::LuminousIntensityRepresentative LuminousIntensityRepresentative;
  static constexpr const LuminousIntensityRepresentative k_luminousIntensityRepresentatives[] = { LuminousIntensityRepresentative("cd", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::FrequencyRepresentative FrequencyRepresentative;
  static constexpr const FrequencyRepresentative k_frequencyRepresentatives[] = { FrequencyRepresentative("Hz", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ForceRepresentative ForceRepresentative;
  static constexpr const ForceRepresentative k_forceRepresentatives[] = { ForceRepresentative("N", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::PressureRepresentative PressureRepresentative;
  static constexpr const PressureRepresentative k_pressureRepresentatives[] = {
    PressureRepresentative("Pa", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale),
    PressureRepresentative("bar", DEFINE_TWICE(100000.), Prefixable::All, Prefixable::LongScale),
    PressureRepresentative("atm", DEFINE_TWICE(101325.), Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::EnergyRepresentative EnergyRepresentative;
  static constexpr const EnergyRepresentative k_energyRepresentatives[] = {
    EnergyRepresentative("J", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale),
    EnergyRepresentative("eV", "1.602176634ᴇ-19", 1.602176634e-19, Prefixable::All, Prefixable::LongScale),
  };
  typedef UnitNode::PowerRepresentative PowerRepresentative;
  static constexpr const PowerRepresentative k_powerRepresentatives[] = { PowerRepresentative("W", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ElectricChargeRepresentative ElectricChargeRepresentative;
  static constexpr const ElectricChargeRepresentative k_electricChargeRepresentatives[] = { ElectricChargeRepresentative("C", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ElectricPotentialRepresentative ElectricPotentialRepresentative;
  static constexpr const ElectricPotentialRepresentative k_electricPotentialRepresentatives[] = { ElectricPotentialRepresentative("V", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ElectricCapacitanceRepresentative ElectricCapacitanceRepresentative;
  static constexpr const ElectricCapacitanceRepresentative k_electricCapacitanceRepresentatives[] = { ElectricCapacitanceRepresentative("F", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ElectricResistanceRepresentative ElectricResistanceRepresentative;
  static constexpr const ElectricResistanceRepresentative k_electricResistanceRepresentatives[] = { ElectricResistanceRepresentative("Ω", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::ElectricConductanceRepresentative ElectricConductanceRepresentative;
  static constexpr const ElectricConductanceRepresentative k_electricConductanceRepresentatives[] = { ElectricConductanceRepresentative("S", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::MagneticFluxRepresentative MagneticFluxRepresentative;
  static constexpr const MagneticFluxRepresentative k_magneticFluxRepresentatives[] = { MagneticFluxRepresentative("Wb", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::MagneticFieldRepresentative MagneticFieldRepresentative;
  static constexpr const MagneticFieldRepresentative k_magneticFieldRepresentatives[] = { MagneticFieldRepresentative("T", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::InductanceRepresentative InductanceRepresentative;
  static constexpr const InductanceRepresentative k_inductanceRepresentatives[] = { InductanceRepresentative("H", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::CatalyticActivityRepresentative CatalyticActivityRepresentative;
  static constexpr const CatalyticActivityRepresentative k_catalyticActivityRepresentatives[] = { CatalyticActivityRepresentative("kat", DEFINE_TWICE(1.), Prefixable::All, Prefixable::LongScale) };
  typedef UnitNode::SurfaceRepresentative SurfaceRepresentative;
  static constexpr const SurfaceRepresentative k_surfaceRepresentatives[] = {
    SurfaceRepresentative("ha", DEFINE_TWICE(10000.), Prefixable::None, Prefixable::None),
    SurfaceRepresentative("acre", DEFINE_TWICE(4046.8564224), Prefixable::None, Prefixable::None),
  };
  typedef UnitNode::VolumeRepresentative VolumeRepresentative;
  static constexpr const VolumeRepresentative k_volumeRepresentatives[] = {
    VolumeRepresentative("L", DEFINE_TWICE(0.001), Prefixable::All, Prefixable::Negative),
    VolumeRepresentative("tsp", DEFINE_TWICE(0.00000492892159375), Prefixable::None, Prefixable::None),
    VolumeRepresentative("tbsp", DEFINE_TWICE(3*0.00000492892159375), Prefixable::None, Prefixable::None),
    VolumeRepresentative("floz", DEFINE_TWICE(0.0000295735295625), Prefixable::None, Prefixable::None),
    VolumeRepresentative("cup", DEFINE_TWICE(8*0.0000295735295625), Prefixable::None, Prefixable::None),
    VolumeRepresentative("pt", DEFINE_TWICE(16*0.0000295735295625), Prefixable::None, Prefixable::None),
    VolumeRepresentative("qt", DEFINE_TWICE(32*0.0000295735295625), Prefixable::None, Prefixable::None),
    VolumeRepresentative("gal", DEFINE_TWICE(128*0.0000295735295625), Prefixable::None, Prefixable::None),
  };

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
  static bool ShouldDisplayAdditionalOutputs(double value, Expression unit, Preferences::UnitFormat unitFormat);
  static int SetAdditionalExpressions(Expression units, double value, Expression * dest, int availableLength, ExpressionNode::ReductionContext reductionContext);
  static Expression BuildSplit(double value, const Unit * units, int length, ExpressionNode::ReductionContext reductionContext);
  static Expression ConvertTemperatureUnits(Expression e, Unit unit, ExpressionNode::ReductionContext reductionContext);
  static bool IsForbiddenTemperatureProduct(Expression e);

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
