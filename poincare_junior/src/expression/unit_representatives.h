#ifndef POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H
#define POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H

#include "unit.h"

namespace PoincareJ {

namespace Representatives {

// Helper class to add overrides using the static member "representatives"
template <class R>
class Helper : public UnitRepresentative {
 public:
  int numberOfRepresentatives() const override {
    return std::size(R::representatives);
  };
  const UnitRepresentative* const* representativesOfSameDimension()
      const override {
    return representativesList();
  };
  constexpr static const UnitRepresentative* const* representativesList() {
    return reinterpret_cast<const UnitRepresentative* const*>(
        R::representatives);
  };
  const DimensionVector dimensionVector() const override {
    return R::Dimension;
  }
  bool isBaseUnit() const override {
    if constexpr (R::Dimension.isSI()) {
      return this == representativesOfSameDimension()[0];
    } else {
      return false;
    }
  }
  using UnitRepresentative::UnitRepresentative;
};

class Time : public Helper<Time> {
 public:
  constexpr static DimensionVector Dimension{.time = 1};

  const static Time second;
  const static Time minute;
  const static Time hour;
  const static Time day;
  const static Time week;
  const static Time month;
  const static Time year;
  constexpr static const Time* representatives[] = {
      &second, &minute, &hour, &day, &week, &month, &year};

#if 0
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return ratio() * value >= representativesOfSameDimension()[1].ratio();
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif
  using Helper::Helper;
};

class Distance : public Helper<Distance> {
 public:
  constexpr static DimensionVector Dimension{.distance = 1};

  const static Distance meter;
  const static Distance inch;
  const static Distance foot;
  const static Distance yard;
  const static Distance mile;
  const static Distance astronomicalUnit;
  const static Distance lightYear;
  const static Distance parsec;
  constexpr static const Distance* representatives[] = {
      &meter,     &inch,  &foot, &yard, &mile, &astronomicalUnit,
      &lightYear, &parsec};

#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return unitFormat == UnitFormat::Imperial;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

  using Helper::Helper;
};

class Angle : public Helper<Angle> {
 public:
  constexpr static DimensionVector Dimension{.angle = 1};

  const static Angle radian;
  const static Angle arcSecond;
  const static Angle arcMinute;
  const static Angle degree;
  const static Angle gradian;
  constexpr static const Angle* representatives[] = {
      &radian, &arcSecond, &arcMinute, &degree, &gradian};

#if 0
  // Returns a beautified expression
  Tree* convertInto(Tree* value, const UnitRepresentative* other,
                    const ReductionContext& reductionContext) const;
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressionsWithExactValue(
        Expression exactValue, double value, Expression* dest,
        int availableLength, const ReductionContext& reductionContext) const;
#endif

  using Helper::Helper;
};

class Mass : public Helper<Mass> {
 public:
  constexpr static DimensionVector Dimension{.mass = 1};

  const static Mass kilogram;
  const static Mass gram;
  const static Mass ton;
  const static Mass ounce;
  const static Mass pound;
  const static Mass shortTon;
  const static Mass longTon;
  const static Mass dalton;
  constexpr static const Mass* representatives[] = {
      &kilogram, &gram, &ton, &ounce, &pound, &shortTon, &longTon, &dalton};

#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return unitFormat == UnitFormat::Imperial;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

  using Helper::Helper;
};

class Current : public Helper<Current> {
 public:
  constexpr static DimensionVector Dimension{.current = 1};

  const static Current ampere;
  constexpr static const Current* representatives[] = {&ampere};

  using Helper::Helper;
};

class Temperature : public Helper<Temperature> {
 public:
  constexpr static DimensionVector Dimension{.temperature = 1};

  const static Temperature kelvin;
  const static Temperature celsius;
  const static Temperature fahrenheit;
  constexpr static const Temperature* representatives[] = {&kelvin, &celsius,
                                                           &fahrenheit};

#if 0
  static double ConvertTemperatures(double value,
                                    const UnitRepresentative* source,
                                    const UnitRepresentative* target);
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override {
    return this;
  }
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif
  using Helper::Helper;

