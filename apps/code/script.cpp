#include "script.h"

namespace Code {

Script::Script(char * textBuffer, size_t sizeOfBuffer) :
  m_bufferSize(sizeOfBuffer),
  m_textBuffer(textBuffer)
{
}

const char * Script::readOnlyContent() const {
  return m_textBuffer;
}

char * Script::editableContent() {
  return m_textBuffer;
}
size_t Script::bufferSize() const {
  return m_bufferSize;
}

}
