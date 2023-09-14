#ifndef POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H
#define POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H

#include "k_tree.h"

namespace PoincareJ {

class TimeRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static TimeRepresentative second;
  const static TimeRepresentative minute;
  const static TimeRepresentative hour;
  const static TimeRepresentative day;
  const static TimeRepresentative week;
  const static TimeRepresentative month;
  const static TimeRepresentative year;
  constexpr static const TimeRepresentative* representatives[] = {
      &second, &minute, &hour, &day, &week, &month, &year};

  constexpr static TimeRepresentative Default() {
    return TimeRepresentative(nullptr, nullptr, Prefixable::None,
                              Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
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
  using UnitRepresentative::UnitRepresentative;
};

class DistanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static DistanceRepresentative meter;
  const static DistanceRepresentative inch;
  const static DistanceRepresentative foot;
  const static DistanceRepresentative yard;
  const static DistanceRepresentative mile;
  const static DistanceRepresentative astronomicalUnit;
  const static DistanceRepresentative lightYear;
  const static DistanceRepresentative parsec;
  constexpr static const DistanceRepresentative* representatives[] = {
      &meter,     &inch,  &foot, &yard, &mile, &astronomicalUnit,
      &lightYear, &parsec};

  constexpr static DistanceRepresentative Default() {
    return DistanceRepresentative(nullptr, nullptr, Prefixable::None,
                                  Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.distance = 1};
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
      return unitFormat == UnitFormat::Imperial;
    }
    int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const override;
#endif

 private:
  using UnitRepresentative::UnitRepresentative;
};

class AngleRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static AngleRepresentative radian;
  const static AngleRepresentative arcSecond;
  const static AngleRepresentative arcMinute;
  const static AngleRepresentative degree;
  const static AngleRepresentative gradian;
  constexpr static const AngleRepresentative* representatives[] = {
      &radian, &arcSecond, &arcMinute, &degree, &gradian};

