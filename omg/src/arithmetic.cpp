#include <omg/arithmetic.h>

namespace OMG::Arithmetic {

size_t Gcd(size_t a, size_t b) {
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

}  // namespace OMG::Arithmetic
