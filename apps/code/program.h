#ifndef CODE_PROGRAM_H
#define CODE_PROGRAM_H

#include <escher.h>

namespace Code {

class Program {
public:
  Program(char * textBuffer, size_t sizeOfBuffer);
  const char * readOnlyContent() const;
  char * editableContent();
  int bufferSize() const;
private:
  size_t m_bufferSize;
  char * m_textBuffer;
};

}

#endif