 private:
  constexpr static double k_celsiusOrigin = 273.15;
  constexpr static double k_fahrenheitOrigin = 459.67;
};

class AmountOfSubstance : public Helper<AmountOfSubstance> {
 public:
  constexpr static DimensionVector Dimension{.amountOfSubstance = 1};

  const static AmountOfSubstance mole;
  constexpr static const AmountOfSubstance* representatives[] = {&mole};

  using Helper::Helper;
};

class LuminousIntensity : public Helper<LuminousIntensity> {
 public:
  constexpr static DimensionVector Dimension{.luminousIntensity = 1};

  const static LuminousIntensity candela;
  constexpr static const LuminousIntensity* representatives[] = {&candela};

  using Helper::Helper;
};

class Frequency : public Helper<Frequency> {
 public:
  constexpr static DimensionVector Dimension{.time = -1};

  const static Frequency hertz;
  constexpr static const Frequency* representatives[] = {&hertz};

  using Helper::Helper;
};

class Force : public Helper<Force> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .distance = 1, .mass = 1};

  const static Force newton;
  constexpr static const Force* representatives[] = {&newton};

  using Helper::Helper;
};

class Pressure : public Helper<Pressure> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .distance = -1, .mass = 1};

  const static Pressure pascal;
  const static Pressure bar;
  const static Pressure atmosphere;
  constexpr static const Pressure* representatives[] = {&pascal, &bar,
                                                        &atmosphere};

  using Helper::Helper;
};

class Energy : public Helper<Energy> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .distance = 2, .mass = 1};

  const static Energy joule;
  const static Energy electronVolt;
  constexpr static const Energy* representatives[] = {&joule, &electronVolt};

#if 0
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

  using Helper::Helper;
};

class Power : public Helper<Power> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -3, .distance = 2, .mass = 1};

  const static Power watt;
  const static Power horsePower;
  constexpr static const Power* representatives[] = {&watt, &horsePower};

  using Helper::Helper;
};

class ElectricCharge : public Helper<ElectricCharge> {
 public:
  constexpr static DimensionVector Dimension{.time = 1, .current = 1};

  const static ElectricCharge coulomb;
  constexpr static const ElectricCharge* representatives[] = {&coulomb};

  using Helper::Helper;
};

class ElectricPotential : public Helper<ElectricPotential> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -3, .distance = 2, .mass = 1, .current = -1};

  const static ElectricPotential volt;
  constexpr static const ElectricPotential* representatives[] = {&volt};

  using Helper::Helper;
};

class ElectricCapacitance : public Helper<ElectricCapacitance> {
 public:
  constexpr static DimensionVector Dimension{
      .time = 4, .distance = -2, .mass = -1, .current = 2};

  const static ElectricCapacitance farad;
  constexpr static const ElectricCapacitance* representatives[] = {&farad};

  using Helper::Helper;
};

class ElectricResistance : public Helper<ElectricResistance> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -3, .distance = 2, .mass = 1, .current = -2};

  const static ElectricResistance ohm;
  constexpr static const ElectricResistance* representatives[] = {&ohm};

  using Helper::Helper;
};

class ElectricConductance : public Helper<ElectricConductance> {
 public:
  constexpr static DimensionVector Dimension{
      .time = 3, .distance = -2, .mass = -1, .current = 2};

  const static ElectricConductance siemens;
  constexpr static const ElectricConductance* representatives[] = {&siemens};

  using Helper::Helper;
};

class MagneticFlux : public Helper<MagneticFlux> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .distance = 2, .mass = 1, .current = -1};

  const static MagneticFlux weber;
  constexpr static const MagneticFlux* representatives[] = {&weber};

  using Helper::Helper;
};

