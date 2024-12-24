#include "representatives.h"

#include <omg/unreachable.h>

#include "unit.h"

namespace Poincare::Internal {
namespace Units {

using enum Representative::Prefixable;

const Time::Representatives<const Time> Time::representatives = {
    .second = {"s", 1_e, All, NegativeLongScale},
    .minute = {"min", 60_e, None, None},
    .hour = {"h", 3600_e, None, None},
    .day = {"day", 86400_e, None, None},
    .week = {"week", 604800_e, None, None},
    .month = {"month", 2629800_e, None, None},
    .year = {"year", 31557600_e, None, None}};

const Distance::Representatives<const Distance> Distance::representatives = {
    .meter = {"m", 1_e, All, NegativeAndKilo},
    .astronomicalUnit = {"au", 149597870700_e, None, None},
    .lightYear = {"ly", KMult(299792458_e, 31557600_e), None, None},
    .parsec = {"pc", KMult(180_e, KDiv(3600_e, π_e), 149587870700_e), None,
               None},
    .inch = {"in", 0.0254_e, None, None, true},
    .foot = {"ft", KMult(12_e, 0.0254_e), None, None, true},
    .yard = {"yd", KMult(36_e, 0.0254_e), None, None, true},
    .mile = {"mi", KMult(63360_e, 0.0254_e), None, None, true}};

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
    .ton = {"t", 1000_e, PositiveLongScale, PositiveLongScale},
    .ounce = {"oz", 0.028349523125_e, None, None, true},
    .pound = {"lb", KMult(16_e, 0.028349523125_e), None, None, true},
    .shortTon = {"shtn", KMult(2000_e, 16_e, 0.028349523125_e), None, None,
                 true},
    .longTon = {"lgtn", KMult(2240_e, 16_e, 0.028349523125_e), None, None,
                true},
    .dalton = {"Da", KDiv(KPow(10_e, -26_e), 6.02214076_e), All, All}};

const Current::Representatives<const Current> Current::representatives = {
    .ampere = {"A", 1_e, All, LongScale}};

// Origin must be used in addition to ratios for temperature conversions.
const Temperature::Representatives<const Temperature>
    Temperature::representatives = {
        .kelvin = {"K", 1_e, All, None},
        .celsius = {"°C", 1_e, None, None},
        .fahrenheit = {"°F", 5_e / 9_e, None, None, true}};

const Tree* Temperature::celsiusOrigin = 273.15_e;
const Tree* Temperature::fahrenheitOrigin = 459.67_e;

const AmountOfSubstance::Representatives<const AmountOfSubstance>
    AmountOfSubstance::representatives = {.mole = {"mol", 1_e, All, LongScale}};

const LuminousIntensity::Representatives<const LuminousIntensity>
    LuminousIntensity::representatives = {
        .candela = {"cd", 1_e, All, LongScale}};

const Frequency::Representatives<const Frequency> Frequency::representatives = {
    .hertz = {"Hz", 1_e, All, LongScale}};

const Force::Representatives<const Force> Force::representatives = {
    .newton = {"N", 1_e, All, LongScale}};

const Pressure::Representatives<const Pressure> Pressure::representatives = {
    .pascal = {"Pa", 1_e, All, LongScale},
    .bar = {"bar", 100000_e, All, LongScale},
    .atmosphere = {"atm", 101325_e, None, None}};

const Energy::Representatives<const Energy> Energy::representatives = {
    .joule = {"J", 1_e, All, LongScale},
    .electronVolt = {"eV", KMult(1.602176634_e, KPow(10_e, -19_e)), All,
                     LongScale}};

const Power::Representatives<const Power> Power::representatives = {
    .watt = {"W", 1_e, All, LongScale},
    .horsePower = {"hp", 745.699872_e, None, None}};

const ElectricCharge::Representatives<const ElectricCharge>
    ElectricCharge::representatives = {.coulomb = {"C", 1_e, All, LongScale}};

const ElectricPotential::Representatives<const ElectricPotential>
    ElectricPotential::representatives = {.volt = {"V", 1_e, All, LongScale}};

const ElectricCapacitance::Representatives<const ElectricCapacitance>
    ElectricCapacitance::representatives = {
        .farad = {"F", 1_e, All, LongScale}};

const ElectricResistance::Representatives<const ElectricResistance>
    ElectricResistance::representatives = {.ohm = {"Ω", 1_e, All, LongScale}};

const ElectricConductance::Representatives<const ElectricConductance>
    ElectricConductance::representatives = {
        .siemens = {"S", 1_e, All, LongScale}};

const MagneticFlux::Representatives<const MagneticFlux>
    MagneticFlux::representatives = {.weber = {"Wb", 1_e, All, LongScale}};

const MagneticField::Representatives<const MagneticField>
    MagneticField::representatives = {.tesla = {"T", 1_e, All, LongScale}};

const Inductance::Representatives<const Inductance>
    Inductance::representatives = {.henry = {"H", 1_e, All, LongScale}};

const CatalyticActivity::Representatives<const CatalyticActivity>
    CatalyticActivity::representatives = {
        .katal = {"kat", 1_e, All, LongScale}};

const Surface::Representatives<const Surface> Surface::representatives = {
    .hectare = {"ha", 10000_e, None, None},
    .acre = {"acre", 4046.8564224_e, None, None, true}};

const Volume::Representatives<const Volume> Volume::representatives = {
    .liter = {BuiltinsAliases::k_litersAliases, 0.001_e, All, Negative},
    .cup = {"cup", KMult(8_e, 0.0000295735295625_e), None, None, true},
    .pint = {"pt", KMult(16_e, 0.0000295735295625_e), None, None, true},
    .quart = {"qt", KMult(32_e, 0.0000295735295625_e), None, None, true},
    .gallon = {"gal", KMult(128_e, 0.0000295735295625_e), None, None, true},
    .teaSpoon = {"tsp", 0.00000492892159375_e, None, None, true},
    .tableSpoon = {"tbsp", KMult(3_e, 0.00000492892159375_e), None, None, true},
    .fluidOnce = {"floz", 0.0000295735295625_e, None, None, true}};

const Speed::Representatives<const Speed> Speed::representatives = {
    .none = {nullptr, 1_e, None, None}};

const Representative* Distance::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const Prefix** prefix) const {
  return unitFormat == UnitFormat::Metric
             ?
             /* Exclude imperial units from the search. */
             defaultFindBestRepresentativeAndPrefix(
                 value, exponent, &representatives.meter, &representatives.inch,
                 prefix)
             :
             /* Exclude meters from the search. */
             defaultFindBestRepresentativeAndPrefix(
                 value, exponent, &representatives.meter + 1,
                 representatives.end(), prefix);
}

