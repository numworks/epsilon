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
      Yes,
      PositiveOnly
    };
    enum class OutputSystem {
      None,
      Imperial,
      Metric,
      All
    };
    template <size_t N>
    constexpr Representative(const char * rootSymbol, const char * definition, const Prefixable prefixable, const Prefix * const (&outputPrefixes)[N], const OutputSystem outputSystem = OutputSystem::All) :
      m_rootSymbol(rootSymbol),
      m_definition(definition),
      m_prefixable(prefixable),
      m_outputPrefixes(outputPrefixes),
      m_outputPrefixesLength(N),
      m_outputSystem(outputSystem)
    {
    }
    const char * rootSymbol() const { return m_rootSymbol; }
    const char * definition() const { return m_definition; }
    bool isPrefixable() const { return m_prefixable != Prefixable::No; }
    const Prefix * const * outputPrefixes() const { return m_outputPrefixes; }
    size_t outputPrefixesLength() const { return m_outputPrefixesLength; }
    bool canParse(const char * symbol, size_t length,
        const Prefix * * prefix) const;
    int serialize(char * buffer, int bufferSize, const Prefix * prefix) const;
    const Prefix * bestPrefixForValue(double & value, const float exponent) const;
    bool canOutputInSystem(Preferences::UnitFormat system) const;
  private:
    const char * m_rootSymbol;
    const char * m_definition;
    const Prefixable m_prefixable;
    const Prefix * const * m_outputPrefixes;
    const size_t m_outputPrefixesLength;
    const OutputSystem m_outputSystem;
  };

  class Dimension {
  public:
    template<typename T>
    struct Vector {
      // SupportSize is defined as the number of distinct base units.
      size_t supportSize() const;
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
  Expression shallowBeautify(ReductionContext reductionContext) override;
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
  /* TODO: Prefix, Representative and Dimension defined below must be defined
   * only once and all units must be constructed from their pointers. This way
   * we can easily check if two Unit objects are equal by comparing pointers.
   * This saves us from overloading the == operator on Prefix, Representative
   * and Dimension and saves time at execution. We should assert at compilation
   * that only one occurence of each is built by maybe privatizing constructors
   * on these classes? */
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
  static constexpr const Prefix * NoPrefix[] = {
      &EmptyPrefix
    };
  static constexpr const Prefix * NegativeLongScalePrefixes[] = {
      &PicoPrefix,
      &NanoPrefix,
      &MicroPrefix,
      &MilliPrefix,
      &EmptyPrefix,
    };
  static constexpr const Prefix * PositiveLongScalePrefixes[] = {
      &EmptyPrefix,
      &KiloPrefix,
      &MegaPrefix,
      &GigaPrefix,
      &TeraPrefix,
    };
  static constexpr const Prefix * LongScalePrefixes[] = {
      &PicoPrefix,
      &NanoPrefix,
      &MicroPrefix,
      &MilliPrefix,
      &EmptyPrefix,
      &KiloPrefix,
      &MegaPrefix,
      &GigaPrefix,
      &TeraPrefix,
    };
  static constexpr const Prefix * NegativePrefixes[] = {
      &PicoPrefix,
      &NanoPrefix,
      &MicroPrefix,
      &MilliPrefix,
      &CentiPrefix,
      &DeciPrefix,
      &EmptyPrefix,
    };
  static constexpr const Prefix * AllPrefixes[] = {
      &PicoPrefix,
      &NanoPrefix,
      &MicroPrefix,
      &MilliPrefix,
      &CentiPrefix,
      &DeciPrefix,
      &EmptyPrefix,
      &DecaPrefix,
      &HectoPrefix,
      &KiloPrefix,
      &MegaPrefix,
      &GigaPrefix,
      &TeraPrefix,
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
        Representative("m", nullptr,
            Representative::Prefixable::Yes,
            LongScalePrefixes,
            Representative::OutputSystem::Metric),
        Representative("au",  "149597870700*_m",
            Representative::Prefixable::No,
            NoPrefix),
        Representative("ly", "299792458*_m/_s*_year",
            Representative::Prefixable::No,
            NoPrefix),
        Representative("pc", "180*60*60/π*_au",
            Representative::Prefixable::No,
            NoPrefix),
        Representative("in", "0.0254*_m",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Imperial),
        Representative("ft", "12*0.0254*_m",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Imperial),
        Representative("yd", "3*12*0.0254*_m",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::None),
        Representative("mi", "1760*3*12*0.0254*_m",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Imperial),
        },
    MassRepresentatives[] = {
        Representative("kg", nullptr,
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Metric),
        Representative("g", "0.001_kg",
            Representative::Prefixable::Yes,
            NegativeLongScalePrefixes,
            Representative::OutputSystem::Metric),
        Representative("t", "1000_kg",
            Representative::Prefixable::PositiveOnly,
            NoPrefix,
            Representative::OutputSystem::Metric),
        Representative("Da", "(6.02214076*10^23*1000)^-1*_kg",
            Representative::Prefixable::Yes,
            NoPrefix),
        Representative("oz", "0.028349523125*_kg",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Imperial),
        Representative("lb", "16*0.028349523125*_kg",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Imperial),
        Representative("shtn", "2000*16*0.028349523125*_kg",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Imperial),
        Representative("lgtn", "2240*16*0.028349523125*_kg",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::None),
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
            LongScalePrefixes),
        },
    CatalyticActivityRepresentatives[] = {
        Representative("kat", "_mol*_s^-1",
            Representative::Prefixable::Yes,
            NoPrefix),
        },
    SurfaceRepresentatives[] = {
        Representative("ha",  "10^4*_m^2",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Metric),
        Representative("acre", "4046.8564224*_m^2",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Imperial),
        },
    VolumeRepresentatives[] = {
        Representative("L", "10^-3*_m^3",
            Representative::Prefixable::Yes,
            NegativePrefixes,
            Representative::OutputSystem::Metric),
        Representative("tsp", "0.00492892159375*_L",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::None),
        Representative("Tbsp", "3*0.00492892159375*_L",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::None),
        Representative("floz", "0.0295735295625*_L",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Imperial),
        Representative("cp", "8*0.0295735295625*_L",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Imperial),
        Representative("pt", "16*0.0295735295625*_L",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::None),
        Representative("qt", "32*0.0295735295625*_L",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::None),
        Representative("gal", "128*0.0295735295625*_L",
            Representative::Prefixable::No,
            NoPrefix,
            Representative::OutputSystem::Imperial),
        };
  // TODO: find a better way to define these pointers
  static_assert(sizeof(TimeRepresentatives)/sizeof(Representative) == 7, "The Unit::SecondRepresentative, Unit::HourRepresentative and so on might require to be fixed if the TimeRepresentatives table was changed.");
  static const Representative constexpr * SecondRepresentative = &TimeRepresentatives[0];
  static const Representative constexpr * MinuteRepresentative = &TimeRepresentatives[1];
  static const Representative constexpr * HourRepresentative = &TimeRepresentatives[2];
  static const Representative constexpr * DayRepresentative = &TimeRepresentatives[3];
  static const Representative constexpr * MonthRepresentative = &TimeRepresentatives[5];
  static const Representative constexpr * YearRepresentative = &TimeRepresentatives[6];
  static const Representative constexpr * MeterRepresentative = &DistanceRepresentatives[0];
  static_assert(sizeof(DistanceRepresentatives)/sizeof(Representative) == 8, "The Unit::MileRepresentative et al. might require to be fixed if the DistanceRepresentatives table was changed.");
  static const Representative constexpr * InchRepresentative = &DistanceRepresentatives[4];
  static const Representative constexpr * FootRepresentative = &DistanceRepresentatives[5];
  static const Representative constexpr * YardRepresentative = &DistanceRepresentatives[6];
  static const Representative constexpr * MileRepresentative = &DistanceRepresentatives[7];
  static const Representative constexpr * KilogramRepresentative = &MassRepresentatives[0];
  static const Representative constexpr * GramRepresentative = &MassRepresentatives[1];
  static_assert(sizeof(MassRepresentatives)/sizeof(Representative) == 8, "The Unit::OunceRepresentative et al. might require to be fixed if the MassRepresentatives table was changed.");
  static const Representative constexpr * OunceRepresentative = &MassRepresentatives[4];
  static const Representative constexpr * PoundRepresentative = &MassRepresentatives[5];
  static const Representative constexpr * ShortTonRepresentative = &MassRepresentatives[6];
  static const Representative constexpr * LiterRepresentative = &VolumeRepresentatives[0];
  static_assert(sizeof(VolumeRepresentatives)/sizeof(Representative) == 8, "The Unit::FluidOunceRepresentative et al. might require to be fixed if the VolumeRepresentatives table was changed.");
  static const Representative constexpr * FluidOunceRepresentative = &VolumeRepresentatives[3];
  static const Representative constexpr * CupRepresentative = &VolumeRepresentatives[4];
  static const Representative constexpr * GallonRepresentative = &VolumeRepresentatives[7];
  static const Representative constexpr * WattRepresentative = &PowerRepresentatives[0];
  static_assert(sizeof(EnergyRepresentatives)/sizeof(Representative) == 2, "The Unit::ElectronVoltRepresentative might require to be fixed if the EnergyRepresentatives table was changed.");
  static const Representative constexpr * ElectronVoltRepresentative = &EnergyRepresentatives[1];
  static_assert(sizeof(SurfaceRepresentatives)/sizeof(Representative) == 2, "The Unit::HectareRepresentative et al. might require to be fixed if the VolumeRepresentatives table was changed.");
  static const Representative constexpr * HectareRepresentative = &SurfaceRepresentatives[0];
  static const Representative constexpr * AcreRepresentative = &SurfaceRepresentatives[1];

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
        &EmptyPrefix
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
  static const Dimension constexpr * SurfaceDimension = &DimensionTable[sizeof(DimensionTable)/sizeof(Dimension)-2];
  static const Dimension constexpr * VolumeDimension = &DimensionTable[sizeof(DimensionTable)/sizeof(Dimension)-1];

  static constexpr const Unit::Dimension * DimensionTableUpperBound =
    DimensionTable + sizeof(DimensionTable)/sizeof(Dimension);
  static bool CanParse(const char * symbol, size_t length,
      const Dimension * * dimension, const Representative * * representative, const Prefix * * prefix);

  Unit(const UnitNode * node) : Expression(node) {}
  static Unit Builder(const Dimension * dimension, const Representative * representative, const Prefix * prefix);
  static Unit Meter() { return Builder(DistanceDimension, MeterRepresentative, &EmptyPrefix); }
  static Unit Kilometer() { return Builder(DistanceDimension, MeterRepresentative, &KiloPrefix); }
  static Unit Inch() { return Builder(DistanceDimension, InchRepresentative, &EmptyPrefix); }
  static Unit Foot() { return Builder(DistanceDimension, FootRepresentative, &EmptyPrefix); }
  static Unit Yard() { return Builder(DistanceDimension, YardRepresentative, &EmptyPrefix); }
  static Unit Mile() { return Builder(DistanceDimension, MileRepresentative, &EmptyPrefix); }
  static Unit Second() { return Builder(TimeDimension, SecondRepresentative, &EmptyPrefix); }
  static Unit Minute() { return Builder(TimeDimension, MinuteRepresentative, &EmptyPrefix); }
  static Unit Hour() { return Builder(TimeDimension, HourRepresentative, &EmptyPrefix); }
  static Unit Day() { return Builder(TimeDimension, DayRepresentative, &EmptyPrefix); }
  static Unit Month() { return Builder(TimeDimension, MonthRepresentative, &EmptyPrefix); }
  static Unit Year() { return Builder(TimeDimension, YearRepresentative, &EmptyPrefix); }
  static Unit Liter() { return Builder(VolumeDimension, LiterRepresentative, &EmptyPrefix); }
  static Unit ElectronVolt() { return Builder(EnergyDimension, ElectronVoltRepresentative, &EmptyPrefix); }
  static Unit Watt() { return Builder(PowerDimension, WattRepresentative, &EmptyPrefix); }
  static Unit Gram() { return Builder(MassDimension, GramRepresentative, &EmptyPrefix); }
  static Unit Ounce() { return Builder(MassDimension, OunceRepresentative, &EmptyPrefix); }
  static Unit Pound() { return Builder(MassDimension, PoundRepresentative, &EmptyPrefix); }
  static Unit ShortTon() { return Builder(MassDimension, ShortTonRepresentative, &EmptyPrefix); }
  static Unit FluidOunce() { return Builder(VolumeDimension, FluidOunceRepresentative, &EmptyPrefix); }
  static Unit Cup() { return Builder(VolumeDimension, CupRepresentative, &EmptyPrefix); }
  static Unit Gallon() { return Builder(VolumeDimension, GallonRepresentative, &EmptyPrefix); }
  static Unit Hectare() { return Builder(SurfaceDimension, HectareRepresentative, &EmptyPrefix); }
  static Unit Acre() { return Builder(SurfaceDimension, AcreRepresentative, &EmptyPrefix); }
  static Expression BuildSplit(double baseValue, const Unit * units, const double * conversionFactors, int numberOfUnits, Context * context);
  static Expression BuildTimeSplit(double seconds, Context * context);
  static Expression BuildImperialDistanceSplit(double inches, Context * context);
  static Expression BuildImperialMassSplit(double ounces, Context * context);
  static Expression BuildImperialVolumeSplit(double fluidOunces, Context * context);
  static double ConvertedValueInUnit(Expression e, Unit unit, ExpressionNode::ReductionContext reductionContext);

  static bool IsSI(Expression & e);
  static bool IsSISpeed(Expression & e);
  static bool IsSIVolume(Expression & e);
  static bool IsSIEnergy(Expression & e);
  static bool IsSITime(Expression & e);
  static bool IsSIDistance(Expression & e);
  static bool IsSIMass(Expression & e);
  static bool IsSISurface(Expression & e);
  bool isMeter() const;
  bool isSecond() const;
  bool isKilogram() const;

  static Expression StandardSpeedConversion(Expression e, ExpressionNode::ReductionContext reductionContext);
  static Expression StandardDistanceConversion(Expression e, ExpressionNode::ReductionContext reductionContext);
  static Expression StandardVolumeConversion(Expression e, ExpressionNode::ReductionContext reductionContext);
  static Expression StandardMassConversion(Expression e, ExpressionNode::ReductionContext reductionContext);
  static Expression StandardSurfaceConversion(Expression e, ExpressionNode::ReductionContext reductionContext);

  // Simplification
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify(ExpressionNode::ReductionContext reductionContext);
  static void ChooseBestRepresentativeAndPrefixForValue(Expression * units, double * value, ExpressionNode::ReductionContext reductionContext) { return ChooseBestMultipleForValue(units, value, true, reductionContext); }
  static void ChooseBestPrefixForValue(Expression * units, double * value, ExpressionNode::ReductionContext reductionContext) { return ChooseBestMultipleForValue(units, value, false, reductionContext); }

  // This could be computed from the time representatives but we save time by using constexpr double
  static constexpr double SecondsPerMinute = 60.0;
private:
  static constexpr double MinutesPerHour = 60.0;
  static constexpr double HoursPerDay = 24.0;
  static constexpr double DaysPerYear = 365.25;
  static constexpr double MonthPerYear = 12.0;
  static constexpr double DaysPerMonth = DaysPerYear/MonthPerYear;
  static constexpr double InchesPerFoot = 12.;
  static constexpr double FeetPerYard = 3.;
  static constexpr double YardsPerMile = 1760.;
  static constexpr double OuncesPerPound = 16.;
  static constexpr double PoundsPerShortTon = 2000.;
  static constexpr double FluidOuncesPerCup = 8.;
  static constexpr double CupsPerGallon = 16.;
  UnitNode * node() const { return static_cast<UnitNode *>(Expression::node()); }
  bool isSI() const;
  static void ChooseBestMultipleForValue(Expression * units, double * value, bool tuneRepresentative, ExpressionNode::ReductionContext reductionContext);
  void chooseBestMultipleForValue(double * value, const float exponent, bool tuneRepresentative, ExpressionNode::ReductionContext reductionContext);
  Expression removeUnit(Expression * unit);
};

}

#endif