class MagneticField : public Helper<MagneticField> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .mass = 1, .current = -1};

  const static MagneticField tesla;
  constexpr static const MagneticField* representatives[] = {&tesla};

  using Helper::Helper;
};

class Inductance : public Helper<Inductance> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .distance = 2, .mass = 1, .current = -2};

  const static Inductance henry;
  constexpr static const Inductance* representatives[] = {&henry};

  using Helper::Helper;
};

class CatalyticActivity : public Helper<CatalyticActivity> {
 public:
  constexpr static DimensionVector Dimension{.time = -1,
                                             .amountOfSubstance = 1};

  const static CatalyticActivity katal;
  constexpr static const CatalyticActivity* representatives[] = {&katal};

  using Helper::Helper;
};

class Surface : public Helper<Surface> {
 public:
  constexpr static DimensionVector Dimension{.distance = 2};

  const static Surface hectare;
  const static Surface acre;
  constexpr static const Surface* representatives[] = {&hectare, &acre};

#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif
  using Helper::Helper;
};

class Volume : public Helper<Volume> {
 public:
  constexpr static DimensionVector Dimension{.distance = 3};

  const static Volume liter;
  const static Volume cup;
  const static Volume pint;
  const static Volume quart;
  const static Volume gallon;
  const static Volume teaSpoon;
  const static Volume tableSpoon;
  const static Volume fluidOnce;
  constexpr static const Volume* representatives[] = {
      &liter, &cup, &pint, &quart, &gallon, &teaSpoon, &tableSpoon, &fluidOnce};

#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override;
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif
  using Helper::Helper;
};

class Speed : public Helper<Speed> {
 public:
  constexpr static DimensionVector Dimension{.time = -1, .distance = 1};

  const static Speed none;
  constexpr static const Speed* representatives[] = {&none};

  const UnitRepresentative* const* representativesOfSameDimension()
      const override {
    return nullptr;
  };

#if 0
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const override {
    return nullptr;
  }
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif
  using Helper::Helper;
};

// Implicit addition

struct RepresentativesList {
  const UnitRepresentative* const* representativesList;
  int length;
};

// These must be sorted in order, from smallest to biggest
constexpr const UnitRepresentative*
    k_timeRepresentativesAllowingImplicitAddition[] = {
        &Time::second, &Time::minute, &Time::hour,
        &Time::day,    &Time::month,  &Time::year};

constexpr static const UnitRepresentative*
    k_distanceRepresentativesAllowingImplicitAddition[] = {
        &Distance::inch, &Distance::foot, &Distance::yard, &Distance::mile};

constexpr static const UnitRepresentative*
    k_massRepresentativesAllowingImplicitAddition[] = {&Mass::ounce,
                                                       &Mass::pound};

constexpr static const UnitRepresentative*
    k_angleRepresentativesAllowingImplicitAddition[] = {
        &Angle::arcSecond, &Angle::arcMinute, &Angle::degree};

constexpr static RepresentativesList
    k_representativesAllowingImplicitAddition[] = {
        {k_timeRepresentativesAllowingImplicitAddition,
         std::size(k_timeRepresentativesAllowingImplicitAddition)},
        {k_distanceRepresentativesAllowingImplicitAddition,
         std::size(k_distanceRepresentativesAllowingImplicitAddition)},
        {k_massRepresentativesAllowingImplicitAddition,
         std::size(k_massRepresentativesAllowingImplicitAddition)},
        {k_angleRepresentativesAllowingImplicitAddition,
         std::size(k_angleRepresentativesAllowingImplicitAddition)}};
constexpr static int k_representativesAllowingImplicitAdditionLength =
    std::size(k_representativesAllowingImplicitAddition);

constexpr static const UnitRepresentative*
    k_representativesWithoutLeftMargin[] = {
        &Angle::arcSecond,     &Angle::arcMinute,        &Angle::degree,
        &Temperature::celsius, &Temperature::fahrenheit,
};

}  // namespace Representatives
}  // namespace PoincareJ

#endif
