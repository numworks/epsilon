#include "unit_representatives.h"

#include "poincare_junior/src/expression/unit.h"

namespace PoincareJ {

namespace Representatives {

using enum UnitRepresentative::Prefixable;

const Time Time::second = {"s", 1._e, All, NegativeLongScale};
const Time Time::minute = {"min", 60._e, None, None};
const Time Time::hour = {"h", 3600._e, None, None};
const Time Time::day = {"day", 86400._e, None, None};
const Time Time::week = {"week", 604800._e, None, None};
const Time Time::month = {"month", 2629800._e, None, None};
const Time Time::year = {"year", 31557600._e, None, None};

const Distance Distance::meter = {"m", 1._e, All, NegativeAndKilo};
const Distance Distance::astronomicalUnit = {"au", 149597870700._e, None, None};
const Distance Distance::lightYear = {"ly", KMult(299792458._e, 31557600._e),
                                      None, None};
const Distance Distance::parsec = {
    "pc", KMult(180._e, KDiv(3600._e, π_e), 149587870700._e), None,
    None

    const Distance Distance::inch = {"in", 0.0254_e, None, None};
const Distance Distance::foot = {"ft", KMult(12._e, 0.0254_e), None, None};
const Distance Distance::yard = {"yd", KMult(36._e, 0.0254_e), None, None};
const Distance Distance::mile = {"mi", KMult(63360._e, 0.0254_e), None, None};

/* Only AngleRepresentative have non-float ratio expression because exact
 * result are expected. */
const Angle Angle::radian = {"rad", 1_e, None, None};
const Angle Angle::arcSecond = {"\"", KDiv(π_e, 648000_e), None, None};
const Angle Angle::arcMinute = {"'", KDiv(π_e, 10800_e), None, None};
const Angle Angle::degree = {"°", KDiv(π_e, 180_e), None, None};
const Angle Angle::gradian = {"gon", KDiv(π_e, 200_e), None, None};

const Mass Mass::gram = {"g", 1._e, All, NegativeAndKilo};
const Mass Mass::ton = {"t", 1000._e, PositiveLongScale, PositiveLongScale};
const Mass Mass::dalton = {"Da", KDiv(KPow(10._e, -26._e), 6.02214076_e), All,
                           All};
const Mass Mass::once = {"oz", 0.028349523125_e, None, None};
const Mass Mass::pound = {"lb", 16._e, 0.028349523125_e, None, None};
const Mass Mass::shortTon = {"shtn", KMult(2000._e, 16._e, 0.028349523125_e),
                             None, None};
const Mass Mass::longTon = {"lgtn", KMult(2240._e, 16._e, 0.028349523125_e),
                            None, None};

const Current Current::ampere = {"A", 1._e, All,
                          LongScale)};

// Ratios are 1.0 because temperatures conversion are an exception.
const Temperature Temperature::kelvin = {"K", 1._e, All, None};
const Temperature Temperature::celsius = {"°C", 1._e, None, None};
const Temperature Temperature::farenheit = {"°F", 1._e, None, None};

const AmountOfSubstance AmountOfSubstance::mole = {
  "mol", 1._e, All, LongScale)};

const LuminousIntensity LuminousIntensity::candela = {
  "cd", 1._e, All, LongScale)};

const Frequency Frequency::hertz = {"Hz", 1._e, All,
  LongScale)};

const Force Force::newton = {"N", 1._e, All,
  LongScale)};

const Pressure Pressure::pascal = {"Pa", 1._e, All, LongScale};
const Pressure Pressure::bar = {"bar", 100000._e, All, LongScale};
const Pressure Pressure::atmosphere = {"atm", 101325._e, None, None};

const Energy Energy::joule = {"J", 1._e, All, LongScale};
const Energy Energy::electronVolt = {
    "eV", KMult(1.602176634_e, KPow(10._e, -19_e)), All, LongScale};

const Power Power::watt = {"W", 1._e, All, LongScale};
const Power Power::horsePower = {"hp", 745.699872_e, None,
  None)};

const ElectricCharge ElectricCharge::coulomb = {
  "C", 1._e, All, LongScale)};

const ElectricPotential ElectricPotential::volt = {
  "V", 1._e, All, LongScale)};

const ElectricCapacitance ElectricCapacitance::farad = {
  "F", 1._e, All, LongScale)};

const ElectricResistance ElectricResistance::ohm = {
  "Ω", 1._e, All, LongScale)};

const ElectricConductance ElectricConductance::siemens = {
  "S", 1._e, All, LongScale)};

const MagneticFlux MagneticFlux::weber = {
  "Wb", 1._e, All, LongScale)};

const MagneticField MagneticField::tesla = {
  "T", 1._e, All, LongScale)};

const Inductance Inductance::henry = {"H", 1._e, All,
  LongScale)};

