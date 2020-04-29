#ifndef POINCARE_UNIT_H
#define POINCARE_UNIT_H

#include <poincare/expression.h>

namespace Poincare {

class UnitNode final : public ExpressionNode {
public:

  /* The units having the same physical dimension are grouped together.
   * Each such group has a standard representative with a standard prefix.
   *
   * Each representative has
   *  - a root symbol
   *  - a definition
   *  - a list of allowed output prefixes
   * Given a Dimension, a representative in that Dimension and a Prefix
   * allowed for that representative, one may get a symbol and an Expression.
   *
   * FIXME ?
   * The UnitNode class holds as members pointers to a Dimension, a
   * Representative, a Prefix. Those nested classes may not be forward
   * declared and must be defined in UnitNode and then aliased in Unit so as
   * to be used outside. That technical limitation could have been avoided if
   * UnitNode were itself a nested class of Unit, say Unit::Node. More
   * generally, turning all the Poincare::...Node classes into nested
   * Poincare::...::Node classes might be a more clever usage of namespaces
   * and scopes.
   */

  // There are 7 base units from which all other units are derived.
  static constexpr size_t NumberOfBaseUnits = 7;

  class Prefix {
  public:
    constexpr Prefix(const char * symbol, int8_t exponent) :
      m_symbol(symbol),
      m_exponent(exponent)
    {}
    const char * symbol() const { return m_symbol; }
    int8_t exponent() const { return m_exponent; }
    int serialize(char * buffer, int bufferSize) const;
  private:
    const char * m_symbol;
    int8_t m_exponent;
  };

  class Representative {
  public:
    /* The following class is meant to be an attribute determining whether a
     * unit symbol is prefixable at all. If Yes, any prefix is accepted as
     * input, whereas the allowed output prefixes are prescribed manually. */
    enum class Prefixable {
      No,
      Yes
    };
    template <size_t N>
    constexpr Representative(const char * rootSymbol, const char * definition, const Prefixable prefixable, const Prefix (&outputPrefixes)[N]) :
      m_rootSymbol(rootSymbol),
      m_definition(definition),
      m_prefixable(prefixable),
      m_outputPrefixes(outputPrefixes),
      m_outputPrefixesUpperBound(outputPrefixes + N)
    {
    }
    const char * rootSymbol() const { return m_rootSymbol; }
    const char * definition() const { return m_definition; }
    bool isPrefixable() const { return m_prefixable == Prefixable::Yes; }
    const Prefix * outputPrefixes() const { return m_outputPrefixes; }
    const Prefix * outputPrefixesUpperBound() const { return m_outputPrefixesUpperBound; }
    bool canParse(const char * symbol, size_t length,
        const Prefix * * prefix) const;
    int serialize(char * buffer, int bufferSize, const Prefix * prefix) const;
    const Prefix * bestPrefixForValue(double & value, const int exponent) const;
  private:
    const char * m_rootSymbol;
    const char * m_definition;
    const Prefixable m_prefixable;
    const Prefix * m_outputPrefixes;
    const Prefix * m_outputPrefixesUpperBound;
  };

  class Dimension {
  public:
    template<typename T>
    struct Vector {
      struct Metrics {
        size_t supportSize;
        T norm;
      };
      Metrics metrics() const;
      static Vector FromBaseUnits(const Expression baseUnits);
      const T coefficientAtIndex(size_t i) const {
        assert(i < NumberOfBaseUnits);
        return *(reinterpret_cast<const T*>(this) + i);
      }
      void setCoefficientAtIndex(size_t i, T c) {
        assert(i < NumberOfBaseUnits);
        *(reinterpret_cast<T*>(this) + i) = c;
      }
      T time;
      T distance;
      T mass;
      T current;
      T temperature;
      T amountOfSubstance;
      T luminuousIntensity;
    };
    template <size_t N>
    constexpr Dimension(Vector<int8_t> vector, const Representative (&representatives)[N], const Prefix * stdRepresentativePrefix) :
      m_vector(vector),
      m_representatives(representatives),
      m_representativesUpperBound(representatives + N),
      m_stdRepresentativePrefix(stdRepresentativePrefix)
    {
    }
    const Vector<int8_t> * vector() const { return &m_vector; }
    const Representative * stdRepresentative() const { return m_representatives; }
    const Representative * representativesUpperBound() const { return m_representativesUpperBound; }
    const Prefix * stdRepresentativePrefix() const { return m_stdRepresentativePrefix; }
    bool canParse(const char * symbol, size_t length,
        const Representative * * representative, const Prefix * * prefix) const;
  private:
    Vector<int8_t> m_vector;
    const Representative * m_representatives;
    const Representative * m_representativesUpperBound;
    const Prefix * m_stdRepresentativePrefix;
  };

