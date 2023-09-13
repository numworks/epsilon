#include "unit_representatives.h"

namespace PoincareJ {

#if 0
int TimeRepresentative::setAdditionalExpressions(
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

const UnitRepresentative* DistanceRepresentative::standardRepresentative(
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

int DistanceRepresentative::setAdditionalExpressions(
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

const UnitRepresentative*
AngleRepresentative::DefaultRepresentativeForAngleUnit(AngleUnit angleUnit) {
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
const UnitRepresentative* AngleRepresentative::standardRepresentative(
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

Expression AngleRepresentative::convertInto(
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

int AngleRepresentative::setAdditionalExpressionsWithExactValue(
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

const UnitPrefix* MassRepresentative::basePrefix() const {
  return isBaseUnit() ? UnitPrefix::Prefixes() + Unit::k_kiloPrefixIndex
                      : UnitPrefix::EmptyPrefix();
}

#if 0
const UnitRepresentative* MassRepresentative::standardRepresentative(
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

int MassRepresentative::setAdditionalExpressions(
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

double TemperatureRepresentative::ConvertTemperatures(
    double value, const UnitRepresentative* source,
    const UnitRepresentative* target) {
  assert(source->dimensionVector() ==
         TemperatureRepresentative::Default().dimensionVector());
  assert(target->dimensionVector() ==
         TemperatureRepresentative::Default().dimensionVector());
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

int TemperatureRepresentative::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  const UnitRepresentative* celsius =
      TemperatureRepresentative::Default().representativesOfSameDimension() +
      Unit::k_celsiusRepresentativeIndex;
  const UnitRepresentative* fahrenheit =
      TemperatureRepresentative::Default().representativesOfSameDimension() +
      Unit::k_fahrenheitRepresentativeIndex;
  const UnitRepresentative* kelvin =
      TemperatureRepresentative::Default().representativesOfSameDimension() +
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
        Float<double>::Builder(TemperatureRepresentative::ConvertTemperatures(
            value, this, targets[i])),
        Unit::Builder(targets[i], UnitPrefix::EmptyPrefix()));
  }
  assert(numberOfExpressionsSet == 2);
  return numberOfExpressionsSet;
}

int EnergyRepresentative::setAdditionalExpressions(
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
      TimeRepresentative::Default().representativesOfSameDimension() +
      Unit::k_hourRepresentativeIndex;
  const UnitRepresentative* watt =
      PowerRepresentative::Default().representativesOfSameDimension() +
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

const UnitRepresentative* SurfaceRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  *prefix = UnitPrefix::EmptyPrefix();
  return representativesOfSameDimension() +
         (reductionContext.unitFormat() == Preferences::UnitFormat::Metric
              ? Unit::k_hectareRepresentativeIndex
              : Unit::k_acreRepresentativeIndex);
}

int SurfaceRepresentative::setAdditionalExpressions(
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

const UnitRepresentative* VolumeRepresentative::standardRepresentative(
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

int VolumeRepresentative::setAdditionalExpressions(
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

int SpeedRepresentative::setAdditionalExpressions(
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
      DistanceRepresentative::Default().representativesOfSameDimension() +
      Unit::k_meterRepresentativeIndex;
  const UnitRepresentative* hour =
      TimeRepresentative::Default().representativesOfSameDimension() +
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
      DistanceRepresentative::Default().representativesOfSameDimension() +
      Unit::k_mileRepresentativeIndex;
  *destImperial = Multiplication::Builder(
      Float<double>::Builder(value / mile->ratio() * hour->ratio()),
      Multiplication::Builder(
          Unit::Builder(mile, UnitPrefix::EmptyPrefix()),
          Power::Builder(Unit::Builder(hour, UnitPrefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  return 2;
}

}  // namespace PoincareJ