  constexpr static AngleRepresentative Default() {
    return AngleRepresentative(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
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

class MassRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static MassRepresentative gram;
  const static MassRepresentative ton;
  const static MassRepresentative ounce;
  const static MassRepresentative pound;
  const static MassRepresentative shortTon;
  const static MassRepresentative longTon;
  const static MassRepresentative dalton;
  constexpr static const MassRepresentative* representatives[] = {
      &gram, &ton, &ounce, &pound, &shortTon, &longTon, &dalton};

  constexpr static MassRepresentative Default() {
    return MassRepresentative(nullptr, nullptr, Prefixable::None,
                              Prefixable::None);
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

class CurrentRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static CurrentRepresentative ampere;
  constexpr static const CurrentRepresentative* representatives[] = {&ampere};

  constexpr static CurrentRepresentative Default() {
    return CurrentRepresentative(nullptr, nullptr, Prefixable::None,
                                 Prefixable::None);
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

class TemperatureRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static TemperatureRepresentative kelvin;
  const static TemperatureRepresentative celsius;
  const static TemperatureRepresentative fahrenheit;
  constexpr static const TemperatureRepresentative* representatives[] = {
      &kelvin, &celsius, &fahrenheit};

#if 0
  static double ConvertTemperatures(double value,
                                    const UnitRepresentative* source,
                                    const UnitRepresentative* target);
#endif
  constexpr static TemperatureRepresentative Default() {
    return TemperatureRepresentative(nullptr, nullptr, Prefixable::None,
                                     Prefixable::None);
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

class AmountOfSubstanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static AmountOfSubstanceRepresentative mole;
  constexpr static const AmountOfSubstanceRepresentative* representatives[] = {
      &mole};

  constexpr static AmountOfSubstanceRepresentative Default() {
    return AmountOfSubstanceRepresentative(nullptr, nullptr, Prefixable::None,
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

class LuminousIntensityRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static LuminousIntensityRepresentative candela;
  constexpr static const LuminousIntensityRepresentative* representatives[] = {
      &candela};

  constexpr static LuminousIntensityRepresentative Default() {
    return LuminousIntensityRepresentative(nullptr, nullptr, Prefixable::None,
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

class FrequencyRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static FrequencyRepresentative hertz;
  constexpr static const FrequencyRepresentative* representatives[] = {&hertz};

  constexpr static FrequencyRepresentative Default() {
    return FrequencyRepresentative(nullptr, nullptr, Prefixable::None,
                                   Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ForceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static ForceRepresentative newton;
  constexpr static const ForceRepresentative* representatives[] = {&newton};

  constexpr static ForceRepresentative Default() {
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

class PressureRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static PressureRepresentative pascal;
  const static PressureRepresentative bar;
  const static PressureRepresentative atmosphere;
  constexpr static const PressureRepresentative* representatives[] = {
      &pascal, &bar, &atmosphere};

  constexpr static PressureRepresentative Default() {
    return PressureRepresentative(nullptr, nullptr, Prefixable::None,
                                  Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2, .distance = -1, .mass = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class EnergyRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static EnergyRepresentative joule;
  const static EnergyRepresentative electronVolt;
  constexpr static const EnergyRepresentative* representatives[] = {
      &joule, &electronVolt};

  constexpr static EnergyRepresentative Default() {
    return EnergyRepresentative(nullptr, nullptr, Prefixable::None,
                                Prefixable::None);
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

class PowerRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static PowerRepresentative watt;
  const static PowerRepresentative horsePower;
  constexpr static const PowerRepresentative* representatives[] = {&watt,
                                                                   &horsePower};

  constexpr static PowerRepresentative Default() {
    return PowerRepresentative(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -3, .distance = 2, .mass = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricChargeRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static ElectricChargeRepresentative coulomb;
  constexpr static const ElectricChargeRepresentative* representatives[] = {
      &coulomb};

  constexpr static ElectricChargeRepresentative Default() {
    return ElectricChargeRepresentative(nullptr, nullptr, Prefixable::None,
                                        Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 1, .current = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricPotentialRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static ElectricPotentialRepresentative volt;
  constexpr static const ElectricPotentialRepresentative* representatives[] = {
      &volt};

  constexpr static ElectricPotentialRepresentative Default() {
    return ElectricPotentialRepresentative(nullptr, nullptr, Prefixable::None,
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

class ElectricCapacitanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static ElectricCapacitanceRepresentative farad;
  constexpr static const ElectricCapacitanceRepresentative* representatives[] =
      {&farad};

  constexpr static ElectricCapacitanceRepresentative Default() {
    return ElectricCapacitanceRepresentative(nullptr, nullptr, Prefixable::None,
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
  const static ElectricResistanceRepresentative ohm;
  constexpr static const ElectricResistanceRepresentative* representatives[] = {
      &ohm};

  constexpr static ElectricResistanceRepresentative Default() {
    return ElectricResistanceRepresentative(nullptr, nullptr, Prefixable::None,
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

class ElectricConductanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static ElectricConductanceRepresentative siemens;
  constexpr static const ElectricConductanceRepresentative* representatives[] =
      {&siemens};

  constexpr static ElectricConductanceRepresentative Default() {
    return ElectricConductanceRepresentative(nullptr, nullptr, Prefixable::None,
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

class MagneticFluxRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static MagneticFluxRepresentative weber;
  constexpr static const MagneticFluxRepresentative* representatives[] = {
      &weber};

  constexpr static MagneticFluxRepresentative Default() {
    return MagneticFluxRepresentative(nullptr, nullptr, Prefixable::None,
                                      Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2, .distance = 2, .mass = 1, .current = -1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class MagneticFieldRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static MagneticFieldRepresentative tesla;
  constexpr static const MagneticFieldRepresentative* representatives[] = {
      &tesla};

  constexpr static MagneticFieldRepresentative Default() {
    return MagneticFieldRepresentative(nullptr, nullptr, Prefixable::None,
                                       Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2, .mass = 1, .current = -1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class InductanceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static InductanceRepresentative henry;
  constexpr static const InductanceRepresentative* representatives[] = {&henry};

  constexpr static InductanceRepresentative Default() {
    return InductanceRepresentative(nullptr, nullptr, Prefixable::None,
                                    Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = -2, .distance = 2, .mass = 1, .current = -2};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class CatalyticActivityRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static CatalyticActivityRepresentative katal;
  constexpr static const CatalyticActivityRepresentative* representatives[] = {
      &katal};

  constexpr static CatalyticActivityRepresentative Default() {
    return CatalyticActivityRepresentative(nullptr, nullptr, Prefixable::None,
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

class SurfaceRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static SurfaceRepresentative hectare;
  const static SurfaceRepresentative acre;
  constexpr static const SurfaceRepresentative* representatives[] = {&hectare,
                                                                     &acre};

  constexpr static SurfaceRepresentative Default() {
    return SurfaceRepresentative(nullptr, nullptr, Prefixable::None,
                                 Prefixable::None);
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

class VolumeRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  const static VolumeRepresentative liter;
  const static VolumeRepresentative cup;
  const static VolumeRepresentative pint;
  const static VolumeRepresentative quart;
  const static VolumeRepresentative gallon;
  const static VolumeRepresentative teaSpoon;
  const static VolumeRepresentative tableSpoon;
  const static VolumeRepresentative fluidOnce;
  constexpr static const VolumeRepresentative* representatives[] = {
      &liter, &cup, &pint, &quart, &gallon, &teaSpoon, &tableSpoon, &fluidOnce};

  constexpr static VolumeRepresentative Default() {
    return VolumeRepresentative(nullptr, nullptr, Prefixable::None,
                                Prefixable::None);
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

class SpeedRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static SpeedRepresentative Default() {
    return SpeedRepresentative(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
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

}  // namespace PoincareJ

#endif
