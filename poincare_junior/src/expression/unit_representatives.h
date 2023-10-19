#ifndef POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H
#define POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H

#include "unit.h"

namespace PoincareJ {
namespace Units {

// Helper class to add overrides using the static member "representatives"
template <class R>
class Helper : public UnitRepresentative {
 protected:
  using Self = R;
  using UnitRepresentative::UnitRepresentative;
  constexpr static size_t NumberOfRepresentatives =
      sizeof(R::representatives) / sizeof(R);

  /* Base class for the list of static representatives, their members should all
   * be subclasses of UnitRepresentative */
  struct Representatives {
    operator const UnitRepresentative*() const {
      return reinterpret_cast<const UnitRepresentative*>(this);
    }
    const UnitRepresentative* end() const {
      return reinterpret_cast<const UnitRepresentative*>(this) +
             NumberOfRepresentatives;
    }
  };

 public:
  int numberOfRepresentatives() const override {
    return NumberOfRepresentatives;
  };
  const UnitRepresentative* representativesOfSameDimension() const override {
    return reinterpret_cast<const UnitRepresentative*>(&R::representatives);
  };
  const DimensionVector dimensionVector() const override {
    return R::Dimension;
  }
  bool isBaseUnit() const override {
    if constexpr (R::Dimension.isSI()) {
      return this == representativesOfSameDimension();
    } else {
      return false;
    }
  }
};

class Time : public Helper<Time> {
 public:
  constexpr static DimensionVector Dimension{.time = 1};

  // The template is required since Time is still incomplete here
  template <class R>
  struct Representatives : Helper::Representatives {
    R second;
    R minute;
    R hour;
    R day;
    R week;
    R month;
    R year;
  };

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
  const static Representatives<const Self> representatives;
};

class Distance : public Helper<Distance> {
 public:
  constexpr static DimensionVector Dimension{.distance = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R meter;
    R astronomicalUnit;
    R lightYear;
    R parsec;
    R inch;
    R foot;
    R yard;
    R mile;
  };

