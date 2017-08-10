#ifndef CODE_PROGRAM_H
#define CODE_PROGRAM_H

#include <escher.h>

namespace Code {

class Program {
public:
  Program();
  const char * readOnlyContent() const;
  char * editableContent();
  void setContent(const char * program);
private:
  constexpr static int k_bufferSize = 1024;
  char m_buffer[k_bufferSize];
};

}

#endif

