#ifndef ESCHER_TEXT_AREA_RENDERER_H
#define ESCHER_TEXT_AREA_RENDERER_H

#include <stddef.h>
#include <kandinsky.h>

class TextAreaRenderingContext {
public:
  virtual void drawLineChunk(const char * text, size_t length, int column, KDColor textColor, KDColor backgroundColor) const = 0;
};

typedef void (*TextAreaRenderer)(const TextAreaRenderingContext * context, const char * text, size_t length, int fromColumn, int toColumn);

#endif
