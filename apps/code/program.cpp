#include "program.h"

namespace Code {

Program::Program(char * textBuffer, size_t sizeOfBuffer) :
  m_bufferSize(sizeOfBuffer),
  m_textBuffer(textBuffer)
{
}

const char * Program::readOnlyContent() const {
  return m_textBuffer;
}

char * Program::editableContent() {
  return m_textBuffer;
}
int Program::bufferSize() const {
  return m_bufferSize;
}

}
