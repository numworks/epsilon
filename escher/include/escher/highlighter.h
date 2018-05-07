#ifndef ESCHER_HIGHLIGHTER_H
#define ESCHER_HIGHLIGHTER_H

#include <stddef.h>

class Highlighter {
public:
  virtual void highlight(char * buffer, char * attr_buffer, size_t bufferSize) = 0;
};

#endif
