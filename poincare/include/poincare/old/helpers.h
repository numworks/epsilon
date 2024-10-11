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
  typedef void (*Swap)(int i, int j, void* context, int numberOfElements);
  typedef bool (*Compare)(int i, int j, void* context, int numberOfElements);

  static size_t Gcd(size_t a, size_t b);

  static bool Rotate(uint32_t* dst, uint32_t* src, size_t len);
  static void Sort(Swap swap, Compare compare, void* context,
                   int numberOfElements);
};

}  // namespace Poincare

#endif
