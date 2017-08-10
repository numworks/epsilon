#include "program.h"

namespace Code {

Program::Program() :
  m_buffer("")
{
  const char * program =

  "import kandinsky\n"
"red = kandinsky.color(255,0,0)\n"
"blue = kandinsky.color(0,0,255)\n"
"for i in range(100):\n"
"  for j in range(100):\n"
"    if ((i+j)%2 == 0):\n"
"      kandinsky.set_pixel(i, j, red)\n"
"    else:\n"
"      kandinsky.set_pixel(i, j, blue)\n";

   memcpy(m_buffer, program, 1024);
}

const char * Program::readOnlyContent() const {
  return m_buffer;
}

char * Program::editableContent() {
  return m_buffer;
}

void Program::setContent(const char * program) {
  memcpy(m_buffer, program, k_bufferSize);
}

}
