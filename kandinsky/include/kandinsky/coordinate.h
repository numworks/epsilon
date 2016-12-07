#ifndef KANDINSKY_COORDINATE_H
#define KANDINSKY_COORDINATE_H

#include <stdint.h>

typedef int16_t KDCoordinate;

static inline KDCoordinate min(KDCoordinate x, KDCoordinate y) { return (x<y ? x : y); }
static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return (x>y ? x : y); }

#endif
