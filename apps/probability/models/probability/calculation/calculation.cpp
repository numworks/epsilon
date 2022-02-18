#include "calculation.h"
#include "probability/models/probability/distribution/distribution.h"
#include "discrete_calculation.h"
#include "finite_integral_calculation.h"
#include "left_integral_calculation.h"
#include "right_integral_calculation.h"
#include <cmath>

namespace Probability {

void Calculation::Initialize(Calculation * calculation, Type type, Distribution * distribution) {
  calculation->~Calculation();
  switch (type) {
    case Type::LeftIntegral:
      new (calculation) LeftIntegralCalculation(distribution);
      return;
    case Type::FiniteIntegral:
      new (calculation) FiniteIntegralCalculation(distribution);
      return;
    case Type::RightIntegral:
      new (calculation) RightIntegralCalculation(distribution);
      return;
    case Type::Discrete:
      new (calculation) DiscreteCalculation(distribution);
      return;
    default:
      return;
  }
}

double Calculation::lowerBound() const {
  return -INFINITY;
}

double Calculation::upperBound() const {
  return INFINITY;
}

}
