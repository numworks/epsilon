#ifndef KANDINSKY_LINE_H
#define KANDINSKY_LINE_H

#include <kandinsky/color.h>
#include <kandinsky/types.h>

void KDDrawLine(KDPoint p1, KDPoint p2, KDColor c);

void KDDrawAntiAliasedLine(KDPoint p1, KDPoint p2, KDCoordinate width, KDColor frontColor, KDColor backColor);

#endif
