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
  const UnitRepresentative* representativesOfSameDimension() const override {
    return R::representatives[0];
  };

 protected:
  using UnitRepresentative::UnitRepresentative;
};

class Time : public Helper<Time> {
 public:
  const static Time second;
  const static Time minute;
  const static Time hour;
  const static Time day;
  const static Time week;
  const static Time month;
  const static Time year;
  constexpr static const Time* representatives[] = {
      &second, &minute, &hour, &day, &week, &month, &year};

  const DimensionVector dimensionVector() const override { return {.time = 1}; }
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }
#if 0
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return ratio() * value >= representativesOfSameDimension()[1].ratio();
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif
 private:
  using Helper::Helper;
};

class Distance : public Helper<Distance> {
 public:
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

  const DimensionVector dimensionVector() const override {
    return {.distance = 1};
  }
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }
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

 private:
  using Helper::Helper;
};

class Angle : public Helper<Angle> {
 public:
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
#endif
  const DimensionVector dimensionVector() const override {
    return {.angle = 1};
  }
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }
#if 0
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

 private:
  using Helper::Helper;
};

class Mass : public Helper<Mass> {
 public:
  const static Mass gram;
  const static Mass ton;
  const static Mass ounce;
  const static Mass pound;
  const static Mass shortTon;
  const static Mass longTon;
  const static Mass dalton;
  constexpr static const Mass* representatives[] = {
      &gram, &ton, &ounce, &pound, &shortTon, &longTon, &dalton};

  const DimensionVector dimensionVector() const override { return {.mass = 1}; }
  const UnitPrefix* basePrefix() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }
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

 private:
  using Helper::Helper;
};

class Current : public Helper<Current> {
 public:
  const static Current ampere;
  constexpr static const Current* representatives[] = {&ampere};

  const DimensionVector dimensionVector() const override {
    return {.current = 1};
  }
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }

 private:
  using Helper::Helper;
};

class Temperature : public Helper<Temperature> {
 public:
  const static Temperature kelvin;
  const static Temperature celsius;
  const static Temperature fahrenheit;
  constexpr static const Temperature* representatives[] = {&kelvin, &celsius,
                                                           &fahrenheit};

#if 0
  static double ConvertTemperatures(double value,
                                    const UnitRepresentative* source,
                                    const UnitRepresentative* target);
#endif
  const DimensionVector dimensionVector() const override {
    return {.temperature = 1};
  }
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }
#if 0
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

 private:
  constexpr static double k_celsiusOrigin = 273.15;
  constexpr static double k_fahrenheitOrigin = 459.67;
  using Helper::Helper;
};

class AmountOfSubstance : public Helper<AmountOfSubstance> {
 public:
  const static AmountOfSubstance mole;
  constexpr static const AmountOfSubstance* representatives[] = {&mole};

  const DimensionVector dimensionVector() const override {
    return {.amountOfSubstance = 1};
  }
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }

 private:
  using Helper::Helper;
};

class LuminousIntensity : public Helper<LuminousIntensity> {
 public:
  const static LuminousIntensity candela;
  constexpr static const LuminousIntensity* representatives[] = {&candela};

  const DimensionVector dimensionVector() const override {
    return {.luminousIntensity = 1};
  }
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }

 private:
  using Helper::Helper;
};

class Frequency : public Helper<Frequency> {
 public:
  const static Frequency hertz;
  constexpr static const Frequency* representatives[] = {&hertz};

  const DimensionVector dimensionVector() const override {
    return {.time = -1};
  }

 private:
  using Helper::Helper;
};

class Force : public Helper<Force> {
 public:
  const static Force newton;
  constexpr static const Force* representatives[] = {&newton};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .distance = 1, .mass = 1};
  }

 private:
  using Helper::Helper;
};

class Pressure : public Helper<Pressure> {
 public:
  const static Pressure pascal;
  const static Pressure bar;
  const static Pressure atmosphere;
  constexpr static const Pressure* representatives[] = {&pascal, &bar,
                                                        &atmosphere};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .distance = -1, .mass = 1};
  }

 private:
  using Helper::Helper;
};

