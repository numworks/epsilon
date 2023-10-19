#include "unit_representatives.h"

#include "poincare_junior/src/expression/unit.h"

namespace PoincareJ {
namespace Units {

using enum UnitRepresentative::Prefixable;

const Time::Representatives<const Time> Time::representatives = {
    .second = {"s", 1._e, All, NegativeLongScale},
    .minute = {"min", 60._e, None, None},
    .hour = {"h", 3600._e, None, None},
    .day = {"day", 86400._e, None, None},
    .week = {"week", 604800._e, None, None},
    .month = {"month", 2629800._e, None, None},
    .year = {"year", 31557600._e, None, None}};

const Distance::Representatives<const Distance> Distance::representatives = {
    .meter = {"m", 1._e, All, NegativeAndKilo},
    .astronomicalUnit = {"au", 149597870700._e, None, None},
    .lightYear = {"ly", KMult(299792458._e, 31557600._e), None, None},
    .parsec = {"pc", KMult(180._e, KDiv(3600._e, π_e), 149587870700._e), None,
               None},
    .inch = {"in", 0.0254_e, None, None},
    .foot = {"ft", KMult(12._e, 0.0254_e), None, None},
    .yard = {"yd", KMult(36._e, 0.0254_e), None, None},
    .mile = {"mi", KMult(63360._e, 0.0254_e), None, None}};

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
    .gram = {"g", 0.001_e, All, Negative},
    .ton = {"t", 1000._e, PositiveLongScale, PositiveLongScale},
    .ounce = {"oz", 0.028349523125_e, None, None},
    .pound = {"lb", KMult(16._e, 0.028349523125_e), None, None},
    .shortTon = {"shtn", KMult(2000._e, 16._e, 0.028349523125_e), None, None},
    .longTon = {"lgtn", KMult(2240._e, 16._e, 0.028349523125_e), None, None},
    .dalton = {"Da", KDiv(KPow(10._e, -26._e), 6.02214076_e), All, All}};

const Current::Representatives<const Current> Current::representatives = {
    .ampere = {"A", 1._e, All, LongScale}};

// Ratios are 1.0 because temperatures conversion are an exception.
const Temperature::Representatives<const Temperature>
    Temperature::representatives = {.kelvin = {"K", 1._e, All, None},
                                    .celsius = {"°C", 1._e, None, None},
                                    .fahrenheit = {"°F", 1._e, None, None}};

const AmountOfSubstance::Representatives<const AmountOfSubstance>
    AmountOfSubstance::representatives = {
        .mole = {"mol", 1._e, All, LongScale}};

const LuminousIntensity::Representatives<const LuminousIntensity>
    LuminousIntensity::representatives = {
        .candela = {"cd", 1._e, All, LongScale}};

const Frequency::Representatives<const Frequency> Frequency::representatives = {
    .hertz = {"Hz", 1._e, All, LongScale}};

const Force::Representatives<const Force> Force::representatives = {
    .newton = {"N", 1._e, All, LongScale}};

const Pressure::Representatives<const Pressure> Pressure::representatives = {
    .pascal = {"Pa", 1._e, All, LongScale},
    .bar = {"bar", 100000._e, All, LongScale},
    .atmosphere = {"atm", 101325._e, None, None}};

const Energy::Representatives<const Energy> Energy::representatives = {
    .joule = {"J", 1._e, All, LongScale},
    .electronVolt = {"eV", KMult(1.602176634_e, KPow(10._e, -19_e)), All,
                     LongScale}};

const Power::Representatives<const Power> Power::representatives = {
    .watt = {"W", 1._e, All, LongScale},
    .horsePower = {"hp", 745.699872_e, None, None}};

const ElectricCharge::Representatives<const ElectricCharge>
    ElectricCharge::representatives = {.coulomb = {"C", 1._e, All, LongScale}};

const ElectricPotential::Representatives<const ElectricPotential>
    ElectricPotential::representatives = {.volt = {"V", 1._e, All, LongScale}};

const ElectricCapacitance::Representatives<const ElectricCapacitance>
    ElectricCapacitance::representatives = {
        .farad = {"F", 1._e, All, LongScale}};

const ElectricResistance::Representatives<const ElectricResistance>
    ElectricResistance::representatives = {.ohm = {"Ω", 1._e, All, LongScale}};

const ElectricConductance::Representatives<const ElectricConductance>
    ElectricConductance::representatives = {
        .siemens = {"S", 1._e, All, LongScale}};

const MagneticFlux::Representatives<const MagneticFlux>
    MagneticFlux::representatives = {.weber = {"Wb", 1._e, All, LongScale}};

const MagneticField::Representatives<const MagneticField>
    MagneticField::representatives = {.tesla = {"T", 1._e, All, LongScale}};

const Inductance::Representatives<const Inductance>
    Inductance::representatives = {.henry = {"H", 1._e, All, LongScale}};

const CatalyticActivity::Representatives<const CatalyticActivity>
    CatalyticActivity::representatives = {
        .katal = {"kat", 1._e, All, LongScale}};

const Surface::Representatives<const Surface> Surface::representatives = {
    .hectare = {"ha", 10000._e, None, None},
    .acre = {"acre", 4046.8564224_e, None, None}};

const Volume::Representatives<const Volume> Volume::representatives = {
    .liter = {BuiltinsAliases::k_litersAliases, 0.001_e, All, Negative},
    .cup = {"cup", KMult(8._e, 0.0000295735295625_e), None, None},
    .pint = {"pt", KMult(16._e, 0.0000295735295625_e), None, None},
    .quart = {"qt", KMult(32._e, 0.0000295735295625_e), None, None},
    .gallon = {"gal", KMult(128._e, 0.0000295735295625_e), None, None},
    .teaSpoon = {"tsp", 0.00000492892159375_e, None, None},
    .tableSpoon = {"tbsp", KMult(3._e, 0.00000492892159375_e), None, None},
    .fluidOnce = {"floz", 0.0000295735295625_e, None, None}};

const Speed::Representatives<const Speed> Speed::representatives = {
    .none = {nullptr, 1_e, None, None}};

#if 0
int Time::setAdditionalExpressions(double value, Expression* dest,
                                   int availableLength,
                                   UnitFormat unitFormat) const {
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
#endif

const UnitRepresentative* Distance::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const UnitPrefix** prefix) const {
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
    double value, double exponent, UnitFormat unitFormat,
    const UnitPrefix** prefix) const {
  if (reductionContext.angleUnit() == AngleUnit::Degree) {
    return defaultFindBestRepresentative(value, exponent, &arcSecond, 3,
                                         prefix);
  }
  return DefaultRepresentativeForAngleUnit(reductionContext.angleUnit());
}

Expression Angle::convertInto(Expression value, const UnitRepresentative* other,
                              UnitFormat unitFormat) const {
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

const UnitRepresentative* Mass::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const UnitPrefix** prefix) const {
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

int Temperature::setAdditionalExpressions(double value, Expression* dest,
                                          int availableLength,
                                          UnitFormat unitFormat) const {
  assert(availableLength >= 2);
  const UnitRepresentative* targets[] = {
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
        Unit::Builder(targets[i], UnitPrefix::EmptyPrefix()));
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
#endif

const UnitRepresentative* Surface::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const UnitPrefix** prefix) const {
  *prefix = UnitPrefix::EmptyPrefix();
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
#endif

const UnitRepresentative* Volume::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const UnitPrefix** prefix) const {
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

}  // namespace Units
}  // namespace PoincareJ
