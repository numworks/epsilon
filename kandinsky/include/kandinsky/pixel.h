#ifndef KANDINSKY_REFERENTIAL_H
#define KANDINSKY_REFERENTIAL_H

#include <kandinsky/types.h>
#include <kandinsky/rect.h>
#include <kandinsky/color.h>

/* The drawing area is a very important yet simple concept.
 * When you set a drawing area (using absolute coordinates), two things happen:
 *  - No drawing will ever happen outside of the specified rect
 *  - All coordinates will then be interpreted as relative to the rect origin */
void KDSetDrawingArea(KDRect rect);

void KDSetPixel(KDPoint p, KDColor c);

//KDColor * KDPixelAddress(KDPoint p);
//#define COLOR(p) *KDPixelAddress(p)

#endif
