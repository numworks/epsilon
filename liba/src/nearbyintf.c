#include <math.h>

#undef nearbyintf

/* nearbyintf is not supposed to be the same as roundf according to OpenBSD's
 * documentation. Indeed:
 * - they round halfway cases to the nearest integer instead of away from zero
 *   (nearbyint(4.5) = 4 while round(4.5) = 5 for example).
 * - nearbyint is guaranteed never to set floating point flags or trigger
 *   floating point exceptions (which are disabled by default)
 *   accroding to OpenBSD open manual page.
 *
 * However, as nearbyintf is required by micropython only, in order to save
 * space and time (nearbyintf OpenBSD 6.0 implementation required some other
 * function implementation), we used roundf. */

float nearbyintf(float x) {
  return roundf(x);
}
