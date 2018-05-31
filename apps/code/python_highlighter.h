#ifndef CODE_PYTHON_HIGHLIGHTER_H
#define CODE_PYTHON_HIGHLIGHTER_H

#include <escher/text_area_renderer.h>

namespace Code {

void PythonHighlighter(const TextAreaRenderingContext * context, const char * text, size_t length, int fromColumn, int toColumn);

}

#endif
