#ifndef ESCHER_HIGHLIGHTER_H
#define ESCHER_HIGHLIGHTER_H

#include <stddef.h>

class Highlighter {
public:
  virtual bool highlight(char * buffer, char * attr_buffer, size_t bufferSize) = 0;
  virtual bool cursorMoved(char * buffer, char * attr_buffer, size_t bufferSize, int location) = 0;
};

#endif