  const UnitRepresentative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const UnitPrefix** prefix) const override;
#if 0
  bool hasSpecialAdditionalExpressions(double value,
                                       UnitFormat unitFormat) const override {
    return unitFormat == UnitFormat::Imperial;
  }
  int setAdditionalExpressions(double value, Expression* dest,
                               int availableLength,
                               UnitFormat unitFormat) const override;
#endif

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Angle : public Helper<Angle> {
 public:
  constexpr static DimensionVector Dimension{.angle = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R radian;
    R arcSecond;
    R arcMinute;
    R degree;
    R gradian;
  };

#if 0
  // Returns a beautified expression
  Tree* convertInto(Tree* value, const UnitRepresentative* other,
                    UnitFormat unitFormat) const;
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const UnitPrefix** prefix) const override;
  bool hasSpecialAdditionalExpressions(double value,
                                       UnitFormat unitFormat) const override {
    return true;
  }
  int setAdditionalExpressionsWithExactValue(Expression exactValue,
                                             double value, Expression* dest,
                                             int availableLength,
                                             UnitFormat unitFormat) const;
#endif

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Mass : public Helper<Mass> {
 public:
  constexpr static DimensionVector Dimension{.mass = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R kilogram;
    R gram;
    R ton;
    R ounce;
    R pound;
    R shortTon;
    R longTon;
    R dalton;
  };

  const UnitRepresentative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const UnitPrefix** prefix) const override;
#if 0
  bool hasSpecialAdditionalExpressions(double value,
                                       UnitFormat unitFormat) const override {
    return unitFormat == UnitFormat::Imperial;
  }
  int setAdditionalExpressions(double value, Expression* dest,
                               int availableLength,
                               UnitFormat unitFormat) const override;
#endif

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Current : public Helper<Current> {
 public:
  constexpr static DimensionVector Dimension{.current = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R ampere;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Temperature : public Helper<Temperature> {
 public:
  constexpr static DimensionVector Dimension{.temperature = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R kelvin;
    R celsius;
    R fahrenheit;
  };

#if 0
  static double ConvertTemperatures(double value,
                                    const UnitRepresentative* source,
                                    const UnitRepresentative* target);
#endif
  const UnitRepresentative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const UnitPrefix** prefix) const override {
    return this;
  }
#if 0
  bool hasSpecialAdditionalExpressions(double value,
                                       UnitFormat unitFormat) const override {
    return true;
  }
  int setAdditionalExpressions(double value, Expression* dest,
                               int availableLength,
                               UnitFormat unitFormat) const override;
#endif
  using Helper::Helper;
  const static Representatives<const Self> representatives;

 private:
  constexpr static double k_celsiusOrigin = 273.15;
  constexpr static double k_fahrenheitOrigin = 459.67;
};

class AmountOfSubstance : public Helper<AmountOfSubstance> {
 public:
  constexpr static DimensionVector Dimension{.amountOfSubstance = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R mole;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class LuminousIntensity : public Helper<LuminousIntensity> {
 public:
  constexpr static DimensionVector Dimension{.luminousIntensity = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R candela;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Frequency : public Helper<Frequency> {
 public:
  constexpr static DimensionVector Dimension{.time = -1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R hertz;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Force : public Helper<Force> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .distance = 1, .mass = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R newton;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Pressure : public Helper<Pressure> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .distance = -1, .mass = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R pascal;
    R bar;
    R atmosphere;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Energy : public Helper<Energy> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .distance = 2, .mass = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R joule;
    R electronVolt;
  };

#if 0
  bool hasSpecialAdditionalExpressions(double value,
                                       UnitFormat unitFormat) const override {
    return true;
  }
  int setAdditionalExpressions(double value, Expression* dest,
                               int availableLength,
                               UnitFormat unitFormat) const override;
#endif

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Power : public Helper<Power> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -3, .distance = 2, .mass = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R watt;
    R horsePower;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class ElectricCharge : public Helper<ElectricCharge> {
 public:
  constexpr static DimensionVector Dimension{.time = 1, .current = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R coulomb;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class ElectricPotential : public Helper<ElectricPotential> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -3, .distance = 2, .mass = 1, .current = -1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R volt;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class ElectricCapacitance : public Helper<ElectricCapacitance> {
 public:
  constexpr static DimensionVector Dimension{
      .time = 4, .distance = -2, .mass = -1, .current = 2};

  template <class R>
  struct Representatives : Helper::Representatives {
    R farad;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class ElectricResistance : public Helper<ElectricResistance> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -3, .distance = 2, .mass = 1, .current = -2};

  template <class R>
  struct Representatives : Helper::Representatives {
    R ohm;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class ElectricConductance : public Helper<ElectricConductance> {
 public:
  constexpr static DimensionVector Dimension{
      .time = 3, .distance = -2, .mass = -1, .current = 2};

  template <class R>
  struct Representatives : Helper::Representatives {
    R siemens;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class MagneticFlux : public Helper<MagneticFlux> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .distance = 2, .mass = 1, .current = -1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R weber;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class MagneticField : public Helper<MagneticField> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .mass = 1, .current = -1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R tesla;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Inductance : public Helper<Inductance> {
 public:
  constexpr static DimensionVector Dimension{
      .time = -2, .distance = 2, .mass = 1, .current = -2};

  template <class R>
  struct Representatives : Helper::Representatives {
    R henry;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class CatalyticActivity : public Helper<CatalyticActivity> {
 public:
  constexpr static DimensionVector Dimension{.time = -1,
                                             .amountOfSubstance = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R katal;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Surface : public Helper<Surface> {
 public:
  constexpr static DimensionVector Dimension{.distance = 2};

  template <class R>
  struct Representatives : Helper::Representatives {
    R hectare;
    R acre;
  };

  const UnitRepresentative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const UnitPrefix** prefix) const override;
#if 0
  bool hasSpecialAdditionalExpressions(double value,
                                       UnitFormat unitFormat) const override {
    return true;
  }
  int setAdditionalExpressions(double value, Expression* dest,
                               int availableLength,
                               UnitFormat unitFormat) const override;
#endif
  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Volume : public Helper<Volume> {
 public:
  constexpr static DimensionVector Dimension{.distance = 3};

  template <class R>
  struct Representatives : Helper::Representatives {
    R liter;
    R cup;
    R pint;
    R quart;
    R gallon;
    R teaSpoon;
    R tableSpoon;
    R fluidOnce;
  };

  const UnitRepresentative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const UnitPrefix** prefix) const override;
#if 0
  bool hasSpecialAdditionalExpressions(double value,
                                       UnitFormat unitFormat) const override {
    return true;
  }
  int setAdditionalExpressions(double value, Expression* dest,
                               int availableLength,
                               UnitFormat unitFormat) const override;
#endif
  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Speed : public Helper<Speed> {
 public:
  constexpr static DimensionVector Dimension{.time = -1, .distance = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R none;
  };

  const UnitRepresentative* representativesOfSameDimension() const override {
    return nullptr;
  };

  const UnitRepresentative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const UnitPrefix** prefix) const override {
    return nullptr;
  }
#if 0
  bool hasSpecialAdditionalExpressions(double value,
                                       UnitFormat unitFormat) const override {
    return true;
  }
  int setAdditionalExpressions(double value, Expression* dest,
                               int availableLength,
                               UnitFormat unitFormat) const override;
#endif
  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

// Implicit addition

struct RepresentativesList {
  const UnitRepresentative* const* representativesList;
  int length;
};

// These must be sorted in order, from smallest to biggest
constexpr const UnitRepresentative*
    k_timeRepresentativesAllowingImplicitAddition[] = {
        &Time::representatives.second, &Time::representatives.minute,
        &Time::representatives.hour,   &Time::representatives.day,
        &Time::representatives.month,  &Time::representatives.year};

constexpr static const UnitRepresentative*
    k_distanceRepresentativesAllowingImplicitAddition[] = {
        &Distance::representatives.inch, &Distance::representatives.foot,
        &Distance::representatives.yard, &Distance::representatives.mile};

constexpr static const UnitRepresentative*
    k_massRepresentativesAllowingImplicitAddition[] = {
        &Mass::representatives.ounce, &Mass::representatives.pound};

constexpr static const UnitRepresentative*
    k_angleRepresentativesAllowingImplicitAddition[] = {
        &Angle::representatives.arcSecond, &Angle::representatives.arcMinute,
        &Angle::representatives.degree};

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
        &Angle::representatives.arcSecond,
        &Angle::representatives.arcMinute,
        &Angle::representatives.degree,
        &Temperature::representatives.celsius,
        &Temperature::representatives.fahrenheit,
};

}  // namespace Units
}  // namespace PoincareJ
#endif
