#include <math.h>

// See nearbyintf.c for comments

#undef nearbyint

double nearbyint(double x) {
  return round(x);
}