const CatalyticActivity CatalyticActivity::katal = {
  "kat", 1._e, All, LongScale)};

const Surface Surface::hectare = {"ha", 10000._e, None, None};
const Surface Surface::acre = {"acre", 4046.8564224_e, None, None};

const Volume Volume::liter = {BuiltinsAliases::k_litersAliases, 0.001_e, All,
                              Negative};
const Volume Volume::teaSpoon = {"tsp", 0.00000492892159375_e, None, None};
const Volume Volume::tableSpoon = {"tbsp", KMult(3._e, 0.00000492892159375_e),
                                   None, None};
const Volume Volume::fluidOnce = {"floz", 0.0000295735295625_e, None, None};
const Volume Volume::cup = {"cup", KMult(8._e, 0.0000295735295625_e), None,
                            None};
const Volume Volume::pint = {"pt", KMult(16._e, 0.0000295735295625_e), None,
                             None};
const Volume Volume::quart = {"qt", KMult(32._e, 0.0000295735295625_e), None,
                              None};
const Volume Volume::gallon = {"gal", KMult(128._e, 0.0000295735295625_e), None,
                               None};

/* Define access points to some prefixes and representatives. */
constexpr static int k_emptyPrefixIndex = 6;
static_assert(k_prefixes[k_emptyPrefixIndex].m_exponent == 0,
              "Index for the Empty UnitPrefix is incorrect.");
constexpr static int k_kiloPrefixIndex = 9;
static_assert(k_prefixes[k_kiloPrefixIndex].m_exponent == 3,
              "Index for the Kilo UnitPrefix is incorrect.");

#if 0
int Time::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  /* Use all representatives but week */
  const Unit splitUnits[] = {
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_secondRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_minuteRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_hourRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_dayRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_monthRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_yearRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, numberOfRepresentatives() - 1,
                             reductionContext);
  return 1;
}

const UnitRepresentative* Distance::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  return (reductionContext.unitFormat() == Preferences::UnitFormat::Metric)
             ?
             /* Exclude imperial units from the search. */
             defaultFindBestRepresentative(
                 value, exponent, representativesOfSameDimension(),
                 Unit::k_inchRepresentativeIndex, prefix)
             :
             /* Exclude m form the search. */
             defaultFindBestRepresentative(
                 value, exponent, representativesOfSameDimension() + 1,
                 numberOfRepresentatives() - 1, prefix);
}

int Distance::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    return 0;
  }
  const Unit splitUnits[] = {
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_inchRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_footRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_yardRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_mileRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                             reductionContext);
  return 1;
}

#endif

const UnitRepresentative* Angle::DefaultRepresentativeForAngleUnit(
    AngleUnit angleUnit) {
  switch (angleUnit) {
    case AngleUnit::Degree:
      return Unit::k_angleRepresentatives + Unit::k_degreeRepresentativeIndex;
    case AngleUnit::Radian:
      return Unit::k_angleRepresentatives + Unit::k_radianRepresentativeIndex;
    default:
      assert(angleUnit == AngleUnit::Gradian);
      return Unit::k_angleRepresentatives + Unit::k_gradianRepresentativeIndex;
  }
}

#if 0
const UnitRepresentative* Angle::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  if (reductionContext.angleUnit() == AngleUnit::Degree) {
    return defaultFindBestRepresentative(
        value, exponent,
        representativesOfSameDimension() + Unit::k_arcSecondRepresentativeIndex,
        3, prefix);
  }
  return DefaultRepresentativeForAngleUnit(reductionContext.angleUnit());
}

Expression Angle::convertInto(
    Expression value, const UnitRepresentative* other,
    const ReductionContext& reductionContext) const {
  assert(dimensionVector() == other->dimensionVector());
  Expression unit = Unit::Builder(other, UnitPrefix::EmptyPrefix());
  Expression inRadians =
      Multiplication::Builder(value, ratioExpressionReduced(reductionContext))
          .shallowReduce(reductionContext);
  Expression inOther =
      Division::Builder(inRadians,
                        other->ratioExpressionReduced(reductionContext))
          .shallowReduce(reductionContext)
          .deepBeautify(reductionContext);
  return Multiplication::Builder(inOther, unit);
}

