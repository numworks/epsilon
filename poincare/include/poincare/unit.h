#ifndef POINCARE_UNIT_H
#define POINCARE_UNIT_H

#include <poincare/expression.h>

namespace Poincare {

class UnitNode final : public ExpressionNode {
public:

  /* The units having the same physical dimension are grouped together.
   * Each such group has a standard representative with a standard prefix.
   *
   * A standard unit is a derived unit, when defined from base units
   * or otherwise a base unit (if no definition is provided).
   *
   * Each representative has
   *  - a root symbol
   *  - a definition
   *  - a list of allowed prefixes
   * Given a Dimension, a representative in that Dimension and a Prefix
   * allowed for that representative, one may get a symbol and an Expression.
   */

  class Prefix {
  public:
    constexpr Prefix(const char * symbol, int8_t exponent) :
      m_symbol(symbol),
      m_exponent(exponent)
    {}
    const char * symbol() const { return m_symbol; }
    const int8_t exponent() const { return m_exponent; }
    int serialize(char * buffer, int bufferSize) const;
  private:
    const char * m_symbol;
    int8_t m_exponent;
  };

  class Representative {
  public:
    constexpr Representative(const char * rootSymbol, const char * definition, const Prefix * allowedPrefixes, size_t numberOfAllowedPrefixes) :
      m_rootSymbol(rootSymbol),
      m_definition(definition),
      m_allowedPrefixes(allowedPrefixes),
      m_allowedPrefixesUpperBound(allowedPrefixes + numberOfAllowedPrefixes)
    {
    }
    const char * rootSymbol() const { return m_rootSymbol; }
    const char * definition() const { return m_definition; }
    const Prefix * allowedPrefixes() const { return m_allowedPrefixes; }
    const Prefix * allowedPrefixesUpperBound() const { return m_allowedPrefixesUpperBound; }
    bool canParse(const char * symbol, size_t length,
        const Prefix * * prefix) const;
    int serialize(char * buffer, int bufferSize, const Prefix * prefix) const;
  private:
    const char * m_rootSymbol;
    const char * m_definition;
    const Prefix * m_allowedPrefixes;
    const Prefix * m_allowedPrefixesUpperBound;
  };

  class Dimension {
  public:
    constexpr Dimension(const Representative * representatives, size_t numberOfRepresentatives, const Prefix * stdRepresentativePrefix) :
      m_representatives(representatives),
      m_representativesUpperBound(representatives + numberOfRepresentatives),
      m_stdRepresentativePrefix(stdRepresentativePrefix)
    {
    }
    const Representative * stdRepresentative() const { return m_representatives; }
    const Representative * representativesUpperBound() const { return m_representativesUpperBound; }
    const Prefix * stdRepresentativePrefix() const { return m_stdRepresentativePrefix; }
    bool canParse(const char * symbol, size_t length,
        const Representative * * representative, const Prefix * * prefix) const;
  private:
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
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Unit";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " symbol=\"" << m_prefix->symbol() << m_representative->rootSymbol() << "\"";
  }
#endif

  // Expression Properties
  Type type() const override { return Type::Unit; }
  Sign sign(Context * context) const override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }

  // Comparison
  int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression shallowBeautify(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::OneLetter; } // TODO

