#include <assert.h>
#include <poincare/old/helpers.h>
#include <poincare/old/list.h>
#include <poincare/old/list_complex.h>
#include <poincare/old/point.h>
#include <poincare/old/point_evaluation.h>

#include <cmath>

namespace Poincare {

size_t Helpers::Gcd(size_t a, size_t b) {
  int i = a;
  int j = b;
  do {
    if (i == 0) {
      return j;
    }
    if (j == 0) {
      return i;
    }
    if (i > j) {
      i = i - j * (i / j);
    } else {
      j = j - i * (j / i);
    }
  } while (true);
}

}  // namespace Poincare