  UnitNode(const Dimension * dimension, const Representative * representative, const Prefix * prefix) :
    ExpressionNode(),
    m_dimension(dimension),
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
  Sign sign(Context * context) const override;
  Expression removeUnit(Expression * unit) override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }

  // Comparison
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::OneLetter; } // TODO

  const Dimension * dimension() const { return m_dimension; }
  const Representative * representative() const { return m_representative; }
  const Prefix * prefix() const { return m_prefix; }
  void setRepresentative(const Representative * representative) { m_representative = representative; }
  void setPrefix(const Prefix * prefix) { m_prefix = prefix; }

private:
  const Dimension * m_dimension;
  const Representative * m_representative;
  const Prefix * m_prefix;

  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class Unit final : public Expression {
  friend class UnitNode;
public:
  typedef UnitNode::Prefix Prefix;
  typedef UnitNode::Representative Representative;
  typedef UnitNode::Dimension Dimension;
  static constexpr const Prefix
    PicoPrefix  = Prefix("p", -12),
    NanoPrefix  = Prefix("n",  -9),
    MicroPrefix = Prefix("μ",  -6),
    MilliPrefix = Prefix("m",  -3),
    CentiPrefix = Prefix("c",  -2),
    DeciPrefix  = Prefix("d",  -1),
    EmptyPrefix = Prefix("",    0),
    DecaPrefix  = Prefix("da",  1),
    HectoPrefix = Prefix("h",   2),
    KiloPrefix  = Prefix("k",   3),
    MegaPrefix  = Prefix("M",   6),
    GigaPrefix  = Prefix("G",   9),
    TeraPrefix  = Prefix("T",  12);
  static constexpr const Prefix
    NoPrefix[] = {
      EmptyPrefix
    },
    NegativeLongScalePrefixes[] = {
      PicoPrefix,
      NanoPrefix,
      MicroPrefix,
      MilliPrefix,
      EmptyPrefix,
    },
    PositiveLongScalePrefixes[] = {
      EmptyPrefix,
      KiloPrefix,
      MegaPrefix,
      GigaPrefix,
      TeraPrefix,
    },
    LongScalePrefixes[] = {
      PicoPrefix,
      NanoPrefix,
      MicroPrefix,
      MilliPrefix,
      EmptyPrefix,
      KiloPrefix,
      MegaPrefix,
      GigaPrefix,
      TeraPrefix,
    },
    NegativePrefixes[] = {
      PicoPrefix,
      NanoPrefix,
      MicroPrefix,
      MilliPrefix,
      CentiPrefix,
      DeciPrefix,
      EmptyPrefix,
    },
    AllPrefixes[] = {
      PicoPrefix,
      NanoPrefix,
      MicroPrefix,
      MilliPrefix,
      CentiPrefix,
      DeciPrefix,
      EmptyPrefix,
      DecaPrefix,
      HectoPrefix,
      KiloPrefix,
      MegaPrefix,
      GigaPrefix,
      TeraPrefix,
    };
  static constexpr size_t NumberOfBaseUnits = UnitNode::NumberOfBaseUnits;
  static constexpr const Representative
    TimeRepresentatives[] = {
        Representative("s",   nullptr,
            Representative::Prefixable::Yes,
            NegativeLongScalePrefixes),
        Representative("min", "60*_s",
            Representative::Prefixable::No,
            NoPrefix),
        Representative("h",   "60*60*_s",
            Representative::Prefixable::No,
            NoPrefix),
        Representative("day", "24*60*60*_s",
            Representative::Prefixable::No,
            NoPrefix),
        Representative("week", "7*24*60*60*_s",
            Representative::Prefixable::No,
            NoPrefix),
        Representative("month", "365.25/12*24*60*60*_s",
            Representative::Prefixable::No,
            NoPrefix),
        Representative("year", "365.25*24*60*60*_s",
            Representative::Prefixable::No,
            NoPrefix),
        },
    DistanceRepresentatives[] = {
        Representative("m",   nullptr,
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        Representative("au",  "149587870700*_m",
            Representative::Prefixable::No,
            NoPrefix),
        Representative("ly",  "299792458*_m/_s*_year",
            Representative::Prefixable::No,
            NoPrefix),
        Representative("pc",  "180*60*60/π*_au",
            Representative::Prefixable::No,
            NoPrefix),
        },
    MassRepresentatives[] = {
        Representative("g",   nullptr,
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        Representative("t",   "1000_kg",
            Representative::Prefixable::Yes,
            NoPrefix),
        Representative("Da",  "(6.02214076*10^23*1000)^-1*_kg",
            Representative::Prefixable::Yes,
            NoPrefix),
        },
    CurrentRepresentatives[] = {
        Representative("A",   nullptr,
            Representative::Prefixable::Yes,
            NegativeLongScalePrefixes),
        },
    TemperatureRepresentatives[] = {
        Representative("K",   nullptr,
            Representative::Prefixable::No,
            NoPrefix),
        },
    AmountOfSubstanceRepresentatives[] = {
        Representative("mol", nullptr,
            Representative::Prefixable::Yes,
            NegativeLongScalePrefixes),
        },
    LuminousIntensityRepresentatives[] = {
        Representative("cd",  nullptr,
            Representative::Prefixable::No,
            NoPrefix),
        },
    FrequencyRepresentatives[] = {
        Representative("Hz",  "_s^-1",
            Representative::Prefixable::Yes,
            PositiveLongScalePrefixes),
        },
    ForceRepresentatives[] = {
        Representative("N",   "_kg*_m*_s^-2",
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        },
    PressureRepresentatives[] = {
        Representative("Pa",  "_kg*_m^-1*_s^-2",
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        Representative("bar", "1000_hPa",
            Representative::Prefixable::Yes,
            NoPrefix),
        Representative("atm", "101325_Pa",
            Representative::Prefixable::Yes,
            NoPrefix),
        },
    EnergyRepresentatives[] = {
        Representative("J",   "_kg*_m^2*_s^-2",
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        Representative("eV",  "1.602176634ᴇ-19*_J",
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        },
    PowerRepresentatives[] = {
        Representative("W",   "_kg*_m^2*_s^-3",
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        },
    ElectricChargeRepresentatives[] = {
        Representative("C",   "_A*_s",
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        },
    ElectricPotentialRepresentatives[] = {
        Representative("V",   "_kg*_m^2*_s^-3*_A^-1",
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        },
    ElectricCapacitanceRepresentatives[] = {
        Representative("F",   "_A^2*_s^4*_kg^-1*_m^-2",
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        },
    ElectricResistanceRepresentatives[] = {
        Representative("Ω", "_kg*_m^2*_s^-3*_A^-2",
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        },
    ElectricConductanceRepresentatives[] = {
        Representative("S",   "_A^2*_s^3*_kg^-1*_m^-2",
            Representative::Prefixable::Yes,
            LongScalePrefixes),
        },
    MagneticFluxRepresentatives[] = {
        Representative("Wb",  "_kg*_m^2*_s^-2*_A^-1",
            Representative::Prefixable::Yes,
            NoPrefix),
        },
    MagneticFieldRepresentatives[] = {
        Representative("T",   "_kg*_s^-2*_A^-1",
            Representative::Prefixable::Yes,
            NoPrefix),
        },
    InductanceRepresentatives[] = {
        Representative("H",   "_kg*_m^2*_s^-2*_A^-2",
            Representative::Prefixable::Yes,
            NoPrefix),
        },
    CatalyticActivityRepresentatives[] = {
        Representative("kat", "_mol*_s^-1",
            Representative::Prefixable::Yes,
            NoPrefix),
        },
    SurfaceRepresentatives[] = {
        Representative("ha",  "10^4*_m^2",
            Representative::Prefixable::No,
            NoPrefix),
        },
    VolumeRepresentatives[] = {
        Representative("L",   "10^-3*_m^3",
            Representative::Prefixable::Yes,
            NegativePrefixes),
        };
  // TODO: find a better way to find defines these pointers
  static_assert(sizeof(TimeRepresentatives)/sizeof(Representative) == 7, "The Unit::SecondRepresentative, Unit::HourRepresentative and so on might require to be fixed if the TimeRepresentatives table was changed.");
  static const Representative constexpr * SecondRepresentative = &TimeRepresentatives[0];
  static const Representative constexpr * MinuteRepresentative = &TimeRepresentatives[1];
  static const Representative constexpr * HourRepresentative = &TimeRepresentatives[2];
  static const Representative constexpr * DayRepresentative = &TimeRepresentatives[3];
  static const Representative constexpr * MonthRepresentative = &TimeRepresentatives[5];
  static const Representative constexpr * YearRepresentative = &TimeRepresentatives[6];
  static const Representative constexpr * MeterRepresentative = &DistanceRepresentatives[0];
  static const Representative constexpr * KilogramRepresentative = &MassRepresentatives[0];
  static const Representative constexpr * LiterRepresentative = &VolumeRepresentatives[0];
  static const Representative constexpr * WattRepresentative = &PowerRepresentatives[0];
  static_assert(sizeof(EnergyRepresentatives)/sizeof(Representative) == 2, "The Unit::ElectronVoltRepresentative might require to be fixed if the EnergyRepresentatives table was changed.");
  static const Representative constexpr * ElectronVoltRepresentative = &EnergyRepresentatives[1];
  static constexpr const Dimension DimensionTable[] = {
    /* The current table is sorted from most to least simple units.
     * The order determines the behavior of simplification.
     */
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 1,
          .distance           = 0,
          .mass               = 0,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        TimeRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 0,
          .distance           = 1,
          .mass               = 0,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        DistanceRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 0,
          .distance           = 0,
          .mass               = 1,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        MassRepresentatives,
        &KiloPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 0,
          .distance           = 0,
          .mass               = 0,
          .current            = 1,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        CurrentRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 0,
          .distance           = 0,
          .mass               = 0,
          .current            = 0,
          .temperature        = 1,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        TemperatureRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 0,
          .distance           = 0,
          .mass               = 0,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 1,
          .luminuousIntensity = 0,
        },
        AmountOfSubstanceRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 0,
          .distance           = 0,
          .mass               = 0,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 1,
        },
        LuminousIntensityRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-1,
          .distance           = 0,
          .mass               = 0,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        FrequencyRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-2,
          .distance           = 1,
          .mass               = 1,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        ForceRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-2,
          .distance           =-1,
          .mass               = 1,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        PressureRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-2,
          .distance           = 2,
          .mass               = 1,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        EnergyRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-3,
          .distance           = 2,
          .mass               = 1,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        PowerRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 1,
          .distance           = 0,
          .mass               = 0,
          .current            = 1,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        ElectricChargeRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-3,
          .distance           = 2,
          .mass               = 1,
          .current            =-1,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        ElectricPotentialRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 4,
          .distance           =-2,
          .mass               =-1,
          .current            = 2,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        ElectricCapacitanceRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-3,
          .distance           = 2,
          .mass               = 1,
          .current            =-2,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        ElectricResistanceRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 3,
          .distance           =-2,
          .mass               =-1,
          .current            = 2,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        ElectricConductanceRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-2,
          .distance           = 2,
          .mass               = 1,
          .current            =-1,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        MagneticFluxRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-2,
          .distance           = 0,
          .mass               = 1,
          .current            =-1,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        MagneticFieldRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-2,
          .distance           = 2,
          .mass               = 1,
          .current            =-2,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        InductanceRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               =-1,
          .distance           = 0,
          .mass               = 0,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 1,
          .luminuousIntensity = 0,
        },
        CatalyticActivityRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 0,
          .distance           = 2,
          .mass               = 0,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        SurfaceRepresentatives,
        &EmptyPrefix
        ),
    Dimension(
        Dimension::Vector<int8_t> {
          .time               = 0,
          .distance           = 3,
          .mass               = 0,
          .current            = 0,
          .temperature        = 0,
          .amountOfSubstance  = 0,
          .luminuousIntensity = 0,
        },
        VolumeRepresentatives,
        &EmptyPrefix
        ),
  };
  // TODO: find a better way to find defines these pointers
  static_assert(sizeof(DimensionTable)/sizeof(Dimension) == 23, "The Unit::TimeDimension, Unit::DistanceDimension and so on might require to be fixed if the Dimension table was changed.");
  static const Dimension constexpr * TimeDimension = &DimensionTable[0] ;
  static const Dimension constexpr * DistanceDimension = &DimensionTable[1];
  static const Dimension constexpr * MassDimension = &DimensionTable[2];
  static const Dimension constexpr * EnergyDimension = &DimensionTable[10];
  static const Dimension constexpr * PowerDimension = &DimensionTable[11];
  static const Dimension constexpr * VolumeDimension = &DimensionTable[sizeof(DimensionTable)/sizeof(Dimension)-1];

  static constexpr const Unit::Dimension * DimensionTableUpperBound =
    DimensionTable + sizeof(DimensionTable)/sizeof(Dimension);
  static bool CanParse(const char * symbol, size_t length,
      const Dimension * * dimension, const Representative * * representative, const Prefix * * prefix);

  Unit(const UnitNode * node) : Expression(node) {}
  static Unit Builder(const Dimension * dimension, const Representative * representative, const Prefix * prefix);
  static Unit Kilometer() { return Builder(DistanceDimension, MeterRepresentative, &KiloPrefix); }
  static Unit Second() { return Builder(TimeDimension, SecondRepresentative, &EmptyPrefix); }
  static Unit Minute() { return Builder(TimeDimension, MinuteRepresentative, &EmptyPrefix); }
  static Unit Hour() { return Builder(TimeDimension, HourRepresentative, &EmptyPrefix); }
  static Unit Day() { return Builder(TimeDimension, DayRepresentative, &EmptyPrefix); }
  static Unit Month() { return Builder(TimeDimension, MonthRepresentative, &EmptyPrefix); }
  static Unit Year() { return Builder(TimeDimension, YearRepresentative, &EmptyPrefix); }
  static Unit Liter() { return Builder(VolumeDimension, LiterRepresentative, &EmptyPrefix); }
  static Unit ElectronVolt() { return Builder(EnergyDimension, ElectronVoltRepresentative, &EmptyPrefix); }
  static Unit Watt() { return Builder(PowerDimension, WattRepresentative, &EmptyPrefix); }
  static Expression BuildTimeSplit(double seconds, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);

  static bool IsISSpeed(Expression & e);
  static bool IsISVolume(Expression & e);
  static bool IsISEnergy(Expression & e);
  static bool IsISTime(Expression & e);
  bool isMeter() const;
  bool isSecond() const;
  bool isKilogram() const;

  // Simplification
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  static void ChooseBestRepresentativeAndPrefixForValue(Expression * units, double * value, ExpressionNode::ReductionContext reductionContext) { return ChooseBestMultipleForValue(units, value, true, reductionContext); }
  static void ChooseBestPrefixForValue(Expression * units, double * value, ExpressionNode::ReductionContext reductionContext) { return ChooseBestMultipleForValue(units, value, false, reductionContext); }

  static constexpr double MeterPerSecondToKilometerPerHourFactor = 60.0*60.0/1000.0;
  static constexpr double CubicMeterToLiterFactor = 10.0*10.0*10.0;
  static constexpr double JouleToWatthourFactor = 1.0/3600.0;
private:
  UnitNode * node() const { return static_cast<UnitNode *>(Expression::node()); }
  static void ChooseBestMultipleForValue(Expression * units, double * value, bool tuneRepresentative, ExpressionNode::ReductionContext reductionContext);
  void chooseBestMultipleForValue(double * value, const int exponent, bool tuneRepresentative, ExpressionNode::ReductionContext reductionContext);
  Expression removeUnit(Expression * unit);
};

}

#endif
