#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <escher.h>

namespace Code {

class Script {
public:
  Script(char * textBuffer = nullptr, size_t sizeOfBuffer = 0);
  const char * readOnlyContent() const;
  char * editableContent();
  size_t bufferSize() const;
private:
  size_t m_bufferSize;
  char * m_textBuffer;
};

}

#endif
