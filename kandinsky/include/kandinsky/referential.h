#ifndef KANDINSKY_REFERENTIAL_H
#define KANDINSKY_REFERENTIAL_H

#include <kandinsky/types.h>
#include <kandinsky/color.h>

void KDSetOrigin(KDPoint origin);
KDPoint KDGetOrigin();

KDColor * KDPixelAddress(KDPoint p);

#define COLOR(p) *KDPixelAddress(p)

#endif
