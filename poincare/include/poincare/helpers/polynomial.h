#pragma once

#include <stddef.h>

namespace Poincare::PolynomialHelpers {

constexpr static size_t k_lineDegree = 1;

constexpr static size_t k_maxSolvableDegree = 3;

constexpr size_t NumberOfCoefficients(size_t degree) { return degree + 1; }

}  // namespace Poincare::PolynomialHelpers