  const Dimension * dimension() const { return m_dimension; }
  const Representative * representative() const { return m_representative; }
  const Prefix * prefix() const { return m_prefix; }
  void setPrefix(const Prefix * prefix) { m_prefix = prefix; }

private:
  const Dimension * m_dimension;
  const Representative * m_representative;
  const Prefix * m_prefix;

  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class Unit final : public Expression {
public:
  typedef UnitNode::Prefix Prefix;
  typedef UnitNode::Representative Representative;
  typedef UnitNode::Dimension Dimension;
  static constexpr const Prefix
    PicoPrefix  = Prefix("p", -12),
    NanoPrefix  = Prefix("n",  -9),
    MicroPrefix = Prefix("u",  -6), // FIXME μ
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
  static constexpr size_t
    NoPrefixCount = sizeof(NoPrefix)/sizeof(Prefix),
    NegativeLongScalePrefixesCount = sizeof(NegativeLongScalePrefixes)/sizeof(Prefix),
    PositiveLongScalePrefixesCount = sizeof(PositiveLongScalePrefixes)/sizeof(Prefix),
    AllPrefixesCount = sizeof(AllPrefixes)/sizeof(Prefix);
  static constexpr const Representative
    TimeRepresentatives[] = {
        Representative("s",   nullptr,
            NegativeLongScalePrefixes, NegativeLongScalePrefixesCount),
        Representative("min", "60*_s",
            NoPrefix, NoPrefixCount),
        Representative("h",   "60*60*_s",
            NoPrefix, NoPrefixCount),
        Representative("day", "24*60*60*_s",
            NoPrefix, NoPrefixCount),
        Representative("week", "7*24*60*60*_s",
            NoPrefix, NoPrefixCount),
        Representative("month", "30*7*24*60*60*_s",
            NoPrefix, NoPrefixCount),
        Representative("year", "365.25*24*60*60*_s",
            NoPrefix, NoPrefixCount),
        },
    DistanceRepresentatives[] = {
        Representative("m",   nullptr,
            AllPrefixes, AllPrefixesCount),
        Representative("ang", "10^-10*_m",
            NoPrefix, NoPrefixCount), //FIXME Codepoint
        Representative("au",  "149587870700*_m",
            NoPrefix, NoPrefixCount),
        Representative("ly",  "299792458*_m/_s*_year",
            NoPrefix, NoPrefixCount),
        Representative("pc",  "180*60*60/π*_au",
            NoPrefix, NoPrefixCount),
        },
    MassRepresentatives[] = {
        Representative("g",   nullptr,
            AllPrefixes, AllPrefixesCount),
        Representative("t",   "1000_kg",
            PositiveLongScalePrefixes, PositiveLongScalePrefixesCount),
        Representative("Da",  "(6.02214076*10^23*1000)^-1*_kg",
            NoPrefix, NoPrefixCount),
        },
    CurrentRepresentatives[] = {
        Representative("A",   nullptr,
            NegativeLongScalePrefixes, NegativeLongScalePrefixesCount),
        },
    TemperatureRepresentatives[] = {
        Representative("K",   nullptr,
            NoPrefix, NoPrefixCount),
        },
    AmountOfSubstanceRepresentatives[] = {
        Representative("mol", nullptr,
            NegativeLongScalePrefixes, NegativeLongScalePrefixesCount),
        },
    LuminousIntensityRepresentatives[] = {
        Representative("cd",  nullptr,
            NoPrefix, NoPrefixCount),
        },
    FrequencyRepresentatives[] = {
        Representative("Hz",  "_s^-1",
            PositiveLongScalePrefixes, PositiveLongScalePrefixesCount),
        },
    ForceRepresentatives[] = {
        Representative("N",   "_kg*_m*_s^-2",
            AllPrefixes, AllPrefixesCount),
        },
    PressureRepresentatives[] = {
        Representative("Pa",  "_kg*_m^-1*_s^-2",
            NoPrefix, NoPrefixCount),
        Representative("bar", "1000hPa",
            NoPrefix, NoPrefixCount),
        Representative("atm", "101325Pa",
            NoPrefix, NoPrefixCount),
        },
    EnergyRepresentatives[] = {
        Representative("J",   "_kg*_m^2*_s^-2",
            AllPrefixes, AllPrefixesCount),
        Representative("eV",  "1.602176634*10^−19*J",
            AllPrefixes, AllPrefixesCount),
        },
    PowerRepresentatives[] = {
        Representative("W",   "_kg*_m^2*_s^-3",
            AllPrefixes, AllPrefixesCount),
        },
    ElectricChargeRepresentatives[] = {
        Representative("C",   "_A*_s",
            AllPrefixes, AllPrefixesCount),
        },
    ElectricPotentialRepresentatives[] = {
        Representative("V",   "_kg*_m^2*_s^-3*_A^-1",
            AllPrefixes, AllPrefixesCount),
        },
    ElectricCapacitanceRepresentatives[] = {
        Representative("F",   "_A^2*_s^4*_kg^-1*_m^-2",
            AllPrefixes, AllPrefixesCount),
        },
    ElectricResistanceRepresentatives[] = {
        Representative("Ohm", "_kg*_m^2*_s^-3*_A^-2",
            AllPrefixes, AllPrefixesCount), //FIXME Omega CodePoint?
        },
    ElectricConductanceRepresentatives[] = {
        Representative("S",   "_A^2*_s^3*_kg^-1*_m^-2",
            AllPrefixes, AllPrefixesCount),
        },
    MagneticFluxRepresentatives[] = {
        Representative("Wb",  "_kg*_m^2*_s^-2*_A^-1",
            NoPrefix, NoPrefixCount),
        },
    MagneticFieldRepresentatives[] = {
        Representative("T",   "_kg*_s^-2*_A^-1",
            NoPrefix, NoPrefixCount),
        },
    InductanceRepresentatives[] = {
        Representative("H",   "_kg*_m^2*_s^-2*_A^-2",
            NoPrefix, NoPrefixCount),
        },
    CatalyticActivityRepresentatives[] = {
        Representative("kat", "_mol*_s^-1",
            NoPrefix, NoPrefixCount),
        },
    SurfaceRepresentatives[] = {
        Representative("ha",  "10^4*_m^2",
            NoPrefix, NoPrefixCount),
        },
    VolumeRepresentatives[] = {
        Representative("L",   "10^-3*_m^3",
            NoPrefix, NoPrefixCount),
        };
  static constexpr const Dimension DimensionTable[] = {
    /* The current table is sorted from most to least simple units.
     * The order determines the behavior of simplification.
     */
    Dimension(
        TimeRepresentatives,
        sizeof(TimeRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        DistanceRepresentatives,
        sizeof(DistanceRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        MassRepresentatives,
        sizeof(MassRepresentatives)/sizeof(Representative),
        &KiloPrefix
        ),
    Dimension(
        CurrentRepresentatives,
        sizeof(CurrentRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        TemperatureRepresentatives,
        sizeof(TemperatureRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        AmountOfSubstanceRepresentatives,
        sizeof(AmountOfSubstanceRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        LuminousIntensityRepresentatives,
        sizeof(LuminousIntensityRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        FrequencyRepresentatives,
        sizeof(FrequencyRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        ForceRepresentatives,
        sizeof(ForceRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        PressureRepresentatives,
        sizeof(PressureRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        EnergyRepresentatives,
        sizeof(EnergyRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        PowerRepresentatives,
        sizeof(PowerRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        ElectricChargeRepresentatives,
        sizeof(ElectricChargeRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        ElectricPotentialRepresentatives,
        sizeof(ElectricPotentialRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        ElectricCapacitanceRepresentatives,
        sizeof(ElectricCapacitanceRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        ElectricResistanceRepresentatives,
        sizeof(ElectricResistanceRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        ElectricConductanceRepresentatives,
        sizeof(ElectricConductanceRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        MagneticFluxRepresentatives,
        sizeof(MagneticFluxRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        MagneticFieldRepresentatives,
        sizeof(MagneticFieldRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        InductanceRepresentatives,
        sizeof(InductanceRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        CatalyticActivityRepresentatives,
        sizeof(CatalyticActivityRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        SurfaceRepresentatives,
        sizeof(SurfaceRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
    Dimension(
        VolumeRepresentatives,
        sizeof(VolumeRepresentatives)/sizeof(Representative),
        &EmptyPrefix
        ),
  };
  static constexpr const Unit::Dimension * DimensionTableUpperBound =
    DimensionTable + sizeof(DimensionTable)/sizeof(Dimension);
  static bool CanParse(const char * symbol, size_t length,
      const Dimension * * dimension, const Representative * * representative, const Prefix * * prefix);

  const Dimension * dimension() const { return static_cast<const UnitNode *>(node())->dimension(); }

  Unit(const UnitNode * node) : Expression(node) {}
  static Unit Builder(const Dimension * dimension, const Representative * representative, const Prefix * prefix);

  // Simplification
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