int Angle::setAdditionalExpressionsWithExactValue(
    Expression exactValue, double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  int numberOfResults = 0;
  // Conversion to degrees should be added to all units not degree related
  if (this == representativesOfSameDimension() +
                  Unit::k_radianRepresentativeIndex ||
      this == representativesOfSameDimension() +
                  Unit::k_gradianRepresentativeIndex) {
    const UnitRepresentative* degree =
        representativesOfSameDimension() + Unit::k_degreeRepresentativeIndex;
    dest[numberOfResults++] =
        convertInto(exactValue.clone(), degree, reductionContext)
            .approximateKeepingUnits<double>(reductionContext);
  }
  // Degrees related units should show their decomposition in DMS
  const Unit splitUnits[] = {
      Unit::Builder(representativesOfSameDimension() +
                        Unit::k_arcSecondRepresentativeIndex,
                    UnitPrefix::EmptyPrefix()),
      Unit::Builder(representativesOfSameDimension() +
                        Unit::k_arcMinuteRepresentativeIndex,
                    UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_degreeRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
  };
  Expression split = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                                      reductionContext);
  if (!split.isUndefined()) {
    dest[numberOfResults++] = split;
  }
  // Conversion to radians should be added to all other units.
  if (this !=
      representativesOfSameDimension() + Unit::k_radianRepresentativeIndex) {
    const UnitRepresentative* radian =
        representativesOfSameDimension() + Unit::k_radianRepresentativeIndex;
    dest[numberOfResults++] = convertInto(exactValue, radian, reductionContext);
  }
  return numberOfResults;
}
#endif

const UnitPrefix* Mass::basePrefix() const {
  return isBaseUnit() ? UnitPrefix::Prefixes() + Unit::k_kiloPrefixIndex
                      : UnitPrefix::EmptyPrefix();
}

#if 0
const UnitRepresentative* Mass::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Imperial) {
    return defaultFindBestRepresentative(
        value, exponent,
        representativesOfSameDimension() + Unit::k_ounceRepresentativeIndex,
        Unit::k_shortTonRepresentativeIndex - Unit::k_ounceRepresentativeIndex +
            1,
        prefix);
  }
  assert(reductionContext.unitFormat() == Preferences::UnitFormat::Metric);
  bool useTon = exponent == 1. && value >= (representativesOfSameDimension() +
                                            Unit::k_tonRepresentativeIndex)
                                               ->ratio();
  int representativeIndex =
      useTon ? Unit::k_tonRepresentativeIndex : Unit::k_gramRepresentativeIndex;
  return defaultFindBestRepresentative(
      value, exponent, representativesOfSameDimension() + representativeIndex,
      1, prefix);
}

int Mass::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    return 0;
  }
  const Unit splitUnits[] = {
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_ounceRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_poundRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(representativesOfSameDimension() +
                        Unit::k_shortTonRepresentativeIndex,
                    UnitPrefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                             reductionContext);
  return 1;
}

double Temperature::ConvertTemperatures(
    double value, const UnitRepresentative* source,
    const UnitRepresentative* target) {
  assert(source->dimensionVector() ==
         Temperature::Default().dimensionVector());
  assert(target->dimensionVector() ==
         Temperature::Default().dimensionVector());
  if (source == target) {
    return value;
  }
  constexpr double origin[] = {0, k_celsiusOrigin, k_fahrenheitOrigin};
  assert(sizeof(origin) == source->numberOfRepresentatives() * sizeof(double));
  double sourceOrigin =
      origin[source - source->representativesOfSameDimension()];
  double targetOrigin =
      origin[target - target->representativesOfSameDimension()];
  /* (T + origin) * ration converts T to Kelvin.
   * T/ratio - origin converts T from Kelvin. */
  return (value + sourceOrigin) * source->ratio() / target->ratio() -
         targetOrigin;
}

int Temperature::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  const UnitRepresentative* celsius =
      Temperature::Default().representativesOfSameDimension() +
      Unit::k_celsiusRepresentativeIndex;
  const UnitRepresentative* fahrenheit =
      Temperature::Default().representativesOfSameDimension() +
      Unit::k_fahrenheitRepresentativeIndex;
  const UnitRepresentative* kelvin =
      Temperature::Default().representativesOfSameDimension() +
      Unit::k_kelvinRepresentativeIndex;
  const UnitRepresentative* targets[] = {
      reductionContext.unitFormat() == Preferences::UnitFormat::Metric
          ? celsius
          : fahrenheit,
      reductionContext.unitFormat() == Preferences::UnitFormat::Metric
          ? fahrenheit
          : celsius,
      kelvin};
  int numberOfExpressionsSet = 0;
  constexpr int numberOfTargets = std::size(targets);
  for (int i = 0; i < numberOfTargets; i++) {
    if (targets[i] == this) {
      continue;
    }
    dest[numberOfExpressionsSet++] = Multiplication::Builder(
        Float<double>::Builder(Temperature::ConvertTemperatures(
            value, this, targets[i])),
        Unit::Builder(targets[i], UnitPrefix::EmptyPrefix()));
  }
  assert(numberOfExpressionsSet == 2);
  return numberOfExpressionsSet;
}

