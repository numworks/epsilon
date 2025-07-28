#include "calculation.h"

#include <cmath>

#include "../distribution/distribution.h"
#include "discrete_calculation.h"
#include "finite_integral_calculation.h"
#include "left_integral_calculation.h"
#include "right_integral_calculation.h"

namespace Distributions {

bool Calculation::Initialize(Calculation* calculation, Type type,
                             Distribution* distribution,
                             bool forceReinitialisation) {
  bool changedType = false;
  if (calculation->type() != type || forceReinitialisation) {
    changedType = true;
    calculation->~Calculation();
    switch (type) {
      case Type::LeftIntegral:
        new (calculation) LeftIntegralCalculation(distribution);
        break;
      case Type::FiniteIntegral:
        new (calculation) FiniteIntegralCalculation(distribution);
        break;
      case Type::RightIntegral:
        new (calculation) RightIntegralCalculation(distribution);
        break;
      case Type::Discrete:
        new (calculation) DiscreteCalculation(distribution);
        break;
      default:
        assert(false);
    }
  }
  if (distribution->allParametersAreInitialized()) {
    calculation->compute(0);
  } else {
    calculation->computeUnknownDistributionParameter();
  }
  return changedType;
}

double Calculation::lowerBound() const { return -INFINITY; }

double Calculation::upperBound() const { return INFINITY; }

}  // namespace Distributions
