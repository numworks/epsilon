#ifndef ESCHER_PYTHON_HIGHLIGHTER_H
#define ESCHER_PYTHON_HIGHLIGHTER_H

#include <escher/highlighter.h>

#include <stddef.h>

class PythonHighlighter : public Highlighter {
public:
  PythonHighlighter();
  bool highlight(char * buffer, char * attr_buffer, size_t bufferSize);
  bool cursorMoved(char * buffer, char * attr_buffer, size_t bufferSize, int location);
private:
  bool m_onParenthesis;
};

#endif
