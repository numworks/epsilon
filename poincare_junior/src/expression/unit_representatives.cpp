#include "unit_representatives.h"

#include "poincare_junior/src/expression/unit.h"

namespace PoincareJ {
namespace Units {

using enum Representative::Prefixable;

const Time::Representatives<const Time> Time::representatives = {
    .second = {"s", 1._de, All, NegativeLongScale},
    .minute = {"min", 60._de, None, None},
    .hour = {"h", 3600._de, None, None},
    .day = {"day", 86400._de, None, None},
    .week = {"week", 604800._de, None, None},
    .month = {"month", 2629800._de, None, None},
    .year = {"year", 31557600._de, None, None}};

const Distance::Representatives<const Distance> Distance::representatives = {
    .meter = {"m", 1._de, All, NegativeAndKilo},
    .astronomicalUnit = {"au", 149597870700._de, None, None},
    .lightYear = {"ly", KMult(299792458._de, 31557600._de), None, None},
    .parsec = {"pc", KMult(180._de, KDiv(3600._de, π_e), 149587870700._de),
               None, None},
    .inch = {"in", 0.0254_de, None, None},
    .foot = {"ft", KMult(12_e, 0.0254_de), None, None},
    .yard = {"yd", KMult(36_e, 0.0254_de), None, None},
    .mile = {"mi", KMult(63360_e, 0.0254_de), None, None}};

/* Only AngleRepresentative have non-float ratio expression because exact
 * result are expected. */
const Angle::Representatives<const Angle> Angle::representatives = {
    .radian = {"rad", 1_e, None, None},
    .arcSecond = {"\"", KDiv(π_e, 648000_e), None, None},
    .arcMinute = {"'", KDiv(π_e, 10800_e), None, None},
    .degree = {"°", KDiv(π_e, 180_e), None, None},
    .gradian = {"gon", KDiv(π_e, 200_e), None, None}};

const Mass::Representatives<const Mass> Mass::representatives = {
    // kg is a dedicated non-prefixable unit to be used in SI
    .kilogram = {"kg", 1_e, None, None},
    .gram = {"g", 0.001_de, All, Negative},
    .ton = {"t", 1000._de, PositiveLongScale, PositiveLongScale},
    .ounce = {"oz", 0.028349523125_de, None, None},
    .pound = {"lb", KMult(16_e, 0.028349523125_de), None, None},
    .shortTon = {"shtn", KMult(2000_e, 16._de, 0.028349523125_de), None, None},
    .longTon = {"lgtn", KMult(2240_e, 16._de, 0.028349523125_de), None, None},
    .dalton = {"Da", KDiv(KPow(10_e, -26_e), 6.02214076_de), All, All}};

const Current::Representatives<const Current> Current::representatives = {
    .ampere = {"A", 1._de, All, LongScale}};

// Ratios are 1.0 because temperatures conversion are an exception.
const Temperature::Representatives<const Temperature>
    Temperature::representatives = {.kelvin = {"K", 1._de, All, None},
                                    .celsius = {"°C", 1._de, None, None},
                                    .fahrenheit = {"°F", 1._de, None, None}};

const AmountOfSubstance::Representatives<const AmountOfSubstance>
    AmountOfSubstance::representatives = {
        .mole = {"mol", 1._de, All, LongScale}};

const LuminousIntensity::Representatives<const LuminousIntensity>
    LuminousIntensity::representatives = {
        .candela = {"cd", 1._de, All, LongScale}};

const Frequency::Representatives<const Frequency> Frequency::representatives = {
    .hertz = {"Hz", 1._de, All, LongScale}};

const Force::Representatives<const Force> Force::representatives = {
    .newton = {"N", 1._de, All, LongScale}};

const Pressure::Representatives<const Pressure> Pressure::representatives = {
    .pascal = {"Pa", 1._de, All, LongScale},
    .bar = {"bar", 100000._de, All, LongScale},
    .atmosphere = {"atm", 101325._de, None, None}};

const Energy::Representatives<const Energy> Energy::representatives = {
    .joule = {"J", 1._de, All, LongScale},
    .electronVolt = {"eV", KMult(1.602176634_de, KPow(10_e, -19_e)), All,
                     LongScale}};

const Power::Representatives<const Power> Power::representatives = {
    .watt = {"W", 1._de, All, LongScale},
    .horsePower = {"hp", 745.699872_de, None, None}};

const ElectricCharge::Representatives<const ElectricCharge>
    ElectricCharge::representatives = {.coulomb = {"C", 1._de, All, LongScale}};

const ElectricPotential::Representatives<const ElectricPotential>
    ElectricPotential::representatives = {.volt = {"V", 1._de, All, LongScale}};

const ElectricCapacitance::Representatives<const ElectricCapacitance>
    ElectricCapacitance::representatives = {
        .farad = {"F", 1._de, All, LongScale}};

const ElectricResistance::Representatives<const ElectricResistance>
    ElectricResistance::representatives = {.ohm = {"Ω", 1._de, All, LongScale}};

const ElectricConductance::Representatives<const ElectricConductance>
    ElectricConductance::representatives = {
        .siemens = {"S", 1._de, All, LongScale}};

const MagneticFlux::Representatives<const MagneticFlux>
    MagneticFlux::representatives = {.weber = {"Wb", 1._de, All, LongScale}};

const MagneticField::Representatives<const MagneticField>
    MagneticField::representatives = {.tesla = {"T", 1._de, All, LongScale}};

const Inductance::Representatives<const Inductance>
    Inductance::representatives = {.henry = {"H", 1._de, All, LongScale}};

const CatalyticActivity::Representatives<const CatalyticActivity>
    CatalyticActivity::representatives = {
        .katal = {"kat", 1._de, All, LongScale}};

const Surface::Representatives<const Surface> Surface::representatives = {
    .hectare = {"ha", 10000._de, None, None},
    .acre = {"acre", 4046.8564224_de, None, None}};

const Volume::Representatives<const Volume> Volume::representatives = {
    .liter = {BuiltinsAliases::k_litersAliases, 0.001_de, All, Negative},
    .cup = {"cup", KMult(8_e, 0.0000295735295625_de), None, None},
    .pint = {"pt", KMult(16_e, 0.0000295735295625_de), None, None},
    .quart = {"qt", KMult(32_e, 0.0000295735295625_de), None, None},
    .gallon = {"gal", KMult(128._de, 0.0000295735295625_de), None, None},
    .teaSpoon = {"tsp", 0.00000492892159375_de, None, None},
    .tableSpoon = {"tbsp", KMult(3._de, 0.00000492892159375_de), None, None},
    .fluidOnce = {"floz", 0.0000295735295625_de, None, None}};

const Speed::Representatives<const Speed> Speed::representatives = {
    .none = {nullptr, 1_e, None, None}};

#if 0
int Time::setAdditionalExpressions(double value, Expression* dest,
                                   int availableLength,
                                   UnitFormat unitFormat) const {
  assert(availableLength >= 1);
  /* Use all representatives but week */
  const Unit splitUnits[] = {
      Unit::Builder(&second, Prefix::EmptyPrefix()),
      Unit::Builder(&minute, Prefix::EmptyPrefix()),
      Unit::Builder(&hour, Prefix::EmptyPrefix()),
      Unit::Builder(&day, Prefix::EmptyPrefix()),
      Unit::Builder(&month, Prefix::EmptyPrefix()),
      Unit::Builder(&year, Prefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, numberOfRepresentatives() - 1,
                             reductionContext);
  return 1;
}
#endif

const Representative* Distance::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const Prefix** prefix) const {
  return unitFormat == UnitFormat::Metric
             ?
             /* Exclude imperial units from the search. */
             defaultFindBestRepresentative(value, exponent,
                                           &representatives.meter,
                                           &representatives.inch, prefix)
             :
             /* Exclude meters from the search. */
             defaultFindBestRepresentative(value, exponent,
                                           &representatives.meter + 1,
                                           representatives.end(), prefix);
}

#if 0
int Distance::setAdditionalExpressions(double value, Expression* dest,
                                       int availableLength,
                                       UnitFormat unitFormat) const {
  assert(availableLength >= 1);
  if (unitFormat == UnitFormat::Metric) {
    return 0;
  }
  const Unit splitUnits[] = {
      Unit::Builder(&inch, Prefix::EmptyPrefix()),
      Unit::Builder(&foot, Prefix::EmptyPrefix()),
      Unit::Builder(&yard, Prefix::EmptyPrefix()),
      Unit::Builder(&mile, Prefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                             reductionContext);
  return 1;
}

const Representative* Angle::DefaultRepresentativeForAngleUnit(
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

const Representative* Angle::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const Prefix** prefix) const {
  if (reductionContext.angleUnit() == AngleUnit::Degree) {
    return defaultFindBestRepresentative(value, exponent, &arcSecond, 3,
                                         prefix);
  }
  return DefaultRepresentativeForAngleUnit(reductionContext.angleUnit());
}

Expression Angle::convertInto(Expression value, const Representative* other,
                              UnitFormat unitFormat) const {
  assert(dimensionVector() == other->dimensionVector());
  Expression unit = Unit::Builder(other, Prefix::EmptyPrefix());
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

int Angle::setAdditionalExpressionsWithExactValue(Expression exactValue,
                                                  double value,
                                                  Expression* dest,
                                                  int availableLength,
                                                  UnitFormat unitFormat) const {
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
      Unit::Builder(&arcSecond, Prefix::EmptyPrefix()),
      Unit::Builder(&arcMinute, Prefix::EmptyPrefix()),
      Unit::Builder(&degree, Prefix::EmptyPrefix()),
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

const Representative* Mass::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const Prefix** prefix) const {
  if (unitFormat == UnitFormat::Imperial) {
    // With shortTon but not longTon
    return defaultFindBestRepresentative(value, exponent,
                                         &representatives.ounce,
                                         &representatives.longTon, prefix);
  }
  assert(unitFormat == UnitFormat::Metric);
  if (exponent == 1. && value >= representatives.ton.ratio()) {
    return defaultFindBestRepresentative(value, exponent, &representatives.ton,
                                         &representatives.ton + 1, prefix);
  }
  return defaultFindBestRepresentative(
      value, exponent, &representatives.kilogram, &representatives.ton, prefix);
}

#if 0
int Mass::setAdditionalExpressions(double value, Expression* dest,
                                   int availableLength,
                                   UnitFormat unitFormat) const {
  assert(availableLength >= 1);
  if (unitFormat == UnitFormat::Metric) {
    return 0;
  }
  const Unit splitUnits[] = {
      Unit::Builder(&ounce, Prefix::EmptyPrefix()),
      Unit::Builder(&pound, Prefix::EmptyPrefix()),
      Unit::Builder(&shortTon, Prefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                             reductionContext);
  return 1;
}

double Temperature::ConvertTemperatures(double value,
                                        const Representative* source,
                                        const Representative* target) {
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

int Temperature::setAdditionalExpressions(double value, Expression* dest,
                                          int availableLength,
                                          UnitFormat unitFormat) const {
  assert(availableLength >= 2);
  const Representative* targets[] = {
      unitFormat == UnitFormat::Metric ? &celsius : &fahrenheit,
      unitFormat == UnitFormat::Metric ? &fahrenheit : &celsius, kelvin};
  int numberOfExpressionsSet = 0;
  constexpr int numberOfTargets = std::size(targets);
  for (int i = 0; i < numberOfTargets; i++) {
    if (targets[i] == this) {
      continue;
    }
    dest[numberOfExpressionsSet++] = Multiplication::Builder(
        Float<double>::Builder(
            Temperature::ConvertTemperatures(value, this, targets[i])),
        Unit::Builder(targets[i], Prefix::EmptyPrefix()));
  }
  assert(numberOfExpressionsSet == 2);
  return numberOfExpressionsSet;
}

int Energy::setAdditionalExpressions(double value, Expression* dest,
                                     int availableLength,
                                     UnitFormat unitFormat) const {
  assert(availableLength >= 2);
  int index = 0;
  /* 1. Convert into Joules
   * As J is just a shorthand for _kg_m^2_s^-2, the value is used as is. */
  const Prefix* joulePrefix = joule->findBestPrefix(value, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(value * std::pow(10., -joulePrefix->exponent())),
      Unit::Builder(&joule, joulePrefix));
  /* 2. Convert into Wh
   * As value is expressed in SI units (ie _kg_m^2_s^-2), the ratio is that of
   * hours to seconds. */
  double adjustedValue = value / hour.ratio() / watt.ratio();
  const Prefix* wattPrefix = watt->findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             std::pow(10., -wattPrefix->exponent())),
      Multiplication::Builder(Unit::Builder(&watt, wattPrefix),
                              Unit::Builder(&hour, Prefix::EmptyPrefix())));
  /* 3. Convert into eV */
  adjustedValue = value / electronVolt.ratio();
  const Prefix* eVPrefix = electronVolt.findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             std::pow(10., -eVPrefix->exponent())),
      Unit::Builder(&electronVolt, eVPrefix));
  return index;
}
#endif

const Representative* Surface::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const Prefix** prefix) const {
  *prefix = Prefix::EmptyPrefix();
  return unitFormat == UnitFormat::Metric ? &representatives.hectare
                                          : &representatives.acre;
}

#if 0
int Surface::setAdditionalExpressions(double value, Expression* dest,
                                      int availableLength,
                                      UnitFormat unitFormat) const {
  assert(availableLength >= 2);
  Expression* destMetric;
  Expression* destImperial = nullptr;
  if (unitFormat == UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to hectares
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(value / hectare.ratio()),
      Unit::Builder(&hectare, Prefix::EmptyPrefix()));
  // 2. Convert to acres
  if (!destImperial) {
    return 1;
  }
  *destImperial =
      Multiplication::Builder(Float<double>::Builder(value / acre.ratio()),
                              Unit::Builder(&acre, Prefix::EmptyPrefix()));
  return 2;
}
#endif

const Representative* Volume::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const Prefix** prefix) const {
  if (unitFormat == UnitFormat::Metric) {
    *prefix = representativesOfSameDimension()->findBestPrefix(value, exponent);
    return &representatives.liter;
  }
  return defaultFindBestRepresentative(value, exponent, &representatives.cup,
                                       representatives.end(), prefix);
}

