#ifndef POINCARE_EXPRESSION_UNITS_K_UNITS_H
#define POINCARE_EXPRESSION_UNITS_K_UNITS_H

#include "representatives.h"

namespace Poincare::Internal::Units {

struct AllRepresentatives {
  /* This structure is used to compute representatives ids from their names in a
   * constexpr way, it should not be instanciated or used for anything else. */
  AllRepresentatives() = delete;
  decltype(Time::representatives) time;
  decltype(Distance::representatives) distance;
  decltype(Angle::representatives) angle;
  decltype(Mass::representatives) mass;
  decltype(Current::representatives) current;
  decltype(Temperature::representatives) temperature;
  decltype(AmountOfSubstance::representatives) amountOfSubstance;
  decltype(LuminousIntensity::representatives) luminousIntensity;
  decltype(Frequency::representatives) frequency;
  decltype(Force::representatives) force;
  decltype(Pressure::representatives) pressure;
  decltype(Energy::representatives) energy;
  decltype(Power::representatives) power;
  decltype(ElectricCharge::representatives) electricCharge;
  decltype(ElectricPotential::representatives) electricPotential;
  decltype(ElectricCapacitance::representatives) electricCapacitance;
  decltype(ElectricResistance::representatives) electricResistance;
  decltype(ElectricConductance::representatives) electricConductance;
  decltype(MagneticFlux::representatives) magneticFlux;
  decltype(MagneticField::representatives) magneticField;
  decltype(Inductance::representatives) inductance;
  decltype(CatalyticActivity::representatives) catalyticActivity;
  decltype(Surface::representatives) surface;
  decltype(Volume::representatives) volume;
  decltype(Speed::representatives) speed;
  // Update Representative::DefaultRepresentatives if you change this
  // TODO: factorize the two lists
};

}  // namespace Poincare::Internal::Units

#define UNIT_ID(DIMENSION, NAME)                                              \
  uint8_t {                                                                   \
    offsetof(Poincare::Internal::Units::AllRepresentatives, DIMENSION.NAME) / \
        sizeof(Poincare::Internal::Units::Representative)                     \
  }

#define DEFINE_KUNIT(DIMENSION, NAME)                                 \
  constexpr KTree NAME =                                              \
      KTree<Poincare::Internal::Type::Unit, UNIT_ID(DIMENSION, NAME), \
            Units::Unit::k_emptyPrefixIndex>()

namespace Poincare::Internal::KTrees::KUnits {

DEFINE_KUNIT(time, second);
DEFINE_KUNIT(time, minute);
DEFINE_KUNIT(time, hour);

DEFINE_KUNIT(distance, meter);
DEFINE_KUNIT(distance, mile);
DEFINE_KUNIT(distance, yard);
DEFINE_KUNIT(distance, foot);
DEFINE_KUNIT(distance, inch);

constexpr KTree kilometer =
    KTree<Poincare::Internal::Type::Unit, UNIT_ID(distance, meter),
          Units::Unit::k_kiloPrefixIndex>();

DEFINE_KUNIT(angle, radian);
DEFINE_KUNIT(angle, degree);
DEFINE_KUNIT(angle, arcMinute);
DEFINE_KUNIT(angle, arcSecond);

DEFINE_KUNIT(mass, ton);
DEFINE_KUNIT(mass, kilogram);
DEFINE_KUNIT(mass, pound);
DEFINE_KUNIT(mass, ounce);

DEFINE_KUNIT(volume, liter);

}  // namespace Poincare::Internal::KTrees::KUnits

#undef DEFINE_KUNIT

#endif