class Energy : public Helper<Energy> {
 public:
  const static Energy joule;
  const static Energy electronVolt;
  constexpr static const Energy* representatives[] = {&joule, &electronVolt};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .distance = 2, .mass = 1};
  }

#if 0
    bool hasSpecialAdditionalExpressions(double value,
                                         UnitFormat unitFormat) const override {
      return true;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

 private:
  using Helper::Helper;
};

class Power : public Helper<Power> {
 public:
  const static Power watt;
  const static Power horsePower;
  constexpr static const Power* representatives[] = {&watt, &horsePower};

  const DimensionVector dimensionVector() const override {
    return {.time = -3, .distance = 2, .mass = 1};
  }

 private:
  using Helper::Helper;
};

class ElectricCharge : public Helper<ElectricCharge> {
 public:
  const static ElectricCharge coulomb;
  constexpr static const ElectricCharge* representatives[] = {&coulomb};

  const DimensionVector dimensionVector() const override {
    return {.time = 1, .current = 1};
  }

 private:
  using Helper::Helper;
};

class ElectricPotential : public Helper<ElectricPotential> {
 public:
  const static ElectricPotential volt;
  constexpr static const ElectricPotential* representatives[] = {&volt};

  const DimensionVector dimensionVector() const override {
    return {.time = -3, .distance = 2, .mass = 1, .current = -1};
  }

 private:
  using Helper::Helper;
};

class ElectricCapacitance : public Helper<ElectricCapacitance> {
 public:
  const static ElectricCapacitance farad;
  constexpr static const ElectricCapacitance* representatives[] = {&farad};

  const DimensionVector dimensionVector() const override {
    return {.time = 4, .distance = -2, .mass = -1, .current = 2};
  }

 private:
  using Helper::Helper;
};

class ElectricResistance : public Helper<ElectricResistance> {
 public:
  const static ElectricResistance ohm;
  constexpr static const ElectricResistance* representatives[] = {&ohm};

  const DimensionVector dimensionVector() const override {
    return {.time = -3, .distance = 2, .mass = 1, .current = -2};
  }

 private:
  using Helper::Helper;
};

class ElectricConductance : public Helper<ElectricConductance> {
 public:
  const static ElectricConductance siemens;
  constexpr static const ElectricConductance* representatives[] = {&siemens};

  const DimensionVector dimensionVector() const override {
    return {.time = 3, .distance = -2, .mass = -1, .current = 2};
  }

 private:
  using Helper::Helper;
};

class MagneticFlux : public Helper<MagneticFlux> {
 public:
  const static MagneticFlux weber;
  constexpr static const MagneticFlux* representatives[] = {&weber};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .distance = 2, .mass = 1, .current = -1};
  }

 private:
  using Helper::Helper;
};

class MagneticField : public Helper<MagneticField> {
 public:
  const static MagneticField tesla;
  constexpr static const MagneticField* representatives[] = {&tesla};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .mass = 1, .current = -1};
  }

 private:
  using Helper::Helper;
};

class Inductance : public Helper<Inductance> {
 public:
  const static Inductance henry;
  constexpr static const Inductance* representatives[] = {&henry};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .distance = 2, .mass = 1, .current = -2};
  }

 private:
  using Helper::Helper;
};

class CatalyticActivity : public Helper<CatalyticActivity> {
 public:
  const static CatalyticActivity katal;
  constexpr static const CatalyticActivity* representatives[] = {&katal};

  const DimensionVector dimensionVector() const override {
    return {.time = -1, .amountOfSubstance = 1};
  }

 private:
  using Helper::Helper;
};

class Surface : public Helper<Surface> {
 public:
  const static Surface hectare;
  const static Surface acre;
  constexpr static const Surface* representatives[] = {&hectare, &acre};

  const DimensionVector dimensionVector() const override {
    return {.distance = 2};
  }
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

 private:
  using Helper::Helper;
};

class Volume : public Helper<Volume> {
 public:
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

  const DimensionVector dimensionVector() const override {
    return {.distance = 3};
  }
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

 private:
  using Helper::Helper;
};

class Speed : public Helper<Speed> {
 public:
  const DimensionVector dimensionVector() const override {
    return {.time = -1, .distance = 1};
  }
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

 private:
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
