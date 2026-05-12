#pragma once

#include <poincare/src/memory/tree.h>

#include "representative.h"

namespace Poincare::Internal {
namespace Units {

bool IsNonKelvinTemperature(const Representative* representative);
// Remove the °C/°F unit and rewrite root as the equivalent Kelvin expression.
void RemoveTemperatureUnitFromRoot(Tree* root);
// From a Kelvin value, return the value in the given temperature
// representative.
double TemperatureFromKelvin(double value,
                             const Representative* representative);

}  // namespace Units
}  // namespace Poincare::Internal
