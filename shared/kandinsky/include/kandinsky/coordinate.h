#ifndef KANDINSKY_COORDINATE_H
#define KANDINSKY_COORDINATE_H

#include <stdint.h>

typedef int16_t KDCoordinate;

#define KDCOORDINATE_MAX INT16_MAX
#define KDCOORDINATE_MIN INT16_MIN

inline bool SumOverflowsKDCoordinate(KDCoordinate a, KDCoordinate b) {
  return (b > 0 && a > KDCOORDINATE_MAX - b) ||
         (b < 0 && a < KDCOORDINATE_MIN - b);
}

#endif
