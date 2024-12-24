#ifndef POINCARE_EXPRESSION_UNITS_REPRESENTATIVES_H
#define POINCARE_EXPRESSION_UNITS_REPRESENTATIVES_H

#include "unit.h"

namespace Poincare::Internal {
namespace Units {

// Helper class to add overrides using the static member "representatives"
template <class R>
class Helper : public Representative {
 protected:
  using Self = R;
  using Representative::Representative;
  constexpr static size_t NumberOfRepresentatives =
      sizeof(R::representatives) / sizeof(R);

  /* Base class for the list of static representatives, their members should all
   * be subclasses of Representative */
  struct Representatives {
    operator const Representative*() const {
      return reinterpret_cast<const Representative*>(this);
    }
    const Representative* end() const {
      return reinterpret_cast<const Representative*>(this) +
             NumberOfRepresentatives;
    }
  };

 public:
  int numberOfRepresentatives() const override {
    return NumberOfRepresentatives;
  };
  const Representative* representativesOfSameDimension() const override {
    return reinterpret_cast<const Representative*>(&R::representatives);
  };
  const SIVector siVector() const override { return R::Dimension; }
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
  constexpr static SIVector Dimension{.time = 1};

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

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Distance : public Helper<Distance> {
 public:
  constexpr static SIVector Dimension{.distance = 1};

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

  const Representative* bestRepresentativeAndPrefix(
      double value, double exponent, UnitFormat unitFormat,
      const Prefix** prefix, const Representative* forcedRepr) const override;

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Angle : public Helper<Angle> {
 public:
  constexpr static SIVector Dimension{.angle = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R radian;
    R arcSecond;
    R arcMinute;
    R degree;
    R gradian;
  };

  static const Representative* DefaultRepresentativeForAngleUnit(
      AngleUnit angleUnit);

#if 0
  const Representative* bestRepresentativeAndPrefix(
      double value, double exponent, UnitFormat unitFormat,
      const Prefix** prefix, const Representative* forcedRepr) const override;
#endif

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Mass : public Helper<Mass> {
 public:
  constexpr static SIVector Dimension{.mass = 1};

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

  const Representative* bestRepresentativeAndPrefix(
      double value, double exponent, UnitFormat unitFormat,
      const Prefix** prefix, const Representative* forcedRepr) const override;

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Current : public Helper<Current> {
 public:
  constexpr static SIVector Dimension{.current = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R ampere;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Temperature : public Helper<Temperature> {
 public:
  constexpr static SIVector Dimension{.temperature = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R kelvin;
    R celsius;
    R fahrenheit;
  };

  const Representative* bestRepresentativeAndPrefix(
      double value, double exponent, UnitFormat unitFormat,
      const Prefix** prefix, const Representative* forcedRepr) const override {
    return this;
  }
  using Helper::Helper;
  const static Representatives<const Self> representatives;

  static const Tree* celsiusOrigin;
  static const Tree* fahrenheitOrigin;
};

class AmountOfSubstance : public Helper<AmountOfSubstance> {
 public:
  constexpr static SIVector Dimension{.amountOfSubstance = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R mole;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class LuminousIntensity : public Helper<LuminousIntensity> {
 public:
  constexpr static SIVector Dimension{.luminousIntensity = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R candela;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Frequency : public Helper<Frequency> {
 public:
  constexpr static SIVector Dimension{.time = -1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R hertz;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Force : public Helper<Force> {
 public:
  constexpr static SIVector Dimension{.time = -2, .distance = 1, .mass = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R newton;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Pressure : public Helper<Pressure> {
 public:
  constexpr static SIVector Dimension{.time = -2, .distance = -1, .mass = 1};

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
  constexpr static SIVector Dimension{.time = -2, .distance = 2, .mass = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R joule;
    R electronVolt;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Power : public Helper<Power> {
 public:
  constexpr static SIVector Dimension{.time = -3, .distance = 2, .mass = 1};

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
  constexpr static SIVector Dimension{.time = 1, .current = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R coulomb;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class ElectricPotential : public Helper<ElectricPotential> {
 public:
  constexpr static SIVector Dimension{
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
  constexpr static SIVector Dimension{
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
  constexpr static SIVector Dimension{
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
  constexpr static SIVector Dimension{
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
  constexpr static SIVector Dimension{
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
  constexpr static SIVector Dimension{.time = -2, .mass = 1, .current = -1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R tesla;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Inductance : public Helper<Inductance> {
 public:
  constexpr static SIVector Dimension{
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
  constexpr static SIVector Dimension{.time = -1, .amountOfSubstance = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R katal;
  };

  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Surface : public Helper<Surface> {
 public:
  constexpr static SIVector Dimension{.distance = 2};

  template <class R>
  struct Representatives : Helper::Representatives {
    R hectare;
    R acre;
  };

  const Representative* bestRepresentativeAndPrefix(
      double value, double exponent, UnitFormat unitFormat,
      const Prefix** prefix, const Representative* forcedRepr) const override;
  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Volume : public Helper<Volume> {
 public:
  constexpr static SIVector Dimension{.distance = 3};

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

  const Representative* bestRepresentativeAndPrefix(
      double value, double exponent, UnitFormat unitFormat,
      const Prefix** prefix, const Representative* forcedRepr) const override;
  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

class Speed : public Helper<Speed> {
 public:
  constexpr static SIVector Dimension{.time = -1, .distance = 1};

  template <class R>
  struct Representatives : Helper::Representatives {
    R none;
  };

  const Representative* representativesOfSameDimension() const override {
    return nullptr;
  };

  const Representative* bestRepresentativeAndPrefix(
      double value, double exponent, UnitFormat unitFormat,
      const Prefix** prefix, const Representative* forcedRepr) const override {
    return nullptr;
  }
  using Helper::Helper;
  const static Representatives<const Self> representatives;
};

// Implicit addition

struct RepresentativesList {
  const Representative* const* representativesList;
  int length;
};

// These must be sorted in order, from smallest to biggest
constexpr const Representative*
    k_timeRepresentativesAllowingImplicitAddition[] = {
        &Time::representatives.second, &Time::representatives.minute,
        &Time::representatives.hour,   &Time::representatives.day,
        &Time::representatives.month,  &Time::representatives.year};

constexpr static const Representative*
    k_distanceRepresentativesAllowingImplicitAddition[] = {
        &Distance::representatives.inch, &Distance::representatives.foot,
        &Distance::representatives.yard, &Distance::representatives.mile};

constexpr static const Representative*
    k_massRepresentativesAllowingImplicitAddition[] = {
        &Mass::representatives.ounce, &Mass::representatives.pound};

constexpr static const Representative*
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

constexpr static const Representative* k_representativesWithoutLeftMargin[] = {
    &Angle::representatives.arcSecond,
    &Angle::representatives.arcMinute,
    &Angle::representatives.degree,
    &Temperature::representatives.celsius,
    &Temperature::representatives.fahrenheit,
};

}  // namespace Units
}  // namespace Poincare::Internal
#endif
