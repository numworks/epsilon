#include <math.h>

#undef ldexp

// For some reason, OpenBSD's libm defines ldexpf but doesn't define ldexp

double ldexp(double x, int n) {
  return scalbn(x, n);
}
