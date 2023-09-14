#ifndef POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H
#define POINCARE_EXPRESSION_UNIT_REPRESENTATIVES_H

#include "k_tree.h"

namespace PoincareJ {

namespace Representatives {

class Time : public UnitRepresentative {
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

  // These must be sorted in order, from smallest to biggest
  constexpr static const Time* representativesAllowingImplicitAddition[] = {
      &second, &minute, &hour, &day, &month, &year};

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
  using Time::Time;
};

class Distance : public Time {
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

  // These must be sorted in order, from smallest to biggest
  constexpr static const Distance* representativesAllowingImplicitAddition[] = {
      &inch, &foot, &yard, &mile};

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
  using UnitRepresentative::UnitRepresentative;
};

class Angle : public UnitRepresentative {
 public:
  const static Angle radian;
  const static Angle arcSecond;
  const static Angle arcMinute;
  const static Angle degree;
  const static Angle gradian;
  constexpr static const Angle* representatives[] = {
      &radian, &arcSecond, &arcMinute, &degree, &gradian};

  // These must be sorted in order, from smallest to biggest
  constexpr static const Angle* representativesAllowingImplicitAddition[] = {
      &arcSecond, &arcMinute, &degree};

#if 0
  // Returns a beautified expression
  Tree* convertInto(Tree* value, const UnitRepresentative* other,
                    const ReductionContext& reductionContext) const;
#endif
  const DimensionVector dimensionVector() const override {
    return {.angle = 1};
  }
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

  // These must be sorted in order, from smallest to biggest
  constexpr static const Mass* representativesAllowingImplicitAddition[] = {
      &ounce, &pound};

  const DimensionVector dimensionVector() const override { return {.mass = 1}; }
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
 public:
  const static Current ampere;
  constexpr static const Current* representatives[] = {&ampere};

  const DimensionVector dimensionVector() const override {
    return {.current = 1};
  }
  const UnitRepresentative* representativesOfSameDimension() const override;
  bool isBaseUnit() const override {
    return this == representativesOfSameDimension();
  }

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Temperature : public UnitRepresentative {
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
 public:
  const static AmountOfSubstance mole;
  constexpr static const AmountOfSubstance* representatives[] = {&mole};

} const DimensionVector dimensionVector() const override {
  return {.amountOfSubstance = 1};
}
const UnitRepresentative* representativesOfSameDimension() const override;
bool isBaseUnit() const override {
  return this == representativesOfSameDimension();
}

private:
using UnitRepresentative::UnitRepresentative;
};

class LuminousIntensity : public UnitRepresentative {
 public:
  const static LuminousIntensity candela;
  constexpr static const LuminousIntensity* representatives[] = {&candela};

} const DimensionVector dimensionVector() const override {
  return {.luminousIntensity = 1};
}
const UnitRepresentative* representativesOfSameDimension() const override;
bool isBaseUnit() const override {
  return this == representativesOfSameDimension();
}

private:
using UnitRepresentative::UnitRepresentative;
};

class Frequency : public UnitRepresentative {
 public:
  const static Frequency hertz;
  constexpr static const Frequency* representatives[] = {&hertz};

  const DimensionVector dimensionVector() const override {
    return {.time = -1};
  }
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Force : public UnitRepresentative {
 public:
  const static Force newton;
  constexpr static const Force* representatives[] = {&newton};

} const DimensionVector dimensionVector() const override {
  return {.time = -2, .distance = 1, .mass = 1};
}
const UnitRepresentative* representativesOfSameDimension() const override;

private:
using UnitRepresentative::UnitRepresentative;
}
;

class Pressure : public UnitRepresentative {
 public:
  const static Pressure pascal;
  const static Pressure bar;
  const static Pressure atmosphere;
  constexpr static const Pressure* representatives[] = {&pascal, &bar,
                                                        &atmosphere};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .distance = -1, .mass = 1};
  }
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Energy : public UnitRepresentative {
 public:
  const static Energy joule;
  const static Energy electronVolt;
  constexpr static const Energy* representatives[] = {&joule, &electronVolt};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .distance = 2, .mass = 1};
  }
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
 public:
  const static Power watt;
  const static Power horsePower;
  constexpr static const Power* representatives[] = {&watt, &horsePower};

  const DimensionVector dimensionVector() const override {
    return {.time = -3, .distance = 2, .mass = 1};
  }
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricCharge : public UnitRepresentative {
 public:
  const static ElectricCharge coulomb;
  constexpr static const ElectricCharge* representatives[] = {&coulomb};

  const DimensionVector dimensionVector() const override {
    return {.time = 1, .current = 1};
  }
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class ElectricPotential : public UnitRepresentative {
 public:
  const static ElectricPotential volt;
  constexpr static const ElectricPotential* representatives[] = {&volt};

} const DimensionVector dimensionVector() const override {
  return {.time = -3, .distance = 2, .mass = 1, .current = -1};
}
const UnitRepresentative* representativesOfSameDimension() const override;

private:
using UnitRepresentative::UnitRepresentative;
}
;

