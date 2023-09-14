#ifndef POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H
#define POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H

#include "k_tree.h"

namespace PoincareJ {

namespace Representatives {

class Time : public UnitRepresentative {
  friend class Unit;

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

  constexpr static Time Default() {
    return Time(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 1};
  }
  int numberOfRepresentatives() const override;
  const Time* representativesOfSameDimension() const override;
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
  using Time::Time;
};

class Distance : public Time {
  friend class Unit;

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

  constexpr static Distance Default() {
    return Distance(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.distance = 1};
  }
  int numberOfRepresentatives() const override;
  const Time* representativesOfSameDimension() const override;
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
  using UnitRepresentative::UnitRepresentative;
};

class Angle : public UnitRepresentative {
  friend class Unit;

 public:
  const static Angle radian;
  const static Angle arcSecond;
  const static Angle arcMinute;
  const static Angle degree;
  const static Angle gradian;
  constexpr static const Angle* representatives[] = {
      &radian, &arcSecond, &arcMinute, &degree, &gradian};

  constexpr static Angle Default() {
    return Angle(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const static UnitRepresentative* DefaultRepresentativeForAngleUnit(
      AngleUnit angleUnit);

#if 0
  // Returns a beautified expression
  Tree* convertInto(Tree* value, const UnitRepresentative* other,
                    const ReductionContext& reductionContext) const;
#endif
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.angle = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
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
  using UnitRepresentative::UnitRepresentative;
};

class Mass : public UnitRepresentative {
  friend class Unit;

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

  constexpr static Mass Default() {
    return Mass(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.mass = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
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
  using UnitRepresentative::UnitRepresentative;
};

class Current : public UnitRepresentative {
  friend class Unit;

 public:
  const static Current ampere;
  constexpr static const Current* representatives[] = {&ampere};

  constexpr static Current Default() {
    return Current(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.current = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Temperature : public UnitRepresentative {
  friend class Unit;

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
  constexpr static Temperature Default() {
    return Temperature(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.temperature = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
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
  using UnitRepresentative::UnitRepresentative;
};

class AmountOfSubstance : public UnitRepresentative {
  friend class Unit;

 public:
  const static AmountOfSubstance mole;
  constexpr static const AmountOfSubstance* representatives[] = {&mole};

  constexpr static AmountOfSubstance Default() {
    return AmountOfSubstance(nullptr, nullptr, Prefixable::None,
                             Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.amountOfSubstance = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }

 private:
  using UnitRepresentative::UnitRepresentative;
};

class LuminousIntensity : public UnitRepresentative {
  friend class Unit;

 public:
  const static LuminousIntensity candela;
  constexpr static const LuminousIntensity* representatives[] = {&candela};

  constexpr static LuminousIntensity Default() {
    return LuminousIntensity(nullptr, nullptr, Prefixable::None,
                             Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.luminousIntensity = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Frequency : public UnitRepresentative {
  friend class Unit;

 public:
  const static Frequency hertz;
  constexpr static const Frequency* representatives[] = {&hertz};

  constexpr static Frequency Default() {
    return Frequency(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Force : public UnitRepresentative {
  friend class Unit;

 public:
  const static Force newton;
  constexpr static const Force* representatives[] = {&newton};

  constexpr static Force Default() {
    return ForceRepresentative(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2, .distance = 1, .mass = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Pressure : public UnitRepresentative {
  friend class Unit;

 public:
  const static Pressure pascal;
  const static Pressure bar;
  const static Pressure atmosphere;
  constexpr static const Pressure* representatives[] = {&pascal, &bar,
                                                        &atmosphere};

  constexpr static Pressure Default() {
    return Pressure(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2, .distance = -1, .mass = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Energy : public UnitRepresentative {
  friend class Unit;

 public:
  const static Energy joule;
  const static Energy electronVolt;
  constexpr static const Energy* representatives[] = {&joule, &electronVolt};

  constexpr static Energy Default() {
    return Energy(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2, .distance = 2, .mass = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

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
  using UnitRepresentative::UnitRepresentative;
};

class Power : public UnitRepresentative {
  friend class Unit;

 public:
  const static Power watt;
  const static Power horsePower;
  constexpr static const Power* representatives[] = {&watt, &horsePower};

  constexpr static Power Default() {
    return Power(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -3, .distance = 2, .mass = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricCharge : public UnitRepresentative {
  friend class Unit;

 public:
  const static ElectricCharge coulomb;
  constexpr static const ElectricCharge* representatives[] = {&coulomb};

  constexpr static ElectricCharge Default() {
    return ElectricCharge(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 1, .current = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricPotential : public UnitRepresentative {
  friend class Unit;

 public:
  const static ElectricPotential volt;
  constexpr static const ElectricPotential* representatives[] = {&volt};

  constexpr static ElectricPotential Default() {
    return ElectricPotential(nullptr, nullptr, Prefixable::None,
                             Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -3, .distance = 2, .mass = 1, .current = -1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricCapacitance : public UnitRepresentative {
  friend class Unit;

 public:
  const static ElectricCapacitance farad;
  constexpr static const ElectricCapacitance* representatives[] = {&farad};

  constexpr static ElectricCapacitance Default() {
    return ElectricCapacitance(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 4, .distance = -2, .mass = -1, .current = 2};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricResistanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static ElectricResistance ohm;
  constexpr static const ElectricResistance* representatives[] = {&ohm};

  constexpr static ElectricResistance Default() {
    return ElectricResistance(nullptr, nullptr, Prefixable::None,
                              Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -3, .distance = 2, .mass = 1, .current = -2};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricConductance : public UnitRepresentative {
  friend class Unit;

 public:
  const static ElectricConductance siemens;
  constexpr static const ElectricConductance* representatives[] = {&siemens};

  constexpr static ElectricConductance Default() {
    return ElectricConductance(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 3, .distance = -2, .mass = -1, .current = 2};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class MagneticFlux : public UnitRepresentative {
  friend class Unit;

 public:
  const static MagneticFlux weber;
  constexpr static const MagneticFlux* representatives[] = {&weber};

  constexpr static MagneticFlux Default() {
    return MagneticFlux(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2, .distance = 2, .mass = 1, .current = -1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class MagneticField : public UnitRepresentative {
  friend class Unit;

 public:
  const static MagneticField tesla;
  constexpr static const MagneticField* representatives[] = {&tesla};

  constexpr static MagneticField Default() {
    return MagneticField(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2, .mass = 1, .current = -1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Inductance : public UnitRepresentative {
  friend class Unit;

 public:
  const static Inductance henry;
  constexpr static const Inductance* representatives[] = {&henry};

  constexpr static Inductance Default() {
    return Inductance(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2, .distance = 2, .mass = 1, .current = -2};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class CatalyticActivity : public UnitRepresentative {
  friend class Unit;

 public:
  const static CatalyticActivity katal;
  constexpr static const CatalyticActivity* representatives[] = {&katal};

  constexpr static CatalyticActivity Default() {
    return CatalyticActivity(nullptr, nullptr, Prefixable::None,
                             Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -1, .amountOfSubstance = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Surface : public UnitRepresentative {
  friend class Unit;

 public:
  const static Surface hectare;
  const static Surface acre;
  constexpr static const Surface* representatives[] = {&hectare, &acre};

  constexpr static Surface Default() {
    return Surface(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.distance = 2};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
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
  using UnitRepresentative::UnitRepresentative;
};

class Volume : public UnitRepresentative {
  friend class Unit;

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

  constexpr static Volume Default() {
    return Volume(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.distance = 3};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
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
  using UnitRepresentative::UnitRepresentative;
};

class Speed : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static Speed Default() {
    return Speed(nullptr, nullptr, Prefixable::None, Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -1, .distance = 1};
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
  using UnitRepresentative::UnitRepresentative;
};

}  // namespace Representatives
}  // namespace PoincareJ

#endif