#if 0
const Representative* Angle::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const Prefix** prefix) const {
  if (reductionContext.angleUnit() == AngleUnit::Degree) {
    return defaultFindBestRepresentativeAndPrefix(value, exponent, &arcSecond, 3,
                                         prefix);
  }
  return DefaultRepresentativeForAngleUnit(reductionContext.angleUnit());
}
#endif

const Representative* Angle::DefaultRepresentativeForAngleUnit(
    AngleUnit angleUnit) {
  switch (angleUnit) {
    case AngleUnit::Degree:
      return &representatives.degree;
    case AngleUnit::Radian:
      return &representatives.radian;
    case AngleUnit::Gradian:
      return &representatives.gradian;
    default:
      OMG::unreachable();
  }
}

const Representative* Mass::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const Prefix** prefix) const {
  if (unitFormat == UnitFormat::Imperial) {
    // With shortTon but not longTon
    return defaultFindBestRepresentativeAndPrefix(
        value, exponent, &representatives.ounce, &representatives.longTon,
        prefix);
  }
  assert(unitFormat == UnitFormat::Metric);
  if (exponent == 1. && value >= representatives.ton.ratio()) {
    return defaultFindBestRepresentativeAndPrefix(
        value, exponent, &representatives.ton, &representatives.ton + 1,
        prefix);
  }
  return defaultFindBestRepresentativeAndPrefix(
      value, exponent, &representatives.kilogram, &representatives.ton, prefix);
}

const Representative* Mass::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat, const Prefix** prefix,
    const Representative* forcedRepr) const {
  // Grams and kilograms are split in two representatives.
  if (forcedRepr == &representatives.gram ||
      forcedRepr == &representatives.kilogram) {
    return defaultFindBestRepresentativeAndPrefix(
        value, exponent, &representatives.kilogram, &representatives.gram + 1,
        prefix);
  }
  return Representative::standardRepresentative(value, exponent, unitFormat,
                                                prefix, forcedRepr);
}

const Representative* Surface::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const Prefix** prefix) const {
  *prefix = Prefix::EmptyPrefix();
  return unitFormat == UnitFormat::Metric ? &representatives.hectare
                                          : &representatives.acre;
}

const Representative* Volume::standardRepresentative(
    double value, double exponent, UnitFormat unitFormat,
    const Prefix** prefix) const {
  if (unitFormat == UnitFormat::Metric) {
    *prefix = representativesOfSameDimension()->findBestPrefix(value, exponent);
    return &representatives.liter;
  }
  return defaultFindBestRepresentativeAndPrefix(
      value, exponent, &representatives.cup, representatives.end(), prefix);
}

}  // namespace Units
}  // namespace Poincare::Internal