class ElectricCapacitance : public UnitRepresentative {
 public:
  const static ElectricCapacitance farad;
  constexpr static const ElectricCapacitance* representatives[] = {&farad};

} const DimensionVector dimensionVector() const override {
  return {.time = 4, .distance = -2, .mass = -1, .current = 2};
}
const UnitRepresentative* representativesOfSameDimension() const override;

private:
using UnitRepresentative::UnitRepresentative;
}
;

class ElectricResistanceRepresentative : public UnitRepresentative {
 public:
  const static ElectricResistance ohm;
  constexpr static const ElectricResistance* representatives[] = {&ohm};

} const DimensionVector dimensionVector() const override {
  return {.time = -3, .distance = 2, .mass = 1, .current = -2};
}
const UnitRepresentative* representativesOfSameDimension() const override;

private:
using UnitRepresentative::UnitRepresentative;
}
;

class ElectricConductance : public UnitRepresentative {
 public:
  const static ElectricConductance siemens;
  constexpr static const ElectricConductance* representatives[] = {&siemens};

} const DimensionVector dimensionVector() const override {
  return {.time = 3, .distance = -2, .mass = -1, .current = 2};
}
const UnitRepresentative* representativesOfSameDimension() const override;

private:
using UnitRepresentative::UnitRepresentative;
}
;

class MagneticFlux : public UnitRepresentative {
 public:
  const static MagneticFlux weber;
  constexpr static const MagneticFlux* representatives[] = {&weber};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .distance = 2, .mass = 1, .current = -1};
  }
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class MagneticField : public UnitRepresentative {
 public:
  const static MagneticField tesla;
  constexpr static const MagneticField* representatives[] = {&tesla};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .mass = 1, .current = -1};
  }
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class Inductance : public UnitRepresentative {
 public:
  const static Inductance henry;
  constexpr static const Inductance* representatives[] = {&henry};

  const DimensionVector dimensionVector() const override {
    return {.time = -2, .distance = 2, .mass = 1, .current = -2};
  }
  const UnitRepresentative* representativesOfSameDimension() const override;

 private:
  using UnitRepresentative::UnitRepresentative;
};

class CatalyticActivity : public UnitRepresentative {
 public:
  const static CatalyticActivity katal;
  constexpr static const CatalyticActivity* representatives[] = {&katal};

} const DimensionVector dimensionVector() const override {
  return {.time = -1, .amountOfSubstance = 1};
}
const UnitRepresentative* representativesOfSameDimension() const override;

private:
using UnitRepresentative::UnitRepresentative;
}
;

class Surface : public UnitRepresentative {
 public:
  const static Surface hectare;
  const static Surface acre;
  constexpr static const Surface* representatives[] = {&hectare, &acre};

  const DimensionVector dimensionVector() const override {
    return {.distance = 2};
  }
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
  using UnitRepresentative::UnitRepresentative;
};

struct RepresentativesList {
  const UnitRepresentative* const* representativesList;
  int length;
};

constexpr static RepresentativesList
    k_representativesAllowingImplicitAddition[] = {
        {Time::representativesAllowingImplicitAddition,
         std::size(Time::representativesAllowingImplicitAddition)},
        {Distance::representativesAllowingImplicitAddition,
         std::size(Distance::representativesAllowingImplicitAddition)},
        {Mass::representativesAllowingImplicitAddition,
         std::size(Mass::representativesAllowingImplicitAddition)},
        {Angle::representativesAllowingImplicitAddition,
         std::size(Angle::representativesAllowingImplicitAddition)}};
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
