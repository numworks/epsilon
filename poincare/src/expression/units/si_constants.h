#ifndef POINCARE_EXPRESSION_UNITS_SI_CONSTANTS_H
#define POINCARE_EXPRESSION_UNITS_SI_CONSTANTS_H

#include "representatives.h"

namespace Poincare::Internal {
namespace Units {

// constexpr helpers to manipulate SI and derived units with the usual notations

consteval SIVector operator*(const SIVector& a, const SIVector& b) {
  SIVector r = a;
  r.addAllCoefficients(b, 1);
  return r;
}

consteval SIVector operator/(const SIVector& a, const SIVector& b) {
  SIVector r = a;
  r.addAllCoefficients(b, -1);
  return r;
}

// Unfortunately ^ has a lower priority than * so it needs to be parenthesized
consteval SIVector operator^(const SIVector& a, int p) {
  SIVector r = {};
  int absP = p > 0 ? p : -p;
  for (int i = 0; i < absP; i++) {
    r.addAllCoefficients(a, p > 0 ? 1 : -1);
  }
  return r;
}

// SI units
constexpr SIVector s = Time::Dimension;
constexpr SIVector m = Distance::Dimension;
constexpr SIVector rad = Angle::Dimension;
constexpr SIVector kg = Mass::Dimension;
constexpr SIVector A = Current::Dimension;
constexpr SIVector K = Temperature::Dimension;
constexpr SIVector mol = AmountOfSubstance::Dimension;
constexpr SIVector cd = LuminousIntensity::Dimension;

// Derived units
constexpr SIVector Hz = Frequency::Dimension;
constexpr SIVector N = Force::Dimension;
constexpr SIVector Pa = Pressure::Dimension;
constexpr SIVector J = Energy::Dimension;
constexpr SIVector W = Power::Dimension;
constexpr SIVector C = ElectricCharge::Dimension;
constexpr SIVector V = ElectricPotential::Dimension;
constexpr SIVector F = ElectricCapacitance::Dimension;
constexpr SIVector Ω = ElectricResistance::Dimension;
constexpr SIVector S = ElectricConductance::Dimension;
constexpr SIVector Wb = MagneticFlux::Dimension;
constexpr SIVector T = MagneticField::Dimension;
constexpr SIVector H = Inductance::Dimension;

static_assert(N == kg * m * (s ^ -2));
static_assert(J == N * m);
static_assert(W == J / s);

}  // namespace Units
}  // namespace Poincare::Internal

#endif
