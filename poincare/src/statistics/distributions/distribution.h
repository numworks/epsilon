#pragma once

#include <poincare/statistics/distribution.h>

namespace Poincare::Internal::Distribution {

using namespace Poincare::Distribution;

template <typename T>
T GetFisherMode(Type type, const ParametersArray<T>& parameters);
template <typename T>
T GetUniformXExtremum(Type type, const ParametersArray<T>& parameters,
                      bool min);
template <typename T>
T GetNormalXExtremum(Type type, const ParametersArray<T>& parameters, bool min);

constexpr static float k_uniformDiracMaximum = 10.0f;
constexpr static float k_fisherDefaultMax = 3.0f;

}  // namespace Poincare::Internal::Distribution
