#ifndef ESCHER_PYTHON_HIGHLIGHTER_H
#define ESCHER_PYTHON_HIGHLIGHTER_H

#include <escher/highlighter.h>

#include <stddef.h>

class PythonHighlighter : public Highlighter {
public:
  void highlight(char * buffer, char * attr_buffer, size_t bufferSize);
};

extern PythonHighlighter pythonHighlighter;

#endif
