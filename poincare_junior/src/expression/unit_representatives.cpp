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
    "pc", KMult(180._e, KDiv(3600._e, π_e), 149587870700._e), None, None};

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

const Mass Mass::kilogram = {"kg", 1_e, None, None};
const Mass Mass::gram = {"g", 0.001_e, All, Negative};
const Mass Mass::ton = {"t", 1000._e, PositiveLongScale, PositiveLongScale};
const Mass Mass::dalton = {"Da", KDiv(KPow(10._e, -26._e), 6.02214076_e), All,
                           All};
const Mass Mass::ounce = {"oz", 0.028349523125_e, None, None};
const Mass Mass::pound = {"lb", KMult(16._e, 0.028349523125_e), None, None};
const Mass Mass::shortTon = {"shtn", KMult(2000._e, 16._e, 0.028349523125_e),
                             None, None};
const Mass Mass::longTon = {"lgtn", KMult(2240._e, 16._e, 0.028349523125_e),
                            None, None};

const Current Current::ampere = {"A", 1._e, All, LongScale};

// Ratios are 1.0 because temperatures conversion are an exception.
const Temperature Temperature::kelvin = {"K", 1._e, All, None};
const Temperature Temperature::celsius = {"°C", 1._e, None, None};
const Temperature Temperature::fahrenheit = {"°F", 1._e, None, None};

const AmountOfSubstance AmountOfSubstance::mole = {"mol", 1._e, All, LongScale};

const LuminousIntensity LuminousIntensity::candela = {"cd", 1._e, All,
                                                      LongScale};

const Frequency Frequency::hertz = {"Hz", 1._e, All, LongScale};

const Force Force::newton = {"N", 1._e, All, LongScale};

const Pressure Pressure::pascal = {"Pa", 1._e, All, LongScale};
const Pressure Pressure::bar = {"bar", 100000._e, All, LongScale};
const Pressure Pressure::atmosphere = {"atm", 101325._e, None, None};

const Energy Energy::joule = {"J", 1._e, All, LongScale};
const Energy Energy::electronVolt = {
    "eV", KMult(1.602176634_e, KPow(10._e, -19_e)), All, LongScale};

const Power Power::watt = {"W", 1._e, All, LongScale};
const Power Power::horsePower = {"hp", 745.699872_e, None, None};

const ElectricCharge ElectricCharge::coulomb = {"C", 1._e, All, LongScale};

const ElectricPotential ElectricPotential::volt = {"V", 1._e, All, LongScale};

const ElectricCapacitance ElectricCapacitance::farad = {"F", 1._e, All,
                                                        LongScale};

const ElectricResistance ElectricResistance::ohm = {"Ω", 1._e, All, LongScale};

const ElectricConductance ElectricConductance::siemens = {"S", 1._e, All,
                                                          LongScale};

const MagneticFlux MagneticFlux::weber = {"Wb", 1._e, All, LongScale};

const MagneticField MagneticField::tesla = {"T", 1._e, All, LongScale};

const Inductance Inductance::henry = {"H", 1._e, All, LongScale};

const CatalyticActivity CatalyticActivity::katal = {"kat", 1._e, All,
                                                    LongScale};

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

const Speed Speed::none = {nullptr, 1_e, None, None};

#if 0
int Time::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  /* Use all representatives but week */
  const Unit splitUnits[] = {
      Unit::Builder(&second, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&minute, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&hour, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&day, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&month, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&year, UnitPrefix::EmptyPrefix()),
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
      Unit::Builder(&inch, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&foot, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&yard, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&mile, UnitPrefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                             reductionContext);
  return 1;
}

const UnitRepresentative* Angle::DefaultRepresentativeForAngleUnit(
    AngleUnit angleUnit) {
  switch (angleUnit) {
    case AngleUnit::Degree:
      return &Angle::degree;
    case AngleUnit::Radian:
      return &Angle::radian;
    default:
      assert(angleUnit == AngleUnit::Gradian);
      return &Angle::gradian;
  }
}

const UnitRepresentative* Angle::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  if (reductionContext.angleUnit() == AngleUnit::Degree) {
    return defaultFindBestRepresentative(value, exponent, &arcSecond, 3,
                                         prefix);
  }
  return DefaultRepresentativeForAngleUnit(reductionContext.angleUnit());
}

Expression Angle::convertInto(Expression value, const UnitRepresentative* other,
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
  if (this == &radian || this == &gradian) {
    dest[numberOfResults++] =
        convertInto(exactValue.clone(), &degree, reductionContext)
            .approximateKeepingUnits<double>(reductionContext);
  }
  // Degrees related units should show their decomposition in DMS
  const Unit splitUnits[] = {
      Unit::Builder(&arcSecond, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&arcMinute, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&degree, UnitPrefix::EmptyPrefix()),
  };
  Expression split = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                                      reductionContext);
  if (!split.isUndefined()) {
    dest[numberOfResults++] = split;
  }
  // Conversion to radians should be added to all other units.
  if (this != &radian) {
    dest[numberOfResults++] =
        convertInto(exactValue, &radian, reductionContext);
  }
  return numberOfResults;
}
#endif