#if 0
int Volume::setAdditionalExpressions(double value, Expression* dest,
                                     int availableLength,
                                     UnitFormat unitFormat) const {
  assert(availableLength >= 2);
  Expression* destMetric;
  Expression* destImperial = nullptr;
  if (unitFormat == UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to liters
  double adjustedValue = value / liter.ratio();
  const Prefix* literPrefix = liter.findBestPrefix(adjustedValue, 1.);
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             pow(10., -literPrefix->exponent())),
      Unit::Builder(&liter, literPrefix));
  // 2. Convert to imperial volumes
  if (!destImperial) {
    return 1;
  }
  const Unit splitUnits[] = {
      Unit::Builder(&cup, Prefix::EmptyPrefix()),
      Unit::Builder(&pint, Prefix::EmptyPrefix()),
      Unit::Builder(&quart, Prefix::EmptyPrefix()),
      Unit::Builder(&gallon, Prefix::EmptyPrefix()),
  };
  *destImperial = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                                   reductionContext);
  return 2;
}

int Speed::setAdditionalExpressions(double value, Expression* dest,
                                    int availableLength,
                                    UnitFormat unitFormat) const {
  assert(availableLength >= 2);
  Expression* destMetric;
  Expression* destImperial = nullptr;
  if (unitFormat == UnitFormat::Metric) {
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
                        Prefix::Prefixes() + Unit::k_kiloPrefixIndex),
          Power::Builder(Unit::Builder(&Time::hour, Prefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  // 2. Convert to mph
  if (!destImperial) {
    return 1;
  }
  *destImperial = Multiplication::Builder(
      Float<double>::Builder(value / Distance::mile.ratio() *
                             Time::hour.ratio()),
      Multiplication::Builder(
          Unit::Builder(&Distance::mile, Prefix::EmptyPrefix()),
          Power::Builder(Unit::Builder(hour, Prefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  return 2;
}
#endif

}  // namespace Units
}  // namespace PoincareJ
