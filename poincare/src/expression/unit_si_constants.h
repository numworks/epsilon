#ifndef POINCARE_EXPRESSION_UNIT_SI_CONSTANTS_H
#define POINCARE_EXPRESSION_UNIT_SI_CONSTANTS_H

#include "unit_representatives.h"

namespace PoincareJ {
namespace Units {

// constexpr helpers to manipulate SI and derived units with the usual notations

consteval DimensionVector operator*(const DimensionVector& a,
                                    const DimensionVector& b) {
  DimensionVector r = a;
  r.addAllCoefficients(b, 1);
  return r;
}

consteval DimensionVector operator/(const DimensionVector& a,
                                    const DimensionVector& b) {
  DimensionVector r = a;
  r.addAllCoefficients(b, -1);
  return r;
}

// Unfortunately ^ has a lower priority than * so it needs to be parenthesized
consteval DimensionVector operator^(const DimensionVector& a, int p) {
  DimensionVector r = {};
  int absP = p > 0 ? p : -p;
  for (int i = 0; i < absP; i++) {
    r.addAllCoefficients(a, p > 0 ? 1 : -1);
  }
  return r;
}

// SI units
constexpr DimensionVector s = Time::Dimension;
constexpr DimensionVector m = Distance::Dimension;
constexpr DimensionVector rad = Angle::Dimension;
constexpr DimensionVector kg = Mass::Dimension;
constexpr DimensionVector A = Current::Dimension;
constexpr DimensionVector K = Temperature::Dimension;
constexpr DimensionVector mol = AmountOfSubstance::Dimension;
constexpr DimensionVector cd = LuminousIntensity::Dimension;

// Derived units
constexpr DimensionVector Hz = Frequency::Dimension;
constexpr DimensionVector N = Force::Dimension;
constexpr DimensionVector Pa = Pressure::Dimension;
constexpr DimensionVector J = Energy::Dimension;
constexpr DimensionVector W = Power::Dimension;
constexpr DimensionVector C = ElectricCharge::Dimension;
constexpr DimensionVector V = ElectricPotential::Dimension;
constexpr DimensionVector F = ElectricCapacitance::Dimension;
constexpr DimensionVector Ω = ElectricResistance::Dimension;
constexpr DimensionVector S = ElectricConductance::Dimension;
constexpr DimensionVector Wb = MagneticFlux::Dimension;
constexpr DimensionVector T = MagneticField::Dimension;
constexpr DimensionVector H = Inductance::Dimension;

static_assert(N == kg * m * (s ^ -2));
static_assert(J == N * m);
static_assert(W == J / s);

}  // namespace Units
}  // namespace PoincareJ

#endif
