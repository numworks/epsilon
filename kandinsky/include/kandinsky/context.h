#ifndef KANDINSKY_CONTEXT_H
#define KANDINSKY_CONTEXT_H

#include "types.h"
#include "rect.h"

typedef struct {
  struct {
    void (*pushRect)(KDRect rect, const KDColor * pixels);
    void (*pushRectUniform)(KDRect rect, KDColor color);
    void (*pullRect)(KDRect rect, KDColor * pixels);
  } io;
  KDPoint origin;
  KDRect clippingRect;
} KDContext;

extern KDContext * KDCurrentContext;

#endif