#if 0
const UnitRepresentative* Mass::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Imperial) {
    return defaultFindBestRepresentative(value, exponent, &ounce,
                                         Unit::k_shortTonRepresentativeIndex -
                                             Unit::k_ounceRepresentativeIndex +
                                             1,
                                         prefix);
  }
  assert(reductionContext.unitFormat() == Preferences::UnitFormat::Metric);
  bool useTon = exponent == 1. && value >= (&ton)->ratio();
  return defaultFindBestRepresentative(value, exponent, useTon ? &ton : &gram,
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
      Unit::Builder(&ounce, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&pound, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&shortTon, UnitPrefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                             reductionContext);
  return 1;
}

double Temperature::ConvertTemperatures(double value,
                                        const UnitRepresentative* source,
                                        const UnitRepresentative* target) {
  assert(source->dimensionVector() == Temperature::Default().dimensionVector());
  assert(target->dimensionVector() == Temperature::Default().dimensionVector());
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
  const UnitRepresentative* targets[] = {
      reductionContext.unitFormat() == Preferences::UnitFormat::Metric
          ? &celsius
          : &fahrenheit,
      reductionContext.unitFormat() == Preferences::UnitFormat::Metric
          ? &fahrenheit
          : &celsius,
      kelvin};
  int numberOfExpressionsSet = 0;
  constexpr int numberOfTargets = std::size(targets);
  for (int i = 0; i < numberOfTargets; i++) {
    if (targets[i] == this) {
      continue;
    }
    dest[numberOfExpressionsSet++] = Multiplication::Builder(
        Float<double>::Builder(
            Temperature::ConvertTemperatures(value, this, targets[i])),
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
  const UnitPrefix* joulePrefix = joule->findBestPrefix(value, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(value * std::pow(10., -joulePrefix->exponent())),
      Unit::Builder(&joule, joulePrefix));
  /* 2. Convert into Wh
   * As value is expressed in SI units (ie _kg_m^2_s^-2), the ratio is that of
   * hours to seconds. */
  double adjustedValue = value / hour.ratio() / watt.ratio();
  const UnitPrefix* wattPrefix = watt->findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             std::pow(10., -wattPrefix->exponent())),
      Multiplication::Builder(Unit::Builder(&watt, wattPrefix),
                              Unit::Builder(&hour, UnitPrefix::EmptyPrefix())));
  /* 3. Convert into eV */
  adjustedValue = value / electronVolt.ratio();
  const UnitPrefix* eVPrefix = electronVolt.findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             std::pow(10., -eVPrefix->exponent())),
      Unit::Builder(&electronVolt, eVPrefix));
  return index;
}

const UnitRepresentative* Surface::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  *prefix = UnitPrefix::EmptyPrefix();
  return reductionContext.unitFormat() == Preferences::UnitFormat::Metric
             ? &hectare
             : &acre
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
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(value / hectare.ratio()),
      Unit::Builder(&hectare, UnitPrefix::EmptyPrefix()));
  // 2. Convert to acres
  if (!destImperial) {
    return 1;
  }
  *destImperial =
      Multiplication::Builder(Float<double>::Builder(value / acre.ratio()),
                              Unit::Builder(&acre, UnitPrefix::EmptyPrefix()));
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
  double adjustedValue = value / liter.ratio();
  const UnitPrefix* literPrefix = liter.findBestPrefix(adjustedValue, 1.);
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             pow(10., -literPrefix->exponent())),
      Unit::Builder(&liter, literPrefix));
  // 2. Convert to imperial volumes
  if (!destImperial) {
    return 1;
  }
  const Unit splitUnits[] = {
      Unit::Builder(&cup, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&pint, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&quart, UnitPrefix::EmptyPrefix()),
      Unit::Builder(&gallon, UnitPrefix::EmptyPrefix()),
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
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(value / 1000. * Time::hour.ratio()),
      Multiplication::Builder(
          Unit::Builder(&Distance::meter,
                        UnitPrefix::Prefixes() + Unit::k_kiloPrefixIndex),
          Power::Builder(Unit::Builder(&Time::hour, UnitPrefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  // 2. Convert to mph
  if (!destImperial) {
    return 1;
  }
  *destImperial = Multiplication::Builder(
      Float<double>::Builder(value / Distance::mile.ratio() *
                             Time::hour.ratio()),
      Multiplication::Builder(
          Unit::Builder(&Distance::mile, UnitPrefix::EmptyPrefix()),
          Power::Builder(Unit::Builder(hour, UnitPrefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  return 2;
}
#endif
}  // namespace Representatives
}  // namespace PoincareJ
