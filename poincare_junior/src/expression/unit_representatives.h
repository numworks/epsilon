#ifndef POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H
#define POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H

#include "k_tree.h"

namespace PoincareJ {

class TimeRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
  constexpr static int k_secondRepresentativeIndex = 0;
  constexpr static int k_minuteRepresentativeIndex = 1;
  constexpr static int k_hourRepresentativeIndex = 2;

  constexpr static int k_dayRepresentativeIndex = 3;

  constexpr static int k_monthRepresentativeIndex = 5;

  constexpr static int k_yearRepresentativeIndex = 6;

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
  constexpr static int k_meterRepresentativeIndex = 0;

  constexpr static int k_inchRepresentativeIndex = 4;

  constexpr static int k_footRepresentativeIndex = 5;

  constexpr static int k_yardRepresentativeIndex = 6;

  constexpr static int k_mileRepresentativeIndex = 7;

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
  constexpr static int k_radianRepresentativeIndex = 0;

  constexpr static int k_arcSecondRepresentativeIndex = 1;

  constexpr static int k_arcMinuteRepresentativeIndex = 2;

  constexpr static int k_degreeRepresentativeIndex = 3;

  constexpr static int k_gradianRepresentativeIndex = 4;

  constexpr static AngleRepresentative Default() {
    return AngleRepresentative(nullptr, nullptr, Prefixable::None,
                               Prefixable::None);
  }
  static const UnitRepresentative* DefaultRepresentativeForAngleUnit(
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
  constexpr static int k_gramRepresentativeIndex = 0;

  constexpr static int k_tonRepresentativeIndex = 1;

  constexpr static int k_ounceRepresentativeIndex = 3;

  constexpr static int k_poundRepresentativeIndex = 4;

  constexpr static int k_shortTonRepresentativeIndex = 5;

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
  constexpr static int k_kelvinRepresentativeIndex = 0;

  constexpr static int k_celsiusRepresentativeIndex = 1;

  constexpr static int k_fahrenheitRepresentativeIndex = 2;

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
  constexpr static int k_jouleRepresentativeIndex = 0;

  constexpr static int k_electronVoltRepresentativeIndex = 1;

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
  constexpr static int k_wattRepresentativeIndex = 0;

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
  using UnitRepresentative::UnitRepresentative;
  constexpr static ElectricChargeRepresentative Default() {
    return ElectricChargeRepresentative(nullptr, nullptr, Prefixable::None,
                                        Prefixable::None);
  }
  const DimensionVector dimensionVector() const override {
    return DimensionVector{.time = 1, .current = 1};
  }
  int numberOfRepresentatives() const override;
  const UnitRepresentative* representativesOfSameDimension() const override;
};

class ElectricPotentialRepresentative : public UnitRepresentative {
  friend class Unit;

 public:
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
  constexpr static int k_hectareRepresentativeIndex = 0;

  constexpr static int k_acreRepresentativeIndex = 1;

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
  constexpr static int k_literRepresentativeIndex = 0;

  constexpr static int k_cupRepresentativeIndex = 4;

  constexpr static int k_pintRepresentativeIndex = 5;

  constexpr static int k_quartRepresentativeIndex = 6;

  constexpr static int k_gallonRepresentativeIndex = 7;

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
