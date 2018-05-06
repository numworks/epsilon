#ifndef ESCHER_PYTHON_HIGHLIGHTER_H
#define ESCHER_PYTHON_HIGHLIGHTER_H

#include <stddef.h>

class PythonHighlighter {
public:
  void highlight(char * buffer, char * attr_buffer, size_t bufferSize);
};

#endif
