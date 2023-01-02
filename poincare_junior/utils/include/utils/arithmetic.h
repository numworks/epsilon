#ifndef UTILS_ARITHMETIC_H
#define UTILS_ARITHMETIC_H

#include <stddef.h>

namespace Arithmetic {

size_t Gcd(size_t a, size_t b);

template <typename T>
T CeilDivision(T numerator, T denominator) { return (numerator + denominator - 1) / denominator; }

}

#endif
