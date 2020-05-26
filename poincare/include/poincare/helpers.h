#ifndef POINCARE_HELPERS_H
#define POINCARE_HELPERS_H

#include <stddef.h>
#include <stdint.h>

namespace Poincare {

namespace Helpers {

size_t AlignedSize(size_t realSize, size_t alignment);
size_t Gcd(size_t a, size_t b);
bool Rotate(uint32_t * dst, uint32_t * src, size_t len);
bool IsApproximatelyEqual(double observedValue, double expectedValue, double precision, double reference);

}

}

#endif
