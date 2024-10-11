#ifndef POINCARE_HELPERS_H
#define POINCARE_HELPERS_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <cmath>

namespace Poincare {

class OList;
template <typename T>
class ListComplex;

class Helpers {
 public:
  static size_t Gcd(size_t a, size_t b);
};

}  // namespace Poincare

#endif