int Energy::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  int index = 0;
  /* 1. Convert into Joules
   * As J is just a shorthand for _kg_m^2_s^-2, the value is used as is. */
  const UnitRepresentative* joule =
      representativesOfSameDimension() + Unit::k_jouleRepresentativeIndex;
  const UnitPrefix* joulePrefix = joule->findBestPrefix(value, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(value * std::pow(10., -joulePrefix->exponent())),
      Unit::Builder(joule, joulePrefix));
  /* 2. Convert into Wh
   * As value is expressed in SI units (ie _kg_m^2_s^-2), the ratio is that of
   * hours to seconds. */
  const UnitRepresentative* hour =
      Time::Default().representativesOfSameDimension() +
      Unit::k_hourRepresentativeIndex;
  const UnitRepresentative* watt =
      Power::Default().representativesOfSameDimension() +
      Unit::k_wattRepresentativeIndex;
  double adjustedValue = value / hour->ratio() / watt->ratio();
  const UnitPrefix* wattPrefix = watt->findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             std::pow(10., -wattPrefix->exponent())),
      Multiplication::Builder(Unit::Builder(watt, wattPrefix),
                              Unit::Builder(hour, UnitPrefix::EmptyPrefix())));
  /* 3. Convert into eV */
  const UnitRepresentative* eV = representativesOfSameDimension() +
                             Unit::k_electronVoltRepresentativeIndex;
  adjustedValue = value / eV->ratio();
  const UnitPrefix* eVPrefix = eV->findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             std::pow(10., -eVPrefix->exponent())),
      Unit::Builder(eV, eVPrefix));
  return index;
}

const UnitRepresentative* Surface::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  *prefix = UnitPrefix::EmptyPrefix();
  return representativesOfSameDimension() +
         (reductionContext.unitFormat() == Preferences::UnitFormat::Metric
              ? Unit::k_hectareRepresentativeIndex
              : Unit::k_acreRepresentativeIndex);
}

int Surface::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  Expression* destMetric;
  Expression* destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to hectares
  const UnitRepresentative* hectare =
      representativesOfSameDimension() + Unit::k_hectareRepresentativeIndex;
  *destMetric =
      Multiplication::Builder(Float<double>::Builder(value / hectare->ratio()),
                              Unit::Builder(hectare, UnitPrefix::EmptyPrefix()));
  // 2. Convert to acres
  if (!destImperial) {
    return 1;
  }
  const UnitRepresentative* acre =
      representativesOfSameDimension() + Unit::k_acreRepresentativeIndex;
  *destImperial =
      Multiplication::Builder(Float<double>::Builder(value / acre->ratio()),
                              Unit::Builder(acre, UnitPrefix::EmptyPrefix()));
  return 2;
}

const UnitRepresentative* Volume::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    *prefix = representativesOfSameDimension()->findBestPrefix(value, exponent);
    return representativesOfSameDimension();
  }
  return defaultFindBestRepresentative(value, exponent,
                                       representativesOfSameDimension() + 1,
                                       numberOfRepresentatives() - 1, prefix);
}

int Volume::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  Expression* destMetric;
  Expression* destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to liters
  const UnitRepresentative* liter =
      representativesOfSameDimension() + Unit::k_literRepresentativeIndex;
  double adjustedValue = value / liter->ratio();
  const UnitPrefix* literPrefix = liter->findBestPrefix(adjustedValue, 1.);
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             pow(10., -literPrefix->exponent())),
      Unit::Builder(liter, literPrefix));
  // 2. Convert to imperial volumes
  if (!destImperial) {
    return 1;
  }
  const Unit splitUnits[] = {
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_cupRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_pintRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_quartRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_gallonRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
  };
  *destImperial = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                                   reductionContext);
  return 2;
}

int Speed::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  Expression* destMetric;
  Expression* destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to km/h
  const UnitRepresentative* meter =
      Distance::Default().representativesOfSameDimension() +
      Unit::k_meterRepresentativeIndex;
  const UnitRepresentative* hour =
      Time::Default().representativesOfSameDimension() +
      Unit::k_hourRepresentativeIndex;
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(value / 1000. * hour->ratio()),
      Multiplication::Builder(
          Unit::Builder(meter, UnitPrefix::Prefixes() + Unit::k_kiloPrefixIndex),
          Power::Builder(Unit::Builder(hour, UnitPrefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  // 2. Convert to mph
  if (!destImperial) {
    return 1;
  }
  const UnitRepresentative* mile =
      Distance::Default().representativesOfSameDimension() +
      Unit::k_mileRepresentativeIndex;
  *destImperial = Multiplication::Builder(
      Float<double>::Builder(value / mile->ratio() * hour->ratio()),
      Multiplication::Builder(
          Unit::Builder(mile, UnitPrefix::EmptyPrefix()),
          Power::Builder(Unit::Builder(hour, UnitPrefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  return 2;
}

}
}  // namespace PoincareJ
