#ifndef KANDINSKY_DRAWING_AREA
#define KANDINSKY_DRAWING_AREA 1

#include <kandinsky/rect.h>

/* The drawing area is a very important yet simple concept.
 * When you set a drawing area (using absolute coordinates), two things happen:
 *  - No drawing will ever happen outside of the specified rect
 *  - All coordinates will then be interpreted as relative to the rect origin */
void KDSetDrawingArea(KDRect rect);

#endif
